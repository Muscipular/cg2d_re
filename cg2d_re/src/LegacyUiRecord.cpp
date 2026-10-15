#include "CharacterSelectSceneDependencies.hpp"
#include "cg/ui/LegacyWindowSelection.hpp"
#include "cg/text/Encoding.hpp"
#include "CharacterSelectState.hpp"
#include "LegacyPanelRuntime.hpp"
#include "LegacyPanelTexts.hpp"
#include "LoginSceneDependencies.hpp"
#include "platform/MainLoopDependencies.hpp"
#include "platform/LegacyFormat.hpp"
#include "ui/WindowRuntimeState.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include "cg/LoginState.hpp"
#include "cg/LoginScene.hpp"
#include "cg/LoginInput.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/DisplaySettings.hpp"
#include "cg/GraphicAlloc.hpp"
#include "cg/LegacyRecordOrder.hpp"
#include "cg/Input.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/Blacklist.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B226、0x0053B2F1、0x0053BB02、0x0053BC41；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304、0x0053D21D、0x0053BC0F；直接沿MSVC CRT。
#include <cstring> // 原CRT接口：0x0053BEC0；原参数／返回合同由直接调用承接。
#include <string>
#include <format>

namespace {

// 仅用于文字提交失败去重，不参与旧界面的业务状态。
void ReportLegacyTextStorageFailure()
{
    static cg::input::InputContext* reportedContext = nullptr;
    static std::wstring reportedText;
    static bool reported = false;
    auto* const context = cg::input::gCurrentInputData;
    const std::wstring contents = context != nullptr ? cg::input::ContextText(*context) : std::wstring{};
    if (!reported || reportedContext != context || reportedText != contents) {
        reportedContext = context;
        reportedText = contents;
        reported = true;
        cg::platform::ShowWideMessage(L"文字无法按GBK保存，或已超出字段容量。", L"文字编码", 0x10U);
    }
}

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

// 原函数：0x0045F100。
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
    return left + right;
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


// 原函数：0x0045EFD0。
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

std::uint32_t gLegacyPanelHitIdD046C0 = 0; // 原全局：0x00D046C0。
LegacyPanelRecord gLegacySkillList585708 { // 原全局：0x00585708。
    0, 0, 272, 438, 18, {0x58DE, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::uint8_t gLegacySkillPendingD67C48 = 0; // 原全局：0x00D67C48。
std::int32_t gLegacyRequiredFpCC31D8 = 0; // 原全局：0x00CC31D8。
std::int32_t gLegacyItemWorkSelection584394 = -1; // 原全局：0x00584394。
std::wstring gLegacyItemWorkNameCC40D8 {}; // 原全局：0x00CC40D8。
std::int32_t gLegacyItemWorkImageCC320C = 0; // 原全局：0x00CC320C。
std::int32_t gLegacyCraftPhaseD67D00 = 0; // 原全局：0x00D67D00。
std::int32_t gLegacyGemPhaseD68A8C = 0; // 原全局：0x00D68A8C。
std::int32_t gLegacyRecipePageD67D0C = 0; // 原全局：0x00D67D0C。
std::int32_t gLegacyInventoryValueC8B388 = 0; // 原全局：0x00C8B388。
std::int32_t gLegacySpecialUseItemC8A96C = 0; // 原全局：0x00C8A96C。
std::int32_t gLegacySpecialUseColumnCF4AA8 = 0; // 原全局：0x00CF4AA8。
std::int32_t gLegacySpecialUseRowD2D614 = 0; // 原全局：0x00D2D614。
std::int32_t gLegacySplitSourceCC3E60 = 0; // 原全局：0x00CC3E60。
std::int32_t gLegacySplitTargetCC21DC = 0; // 原全局：0x00CC21DC。
std::int32_t gLegacySplitAmountC8A948 = 0; // 原全局：0x00C8A948。
std::int32_t gLegacyLastUsedItemCB01EC = 0; // 原全局：0x00CB01EC。
// 原PE.data载入填零；图号/页数运行时生产仍待，不在开页时重置。


 // 原虚拟零初值；旧58翻页写入，旧59/60/61共用。



std::int32_t gLegacyShopChoiceModeC9A94C = 0; // 原全局：0x00C9A94C。 原PE虚拟零S32；运行时生产仍须事实。
// 原PE.data载入填零；人数/页数运行时生产仍待，不借现代名单计数。


 // 原虚拟零初值；旧51翻页发布，不在开页时重置。

namespace {

// 原[593A00,593F74)：1040B显示区、S32时间、4B保留区与348B输入共用连续owner。
// 装填可沿原顺序写入第17行并覆盖相邻已证字段，不扩大显示容量或新增裁剪。
struct LegacyLogStorage {
    std::array<char, 1040> rows;
    std::int32_t time593E10; // 原全局：0x00593E10。
    std::array<std::byte, 4> reserved593E14; // 原全局：0x00593E14。
    input::InputContext input593E18; // 原全局：0x00593E18。
};
static_assert(offsetof(LegacyLogStorage, time593E10) == 1040);
static_assert(offsetof(LegacyLogStorage, input593E18) == 1048);
LegacyLogStorage gLegacyLogStorage593A00 {}; // 原全局：0x00593A00。 PE.data未载入尾，沿既有节头填零证据。
LegacyPanelRecord gLegacyLogPanel553440 { // 原全局：0x00553440。
    20, 10, 600, 438, 98, {31501, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyLogPageCount553504 = 1; // 原全局：0x00553504。
std::int32_t gLegacyLogPage553508 = 1; // 原全局：0x00553508。
platform::LegacyFile* gLegacyLogFile593F78 = nullptr; // 原全局：0x00593F78。
std::int32_t gLegacyLogError593F7C = 0; // 原全局：0x00593F7C。
std::int32_t gLegacyLogLineCount593F80 = 0; // 原全局：0x00593F80。
platform::LegacyTm53B8C6* gLegacyLogCalendar593F88 = nullptr; // 原全局：0x00593F88。 借用原共享localtime九槽。
input::InputContext* gLegacyLogInput593F8C = nullptr; // 原全局：0x00593F8C。 借同一连续owner，不另建输入副本。
constexpr wchar_t kLegacyLogFailure55358C[] = L"無法開啟冒險者日誌"; // 原常量参考：0x0055358C。
constexpr wchar_t kLegacyLogAfterDay553534[] = L"後一天的日誌。"; // 原常量参考：0x00553534。
constexpr wchar_t kLegacyLogBeforeDay553544[] = L"前一天的日誌。"; // 原常量参考：0x00553544。
constexpr wchar_t kLegacyLogWrite553554[] = L"寫入日誌。"; // 原常量参考：0x00553554。
constexpr wchar_t kLegacyLogLast553560[] = L"最後一頁"; // 原常量参考：0x00553560。
constexpr wchar_t kLegacyLogFirst55356C[] = L"第一頁"; // 原常量参考：0x0055356C。
constexpr wchar_t kLegacyLogNext553574[] = L"下一頁。"; // 原常量参考：0x00553574。
constexpr wchar_t kLegacyLogPrevious553580[] = L"上一頁。"; // 原常量参考：0x00553580。
constexpr char kLegacyLogWriteFormat5535A0[] = "\x0A\x20%02d:%02d:%02d\x81\x40%s"; // 原常量参考：0x005535A0。
constexpr wchar_t kLegacyLogClose55331C[] = L"關閉這個視窗。"; // 原常量参考：0x0055331C。

// 原旧53/54共享状态；虚拟零仅承载映像初值，布局/文本/输入生产仍待。
LegacyPanelRecord gLegacyDialogPanelD2D9C8 {}; // 原全局：0x00D2D9C8。
LegacyPanelRecord gLegacyLineDialogPanelCC4150 {}; // 原全局：0x00CC4150。
std::int32_t gLegacyDialogLineCountCA043C = 0; // 原全局：0x00CA043C。
std::int32_t gLegacyDialogOffsetXCAFEAC = 0; // 原全局：0x00CAFEAC。
std::int32_t gLegacyDialogOffsetYCAFEA8 = 0; // 原全局：0x00CAFEA8。
std::int32_t gLegacyDialogColumnsCA0010 = 0; // 原全局：0x00CA0010。
std::int32_t gLegacyDialogFirstLineD673D4 = 0; // 原全局：0x00D673D4。
input::InputContext* gLegacyDialogInputD67C4C = nullptr; // 原全局：0x00D67C4C。
constexpr wchar_t kLegacyDialogButton58599C[] = L" 確定 "; // 原常量参考：0x0058599C。
constexpr wchar_t kLegacyDialogButton585994[] = L" 取消 "; // 原常量参考：0x00585994。
constexpr wchar_t kLegacyDialogButton58598C[] = L"  是  "; // 原常量参考：0x0058598C。
constexpr wchar_t kLegacyDialogButton585984[] = L"  否  "; // 原常量参考：0x00585984。
constexpr wchar_t kLegacyDialogButton58597C[] = L"上一頁"; // 原常量参考：0x0058597C。
constexpr wchar_t kLegacyDialogButton585974[] = L"下一頁"; // 原常量参考：0x00585974。
std::array<character_select::detail::CharacterModalUiRecord, 6> gLegacyDialogButtons581808 {{ // 原全局：0x00581808。
    {0, 0, 0, 0, 54, 16, kLegacyDialogButton58599C},
    {0, 0, 0, 0, 54, 16, kLegacyDialogButton585994},
    {0, 0, 0, 0, 51, 16, kLegacyDialogButton58598C},
    {0, 0, 0, 0, 51, 16, kLegacyDialogButton585984},
    {0, 0, 0, 0, 52, 16, kLegacyDialogButton58597C},
    {0, 0, 0, 0, 52, 16, kLegacyDialogButton585974},
}};

LegacyPanelRecord gLegacySkillChoicePanel581D60 { // 原全局：0x00581D60。
    0, 0, 292, 260, 55, {23050, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacySkillChoiceActionD67C58 = nullptr; // 原全局：0x00D67C58。 原旧55/56/57同源借用，关闭标记死亡后清槽。
constexpr wchar_t kLegacySkillChoiceLearn585AC4[] = L"想學習技能"; // 原常量参考：0x00585AC4。
constexpr wchar_t kLegacySkillChoiceForget588BC8[] = L"想忘記技能"; // 原常量参考：0x00588BC8。
constexpr wchar_t kLegacySkillChoiceCancel588BBC[] = L"還是算了"; // 原常量参考：0x00588BBC。
constexpr wchar_t kLegacySkillChoiceLearnHint588BA8[] = L"向此人學習技能。"; // 原常量参考：0x00588BA8。
constexpr wchar_t kLegacySkillChoiceForgetHint588B90[] = L"忘記自己所持有的技能。"; // 原常量参考：0x00588B90。
constexpr wchar_t kLegacySkillChoiceCancelHint588B80[] = L"停止學習技能。"; // 原常量参考：0x00588B80。

LegacyPanelRecord gLegacySkillLearningPanel581E28 { // 原全局：0x00581E28。
    0, 0, 576, 432, 56, {23301, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacySkillLearningAccept588C20[] = L" 學  習"; // 原常量参考：0x00588C20。
constexpr wchar_t kLegacySkillLearningCancel588C18[] = L" 不  學"; // 原常量参考：0x00588C18。
constexpr wchar_t kLegacySkillLearningSlots588C00[] = L"使用技能欄位數：{} 欄"; // 原常量参考：0x00588C00。
constexpr wchar_t kLegacySkillLearningCost588BEC[] = L"學習費用：{} 金幣"; // 原常量参考：0x00588BEC。
constexpr wchar_t kLegacySkillLearningAcceptHint588BE0[] = L"學習技能。"; // 原常量参考：0x00588BE0。
constexpr wchar_t kLegacySkillLearningCancelHint588BD4[] = L"停止學習。"; // 原常量参考：0x00588BD4。

LegacyPanelRecord gLegacySkillForgetPanel581EF0 { // 原全局：0x00581EF0。
    0, 0, 576, 432, 57, {23300, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacySkillForgetCancel588CBC[] = L"還是不要忘記好了"; // 原常量参考：0x00588CBC。
constexpr wchar_t kLegacySkillForgetNotes588CA4[] = L"※※※  注意  ※※※"; // 原常量参考：0x00588CA4。
constexpr wchar_t kLegacySkillForgetWarning588C94[] = L"技能只要一忘記"; // 原常量参考：0x00588C94。
constexpr wchar_t kLegacySkillForgetNoRestore588C88[] = L"就無法復原"; // 原常量参考：0x00588C88。
constexpr wchar_t kLegacySkillForgetCareful588C7C[] = L"請注意。"; // 原常量参考：0x00588C7C。
constexpr wchar_t kLegacySkillForgetSelect588C68[] = L"請選擇要忘記的技能"; // 原常量参考：0x00588C68。
constexpr wchar_t kLegacySkillForgetTotal588C48[] = L"現在技能欄使用量的合計：{:2}/10"; // 原常量参考：0x00588C48。
constexpr wchar_t kLegacySkillForgetCancelHint588C38[] = L"停止忘記技能。"; // 原常量参考：0x00588C38。
constexpr wchar_t kLegacySkillForgetRowHint588C28[] = L"忘記此技能。"; // 原常量参考：0x00588C28。

LegacyPanelRecord gLegacySkillStatusPanel583C90 { // 原全局：0x00583C90。
    64, 108, 512, 240, 36, {}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacySkillStatusPrompt589B58[] = L"請選擇使用對象。"; // 原常量参考：0x00589B58。
constexpr wchar_t kLegacySkillStatusHeader589B2C[] = L"健康  等級 姓名             生命      魔力"; // 原常量参考：0x00589B2C。
constexpr wchar_t kLegacySkillStatusCancelTransform589B20[] = L"停止變裝。"; // 原常量参考：0x00589B20。
constexpr wchar_t kLegacySkillStatusCancelDisguise589B14[] = L"停止變身。"; // 原常量参考：0x00589B14。
constexpr wchar_t kLegacySkillStatusCancelRestore589B08[] = L"停止急救。"; // 原常量参考：0x00589B08。
constexpr wchar_t kLegacySkillStatusCancelHeal589AFC[] = L"停止治療。"; // 原常量参考：0x00589AFC。
constexpr wchar_t kLegacySkillStatusTransform589AEC[] = L"變裝成此角色。"; // 原常量参考：0x00589AEC。
constexpr wchar_t kLegacySkillStatusDisguise589ADC[] = L"變身成此寵物。"; // 原常量参考：0x00589ADC。
constexpr wchar_t kLegacySkillStatusSelf589ACC[] = L"替自己療傷。"; // 原常量参考：0x00589ACC。
constexpr wchar_t kLegacySkillStatusPet589AB8[] = L"替這個寵物療傷。"; // 原常量参考：0x00589AB8。
constexpr wchar_t kLegacySkillStatusPerson589AA8[] = L"替此人療傷。"; // 原常量参考：0x00589AA8。

LegacyPanelRecord gLegacySkillTargetPanel583D58 { // 原全局：0x00583D58。
    160, 132, 320, 192, 35, {}, {}, 0, 0, {}, {},
};


constexpr wchar_t kLegacySkillTargetTransform589BD8[] = L"變成這個角色。"; // 原常量参考：0x00589BD8。
constexpr wchar_t kLegacySkillTargetDisguiseSelf589BC4[] = L"變身成自己的寵物。"; // 原常量参考：0x00589BC4。
constexpr wchar_t kLegacySkillTargetDisguisePerson589BAC[] = L"變身成此角色的寵物。"; // 原常量参考：0x00589BAC。
constexpr wchar_t kLegacySkillTargetRestoreSelf589B98[] = L"替自己或寵物療傷。"; // 原常量参考：0x00589B98。
constexpr wchar_t kLegacySkillTargetRestorePerson589B80[] = L"替這個角色或寵物療傷。"; // 原常量参考：0x00589B80。

LegacyPanelRecord gLegacyItemTargetPanel582D40 { // 原全局：0x00582D40。
    160, 132, 320, 192, 77, {}, {}, 0, 0, {}, {},
};


 // 4E4EA0发送前发布，发送后重读。


LegacyPanelRecord gLegacyTextNoticePanel583EE8 { // 原全局：0x00583EE8。
    130, 180, 336, 96, 81, {}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyTextNoticePanel583FB0 { // 原全局：0x00583FB0。
    130, 180, 336, 96, 82, {}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyTextNotice589C24[] = LR"(包含 \ 和 | 。 )"; // 原常量参考：0x00589C24。
constexpr wchar_t kLegacyTextNotice589C08[] = LR"(刻印不能使用 \ 和 | 的字。)"; // 原常量参考：0x00589C08。
constexpr wchar_t kLegacyTextNotice589C4C[] = L"不包括空白以外的文字。"; // 原常量参考：0x00589C4C。
constexpr wchar_t kLegacyTextNotice589C34[] = L"刻印名不能是空白的。"; // 原常量参考：0x00589C34。

LegacyPanelRecord gLegacyBuyPanel5823A0 { // 原全局：0x005823A0。
    0, 0, 576, 432, 63, {23055, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyBuyConfirmPanel582468 { // 原全局：0x00582468。
    0, 36, 640, 384, 65, {}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyBuyActionD67C6C = nullptr; // 原全局：0x00D67C6C。
std::int32_t gLegacyBuyImageD2DBFC = 0; // 原全局：0x00D2DBFC。 独立图号，原生产待证。
std::int32_t gLegacyBuyPageCountD2D63C = 0; // 原全局：0x00D2D63C。 原生产待证，不以初值替代实际页数。
std::int32_t gLegacyBuyDetailPageD67C70 = 0; // 原全局：0x00D67C70。
std::int32_t gLegacyBuyPreviousHover582464 = -1; // 原全局：0x00582464。
std::int32_t gLegacyBuyTotalC8AF6C = 0; // 原全局：0x00C8AF6C。 旧63统计，旧65借读。
std::int32_t gLegacyBuyMessageD30414 = 0; // 原全局：0x00D30414。
LegacyPanelRecord gLegacySellPanel582530 { // 原全局：0x00582530。
    0, 0, 576, 432, 64, {23055, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacySellConfirmPanel5825F8 { // 原全局：0x005825F8。
    0, 36, 640, 384, 66, {}, {}, 0, 0, {}, {},
};
YobiAction* gLegacySellActionD67C74 = nullptr; // 原全局：0x00D67C74。 旧64预览借用，关闭时标记死亡。
std::int32_t gLegacySellImageCC3EFC = 0; // 原全局：0x00CC3EFC。 独立图号，原生产待证。
std::int32_t gLegacySellPageCF49FC = 0; // 原全局：0x00CF49FC。 独立旧64页号，初始生产待证。
std::int32_t gLegacySellPageCountCA0ABC = 0; // 原全局：0x00CA0ABC。 原生产待证，不用虚拟零推定页数。
std::int32_t gLegacySellDetailPageD67C78 = 0; // 原全局：0x00D67C78。
std::int32_t gLegacySellPreviousHover5825F4 = -1; // 原全局：0x005825F4。
std::int32_t gLegacySellTotalCF3FCC = 0; // 原全局：0x00CF3FCC。 旧64入口统计，旧66借读。
std::int32_t gLegacySellLimitMessageCB0A88 = 0; // 原全局：0x00CB0A88。 增量金额失败写1，数量成功后清0。
LegacyPanelRecord gLegacyHealingPanel5826C0 { // 原全局：0x005826C0。
    96, 86, 448, 288, 67, {}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyHealingPrompt588FF8[] = L"請選擇治療對象"; // 原常量参考：0x00588FF8。
constexpr wchar_t kLegacyHealingHeader588FD0[] = L"健康 等級     姓   名        所需花費"; // 原常量参考：0x00588FD0。
constexpr wchar_t kLegacyHealingTotal588FA8[] = L"全部一起治療     {:8} 金幣"; // 原常量参考：0x00588FA8。
constexpr wchar_t kLegacyHealingGold588F94[] = L"持有金額 {:7} 金幣"; // 原常量参考：0x00588F94。
constexpr wchar_t kLegacyHealingCancel588F34[] = L"取消治療。"; // 原常量参考：0x00588F34。
constexpr wchar_t kLegacyHealingOne588F40[] = L"治療這個角色的傷。"; // 原常量参考：0x00588F40。
constexpr wchar_t kLegacyHealingHealthy588F54[] = L"這個角色並沒有受傷。"; // 原常量参考：0x00588F54。
constexpr wchar_t kLegacyHealingAllHealthy588F6C[] = L"沒有任何角色受傷。"; // 原常量参考：0x00588F6C。
constexpr wchar_t kLegacyHealingAll588F80[] = L"治療所有受傷角色。"; // 原常量参考：0x00588F80。

LegacyPanelRecord gLegacyRepairPanel582788 { // 原全局：0x00582788。
    32, 12, 576, 432, 70, {23055, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyRepairConfirmPanel582850 { // 原全局：0x00582850。
    0, 36, 640, 384, 71, {}, {}, 0, 0, {}, {},
};
// 原CE4548独立二十条C48H记录，字段合同见legacy_repair_list.md；上游生产待补。
std::array<char, 20 * 0xC48> gLegacyRepairItemsCE4548 {};
YobiAction* gLegacyRepairActionD67C7C = nullptr; // 原全局：0x00D67C7C。 旧70预览借用，关闭时标记死亡。
std::int32_t gLegacyRepairImageD673E8 = 0; // 原全局：0x00D673E8。 独立图号，原生产待证。
std::int32_t gLegacyRepairPageCF4A08 = 0; // 原全局：0x00CF4A08。 独立页号，初始生产待证。
std::int32_t gLegacyRepairPageCountCC4B24 = 0; // 原全局：0x00CC4B24。 原生产待证，不用虚拟零推定页数。
std::int32_t gLegacyRepairDetailPageD67C80 = 0; // 原全局：0x00D67C80。
std::int32_t gLegacyRepairPreviousHover58284C = -1; // 原全局：0x0058284C。
std::int32_t gLegacyRepairTotalD460C4 = 0; // 原全局：0x00D460C4。 旧70入口统计，旧71借读。
std::int32_t gLegacyRepairMessageD64E14 = 0; // 原全局：0x00D64E14。 初始化/数量成功/翻页清0，金额不足写1。
constexpr wchar_t kLegacyRepairWelcome587D2C[] = L"歡迎光臨。"; // 原常量参考：0x00587D2C。
constexpr wchar_t kLegacyRepairDecrease589010[] = L"取消修理此物品。"; // 原常量参考：0x00589010。
constexpr wchar_t kLegacyRepairIncrease589024[] = L"修理此物品。"; // 原常量参考：0x00589024。
constexpr wchar_t kLegacyRepairCancel589034[] = L"取消修理指定物品。"; // 原常量参考：0x00589034。
constexpr wchar_t kLegacyRepairConfirm589048[] = L"修理指定物品。"; // 原常量参考：0x00589048。
constexpr wchar_t kLegacyRepairPrompt589058[] = L"要修理哪一個呢？"; // 原常量参考：0x00589058。
constexpr wchar_t kLegacyRepairNoMoney58906C[] = L"你的錢不夠！"; // 原常量参考：0x0058906C。
constexpr wchar_t kLegacyRepairStopHint58907C[] = L"不修。"; // 原常量参考：0x0058907C。
constexpr wchar_t kLegacyRepairAcceptHint589084[] = L"修理。"; // 原常量参考：0x00589084。
constexpr wchar_t kLegacyRepairQuestion58908C[] = L"修理以上物品。可以嗎？"; // 原常量参考：0x0058908C。
constexpr wchar_t kLegacyRepairSummary5890A4[] = L"持有金額  {:7} 金幣    修理金額  {:7} 金幣"; // 原常量参考：0x005890A4。

// SCP01旧62完整本体承载；独立面板/预览复用，模式生产与未写命令仍未闭。
constexpr wchar_t kLegacyShopBuyInitial588DCC[] = L" 想 買 "; // 原常量参考：0x00588DCC。
constexpr wchar_t kLegacyShopSellInitial588DC4[] = L" 想 賣 "; // 原常量参考：0x00588DC4。
constexpr wchar_t kLegacyShopStopInitial588DB8[] = L"不 必 了"; // 原常量参考：0x00588DB8。
constexpr wchar_t kLegacyShopBuy588DAC[] = L"  想  買"; // 原常量参考：0x00588DAC。
constexpr wchar_t kLegacyShopSell588DA0[] = L"  想  賣"; // 原常量参考：0x00588DA0。
constexpr wchar_t kLegacyShopCancel588D94[] = L" 不 必 了"; // 原常量参考：0x00588D94。
constexpr wchar_t kLegacyShopBuyHint588D84[] = L"購買此店物品。"; // 原常量参考：0x00588D84。
constexpr wchar_t kLegacyShopSellHint588D70[] = L"將物品賣給此店。"; // 原常量参考：0x00588D70。
LegacyPanelRecord gLegacyShopChoicePanel5822D8 { // 原全局：0x005822D8。
    0, 0, 292, 260, 62, {23050, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyShopChoiceActionD67C68 = nullptr; // 原全局：0x00D67C68。


 // 4D7440发布，4D7470关闭时标死/清槽。

 // 旧59独立预览，不共享旧58对象槽。



 // 旧60写入，旧61借读；不同于现代替换选择。
















LegacyPanelRecord gLegacyIdentifyPanel582918 { // 原全局：0x00582918。
    32, 12, 576, 432, 68, {23055, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyIdentifyConfirmPanel5829E0 { // 原全局：0x005829E0。
    0, 36, 640, 384, 69, {}, {}, 0, 0, {}, {},
};
// 原C8B390独立二十条C48H记录，末端C9A930；原字段合同见legacy_identify_list.md。
std::array<char, 20 * 0xC48> gLegacyIdentifyItemsC8B390 {}; // 原全局：0x00C8B390。
YobiAction* gLegacyIdentifyActionD67C84 = nullptr; // 原全局：0x00D67C84。 旧68预览借用，关闭时标记死亡。
std::int32_t gLegacyIdentifyImageCF4698 = 0; // 原全局：0x00CF4698。 独立图号，原生产待证。
std::int32_t gLegacyIdentifyPageCC3F00 = 0; // 原全局：0x00CC3F00。 独立页号，初始生产待证。
std::int32_t gLegacyIdentifyPageCountC9A950 = 0; // 原全局：0x00C9A950。 原生产待证，不用虚拟零推定页数。
std::int32_t gLegacyIdentifyDetailPageD67C88 = 0; // 原全局：0x00D67C88。
std::int32_t gLegacyIdentifyPreviousHover5829DC = -1; // 原全局：0x005829DC。
std::int32_t gLegacyIdentifyTotalCB1E3C = 0; // 原全局：0x00CB1E3C。 入口统计，翻页/数量成功清0，旧69借读。
std::int32_t gLegacyIdentifyMessageC9A948 = 0; // 原全局：0x00C9A948。 初始化清0，金币不足写1；翻页/数量成功不清。
constexpr wchar_t kLegacyIdentifyDecrease5890D0[] = L"取消鑑定此物品。"; // 原常量参考：0x005890D0。
constexpr wchar_t kLegacyIdentifyIncrease5890E4[] = L"鑑定此物品。"; // 原常量参考：0x005890E4。
constexpr wchar_t kLegacyIdentifyConfirm5890F4[] = L"鑑定指定物品。"; // 原常量参考：0x005890F4。
constexpr wchar_t kLegacyIdentifyCancel589104[] = L"取消鑑定。"; // 原常量参考：0x00589104。
constexpr wchar_t kLegacyIdentifyPrompt589110[] = L"要鑑定哪一個物品？"; // 原常量参考：0x00589110。
constexpr wchar_t kLegacyIdentifyStopHint589124[] = L"不鑑定。"; // 原常量参考：0x00589124。
constexpr wchar_t kLegacyIdentifyAcceptHint589130[] = L"鑑定。"; // 原常量参考：0x00589130。
constexpr wchar_t kLegacyIdentifyQuestion589138[] = L"鑑定以上物品。可以嗎？"; // 原常量参考：0x00589138。
constexpr wchar_t kLegacyIdentifySummary589150[] = L"持有金額  {:7} 金幣    鑑定金額  {:7} 金幣"; // 原常量参考：0x00589150。

LegacyPanelRecord gLegacyMessagePanel582AA8 { // 原全局：0x00582AA8。
    20, 9, 600, 438, 73, {23224, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
input::InputContext* gLegacyMessageInputD67C8C = nullptr; // 原全局：0x00D67C8C。 非拥有CC46E0借用，关闭只清槽/归还焦点。
constexpr wchar_t kLegacyMessageDelete58917C[] = L"刪除此留言。"; // 原常量参考：0x0058917C。
constexpr wchar_t kLegacyMessagePrevious58918C[] = L"前一頁。"; // 原常量参考：0x0058918C。
constexpr wchar_t kLegacyMessageCancel589198[] = L"取消留言。"; // 原常量参考：0x00589198。
constexpr wchar_t kLegacyMessageEmpty5891A4[] = L"請輸入文字。"; // 原常量参考：0x005891A4。
constexpr wchar_t kLegacyMessagePost5891B4[] = L"留言。"; // 原常量参考：0x005891B4。
constexpr wchar_t kLegacyMessageNext553574[] = L"下一頁。"; // 原常量参考：0x00553574。

constexpr wchar_t kLegacySellDecrease588E74[] = L"減少賣掉此物品數量。"; // 原常量参考：0x00588E74。
constexpr wchar_t kLegacySellIncrease588E8C[] = L"增加賣掉此物品數量。"; // 原常量参考：0x00588E8C。
constexpr wchar_t kLegacySellConfirm588EA4[] = L"賣掉指定物品。"; // 原常量参考：0x00588EA4。
constexpr wchar_t kLegacySellCancel588EB4[] = L"不賣了。"; // 原常量参考：0x00588EB4。
constexpr wchar_t kLegacySellLimitLine588ECC[] = L"無法再拿"; // 原常量参考：0x00588ECC。
constexpr wchar_t kLegacySellLimitLine588EC0[] = L"更多的錢了"; // 原常量参考：0x00588EC0。
constexpr wchar_t kLegacySellStopHint588ED8[] = L"不賣。"; // 原常量参考：0x00588ED8。
constexpr wchar_t kLegacySellAcceptHint588EE0[] = L"賣出。"; // 原常量参考：0x00588EE0。
constexpr wchar_t kLegacySellQuestion588EE8[] = L"賣出以上物品。可以嗎？"; // 原常量参考：0x00588EE8。
constexpr wchar_t kLegacySellSummary588F00[] = L"持有金額  {:7} 金幣   賣出金額  {:7} 金幣"; // 原常量参考：0x00588F00。
constexpr wchar_t kLegacySellQuantity588F28[] = L"{}({}×{})"; // 原常量参考：0x00588F28。

constexpr wchar_t kLegacyBuyUnavailable588E20[] = L"×"; // 原常量参考：0x00588E20。
constexpr wchar_t kLegacyBuyCancel588E14[] = L"不買了。"; // 原常量参考：0x00588E14。
constexpr wchar_t kLegacyBuyConfirm588E04[] = L"購買指定物品。"; // 原常量参考：0x00588E04。
constexpr wchar_t kLegacyBuyDecrease588DD4[] = L"減少此物品之購買數量。"; // 原常量参考：0x00588DD4。
constexpr wchar_t kLegacyBuyIncrease588DEC[] = L"增加此物品之購買數量。"; // 原常量参考：0x00588DEC。
constexpr wchar_t kLegacyBuyUnavailableHint586BF4[] = L"＊現在無法裝備"; // 原常量参考：0x00586BF4。
constexpr wchar_t kLegacyBuyQuestion588E34[] = L"購買以上物品。可以嗎？"; // 原常量参考：0x00588E34。
constexpr wchar_t kLegacyBuyAcceptHint588E2C[] = L"購買。"; // 原常量参考：0x00588E2C。
constexpr wchar_t kLegacyBuyStopHint588E24[] = L"不買。"; // 原常量参考：0x00588E24。
constexpr wchar_t kLegacyBuySummary588E4C[] = // 原常量参考：0x00588E4C。
    L"持有金額  {:7} 金幣  購買金額  {:7} 金幣";

// 原CC1A0C／CF4A34／CC4B58：旧文本跨度尾部独立S32状态；文字宽化后各保唯一owner。
std::int32_t gLegacyBuyPageCC1A0C {};
std::int32_t gLegacyStorageTabCF4A34 {};
std::int32_t gLegacyStorageDepositDraftCC4B58 {};
std::int32_t ReadLegacyBuyPageCC1A0C()
{
    return gLegacyBuyPageCC1A0C;
}

void WriteLegacyBuyPageCC1A0C(const std::int32_t page)
{
    gLegacyBuyPageCC1A0C = page;
}

ui::detail::WindowShopItem* BorrowLegacyShopItemCF4FE0(const std::int32_t index)
{
    return &ui::detail::gShopItemsCF4FE0[index];
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
    std::array<char, 29> name; // 原成员偏移：+0x01。
    std::array<std::array<char, 96>, 32> descriptions; // 原成员偏移：+0x1E。
    std::array<std::byte, 2> reservedC1E; // 原成员偏移：+0xC1E。
    std::int32_t image; // 原成员偏移：+0xC20。
    std::int32_t cost; // 原成员偏移：+0xC24。
    std::int32_t selected; // 原成员偏移：+0xC28。
};
static_assert(sizeof(LegacyBarterItemRecord) == 0xC2C);
static_assert(offsetof(LegacyBarterItemRecord, name) == 1);
static_assert(offsetof(LegacyBarterItemRecord, descriptions) == 0x1E);
static_assert(offsetof(LegacyBarterItemRecord, image) == 0xC20);
static_assert(offsetof(LegacyBarterItemRecord, cost) == 0xC24);
static_assert(offsetof(LegacyBarterItemRecord, selected) == 0xC28);
std::array<LegacyBarterItemRecord, 20> gLegacyBarterItemsCA0B30 {}; // 原全局：0x00CA0B30。
LegacyPanelRecord gLegacyBarterPanel5818B0 { // 原全局：0x005818B0。
    0, 0, 576, 432, 75, {23330, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyBarterConfirmPanel581978 { // 原全局：0x00581978。
    0, 36, 640, 384, 76, {}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyBarterActionD67C50 = nullptr; // 原全局：0x00D67C50。
std::int32_t gLegacyBarterImageCF4A0C = 0; // 原全局：0x00CF4A0C。 独立图号，原生产待证。
std::int32_t gLegacyBarterDetailPageD67C54 = 0; // 原全局：0x00D67C54。
std::int32_t gLegacyBarterPreviousHover581974 = -1; // 原全局：0x00581974。
std::int32_t gLegacyBarterPageCB10D0 = 0; // 原全局：0x00CB10D0。
std::int32_t gLegacyBarterPageCountC8A954 = 0; // 原全局：0x00C8A954。 原生产待证，不以初值替代实际页数。
std::int32_t gLegacyBarterTotalD2F334 = 0; // 原全局：0x00D2F334。 旧75统计，旧76借读。
std::int32_t gLegacyBarterMessageCC2E54 = 0; // 原全局：0x00CC2E54。
constexpr wchar_t kLegacyBarterCancel588994[] = L"取消交易物品。"; // 原常量参考：0x00588994。
constexpr wchar_t kLegacyBarterConfirm588984[] = L"交易指定物品。"; // 原常量参考：0x00588984。
constexpr wchar_t kLegacyBarterIncrease58896C[] = L"增加此物品交易數量。"; // 原常量参考：0x0058896C。
constexpr wchar_t kLegacyBarterDecrease588954[] = L"減少此物品交易數量。"; // 原常量参考：0x00588954。


constexpr wchar_t kLegacyBarterQuestion5889C0[] = L"交易以上物品。可以嗎？"; // 原常量参考：0x005889C0。
constexpr wchar_t kLegacyBarterAcceptHint5889B8[] = L"交易。"; // 原常量参考：0x005889B8。
constexpr wchar_t kLegacyBarterStopHint5889AC[] = L"停止交易。"; // 原常量参考：0x005889AC。
constexpr wchar_t kLegacyBarterSummary5889D8[] = // 原常量参考：0x005889D8。
    L"{}  {:7} 個   交換個數  {:7} 個";

LegacyPanelRecord gLegacyExchangePanel581A40 { // 原全局：0x00581A40。
    0, 0, 292, 260, 74, {23050, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyGiveReceivePanel581B08 { // 原全局：0x00581B08。
    0, 0, 292, 260, 83, {23050, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyExchangeActionCF4B5C = nullptr; // 原全局：0x00CF4B5C。
YobiAction* gLegacyGiveReceiveActionCB2228 = nullptr; // 原全局：0x00CB2228。
std::int32_t gLegacyGiveReceiveModeC8AA98 = 0; // 原全局：0x00C8AA98。 独立原状态，非4A0850的receive局部；非零生产待证。
LegacyPanelRecord gLegacyGiveReceiveQuantityPanel581BD0 { // 原全局：0x00581BD0。
    0, 0, 576, 432, 84, {23330, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyGiveConfirmPanel581C98 { // 原全局：0x00581C98。
    0, 0, 448, 240, 85, {0, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyGiveConfirmReceiveFormat588B40[] = LR"(接受了{:>2}　{:2}　個。
可以嗎？)"; // 原常量参考：0x00588B40。
constexpr wchar_t kLegacyGiveConfirmGiveFormat588B60[] = LR"(交出了{:>2}　{:2}　個。
可以嗎？)"; // 原常量参考：0x00588B60。
constexpr wchar_t kLegacyGiveConfirmReceiveHint588B20[] = L"接受了物品。"; // 原常量参考：0x00588B20。
constexpr wchar_t kLegacyGiveConfirmGiveHint588B30[] = L"交出了物品。"; // 原常量参考：0x00588B30。
constexpr wchar_t kLegacyGiveConfirmCancelReceive588B10[] = L"取消接受物品。"; // 原常量参考：0x00588B10。
YobiAction* gLegacyGiveReceiveQuantityActionCB1DD4 = nullptr; // 原全局：0x00CB1DD4。
std::int32_t gLegacyGiveReceiveImageD2D814 = 0; // 原全局：0x00D2D814。 独立图号，生产待证，不借现代D2DAD0。
std::int32_t gLegacyGiveReceiveDetailPageD2EAB0 = 0; // 原全局：0x00D2EAB0。
constexpr wchar_t kLegacyExchangeChoice588A40[] = L" 想 交 易"; // 原常量参考：0x00588A40。
constexpr wchar_t kLegacyExchangeCancel588A34[] = L"   沒 事"; // 原常量参考：0x00588A34。
constexpr wchar_t kLegacyExchangeHint588A20[] = L"與此店交易物品。"; // 原常量参考：0x00588A20。
constexpr wchar_t kLegacyExchangeCancelHint588A10[] = L"不想交易物品。"; // 原常量参考：0x00588A10。
constexpr wchar_t kLegacyGiveReceiveCancel588A70[] = L"　　取消　　"; // 原常量参考：0x00588A70。
constexpr wchar_t kLegacyGiveChoice588A64[] = L"　交出物品"; // 原常量参考：0x00588A64。
constexpr wchar_t kLegacyReceiveChoice585A40[] = L"接受物品"; // 原常量参考：0x00585A40。
constexpr wchar_t kLegacyGiveHint588A58[] = L"交出物品。"; // 原常量参考：0x00588A58。
constexpr wchar_t kLegacyGiveReceiveCancelHint588A4C[] = L"取消交易。"; // 原常量参考：0x00588A4C。
constexpr wchar_t kLegacyQuantityUnit5889A4[] = L"個"; // 原常量参考：0x005889A4。
constexpr wchar_t kLegacyQuantityIncreaseLimit588ABC[] = L"無法再增加。"; // 原常量参考：0x00588ABC。
constexpr wchar_t kLegacyReceiveIncrease588AA8[] = L"增加接受的數量。"; // 原常量参考：0x00588AA8。
constexpr wchar_t kLegacyGiveIncrease588ACC[] = L"增加交出的數量。"; // 原常量参考：0x00588ACC。
constexpr wchar_t kLegacyQuantityDecreaseLimit588944[] = L"無法再減少。"; // 原常量参考：0x00588944。
constexpr wchar_t kLegacyReceiveDecrease588A80[] = L"減少接受的數量。"; // 原常量参考：0x00588A80。
constexpr wchar_t kLegacyGiveDecrease588A94[] = L"減少交出的數量。"; // 原常量参考：0x00588A94。
constexpr wchar_t kLegacyReceiveCancel588AEC[] = L"取消接收物品。"; // 原常量参考：0x00588AEC。
constexpr wchar_t kLegacyGiveCancel588AFC[] = L"取消交出物品。"; // 原常量参考：0x00588AFC。
constexpr wchar_t kLegacyReceiveHint588AE0[] = L"接受物品。"; // 原常量参考：0x00588AE0。

// 旧74/83完全同构的标题及五行正文；每次绘制后仍重读面板坐标。
void DrawLegacySmallNpcBody(const LegacyPanelRecord* const panel)
{
    const auto* const lines = ui::detail::WindowDialog_BorrowNpcLines();
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

LegacyPanelRecord gLegacyAlbumDetailPanel5846C0 { // 原全局：0x005846C0。
    0, 0, 272, 438, 24, {22890, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyAlbumListPanel584788 { // 原全局：0x00584788。
    0, 0, 272, 438, 23, {22880, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyAlbumSelectionC8AF7C = 0; // 原全局：0x00C8AF7C。
std::int32_t gLegacyAlbumPageC8B1B0 = 0; // 原全局：0x00C8B1B0。
std::int32_t gLegacyAlbumReturningCC40D0 = 0; // 原全局：0x00CC40D0。
YobiAction* gLegacyAlbumActionD67D14 = nullptr; // 原全局：0x00D67D14。
constexpr wchar_t kLegacyAlbumSkipTen58A0F0[] = L"跳過10頁。"; // 原常量参考：0x0058A0F0。
constexpr wchar_t kLegacyAlbumBack58A0FC[] = L"回到圖鑑視窗。"; // 原常量参考：0x0058A0FC。
constexpr wchar_t kLegacyAlbumOpen58A114[] = L"打開此寵物的圖鑑視窗。"; // 原常量参考：0x0058A114。

// 原旧20列表面板及两帧展开标志；4F7140初始化，4F7150/4F7870共享。
LegacyPanelRecord gLegacyPetListPanel584850 { // 原全局：0x00584850。
    0, 0, 272, 408, 20, {22840, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int16_t gLegacyPetStateOpenD67D18 = 0; // 原全局：0x00D67D18。
std::int16_t gLegacyPetStatePreviousD2D48C = 0; // 原全局：0x00D2D48C。
// 原旧20状态选择器的独立S16待提交数据；五物理槽，与现代窗口状态无别名。
std::array<std::int16_t, 5> gLegacyPetStateValuesCF4E38 {}; // 原全局：0x00CF4E38。
std::int16_t gLegacyPetStandbyCountCC4D18 = 0; // 原全局：0x00CC4D18。
std::int16_t gLegacyPetBattleCountD45F8C = 0; // 原全局：0x00D45F8C。 不属于D45F78金额文本或D45F88编号。
constexpr wchar_t kLegacyPetStateConfirm58A1F0[] = L"決定寵物的狀態。"; // 原常量参考：0x0058A1F0。
constexpr wchar_t kLegacyPetStateCancel58A1D8[] = L"停止寵物的狀態選擇。"; // 原常量参考：0x0058A1D8。
constexpr wchar_t kLegacyPetStateBattle586C40[] = L"使這隻寵物處於戰鬥狀態。"; // 原常量参考：0x00586C40。
constexpr wchar_t kLegacyPetStateBattleCurrent586C5C[] = L"這隻寵物正處於戰鬥狀態。"; // 原常量参考：0x00586C5C。
constexpr wchar_t kLegacyPetStateStandby586C78[] = L"使這隻寵物處於待命狀態。"; // 原常量参考：0x00586C78。
constexpr wchar_t kLegacyPetStateStandbyCurrent586C94[] = L"這隻寵物正處於待命狀態。"; // 原常量参考：0x00586C94。
constexpr wchar_t kLegacyPetStateRestCurrent58A140[] = L"這隻寵物正處於休息狀態"; // 原常量参考：0x0058A140。
constexpr wchar_t kLegacyPetStateRest58A158[] = L"使這隻寵物處於休息狀態。"; // 原常量参考：0x0058A158。
constexpr wchar_t kLegacyPetStateStandbyRejected58A174[] = L"無法使這隻寵物處於待命狀態。"; // 原常量参考：0x0058A174。
constexpr wchar_t kLegacyPetStateBattleRejected58A194[] = L"無法這隻寵物處於戰鬥狀態。"; // 原常量参考：0x0058A194。
constexpr wchar_t kLegacyPetStateSwap58A1B0[] = L"把這隻寵物和設為參戰狀態的寵物替換。"; // 原常量参考：0x0058A1B0。
constexpr std::array<std::int32_t, 10> kLegacyPetTribeImages57CB2C { // 原常量参考：0x0057CB2C。
    22865, 22866, 22867, 22868, 22869, 22870, 22871, 22872, 22873, 22874,
};
constexpr wchar_t kLegacyPetOpenDetails586D0C[] = L"打開此寵物的能力視窗。"; // 原常量参考：0x00586D0C。
constexpr wchar_t kLegacyPetChooseState58A12C[] = L"指定寵物的狀態。"; // 原常量参考：0x0058A12C。

constexpr wchar_t kLegacyPetBasicTab585958[] = L"切換至此寵物的能力視窗。"; // 原常量参考：0x00585958。
constexpr wchar_t kLegacyPetAttributeTab58593C[] = L"切換至此寵物的詳細視窗。"; // 原常量参考：0x0058593C。
constexpr wchar_t kLegacyPetSkillTab585920[] = L"切換至此寵物的技能視窗。"; // 原常量参考：0x00585920。
constexpr wchar_t kLegacyPetBack58A204[] = L"回到寵物視窗。"; // 原常量参考：0x0058A204。
constexpr wchar_t kLegacyPetRename586D38[] = L"變更寵物名字。"; // 原常量参考：0x00586D38。
constexpr wchar_t kLegacyPetLevelLimit58A214[] = L"   ---   "; // 原常量参考：0x0058A214。
constexpr wchar_t kLegacyPetVitality58A288[] = L"將點數分配至生命。"; // 原常量参考：0x0058A288。
constexpr wchar_t kLegacyPetToughness58A260[] = L"將點數分配至防禦。"; // 原常量参考：0x0058A260。
constexpr wchar_t kLegacyPetMagic58A238[] = L"將點數分配至魔法。"; // 原常量参考：0x0058A238。
LegacyPanelRecord gLegacyPetDetailPanel584918 { // 原全局：0x00584918。
    0, 0, 272, 438, 21, {22800, 22810, 22824, 0, 0},
    {{{1, 6, 415, 78, 17, kLegacyPetBasicTab585958},
      {1, 97, 415, 78, 17, kLegacyPetAttributeTab58593C},
      {1, 187, 415, 78, 17, kLegacyPetSkillTab585920}, {}, {}}},
    0, 0, {}, {},
};
std::int32_t gLegacyPetSelectionD67ADC = 0; // 原全局：0x00D67ADC。 旧20生产，旧21/22共用位置，不是PetPair中的槽号。
YobiAction* gLegacyPetDetailActionD67AE0 = nullptr; // 原全局：0x00D67AE0。
LegacyPanelRecord gLegacyPetRenamePanel5849E0 { // 原全局：0x005849E0。
    0, 0, 272, 200, 22, {22809, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
input::InputContext* gLegacyPetRenameInputD67D1C = nullptr; // 原全局：0x00D67D1C。 借用独立D2DFD0，不复用现代宠物输入。

bool HandleLegacyPetNavigation(const LegacyPanelButtonRecord* const back,
    const LegacyPanelButtonRecord* const previous, const LegacyPanelButtonRecord* const next)
{
    if ((QueryPanelButtonState(back) & 1) != 0) {
        static_cast<void>(ui::Sub_5042D0(gLegacyPetDetailPanel584918.id));
        static_cast<void>(ui::Sub_504180(20));
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
LegacyPanelRecord gLegacyTradePanel582F00 { // 原全局：0x00582F00。
    40, 0, 560, 364, 52, {23230, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr std::array<std::array<std::int32_t, 4>, 2> kLegacyTradeGrid582ECC {{ // 原常量参考：0x00582ECC。
    {13, 94, 60, 141}, {291, 94, 338, 141},
}};
std::int32_t gLegacyTradeSplitSourceD45F90 = 0; // 原全局：0x00D45F90。
std::int32_t gLegacyTradeSplitTargetD2D668 = 0; // 原全局：0x00D2D668。
std::int32_t gLegacyTradeSplitAmountD2D490 = 0; // 原全局：0x00D2D490。
std::int32_t gLegacyTradeMouseXCF3FEC = 0; // 原全局：0x00CF3FEC。
std::int32_t gLegacyTradeMouseYCF3FE8 = 0; // 原全局：0x00CF3FE8。
std::int32_t gLegacyTradeDetailPageD67CC8 = 0; // 原全局：0x00D67CC8。
std::int32_t gLegacyTradePrevious582FCC = -1; // 原全局：0x00582FCC。
constexpr std::array<std::array<std::int32_t, 2>, 2> kLegacyTradePetRows582EEC {{ // 原常量参考：0x00582EEC。
    {70, 123}, {348, 123},
}};
std::int32_t gLegacyTradeRemoteItemPrevious582FC4 = -1; // 原全局：0x00582FC4。
std::int32_t gLegacyTradeRemotePetPrevious582FC8 = -1; // 原全局：0x00582FC8。
std::int32_t gLegacyTradeLocalPetPrevious582FD0 = -1; // 原全局：0x00582FD0。
std::int32_t gLegacyTradeRemoteItemPageD67CC0 = 0; // 原全局：0x00D67CC0。
std::int32_t gLegacyTradeRemotePetPageD67CC4 = 0; // 原全局：0x00D67CC4。
std::int32_t gLegacyTradeLocalPetPageD67CCC = 0; // 原全局：0x00D67CCC。
YobiAction* gLegacyTradePetActionD67CB4 = nullptr; // 原全局：0x00D67CB4。 旧52外层创建/关闭，属性页借用。
constexpr wchar_t kLegacyTradeAvailable589524[] = L"此物品可以拿來交易。"; // 原常量参考：0x00589524。
constexpr wchar_t kLegacyTradeUnavailable58953C[] = L"此物品不能拿來交易。"; // 原常量参考：0x0058953C。
constexpr wchar_t kLegacyTradeSelected589554[] = L"把此物品拿來交易。"; // 原常量参考：0x00589554。
constexpr wchar_t kLegacyTradeRemoteItem5894DC[] = L"這是對方想交易的物品。"; // 原常量参考：0x005894DC。
constexpr wchar_t kLegacyTradeRemotePet5894F4[] = L"這是對方想交易的寵物。"; // 原常量参考：0x005894F4。
constexpr wchar_t kLegacyTradeRemoteMoney58950C[] = L"這是對方想交易的金額。"; // 原常量参考：0x0058950C。
constexpr wchar_t kLegacyTradePetAvailable589568[] = L"此寵物可以拿來交易。"; // 原常量参考：0x00589568。
constexpr wchar_t kLegacyTradePetSelected589580[] = L"把此寵物拿來交易。"; // 原常量参考：0x00589580。
constexpr wchar_t kLegacyTradeChoosing5894CC[] = L"選取交易物品中"; // 原常量参考：0x005894CC。
constexpr wchar_t kLegacyTradePublished5894B8[] = L"交易物品選取完成"; // 原常量参考：0x005894B8。
constexpr wchar_t kLegacyTradeConfirming5894AC[] = L"確認交易中"; // 原常量参考：0x005894AC。
constexpr wchar_t kLegacyTradeConfirmSelected589484[] = L"現在可以交易。"; // 原常量参考：0x00589484。
constexpr wchar_t kLegacyTradeConfirm589478[] = L"進行交易。"; // 原常量参考：0x00589478。

constexpr wchar_t kLegacyTradeRemoteItemTab589458[] = L"切換至交易對手的物品欄那一頁。"; // 原常量参考：0x00589458。
constexpr wchar_t kLegacyTradeRemotePetTab589438[] = L"切換至交易對手的寵物欄那一頁。"; // 原常量参考：0x00589438。
constexpr wchar_t kLegacyTradeRemoteMoneyTab589418[] = L"切換至交易對手的金錢欄那一頁。"; // 原常量参考：0x00589418。
constexpr wchar_t kLegacyTradeLocalItemTab589400[] = L"切換至物品欄那一頁。"; // 原常量参考：0x00589400。
constexpr wchar_t kLegacyTradeLocalPetTab5893E8[] = L"切換至寵物欄那一頁。"; // 原常量参考：0x005893E8。
constexpr wchar_t kLegacyTradeLocalMoneyTab5893D0[] = L"切換至金錢欄那一頁。"; // 原常量参考：0x005893D0。
constexpr wchar_t kLegacyTradePublishItems5893B4[] = L"把選擇的物品拿給對方看。"; // 原常量参考：0x005893B4。
constexpr wchar_t kLegacyTradePublishPets589398[] = L"把選擇的寵物拿給對方看。"; // 原常量参考：0x00589398。
constexpr wchar_t kLegacyTradePublishMoney589380[] = L"把指定金額拿給對方看。"; // 原常量参考：0x00589380。
constexpr wchar_t kLegacyTradePetCount58949C[] = L"{}隻"; // 原常量参考：0x0058949C。
constexpr wchar_t kLegacyTradeItemCount5894A4[] = L"{:2}個"; // 原常量参考：0x005894A4。
constexpr wchar_t kLegacyTradeMoney589494[] = L"{:7}金幣"; // 原常量参考：0x00589494。

ui::detail::WindowTradeItem* BorrowLegacyTradeItem(const std::int32_t side, const std::int32_t item)
{
    return &ui::detail::gTradeItemsD460D0[side * 20 + item];
}

ui::detail::WindowTradePet* BorrowLegacyTradePet(const std::int32_t side, const std::int32_t pet)
{
    return &ui::detail::gTradePetsC9C108[side * 5 + pet];
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
LegacyPanelRecord gLegacyStoragePanel582B70 { // 原全局：0x00582B70。
    40, 16, 560, 364, 72, {23340, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr std::array<std::array<std::int32_t, 4>, 2> kLegacyStorageGrid582C34 {{ // 原常量参考：0x00582C34。
    {13, 94, 60, 141}, {291, 94, 338, 141},
}};
std::int32_t gLegacyStoragePreviousLeft582C64 = -1; // 原全局：0x00582C64。
std::int32_t gLegacyStoragePreviousRight582C68 = -1; // 原全局：0x00582C68。
std::int32_t gLegacyStorageSplitModeCA0AA8 = 0; // 原全局：0x00CA0AA8。
std::int32_t gLegacyStorageSplitSourceD3FACC = 0; // 原全局：0x00D3FACC。
std::int32_t gLegacyStorageSplitTargetCC4D38 = 0; // 原全局：0x00CC4D38。
std::int32_t gLegacyStorageSplitAmountCB2220 = 0; // 原全局：0x00CB2220。
std::int32_t gLegacyStorageDragSlotCC4C98 = 0; // 原全局：0x00CC4C98。
std::int32_t gLegacyStorageDragImageCB09D4 = 0; // 原全局：0x00CB09D4。
std::int32_t gLegacyStorageDetailPageD67C94 = 0; // 原全局：0x00D67C94。
std::int32_t gLegacyStorageMessageD67C98 = 0; // 原全局：0x00D67C98。
std::uint32_t gLegacyStorageMessageTickD67C9C = 0; // 原全局：0x00D67C9C。
std::int32_t gLegacyStorageEmptySlotsCF49BC = 0; // 原全局：0x00CF49BC。
std::int32_t gLegacyStorageTakeCountCF4A04 = 0; // 原全局：0x00CF4A04。
std::int32_t gLegacyStoragePutCountD2DAD4 = 0; // 原全局：0x00D2DAD4。
std::int32_t gLegacyStorageCapacityD30404 = 0; // 原全局：0x00D30404。
std::int32_t gLegacyStorageFeeD40B04 = 0; // 原全局：0x00D40B04。
std::int32_t gLegacyStorageMoneyD2F79C = 0; // 原全局：0x00D2F79C。
std::int32_t gLegacyStorageWithdrawDraftD2EA98 = 0; // 原全局：0x00D2EA98。
std::int32_t gLegacyStorageWithdrawAmountCF49B8 = 0; // 原全局：0x00CF49B8。
std::int32_t gLegacyStorageDepositMessageD67CAC = 0; // 原全局：0x00D67CAC。
std::uint32_t gLegacyStorageDepositTickD67CB0 = 0; // 原全局：0x00D67CB0。
constexpr std::array<std::array<std::int32_t, 2>, 2> kLegacyStoragePetRows582C54 {{ // 原常量参考：0x00582C54。
    {70, 123}, {348, 123},
}};
std::int32_t gLegacyStoragePetPreviousLeft582C6C = -1; // 原全局：0x00582C6C。
std::int32_t gLegacyStoragePetPreviousRight582C70 = -1; // 原全局：0x00582C70。
std::int32_t gLegacyStoragePetPageD67CA0 = 0; // 原全局：0x00D67CA0。
std::int32_t gLegacyStoragePetMessageD67CA4 = 0; // 原全局：0x00D67CA4。
std::uint32_t gLegacyStoragePetTickD67CA8 = 0; // 原全局：0x00D67CA8。
std::int32_t gLegacyStoragePetPutCountD2D600 = 0; // 原全局：0x00D2D600。
std::int32_t gLegacyStoragePetTakeCountD2D824 = 0; // 原全局：0x00D2D824。
std::int32_t gLegacyStoragePetCapacityCB1DDC = 0; // 原全局：0x00CB1DDC。
YobiAction* gLegacyStoragePetActionD67C90 = nullptr; // 原全局：0x00D67C90。
constexpr wchar_t kLegacyStorageNoMoney58906C[] = L"你的錢不夠！"; // 原常量参考：0x0058906C。
constexpr wchar_t kLegacyStorageFee589284[] = L"手續費"; // 原常量参考：0x00589284。
constexpr wchar_t kLegacyStorageGold589278[] = L"持有金額"; // 原常量参考：0x00589278。
constexpr wchar_t kLegacyStorageAmount588FC4[] = L"{:7} 金幣"; // 原常量参考：0x00588FC4。
constexpr wchar_t kLegacyStorageTake589268[] = L"取出此物品。"; // 原常量参考：0x00589268。
constexpr wchar_t kLegacyStorageFull589230[] = L"無法再存放物品。"; // 原常量参考：0x00589230。
constexpr wchar_t kLegacyStorageCanPut589244[] = L"此物品可以存放。"; // 原常量参考：0x00589244。
constexpr wchar_t kLegacyStoragePut589258[] = L"存放此物品。"; // 原常量参考：0x00589258。
constexpr wchar_t kLegacyStorageCannotPut5878BC[] = L"此物品無法存放。"; // 原常量参考：0x005878BC。
constexpr wchar_t kLegacyStorageClear5859FC[] = L"把指定金額歸零。"; // 原常量参考：0x005859FC。
constexpr wchar_t kLegacyStorageAmountLimit5892D4[] = L"無法指定更多金額。"; // 原常量参考：0x005892D4。
constexpr wchar_t kLegacyStorageDeposit5892F4[] = L"存入指定金額。"; // 原常量参考：0x005892F4。
constexpr wchar_t kLegacyStorageCannotDeposit5892E8[] = L"無法存入。"; // 原常量参考：0x005892E8。
constexpr wchar_t kLegacyStorageBalance589314[] = L"存款"; // 原常量参考：0x00589314。
constexpr wchar_t kLegacyStorageWithdraw589304[] = L"取出指定金額。"; // 原常量参考：0x00589304。
constexpr wchar_t kLegacyStoragePetTake5892C4[] = L"取出這隻寵物。"; // 原常量参考：0x005892C4。
constexpr wchar_t kLegacyStoragePetPut5892B4[] = L"存放這隻寵物。"; // 原常量参考：0x005892B4。
constexpr wchar_t kLegacyStoragePetCanPut5892A0[] = L"此寵物可以存放。"; // 原常量参考：0x005892A0。
constexpr wchar_t kLegacyStoragePetFull58928C[] = L"無法再存放寵物。"; // 原常量参考：0x0058928C。
constexpr wchar_t kLegacyStorageItemTab589214[] = L"物品存放與取出。"; // 原常量参考：0x00589214。
constexpr wchar_t kLegacyStoragePetTab589200[] = L"寵物存放與取出。"; // 原常量参考：0x00589200。
constexpr wchar_t kLegacyStorageDepositTab5891F8[] = L"存錢。"; // 原常量参考：0x005891F8。
constexpr wchar_t kLegacyStorageWithdrawTab5891F0[] = L"領錢。"; // 原常量参考：0x005891F0。
constexpr wchar_t kLegacyStorageConfirm5891C4[] = L"確定。"; // 原常量参考：0x005891C4。
constexpr wchar_t kLegacyStorageCancel5891BC[] = L"取消。"; // 原常量参考：0x005891BC。
constexpr wchar_t kLegacyStorageItemCount5891D4[] = L"{:2} 個"; // 原常量参考：0x005891D4。
constexpr wchar_t kLegacyStoragePetCount5891CC[] = L"{} 隻"; // 原常量参考：0x005891CC。

ui::detail::WindowStorageItem* BorrowLegacyStorageItem(const std::int32_t side, const std::int32_t item)
{
    return &ui::detail::gItemDetailStorageCC4D58[side].items[item];
}

ui::detail::WindowStoragePetSummary* BorrowLegacyStoragePet(const std::int32_t side, const std::int32_t pet)
{
    return &ui::detail::gItemDetailStorageCC4D58[side].pets[pet];
}

// 原旧19唯一585640面板；5856E0借同一selected，非列表页签副本。
LegacyPanelRecord gLegacySkillDetail585640 { // 原全局：0x00585640。
    0, 0, 272, 448, 19, {22770, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacySkillDetailNoFp58A9D4[] = L"魔法不足。"; // 原常量参考：0x0058A9D4。
constexpr wchar_t kLegacySkillDetailNoSpace58A9C4[] = L"物品欄滿了。"; // 原常量参考：0x0058A9C4。
constexpr wchar_t kLegacySkillDetailHiddenFp58A9BC[] = L"????"; // 原常量参考：0x0058A9BC。
constexpr wchar_t kLegacySkillDetailUse58A9AC[] = L"使用這個技能。"; // 原常量参考：0x0058A9AC。
constexpr wchar_t kLegacySkillDetailBack554088[] = L"回到技能視窗。"; // 原常量参考：0x00554088。
constexpr wchar_t kLegacySkillDetailUnavailable58A994[] = L"現在可以進行精靈變身。"; // 原常量参考：0x0058A994。
constexpr wchar_t kLegacySkillDetailTransform554104[] = L"進行精靈變身。"; // 原常量参考：0x00554104。
constexpr wchar_t kLegacySkillDetailRelease554114[] = L"解除精靈變身。"; // 原常量参考：0x00554114。
constexpr wchar_t kLegacySkillClose55331C[] = // 原常量参考：0x0055331C。
    L"關閉這個視窗。";
constexpr wchar_t kLegacySkillOpen5864C8[] = // 原常量参考：0x005864C8。
    L"打開這個技能的技能視窗。";
constexpr wchar_t kLegacyAttributeLimit58A220[] = // 原常量参考：0x0058A220。
    L"已經不能再分配點數了。";
constexpr wchar_t kLegacyAttributeHint58AA08[] = // 原常量参考：0x0058AA08。
    L"將點數分配至體力。";
constexpr wchar_t kLegacyAttributeHint58A274[] = // 原常量参考：0x0058A274。
    L"將點數分配至力量。";
constexpr wchar_t kLegacyAttributeHint58A9F4[] = // 原常量参考：0x0058A9F4。
    L"將點數分配至強度。";
constexpr wchar_t kLegacyAttributeHint58A24C[] = // 原常量参考：0x0058A24C。
    L"將點數分配至速度。";
constexpr wchar_t kLegacyAttributeHint58A9E0[] = // 原常量参考：0x0058A9E0。
    L"將點數分配至魔力。";
constexpr wchar_t kLegacyProfileTab5858D8[] = // 原常量参考：0x005858D8。
    L"切換成玩家狀態視窗。";
constexpr wchar_t kLegacyProfileTab5858C0[] = // 原常量参考：0x005858C0。
    L"切換成玩家明細視窗。";
constexpr wchar_t kLegacyProfileTab5858A8[] = // 原常量参考：0x005858A8。
    L"切換成玩家稱號視窗。";
constexpr wchar_t kLegacyProfileRear586344[] = // 原常量参考：0x00586344。
    L"把戰鬥時的位置變更為後排。";
constexpr wchar_t kLegacyProfileFront586360[] = // 原常量参考：0x00586360。
    L"把戰鬥時的位置變更為前排。";
constexpr std::array<std::array<std::uint32_t, 2>, 4> kLegacyAttributeBars57CBB8 {{ // 原常量参考：0x0057CBB8。
    {250, 2}, {254, 6}, {249, 1}, {251, 3},
}};
std::uint32_t gLegacyProfilePositionTickD68860 = 0; // 原全局：0x00D68860。
input::InputContext* gLegacyProfileInputD6885C = nullptr; // 原全局：0x00D6885C。
std::int32_t gLegacyProfilePreviousPage585894 = -1; // 原全局：0x00585894。
std::uint32_t gLegacyProfileEditTickD68864 = 0; // 原全局：0x00D68864。
std::int8_t gLegacyTitlePageCF49B4 = 0; // 原全局：0x00CF49B4。
std::int8_t gLegacyTitlePageCountC8AF80 = 0; // 原全局：0x00C8AF80。
std::int8_t gLegacyTitleDeleteCF4678 = 0; // 原全局：0x00CF4678。
LegacyPanelRecord gLegacyMapPanelD68868 {}; // 原全局：0x00D68868。
std::int32_t gLegacyMapDefaultX57CB24 = 392; // 原全局：0x0057CB24。
std::int32_t gLegacyMapDefaultYD67AD4 = 0; // 原全局：0x00D67AD4。
constexpr wchar_t kLegacyMapEast58AA54[] = L"東 {:3}"; // 原常量参考：0x0058AA54。
constexpr wchar_t kLegacyMapSouth58AA4C[] = L"南 {:3}"; // 原常量参考：0x0058AA4C。
LegacyPanelRecord gLegacyPointResultPanel5838A8 { // 原全局：0x005838A8。
    236, 152, 168, 152, 41, {23385, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyBattleResultPanel583970 { // 原全局：0x00583970。
    12, 74, 616, 308, 40, {23380, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::uint32_t gLegacyPointResultTickCC3E74 = 0; // 原全局：0x00CC3E74。
std::uint32_t gLegacyBattleResultTickD3FAB8 = 0; // 原全局：0x00D3FAB8。
// 旧选择39；结果数据借窗口12，独立开关/时间槽不能套用其现代窗口状态机。
LegacyPanelRecord gLegacyGatherResultPanel583A38 { // 原全局：0x00583A38。
    0, 0, 272, 440, 39, {23353, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGatherRepeat583AFC = 1; // 原全局：0x00583AFC。
std::uint32_t gLegacyGatherResultTickCA0AB8 = 0; // 原全局：0x00CA0AB8。
YobiAction* gLegacyGatherResultActionD67CD4 = nullptr; // 原全局：0x00D67CD4。
constexpr wchar_t kLegacyGatherNoFp5864F0[] = L"魔力不足。"; // 原常量参考：0x005864F0。
constexpr wchar_t kLegacyGatherNoSpace589868[] = L"物品欄已滿。"; // 原常量参考：0x00589868。
constexpr wchar_t kLegacyGatherLimit589858[] = L"   --   "; // 原常量参考：0x00589858。
constexpr wchar_t kLegacyGatherStop589848[] = L"將再執行一次。"; // 原常量参考：0x00589848。
constexpr wchar_t kLegacyGatherStart58983C[] = L"再執行一次"; // 原常量参考：0x0058983C。
LegacyPanelRecord gLegacySkillResultPanel583BC8 { // 原全局：0x00583BC8。
    0, 0, 272, 440, 37, {23356, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacySkillResultActionD67CDC = nullptr; // 原全局：0x00D67CDC。
LegacyPanelRecord gLegacyFpNoticePanel583E20 { // 原全局：0x00583E20。
    160, 180, 320, 96, 34, {}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacySkillRetry589A94[] = // 原常量参考：0x00589A94。
    L"再使用此技能一次。";
constexpr wchar_t kLegacyFpConfirm55C5D4[] = L"確定"; // 原常量参考：0x0055C5D4。
constexpr wchar_t kLegacyFpNotice589BFC[] = L"魔力不夠。"; // 原常量参考：0x00589BFC。
constexpr wchar_t kLegacyFpRequired589BE8[] = // 原常量参考：0x00589BE8。
    L"需要{}點的魔力值。";
LegacyPanelRecord gLegacyItemWorkResultPanel584140 { // 原全局：0x00584140。
    0, 0, 272, 440, 33, {23353, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyItemWorkResultActionD67CE0 = nullptr; // 原全局：0x00D67CE0。
LegacyPanelRecord gLegacyItemWorkNamePanel584208 { // 原全局：0x00584208。
    368, 0, 272, 104, 32, {23355, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyItemWorkPanel5842D0 { // 原全局：0x005842D0。
    368, 0, 272, 440, 31, {23354, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
input::InputContext* gLegacyItemWorkInputD67CE4 = nullptr; // 原全局：0x00D67CE4。
std::wstring gLegacyItemWorkDraftCC31E0; // 原全局：0x00CC31E0。 仅已证NUL复制合同，不猜原固定容量。
std::int32_t gLegacyItemWorkInitGateD67CE8 = 0; // 原全局：0x00D67CE8。 只证W32测试/返回，尚无地址用途。
std::int32_t gLegacyItemWorkButtonKindD67CF4 = 0; // 原全局：0x00D67CF4。
std::int32_t gLegacyItemWorkPhaseCC4220 = 0; // 原全局：0x00CC4220。
YobiAction* gLegacyItemWorkActionD67CEC = nullptr; // 原全局：0x00D67CEC。
std::int32_t gLegacyItemWorkHover584398 = -1; // 原全局：0x00584398。
std::int32_t gLegacyItemWorkPageD67CF8 = 0; // 原全局：0x00D67CF8。
std::uint32_t gLegacyItemWorkTickD2E12C = 0; // 原全局：0x00D2E12C。
std::uint32_t gLegacyItemWorkDurationCC3EDC = 0; // 原全局：0x00CC3EDC。
std::wstring gLegacyItemWorkMessageCA0ACC; // 原全局：0x00CA0ACC。 原NUL格式化/分行合同，未知阶段保留旧文本。
constexpr wchar_t kItemWorkText589F84[] = L"請選擇要刻印的物品。"; // 原常量参考：0x00589F84。
constexpr wchar_t kItemWorkText589F6C[] = L"請選擇要鑑定的物品。"; // 原常量参考：0x00589F6C。
constexpr wchar_t kItemWorkText589F54[] = L"請選擇要修理的物品。"; // 原常量参考：0x00589F54。
constexpr wchar_t kItemWorkText589F40[] = L"請選擇對象物品。"; // 原常量参考：0x00589F40。
constexpr wchar_t kItemWorkText589F2C[] = L"需要{}點的魔力。"; // 原常量参考：0x00589F2C。
constexpr wchar_t kItemWorkText589F1C[] = L"已經鑑定過了。"; // 原常量参考：0x00589F1C。
constexpr wchar_t kItemWorkText589F04[] = L"未經鑑定物品無法修理。"; // 原常量参考：0x00589F04。
constexpr wchar_t kItemWorkText589EF4[] = L"已經刻印過了。"; // 原常量参考：0x00589EF4。
constexpr wchar_t kItemWorkText589EDC[] = LR"(

      修理中......)"; // 原常量参考：0x00589EDC。
constexpr wchar_t kItemWorkText589EC4[] = LR"(

      鑑定中......)"; // 原常量参考：0x00589EC4。
constexpr wchar_t kItemWorkText589EAC[] = LR"(

      刻印中......)"; // 原常量参考：0x00589EAC。
constexpr wchar_t kItemWorkText589E94[] = L"無法以該技能進行修理。"; // 原常量参考：0x00589E94。
constexpr wchar_t kItemWorkText589E80[] = L"無法修理此物品。"; // 原常量参考：0x00589E80。
constexpr wchar_t kItemWorkText589E58[] = L"該物品以現在的技能等級是無法鑑定的。"; // 原常量参考：0x00589E58。
constexpr wchar_t kItemWorkText589E30[] = L"該物品以現在的技能等級是無法修理的。"; // 原常量参考：0x00589E30。
constexpr wchar_t kItemWorkText589E1C[] = L"無法刻印此物品。"; // 原常量参考：0x00589E1C。
constexpr wchar_t kItemWorkText589E04[] = L"無法刻印未鑑定的物品。"; // 原常量参考：0x00589E04。
constexpr wchar_t kItemWorkText589DF4[] = L"刻印所選物品。"; // 原常量参考：0x00589DF4。
constexpr wchar_t kItemWorkText589DE0[] = L"請選擇刻印物品。"; // 原常量参考：0x00589DE0。
constexpr wchar_t kItemWorkText589DD0[] = L"鑑定所選物品。"; // 原常量参考：0x00589DD0。
constexpr wchar_t kItemWorkText589DBC[] = L"請選擇鑑定物品。"; // 原常量参考：0x00589DBC。
constexpr wchar_t kItemWorkText589DAC[] = L"修理所選物品。"; // 原常量参考：0x00589DAC。
constexpr wchar_t kItemWorkText589D98[] = L"請選擇修理物品。"; // 原常量参考：0x00589D98。
constexpr wchar_t kItemWorkText589D88[] = L"刻印此物品。"; // 原常量参考：0x00589D88。
constexpr wchar_t kItemWorkText589D74[] = L"此物品無法刻印。"; // 原常量参考：0x00589D74。
constexpr wchar_t kItemWorkText589D54[] = L"以現在的技能等級是無法鑑定的。"; // 原常量参考：0x00589D54。
constexpr wchar_t kItemWorkText589D44[] = L"鑑定該物品。"; // 原常量参考：0x00589D44。
constexpr wchar_t kItemWorkText589D30[] = L"此物品已鑑定過了。"; // 原常量参考：0x00589D30。
constexpr wchar_t kItemWorkText589D10[] = L"未經鑑定的物品是無法修理的。"; // 原常量参考：0x00589D10。
constexpr wchar_t kItemWorkText589024[] = L"修理此物品。"; // 原常量参考：0x00589024。
LegacyPanelRecord gLegacyCraftResultPanel5843A0 { // 原全局：0x005843A0。
    0, 0, 272, 440, 28, {23353, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyCraftResultActionD67CFC = nullptr; // 原全局：0x00D67CFC。
LegacyPanelRecord gLegacyCraftExecutionPanel584530 { // 原全局：0x00584530。
    0, 0, 344, 380, 26, {23351, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyCraftExecutionActionD67D08 = nullptr; // 原全局：0x00D67D08。
std::uint32_t gLegacyCraftExecutionTickD2F78C = 0; // 原全局：0x00D2F78C。
std::uint32_t gLegacyCraftExecutionDurationD2DC08 = 0; // 原全局：0x00D2DC08。
constexpr wchar_t kLegacyCraftMissing58A098[] = L"材料未被登錄。"; // 原常量参考：0x0058A098。
constexpr wchar_t kLegacyCraftExecute58A0A8[] = L"將此材料合成。"; // 原常量参考：0x0058A0A8。
constexpr wchar_t kLegacyCraftBack58A084[] = L"回到上一個視窗。"; // 原常量参考：0x0058A084。
LegacyPanelRecord gLegacyCraftMaterialsPanel584468 { // 原全局：0x00584468。
    368, 0, 272, 380, 27, {23352, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<std::uint8_t, 20> gLegacyCraftMarksC8AA7C {}; // 原全局：0x00C8AA7C。
std::int32_t gLegacyCraftMaterialPageD67D04 = 0; // 原全局：0x00D67D04。
std::int32_t gLegacyCraftMaterialHoverD45F84 = 0; // 原全局：0x00D45F84。 位于12B金额文本之后，非文本别名。
std::int32_t gLegacyCraftDragSlotD2F330 = 0; // 原全局：0x00D2F330。
std::int32_t gLegacyCraftDragImageCC3E7C = 0; // 原全局：0x00CC3E7C。
constexpr wchar_t kLegacyMaterialText58A078[] = L"材料不足。"; // 原常量参考：0x0058A078。
constexpr wchar_t kLegacyMaterialText58A064[] = L"材料的選擇不正確。"; // 原常量参考：0x0058A064。
constexpr wchar_t kLegacyMaterialText58A054[] = L"尚未登錄材料。"; // 原常量参考：0x0058A054。
constexpr wchar_t kLegacyMaterialText58A044[] = L"材料登錄了。"; // 原常量参考：0x0058A044。
constexpr wchar_t kLegacyMaterialText58A034[] = L"加工中......"; // 原常量参考：0x0058A034。
constexpr wchar_t kLegacyMaterialText58A010[] = L"沒有選擇材料。"; // 原常量参考：0x0058A010。
constexpr wchar_t kLegacyMaterialText58A020[] = L"登錄所選的材料。"; // 原常量参考：0x0058A020。
constexpr wchar_t kLegacyMaterialText589FFC[] = L"此物品已被選擇。"; // 原常量参考：0x00589FFC。
constexpr wchar_t kLegacyMaterialText589FEC[] = L"選擇此物品。"; // 原常量参考：0x00589FEC。
LegacyPanelRecord gLegacyCraftRecipePanel5845F8 { // 原全局：0x005845F8。
    0, 0, 344, 380, 25, {23350, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyRecipePageCountD67D10 = 0; // 原全局：0x00D67D10。

constexpr wchar_t kLegacyRecipeBack58A0E0[] = L"回到能力視窗。"; // 原常量参考：0x0058A0E0。
constexpr wchar_t kLegacyRecipeUnavailable58A0B8[] = L"現在無法製作此項物品。"; // 原常量参考：0x0058A0B8。
constexpr wchar_t kLegacyRecipeSelect58A0D0[] = L"製作此項物品。"; // 原常量参考：0x0058A0D0。
LegacyPanelRecord gLegacyInventoryPanel584C00 { // 原全局：0x00584C00。
    368, 0, 272, 436, 14, {22540, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyInventoryActionD67D2C = nullptr; // 原全局：0x00D67D2C。
std::int32_t gLegacyInventoryPageD67D28 = 0; // 原全局：0x00D67D28。
std::int32_t gLegacyInventoryHoverC8AF78 = 0; // 原全局：0x00C8AF78。
constexpr std::array<std::array<std::int32_t, 2>, 28> kLegacyInventoryPositions584CC4 {{ // 原常量参考：0x00584CC4。
    {69, 39}, {11, 149}, {11, 94}, {127, 94}, {127, 149}, {11, 39}, {127, 39}, {202, 144},
    {9, 215}, {61, 215}, {113, 215}, {165, 215}, {217, 215},
    {9, 268}, {61, 268}, {113, 268}, {165, 268}, {217, 268},
    {9, 321}, {61, 321}, {113, 321}, {165, 321}, {217, 321},
    {9, 374}, {61, 374}, {113, 374}, {165, 374}, {217, 374},
}};
constexpr wchar_t kLegacyInventoryTarget58A394[] = // 原常量参考：0x0058A394。
    L"請選擇要將物品使用在誰身上。";
constexpr wchar_t kLegacyInventoryMoney58A37C[] = L"打開金幣視窗。"; // 原常量参考：0x0058A37C。
constexpr wchar_t kLegacyInventoryHead585C38[] = L"裝備頭部防具的位置。"; // 原常量参考：0x00585C38。
constexpr wchar_t kLegacyInventoryBody585C20[] = L"裝備身體防具的位置。"; // 原常量参考：0x00585C20。
constexpr wchar_t kLegacyInventoryHand585C08[] = L"裝備武器跟盾的位置。"; // 原常量参考：0x00585C08。
constexpr wchar_t kLegacyInventoryFeet585BF0[] = L"裝備腳部防具的位置。"; // 原常量参考：0x00585BF0。
constexpr wchar_t kLegacyInventoryAccessory585BDC[] = L"裝備裝飾品的位置。"; // 原常量参考：0x00585BDC。
constexpr wchar_t kLegacyInventoryCrystal585BC8[] = L"裝備水晶的位置。"; // 原常量参考：0x00585BC8。
constexpr wchar_t kLegacyInventoryMemo58A38C[] = L"『{}』"; // 原常量参考：0x0058A38C。
LegacyPanelRecord gLegacyItemSplitPanel584078 { // 原全局：0x00584078。
    440, 0, 200, 200, 16, {23310, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacySplitConfirm589CC4[] = L"指定分配個數"; // 原常量参考：0x00589CC4。
constexpr wchar_t kLegacySplitEmpty589CB4[] = L"請指定個數。"; // 原常量参考：0x00589CB4。
constexpr wchar_t kLegacySplitBack589CA4[] = L"回到物品視窗。"; // 原常量参考：0x00589CA4。
constexpr wchar_t kLegacySplitAll589C98[] = L"全部指定。"; // 原常量参考：0x00589C98。
constexpr wchar_t kLegacySplitClear589C84[] = L"將指定個數歸零。"; // 原常量参考：0x00589C84。
constexpr wchar_t kLegacySplitBackspace585B34[] = L"減少一位數。"; // 原常量参考：0x00585B34。
constexpr wchar_t kLegacySplitLimit589C64[] = L"無法指定更多個數。"; // 原常量参考：0x00589C64。
constexpr wchar_t kLegacySplitDigit589C78[] = L"指定個數。"; // 原常量参考：0x00589C78。
LegacyPanelRecord gLegacyMoneyPanel584DA8 { // 原全局：0x00584DA8。
    440, 0, 200, 200, 15, {22930, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyMoneyAmountD2D618 = 0; // 原全局：0x00D2D618。
constexpr wchar_t kLegacyMoneyDrop585B54[] = L"將指定的金額放地上。"; // 原常量参考：0x00585B54。
constexpr wchar_t kLegacyMoneyEmpty585B44[] = L"請指定金額。"; // 原常量参考：0x00585B44。
constexpr wchar_t kLegacyMoneyAll585B28[] = L"指定全額。"; // 原常量参考：0x00585B28。
constexpr wchar_t kLegacyMoneyClear585B14[] = L"將指定金額歸零。"; // 原常量参考：0x00585B14。
constexpr wchar_t kLegacyMoneyDigit585B6C[] = L"指定金額。"; // 原常量参考：0x00585B6C。
constexpr wchar_t kLegacyMoneyLimit58A3B4[] = L"不能超出這個金額。"; // 原常量参考：0x0058A3B4。
LegacyPanelRecord gLegacyEngravingPanel582FD8 { // 原全局：0x00582FD8。
    368, 0, 272, 252, 79, {23358, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyEngravingHover58309C = -1; // 原全局：0x0058309C。
std::int32_t gLegacyEngravingPageC8AA9C = 0; // 原全局：0x00C8AA9C。
std::int32_t gLegacyEngravingTargetCC1A14 = 0; // 原全局：0x00CC1A14。
constexpr wchar_t kLegacyEngravingSelect5895C0[] = L"刻印此道具。"; // 原常量参考：0x005895C0。
constexpr wchar_t kLegacyEngravingReject5895AC[] = L"這個道具無法刻印。"; // 原常量参考：0x005895AC。
constexpr wchar_t kLegacyEngravingClose5868EC[] = L"關閉此視窗"; // 原常量参考：0x005868EC。
constexpr wchar_t kLegacyEngravingChoose589594[] = L"請選擇要刻印的道具。"; // 原常量参考：0x00589594。
// 旧80独立状态；583164为已加载-1，其余五槽按原虚拟零初始化。
LegacyPanelRecord gLegacyBurialPanel5830A0 { // 原全局：0x005830A0。
    368, 0, 272, 252, 80, {23358, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyBurialHover583164 = -1; // 原全局：0x00583164。
std::int32_t gLegacyBurialPageCC4B64 = 0; // 原全局：0x00CC4B64。
std::int32_t gLegacyBurialSelectedCC4214 = 0; // 原全局：0x00CC4214。
std::int32_t gLegacyBurialExcludedCC48AC = 0; // 原全局：0x00CC48AC。
std::int32_t gLegacyBurialExcludedColumnD2E2B4 = 0; // 原全局：0x00D2E2B4。
std::int32_t gLegacyBurialExcludedRowD671D0 = 0; // 原全局：0x00D671D0。
constexpr wchar_t kLegacyBurialSelect5895FC[] = L"埋起此物品。"; // 原常量参考：0x005895FC。
constexpr wchar_t kLegacyBurialReject5895E4[] = L"這個物品無法埋起來。"; // 原常量参考：0x005895E4。
constexpr wchar_t kLegacyBurialChoose5895D0[] = L"請選擇要埋的物品。"; // 原常量参考：0x005895D0。
// 旧50赠送页；CC1A2C与后续旧49共用唯一选择，不复制邮件草稿。
LegacyPanelRecord gLegacyMailGiftPanel583168 { // 原全局：0x00583168。
    368, 0, 272, 252, 50, {23205, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyMailGiftHover58322C = -1; // 原全局：0x0058322C。
std::int32_t gLegacyMailGiftPageCC2E58 = 0; // 原全局：0x00CC2E58。
std::int32_t gLegacyMailGiftSelectedCC1A2C = 0; // 原全局：0x00CC1A2C。
std::int32_t gLegacyMailGiftColumnD2D498 = 0; // 原全局：0x00D2D498。
std::int32_t gLegacyMailGiftRowCC3E6C = 0; // 原全局：0x00CC3E6C。
constexpr wchar_t kLegacyMailGift58960C[] = L"將此物品送給名片上的好友。"; // 原常量参考：0x0058960C。
// 旧49联系人/协议值唯一承载；分别由旧42联系人发布与旧48邮件宠物选择生产。
LegacyPanelRecord gLegacyMailComposePanel583230 { // 原全局：0x00583230。
    0, 0, 272, 438, 49, {23200, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyMailContactCC4228 = 0; // 原全局：0x00CC4228。
std::int32_t gLegacyMailValueCC2E60 = 0; // 原全局：0x00CC2E60。
constexpr wchar_t kLegacyMailSend589640[] = L"送郵件給選擇的名片上的好友。"; // 原常量参考：0x00589640。
constexpr wchar_t kLegacyMailReturn589628[] = L"回到名片登錄本的視窗。"; // 原常量参考：0x00589628。
LegacyPanelRecord gLegacyMailPetPanel5832F8 { // 原全局：0x005832F8。
    0, 0, 272, 238, 48, {23189, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyMailPet589660[] = L"讓這隻寵物運送郵件。"; // 原常量参考：0x00589660。
// 旧42四个未读提示T*槽，仅借用真实动画；初始化补空，关闭逐槽标死并清引用。
LegacyPanelRecord gLegacyMailContactPanel5837A8 { // 原全局：0x005837A8。
    0, 0, 272, 451, 42, {238521, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<YobiAction*, 4> gLegacyMailUnreadActionsCC3E48 {}; // 原全局：0x00CC3E48。
std::int32_t gLegacyMailSelectedD30410 = 0; // 原全局：0x00D30410。
std::int32_t gLegacyMailPreviousPageD2DACC = 0; // 原全局：0x00D2DACC。
constexpr std::array<std::int32_t, 15> kLegacyMailStatusImages58386C { // 原常量参考：0x0058386C。
    30131, 30231, 30331, 30431, 30531, 30631, 30731, 30831,
    30931, 31131, 31231, 31331, 31431, 31531, 31631,
};
constexpr wchar_t kLegacyMailEdit5897AC[] = L"將名片的表示順序排序。"; // 原常量参考：0x005897AC。
constexpr wchar_t kLegacyMailGroup589798[] = L"顯示家族通訊錄。"; // 原常量参考：0x00589798。
constexpr wchar_t kLegacyMailContactSend589780[] = L"送郵件給這名片上的好友"; // 原常量参考：0x00589780。
constexpr wchar_t kLegacyMailContactRead589760[] = L"看這名片上好友的郵件通信紀錄。"; // 原常量参考：0x00589760。
constexpr wchar_t kLegacyMailContactDelete589750[] = L"刪除這張名片。"; // 原常量参考：0x00589750。
LegacyPanelRecord gLegacyMailDeliveryPanel583550 { // 原全局：0x00583550。
    0, 0, 192, 174, 45, {23184, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyMailNormalMode5896E0[] = L"以普通模式送郵件。"; // 原常量参考：0x005896E0。
constexpr wchar_t kLegacyMailPetMode5896D0[] = L"以寵物送郵件。"; // 原常量参考：0x005896D0。
LegacyPanelRecord gLegacyMailDeletePanel583618 { // 原全局：0x00583618。
    0, 0, 320, 240, 44, {}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyMailDeleteYes589728[] = L" 是"; // 原常量参考：0x00589728。
constexpr wchar_t kLegacyMailDeleteLead589714[] = L"自名片登錄本刪除"; // 原常量参考：0x00589714。
constexpr wchar_t kLegacyMailDeleteEnd589708[] = L"的名片嗎？"; // 原常量参考：0x00589708。
constexpr wchar_t kLegacyMailDeleteHint589700[] = L"刪除。"; // 原常量参考：0x00589700。
constexpr wchar_t kLegacyMailKeepHint5896F4[] = L"不刪除。"; // 原常量参考：0x005896F4。
// 旧46普通邮件；CB0AA8的60个U8由本页初始化生产，与旧42共享目标和页号。
LegacyPanelRecord gLegacyMailNormalPanel583488 { // 原全局：0x00583488。
    0, 0, 272, 438, 46, {23195, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<std::uint8_t, 60> gLegacyMailSelectedFlagsCB0AA8 {}; // 原全局：0x00CB0AA8。
constexpr wchar_t kLegacyMailChooseRecipients5896A4[] = L"請選擇送信的對象。"; // 原常量参考：0x005896A4。
constexpr wchar_t kLegacyMailSendSelected5896B8[] = L"送郵件給選擇的好友。"; // 原常量参考：0x005896B8。
constexpr wchar_t kLegacyMailUnselect58968C[] = L"停止送郵件給這位好友。"; // 原常量参考：0x0058968C。
constexpr wchar_t kLegacyMailSelect589678[] = L"送郵件給這位好友。"; // 原常量参考：0x00589678。
LegacyPanelRecord gLegacyMailSortPanel5836E0 { // 原全局：0x005836E0。
    0, 0, 192, 254, 43, {23170, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyMailSortFirst589744[] = L"登錄順序"; // 原常量参考：0x00589744。
constexpr wchar_t kLegacyMailSortSecond589738[] = L"拼音順序"; // 原常量参考：0x00589738。
constexpr wchar_t kLegacyMailSortThird58972C[] = L"上線順序"; // 原常量参考：0x0058972C。
// 旧47只借同源MailState消息、联系人及页号，面板独立于现代多类别阅读窗口。
LegacyPanelRecord gLegacyMailReadPanel5833C0 { // 原全局：0x005833C0。
    0, 0, 272, 428, 47, {23190, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
// 旧94独立面板与导航状态；成员/排序/职级文本借既有唯一背板，详见旧公会通讯录SPEC。
LegacyPanelRecord gLegacyGuildContactPanel58EF90 { // 原全局：0x0058EF90。
    0, 0, 272, 451, 94, {238522, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildPage1050B78 = 0; // 原全局：0x01050B78。 52FCF0写入、旧94翻页与四行消费。
std::int32_t gLegacyGuildInitialPage1050DA4 = 0; // 原全局：0x01050DA4。 原虚拟零；直接xref只有初始化读取，间接生产仍待。
std::int32_t gLegacyGuildReadTarget1050DB4 = 0; // 原全局：0x01050DB4。 旧94发布，旧89读取目标。
std::array<YobiAction*, 4> gLegacyGuildUnreadActions1050DB8 {}; // 原全局：0x01050DB8。 专用未读动画的四个非拥有槽。
std::int32_t gLegacyGuildComposeTarget1051340 = 0; // 原全局：0x01051340。 旧94发布普通撰写初选。
std::int32_t gLegacyGuildComposePage1051330 = 0; // 原全局：0x01051330。 旧94发布、旧90继续翻页。
std::int32_t gLegacyGuildMailTarget1050DCC = 0; // 原全局：0x01050DCC。 旧94发布、旧87宠物发送消费。
std::int32_t gLegacyGuildDeleteTarget105132C = 0; // 原全局：0x0105132C。 旧94发布、旧92删除消费。
std::int32_t gLegacyGuildRankTarget1057C70 = 0; // 原全局：0x01057C70。 旧94发布、旧96职级页消费。
constexpr std::array<std::int32_t, 17> kLegacyGuildStatusImages58F054 { // 原常量参考：0x0058F054。
    23110, 23120, 23121, 23122, 23123, 23124, 23125, 23126, 23127,
    23128, 23129, 23130, 23131, 23132, 23133, 23134, 23135,
};
constexpr wchar_t kLegacyGuildClose58F248[] = L"關閉此視窗。"; // 原常量参考：0x0058F248。
constexpr wchar_t kLegacyGuildPage58F2D4[] = L"切換頁面。"; // 原常量参考：0x0058F2D4。
constexpr wchar_t kLegacyGuildSort58F4A8[] = L"變更家族成員的顯示順序。"; // 原常量参考：0x0058F4A8。
constexpr wchar_t kLegacyGuildDetails58F490[] = L"顯示家族詳細資料。"; // 原常量参考：0x0058F490。
constexpr wchar_t kLegacyGuildContacts58F474[] = L"顯示名片簿。"; // 原常量参考：0x0058F474。
constexpr wchar_t kLegacyGuildSend58F2EC[] = L"寄信給這位家族成員。"; // 原常量参考：0x0058F2EC。
constexpr wchar_t kLegacyGuildRead58F44C[] = L"查看與這位家族成員的通信記錄。"; // 原常量参考：0x0058F44C。
constexpr wchar_t kLegacyGuildRemove58F42C[] = L"剔除這位家族成員。"; // 原常量参考：0x0058F42C。
constexpr wchar_t kLegacyGuildRank58F410[] = L"變更家族稱號。"; // 原常量参考：0x0058F410。
LegacyPanelRecord gLegacyGuildSortPanel58EEC8 { // 原全局：0x0058EEC8。
    0, 0, 192, 254, 93, {23170, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyGuildSortFirst58F408[] = L"登錄順序"; // 原常量参考：0x0058F408。
constexpr wchar_t kLegacyGuildSortSecond58F400[] = L"名稱順序"; // 原常量参考：0x0058F400。
constexpr wchar_t kLegacyGuildSortThird58F3F4[] = L"在線狀態"; // 原常量参考：0x0058F3F4。
LegacyPanelRecord gLegacyGuildDeletePanel58EE00 { // 原全局：0x0058EE00。
    0, 0, 320, 240, 92, {}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyGuildDeleteYes58F3EC[] = L"是"; // 原常量参考：0x0058F3EC。
constexpr wchar_t kLegacyGuildDeleteNo58F3E4[] = L"否"; // 原常量参考：0x0058F3E4。
constexpr wchar_t kLegacyGuildDeleteDenied58F3C8[] = L"你沒有剔除家族成員的權限"; // 原常量参考：0x0058F3C8。
constexpr wchar_t kLegacyGuildDeleteLead58F3B8[] = L"從家族名簿中"; // 原常量参考：0x0058F3B8。
constexpr wchar_t kLegacyGuildDeleteEnd58F3AC[] = L"刪除此成員"; // 原常量参考：0x0058F3AC。
constexpr wchar_t kLegacyGuildDeleteHint58F3A0[] = L"確認刪除。"; // 原常量参考：0x0058F3A0。
constexpr wchar_t kLegacyGuildKeepHint58F38C[] = L"取消刪除。"; // 原常量参考：0x0058F38C。
LegacyPanelRecord gLegacyGuildDeliveryPanel58ED38 { // 原全局：0x0058ED38。
    0, 0, 192, 174, 91, {23184, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyGuildNormalMode58F374[] = L"以一般郵件寄送。"; // 原常量参考：0x0058F374。
constexpr wchar_t kLegacyGuildPetMode58F358[] = L"使用寵物郵件寄送。"; // 原常量参考：0x0058F358。
constexpr wchar_t kLegacyGuildReturn58F264[] = L"返回家族名簿。"; // 原常量参考：0x0058F264。
LegacyPanelRecord gLegacyGuildPetPanel58EAE0 { // 原全局：0x0058EAE0。
    0, 0, 272, 238, 88, {23189, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildPetIndex1050DB0 = 0; // 原全局：0x01050DB0。 旧88写入、旧87发送消费，独立于普通邮件CC2E60。
constexpr wchar_t kLegacyGuildPetHint58F2B4[] = L"讓這隻寵物運送郵件。"; // 原常量参考：0x0058F2B4。
// 旧86独立赠品状态；58EA14已加载-1，其余四槽按原虚拟零初始化。
LegacyPanelRecord gLegacyGuildGiftPanel58E950 { // 原全局：0x0058E950。
    368, 0, 272, 252, 86, {23205, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildGiftHover58EA14 = -1; // 原全局：0x0058EA14。
std::int32_t gLegacyGuildGiftPage1050DAC = 0; // 原全局：0x01050DAC。
std::int32_t gLegacyGuildGiftSelected1050DA8 = 0; // 原全局：0x01050DA8。 旧86生产、旧87消费，不能借普通邮件赠品槽。
std::int32_t gLegacyGuildGiftColumn1050F10 = 0; // 原全局：0x01050F10。
std::int32_t gLegacyGuildGiftRow1050DC8 = 0; // 原全局：0x01050DC8。
constexpr wchar_t kLegacyGuildGift58F224[] = L"把此物品寄給家族成員。"; // 原常量参考：0x0058F224。
LegacyPanelRecord gLegacyGuildComposePanel58EA18 { // 原全局：0x0058EA18。
    0, 0, 272, 438, 87, {23200, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyGuildSendMail58F288[] = L"寄信給選定的家族成員。"; // 原常量参考：0x0058F288。
LegacyPanelRecord gLegacyGuildReadPanel58EBA8 { // 原全局：0x0058EBA8。
    0, 0, 272, 428, 89, {23190, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildReadPage1050EF8 = 0; // 原全局：0x01050EF8。 旧89独立页号，读取同源公会十条邮件。
LegacyPanelRecord gLegacyGuildNormalPanel58EC70 { // 原全局：0x0058EC70。
    0, 0, 272, 438, 90, {23195, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<std::uint8_t, 200> gLegacyGuildSelectedFlags1050CDC {}; // 原全局：0x01050CDC。 旧90多人选择，不包含1050DA4。
constexpr wchar_t kLegacyGuildUnselect58F310[] = L"取消寄信給這位家族成員。"; // 原常量参考：0x0058F310。
constexpr wchar_t kLegacyGuildChooseRecipients58F33C[] = L"請選擇收件人。"; // 原常量参考：0x0058F33C。
LegacyPanelRecord gLegacyGuildRankPanel58F160 { // 原全局：0x0058F160。
    0, 0, 272, 411, 96, {238556, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildRankReady1057C8C = 0; // 原全局：0x01057C8C。
std::int32_t gLegacyGuildRankPage1051334 = 0; // 原全局：0x01051334。
std::int32_t gLegacyGuildRankPageCount105133C = 0; // 原全局：0x0105133C。
std::int32_t gLegacyGuildRankSelected1051338 = 0; // 原全局：0x01051338。
std::array<std::int32_t, 8> gLegacyGuildRankHighlights1050ED8 {}; // 原全局：0x01050ED8。
std::uint32_t gLegacyGuildRankTick1057C90 = 0; // 原全局：0x01057C90。
std::int32_t gLegacyGuildEditAllowed1050F14 = 0; // 原全局：0x01050F14。 旧95/96共享资格，532430按本人职级是否0生产。
constexpr wchar_t kLegacyGuildRankRequired58F584[] = L"請選擇家族稱號"; // 原常量参考：0x0058F584。
constexpr wchar_t kLegacyGuildRankRemove58F568[] = L"移除家族稱號。"; // 原常量参考：0x0058F568。
constexpr wchar_t kLegacyGuildRankSelect58F4FC[] = L"選擇家族稱號。"; // 原常量参考：0x0058F4FC。
LegacyPanelRecord gLegacyGuildDetailsPanel58F098 { // 原全局：0x0058F098。
    0, 0, 300, 438, 95, {238535, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGuildDetailsSelected58F15C = -1; // 原全局：0x0058F15C。
std::array<input::InputContext*, 3> gLegacyGuildDetailsInputs1057C7C {}; // 原全局：0x01057C7C。 原三T*缓存，只借1050F18实体。
std::uint32_t gLegacyGuildDetailsTick1057C88 = 0; // 原全局：0x01057C88。
std::array<char, 128> gLegacyGuildNameCache1050DD0 {}; // 原全局：0x01050DD0。
std::array<char, 128> gLegacyGuildSecondCache1050E50 {}; // 原全局：0x01050E50。
std::int32_t gLegacyGuildDetailsPageCount1050ED0 = 0; // 原全局：0x01050ED0。
std::int32_t gLegacyGuildDetailsPage1050ED4 = 0; // 原全局：0x01050ED4。
std::array<std::int32_t, 5> gLegacyGuildDetailsHighlights1050EFC {}; // 原全局：0x01050EFC。
constexpr wchar_t kLegacyGuildNameEdit58F550[] = L"輸入家族名稱。"; // 原常量参考：0x0058F550。
constexpr wchar_t kLegacyGuildSecondEdit58F534[] = L"輸入家族住所名稱。"; // 原常量参考：0x0058F534。
constexpr wchar_t kLegacyGuildRankEdit58F518[] = L"輸入家族稱號。"; // 原常量参考：0x0058F518。
constexpr wchar_t kLegacyGuildPermissionFirst58F4E4[] = L"允許招收成員。"; // 原常量参考：0x0058F4E4。
constexpr wchar_t kLegacyGuildPermissionSecond58F4D0[] = L"允許變更名稱。"; // 原常量参考：0x0058F4D0。
LegacyPanelRecord gLegacyEmotePanel584AA8 { // 原全局：0x00584AA8。
    448, 0, 192, 360, 17, {22640, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr const wchar_t* kLegacyEmoteLabels58A370[18] { // 原常量参考：0x0058A370。
    L" 坐　下 ", // 原58A370
    L" 站　立 ", // 原58A364
    L" 揮  手 ", // 原58A358
    L" 走　路 ", // 原58A34C
    L" 點  頭 ", // 原58A340
    L" 倒  下 ", // 原58A334
    L" 高  興 ", // 原58A328
    L" 攻  擊 ", // 原58A31C
    L" 憤  怒 ", // 原58A310
    L" 防  禦 ", // 原58A304
    L" 悲  傷 ", // 原58A2F8
    L" 受  傷 ", // 原58A2EC
    L" 投  擲 ", // 原58A2E0
    L" 魔  法 ", // 原58A2D4
    L" 剪  刀 ", // 原58A2C8
    L" 移動中 ", // 原58A2BC
    L"   布   ", // 原58A2B0
    L" 石  頭 ", // 原58A2A4
};
constexpr const wchar_t* kLegacyEmoteHints5871B0[18] { // 原常量参考：0x005871B0。
    L"做「坐下」這個動作。", // 原5871B0
    L"做「站立」這個動作。", // 原5870D8
    L"做「揮手」這個動作。", // 原587198
    L"做「走路」這個動作。", // 原5870C0
    L"做「點頭」這個動作。", // 原587180
    L"做「倒下」這個動作。", // 原5870A8
    L"做「高興」這個動作。", // 原587168
    L"做「攻擊」這個動作。", // 原587090
    L"做「憤怒」這個動作。", // 原587150
    L"做「防禦」這個動作。", // 原587078
    L"做「悲傷」這個動作。", // 原587138
    L"做「受傷」這個動作。", // 原587060
    L"做「投擲」這個動作。", // 原587120
    L"做「魔法」這個動作。", // 原587048
    L"做「出剪刀」這個動作。", // 原587108
    L"做「移動中」這個動作。", // 原587030
    L"做「出布」這個動作。", // 原5870F0
    L"做「出石頭」這個動作。", // 原587018
};
constexpr wchar_t kLegacyBlacklistText5533F4[] = L"---------------------------熱鍵使用說明--------------------------"; // 原常量参考：0x005533F4。
constexpr wchar_t kLegacyBlacklistText5533B4[] = L"滑鼠框框對準要加入的人 + [CTRL] + [Insert] = 把那個人放在黑名單"; // 原常量参考：0x005533B4。
constexpr wchar_t kLegacyBlacklistText553370[] = L"滑鼠框框對準要移除的人 + [CTRL] + [Delete] = 把那個人從黑名單移除"; // 原常量参考：0x00553370。
constexpr wchar_t kLegacyBlacklistText55332C[] = L"----------------------目前共有{:2}人在黑名單中---------------------"; // 原常量参考：0x0055332C。
constexpr wchar_t kLegacyBlacklistText553304[] = L"新增／移除黑名單的內容"; // 原常量参考：0x00553304。
constexpr wchar_t kLegacyBlacklistText5532EC[] = L"已將{}從黑名單中移除"; // 原常量参考：0x005532EC。
constexpr wchar_t kLegacyBlacklistText5532D8[] = L"從黑名單移除{}失敗"; // 原常量参考：0x005532D8。
constexpr wchar_t kLegacyBlacklistText553294[] = L"你的黑名單已滿，無法再加入"; // 原常量参考：0x00553294。
constexpr wchar_t kLegacyBlacklistText5532C4[] = L"已將{}加入黑名單"; // 原常量参考：0x005532C4。
constexpr wchar_t kLegacyBlacklistText5532B0[] = L"{}加入黑名單失敗"; // 原常量参考：0x005532B0。
LegacyPanelRecord gLegacyBlacklistPanel5531D0 { // 原全局：0x005531D0。
    20, 10, 600, 438, 99, {99100, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
input::InputContext* gLegacyBlacklistInput5939B8 = nullptr; // 原全局：0x005939B8。 旧99借用槽，独立于当前输入焦点。
constexpr wchar_t kLegacyPresetConfirm58A6C0[] = L"結束設定常用句。"; // 原常量参考：0x0058A6C0。
constexpr wchar_t kLegacyPresetCtrl58A68C[] = L"將Ctrl+Ｆ{}鍵設定為常用句。"; // 原常量参考：0x0058A68C。
constexpr wchar_t kLegacyPresetShift58A66C[] = L"將Shift+Ｆ{}鍵設定為常用句。"; // 原常量参考：0x0058A66C。
constexpr wchar_t kLegacyPresetNormal58A6A8[] = L"將Ｆ{}鍵設定為常用句。"; // 原常量参考：0x0058A6A8。
constexpr wchar_t kLegacyPresetTab585910[] = L"切換到第一頁。"; // 原常量参考：0x00585910。
constexpr wchar_t kLegacyPresetTab585900[] = L"切換到第二頁。"; // 原常量参考：0x00585900。
constexpr wchar_t kLegacyPresetTab5858F0[] = L"切換到第三頁。"; // 原常量参考：0x005858F0。
LegacyPanelRecord gLegacyPresetPanel5850C8 { // 原全局：0x005850C8。
    0, 0, 272, 438, 6, {22950, 22951, 22952, 0, 0},
    {{{22953, 24, 43, 52, 18, kLegacyPresetTab585910},
      {22954, 110, 43, 52, 18, kLegacyPresetTab585900},
      {22955, 196, 43, 52, 18, kLegacyPresetTab5858F0}, {}, {}}},
    0, 0, {}, {},
};
std::int32_t gLegacyPresetRowD30408 = 0; // 原全局：0x00D30408。 页号直接借面板selected，只有行号是独立S32。
LegacyPanelRecord gLegacyItemUseStatusPanel582C78 { // 原全局：0x00582C78。
    64, 108, 512, 240, 78, {}, {}, 0, 0, {}, {},
};

constexpr wchar_t kLegacyItemUsePrevious58597C[] = L"上一頁"; // 原常量参考：0x0058597C。

constexpr wchar_t kLegacyItemUseHeader58932C[] = // 原常量参考：0x0058932C。
    L"健康  等級 姓名             生命力    魔力";
LegacyPanelRecord gLegacyGemExecutionPanel58AB28 { // 原全局：0x0058AB28。
    0, 0, 344, 380, 29, {23351, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyGemActionD68A88 = nullptr; // 原全局：0x00D68A88。
std::uint32_t gLegacyGemTickD68AB4 = 0; // 原全局：0x00D68AB4。
std::int32_t gLegacyGemDurationD68AB8 = 0; // 原全局：0x00D68AB8。
std::int32_t gLegacyGemElapsedD68ABC = 0; // 原全局：0x00D68ABC。
constexpr wchar_t kLegacyGemMissing58AC7C[] = L"素材沒有被登錄。"; // 原常量参考：0x0058AC7C。
constexpr wchar_t kLegacyGemExecute58ACB0[] = L"鑲入寶石。"; // 原常量参考：0x0058ACB0。
constexpr wchar_t kLegacyGemBack5871F0[] = L"返回技能視窗。"; // 原常量参考：0x005871F0。
LegacyPanelRecord gLegacyGemMaterialsPanel58AA60 { // 原全局：0x0058AA60。
    368, 0, 272, 380, 30, {23352, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyGemMaterialHoverD68A84 = 0; // 原全局：0x00D68A84。
std::int32_t gLegacyGemDragImageD68AA4 = 0; // 原全局：0x00D68AA4。
std::int32_t gLegacyGemDragSlotD68AA8 = 0; // 原全局：0x00D68AA8。
std::int32_t gLegacyGemMaterialPageD68AAC = 0; // 原全局：0x00D68AAC。
constexpr wchar_t kLegacyGemTooFew58ACA4[] = L"素材不足。"; // 原常量参考：0x0058ACA4。
constexpr wchar_t kLegacyGemInvalid58AC90[] = L"選擇的素材不正確。"; // 原常量参考：0x0058AC90。
constexpr wchar_t kLegacyGemRegistered58AC6C[] = L"素材已登錄。"; // 原常量参考：0x0058AC6C。
constexpr wchar_t kLegacyGemWorking58AC58[] = L"加工中······"; // 原常量参考：0x0058AC58。
constexpr wchar_t kLegacyGemNone58AC28[] = L"沒有選擇到素材。"; // 原常量参考：0x0058AC28。
constexpr wchar_t kLegacyGemRegister58AC3C[] = L"將選到的道具登錄為素材。"; // 原常量参考：0x0058AC3C。
constexpr wchar_t kLegacyGemSelected58AC10[] = L"這個道具已經被選擇。"; // 原常量参考：0x0058AC10。
constexpr wchar_t kLegacyGemSelect58AC00[] = L"選擇這個道具。"; // 原常量参考：0x0058AC00。
constexpr wchar_t kLegacyGemReject58ABEC[] = L"無法選擇這個道具。"; // 原常量参考：0x0058ABEC。
constexpr wchar_t kLegacyCraftAdd589FC8[] = L"回到追加視窗。"; // 原常量参考：0x00589FC8。
constexpr wchar_t kLegacyCraftChoose589FD8[] = // 原常量参考：0x00589FD8。
    L"回到生成選擇視窗。";
constexpr wchar_t kLegacyResultLimit5897D4[] = L"  --  "; // 原常量参考：0x005897D4。
constexpr wchar_t kLegacyTitleSubmit5863C4[] = // 原常量参考：0x005863C4。
    L"設定自己的稱號。";
constexpr wchar_t kLegacyTitlePage58A98C[] = L"換頁。"; // 原常量参考：0x0058A98C。
constexpr wchar_t kLegacyTitleDelete5863E8[] = // 原常量参考：0x005863E8。
    L"刪除所選擇的稱號。";
constexpr wchar_t kLegacyTitleCancelDelete58AA3C[] = // 原常量参考：0x0058AA3C。
    L"不能刪除稱號。";
constexpr wchar_t kLegacyTitleClear5863FC[] = // 原常量参考：0x005863FC。
    L"取消所選擇的稱號。";
constexpr wchar_t kLegacyTitleRemove58AA1C[] = // 原常量参考：0x0058AA1C。
    L"取消這個稱號。";
constexpr wchar_t kLegacyTitleSelect58AA2C[] = // 原常量参考：0x0058AA2C。
    L"設定這個稱號。";
std::uint32_t gLegacyAttributeBlinkCC3EF8 = 0; // 原全局：0x00CC3EF8。
std::uint32_t gLegacyAttributeTickD67440 = 0; // 原全局：0x00D67440。

LegacyPanelRecord gLegacySettingsMenu584E70 { // 原全局：0x00584E70。
    0, 0, 192, 304, 3, {22500, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 8> gLegacySettingsButtonsD67D38 {}; // 原全局：0x00D67D38。
constexpr wchar_t kLegacySettingsText58A4A4[] = L"登       出"; // 原常量参考：0x0058A4A4。
constexpr wchar_t kLegacySettingsHint586DE8[] = L"登出伺服器。"; // 原常量参考：0x00586DE8。
constexpr wchar_t kLegacySettingsText58A498[] = L"聊 天 設 定"; // 原常量参考：0x0058A498。
constexpr wchar_t kLegacySettingsHint58A484[] = L"設定聊天的選項。"; // 原常量参考：0x0058A484。
constexpr wchar_t kLegacySettingsText58A478[] = L"音 樂 設 定"; // 原常量参考：0x0058A478。
constexpr wchar_t kLegacySettingsHint58A460[] = L"設定背景音樂的選項。"; // 原常量参考：0x0058A460。
constexpr wchar_t kLegacySettingsText58A454[] = L"音 效 設 定"; // 原常量参考：0x0058A454。
constexpr wchar_t kLegacySettingsHint58A440[] = L"設定音效的選項。"; // 原常量参考：0x0058A440。
constexpr wchar_t kLegacySettingsText58A434[] = L"滑 鼠 設 定"; // 原常量参考：0x0058A434。
constexpr wchar_t kLegacySettingsHint58A420[] = L"設定滑鼠的選項。"; // 原常量参考：0x0058A420。
constexpr wchar_t kLegacySettingsText58A414[] = L"鍵 盤 熱 鍵"; // 原常量参考：0x0058A414。
constexpr wchar_t kLegacySettingsHint58A400[] = L"鍵盤熱鍵的說明。"; // 原常量参考：0x0058A400。
constexpr wchar_t kLegacySettingsText58A3F4[] = L"繪 圖 設 定"; // 原常量参考：0x0058A3F4。
constexpr wchar_t kLegacySettingsHint58A3E4[] = L"設定繪圖模式。"; // 原常量参考：0x0058A3E4。
constexpr wchar_t kLegacySettingsText58A3D8[] = L"設定按鈕"; // 原常量参考：0x0058A3D8。
constexpr wchar_t kLegacySettingsHint58A3C8[] = L"進行按鈕設定"; // 原常量参考：0x0058A3C8。

LegacyPanelRecord gLegacyChatSettings585000 { // 原全局：0x00585000。
    0, 0, 192, 304, 5, {22960, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 18> gLegacyChatRecordsD67E80 {}; // 原全局：0x00D67E80。
constexpr wchar_t kLegacyChat58A584[] = L"設定顯示行數"; // 原常量参考：0x0058A584。
constexpr wchar_t kLegacyChat58A57C[] = L"目前"; // 原常量参考：0x0058A57C。
constexpr wchar_t kLegacyChat58A578[] = L"行"; // 原常量参考：0x0058A578。
constexpr wchar_t kLegacyChat58A574[] = L"↑"; // 原常量参考：0x0058A574。
constexpr wchar_t kLegacyChat58A570[] = L"↓"; // 原常量参考：0x0058A570。
constexpr wchar_t kLegacyChat58A560[] = L"文字顏色設定"; // 原常量参考：0x0058A560。
constexpr wchar_t kLegacyChat58A554[] = L"目前顏色"; // 原常量参考：0x0058A554。
constexpr wchar_t kLegacyChat58A54C[] = L"<變更>"; // 原常量参考：0x0058A54C。
constexpr wchar_t kLegacyChat5877B4[] = L"變更文字的顏色。"; // 原常量参考：0x005877B4。
constexpr wchar_t kLegacyChat58A540[] = L"常用句設定"; // 原常量参考：0x0058A540。
constexpr wchar_t kLegacyChat58A538[] = L"<設定>"; // 原常量参考：0x0058A538。
constexpr wchar_t kLegacyChat58A524[] = L"打開常用句視窗。"; // 原常量参考：0x0058A524。
constexpr wchar_t kLegacyChat58A514[] = L"設定聊天音效"; // 原常量参考：0x0058A514。
constexpr wchar_t kLegacyChat58A508[] = L"範圍設定"; // 原常量参考：0x0058A508。
constexpr wchar_t kLegacyChat58A500[] = L"現在"; // 原常量参考：0x0058A500。
constexpr wchar_t kLegacyChat58A4F8[] = L" 上一頁"; // 原常量参考：0x0058A4F8。
constexpr wchar_t kLegacyChat58A4E8[] = L"回到系統視窗。"; // 原常量参考：0x0058A4E8。
constexpr wchar_t kLegacyChat58A668[] = L"開"; // 原常量参考：0x0058A668。
constexpr wchar_t kLegacyChat58A664[] = L"關"; // 原常量参考：0x0058A664。
constexpr wchar_t kLegacyChat58A654[] = L"增加顯示行數。"; // 原常量参考：0x0058A654。
constexpr wchar_t kLegacyChat58A63C[] = L"不能再增加顯示行數了。"; // 原常量参考：0x0058A63C。
constexpr wchar_t kLegacyChat58A62C[] = L"減少顯示行數。"; // 原常量参考：0x0058A62C。
constexpr wchar_t kLegacyChat58A614[] = L"不能再減少顯示行數了。"; // 原常量参考：0x0058A614。
constexpr wchar_t kLegacyChat58A604[] = L"關閉聊天音效。"; // 原常量参考：0x0058A604。
constexpr wchar_t kLegacyChat58A5F4[] = L"開啟聊天音效。"; // 原常量参考：0x0058A5F4。
constexpr wchar_t kLegacyChat58A5E0[] = L"增大聊天的範圍。"; // 原常量参考：0x0058A5E0。
constexpr wchar_t kLegacyChat58A5C4[] = L"不能再增大聊天的範圍了。"; // 原常量参考：0x0058A5C4。
constexpr wchar_t kLegacyChat58A5B0[] = L"縮小聊天的範圍。"; // 原常量参考：0x0058A5B0。
constexpr wchar_t kLegacyChat58A594[] = L"不能再減少聊天的範圍了。"; // 原常量参考：0x0058A594。
constexpr wchar_t kLegacyChat553FB4[] = L"{:2}"; // 原常量参考：0x00553FB4。
constexpr wchar_t kLegacyChat5531A4[] = L"{}"; // 原常量参考：0x005531A4。

LegacyPanelRecord gLegacyMusicSettings585190 { // 原全局：0x00585190。
    0, 0, 192, 304, 7, {22961, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 5> gLegacyMusicRecordsD68150 {}; // 原全局：0x00D68150。
LegacyPanelRecord gLegacySoundSettings585258 { // 原全局：0x00585258。
    0, 0, 192, 304, 8, {22962, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 8> gLegacySoundRecordsD68218 {}; // 原全局：0x00D68218。
constexpr wchar_t kLegacyAudio58A6D4[] = L"音量設定"; // 原58A6D4 音量設定
constexpr wchar_t kLegacyAudio58A720[] = L"設定聲音模式"; // 原58A720 設定聲音模式
constexpr wchar_t kLegacyAudio58A76C[] = L"單聲道"; // 原58A76C 單聲道
constexpr wchar_t kLegacyAudio58A764[] = L"立體聲"; // 原58A764 立體聲
constexpr wchar_t kLegacyAudio58A714[] = L"調高音量。"; // 原58A714 調高音量。
constexpr wchar_t kLegacyAudio58A700[] = L"不能再調高音量了"; // 原58A700 不能再調高音量了
constexpr wchar_t kLegacyAudio58A6F4[] = L"降低音量。"; // 原58A6F4 降低音量。
constexpr wchar_t kLegacyAudio58A6E0[] = L"不能再降低音量了。"; // 原58A6E0 不能再降低音量了。
constexpr wchar_t kLegacyAudio58A750[] = L"不能再調高音量了。"; // 原58A750 不能再調高音量了。
constexpr wchar_t kLegacyAudio58A740[] = L"變更為單聲道。"; // 原58A740 變更為單聲道。
constexpr wchar_t kLegacyAudio58A730[] = L"變更為立體聲。"; // 原58A730 變更為立體聲。

LegacyPanelRecord gLegacyMouseSettings585320 { // 原全局：0x00585320。
    0, 0, 192, 304, 9, {22963, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 11> gLegacyMouseRecordsD68358 {}; // 原全局：0x00D68358。
constexpr wchar_t kLegacyMouse58A7B0[] = L"滑鼠游標設定"; // 原常量参考：0x0058A7B0。
constexpr wchar_t kLegacyMouse58A7A0[] = L"設定物品的移動"; // 原常量参考：0x0058A7A0。
constexpr wchar_t kLegacyMouse58A790[] = L"調整滑鼠速度"; // 原常量参考：0x0058A790。
constexpr wchar_t kLegacyMouse58A784[] = L"目前速度"; // 原常量参考：0x0058A784。
constexpr wchar_t kLegacyMouse58A774[] = L"到系統視窗。"; // 原常量参考：0x0058A774。
constexpr wchar_t kLegacyMouse58A838[] = L"正常"; // 原常量参考：0x0058A838。
constexpr wchar_t kLegacyMouse58A830[] = L"平滑"; // 原常量参考：0x0058A830。
constexpr wchar_t kLegacyMouse58A820[] = L"Ctrl+滑鼠左鍵"; // 原常量参考：0x0058A820。
constexpr wchar_t kLegacyMouse58A814[] = L"滑鼠左鍵"; // 原常量参考：0x0058A814。
constexpr wchar_t kLegacyMouse58A7FC[] = L"變更為正常。"; // 原常量参考：0x0058A7FC。
constexpr wchar_t kLegacyMouse58A7EC[] = L"變更為平滑。"; // 原常量参考：0x0058A7EC。
constexpr wchar_t kLegacyMouse58A7D4[] = L"變更為Ctrl+滑鼠左鍵。"; // 原常量参考：0x0058A7D4。
constexpr wchar_t kLegacyMouse58A7C0[] = L"變更為滑鼠左鍵。"; // 原常量参考：0x0058A7C0。
constexpr wchar_t kLegacyMouse5535F8[] = L"{}"; // 原常量参考：0x005535F8。
constexpr wchar_t kLegacyMouse58A80C[] = L"{:2.1f}"; // 原常量参考：0x0058A80C。

LegacyPanelRecord gLegacyHotkeyHelp5853E8 { // 原全局：0x005853E8。
    0, 0, 192, 304, 10, {22964, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 13> gLegacyHotkeyRecordsD68510 {}; // 原全局：0x00D68510。
constexpr wchar_t kLegacyHotkey58A8AC[] = L"人物名稱顯示"; // 原常量参考：0x0058A8AC。
constexpr wchar_t kLegacyHotkey58A8A8[] = L"F11"; // 原常量参考：0x0058A8A8。
constexpr wchar_t kLegacyHotkey58A89C[] = L"螢幕擷取"; // 原常量参考：0x0058A89C。
constexpr wchar_t kLegacyHotkey58A898[] = L"F12"; // 原常量参考：0x0058A898。
constexpr wchar_t kLegacyHotkey58A884[] = L"清除螢幕聊天訊息"; // 原常量参考：0x0058A884。
constexpr wchar_t kLegacyHotkey58A87C[] = L"HOME"; // 原常量参考：0x0058A87C。
constexpr wchar_t kLegacyHotkey58A86C[] = L"關閉聊天頻道"; // 原常量参考：0x0058A86C。
constexpr wchar_t kLegacyHotkey58A85C[] = L"Shift + HOME"; // 原常量参考：0x0058A85C。
constexpr wchar_t kLegacyHotkey58A850[] = L"冒險者日誌"; // 原常量参考：0x0058A850。
constexpr wchar_t kLegacyHotkey58A84C[] = L"F9"; // 原常量参考：0x0058A84C。
constexpr wchar_t kLegacyHotkey58A840[] = L"黑名單列表"; // 原常量参考：0x0058A840。
constexpr wchar_t kLegacyHotkey553F00[] = L"END"; // 原常量参考：0x00553F00。
LegacyPanelRecord gLegacyScanlineSettings5854B0 { // 原全局：0x005854B0。
    0, 0, 192, 304, 11, {22965, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 4> gLegacyScanlineRecordsD68718 {}; // 原全局：0x00D68718。
constexpr wchar_t kLegacyScanline586E24[] = L"掃描線同期設定"; // 原常量参考：0x00586E24。
constexpr wchar_t kLegacyScanline58A8F8[] = L"掃描線同步"; // 原常量参考：0x0058A8F8。
constexpr wchar_t kLegacyScanline58A8E8[] = L"掃描線非同步"; // 原常量参考：0x0058A8E8。
constexpr wchar_t kLegacyScanline58A8D4[] = L"掃描線變更為同步。"; // 原常量参考：0x0058A8D4。
constexpr wchar_t kLegacyScanline58A8BC[] = L"掃描線變更為非同步。"; // 原常量参考：0x0058A8BC。

LegacyPanelRecord gLegacyButtonSettings585578 { // 原全局：0x00585578。
    0, 0, 192, 304, 12, {22549, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::array<LegacyPanelTextButtonRecord, 4> gLegacyButtonRecordsD687B8 {}; // 原全局：0x00D687B8。
std::int32_t gLegacyUiHeight556C28 = 64; // 原全局：0x00556C28。
std::int32_t gLegacyUiY556C2C = -1; // 原全局：0x00556C2C。
std::int32_t gLegacyUiWidth556C30 = 192; // 原全局：0x00556C30。
std::int32_t gLegacyUiX62B39C = 0; // 原全局：0x0062B39C。
// 455490组只存已证的八槽初始化值；不据此公开未知消费者的记录布局。
std::array<std::array<std::int32_t, 8>, 5> gLegacyUiInitWords62B3B8 {}; // 原全局：0x0062B3B8。
std::array<std::int32_t, 8> gLegacyUiInitWords62B460 {}; // 原全局：0x0062B460。
std::array<std::int32_t, 8> gLegacyUiInitWords62B488 {}; // 原全局：0x0062B488。
std::array<std::array<std::int32_t, 8>, 3> gLegacyUiInitWords62B4A8 {}; // 原全局：0x0062B4A8。
std::array<std::array<std::int32_t, 8>, 5> gLegacyUiInitWords62B508 {}; // 原全局：0x0062B508。
std::int32_t gLegacyBottomWidth5858A0 = 640; // 原全局：0x005858A0。
std::int32_t gLegacyBottomHeight5858A4 = 24; // 原全局：0x005858A4。
std::int32_t gLegacyBottomXD68930 = 0; // 原全局：0x00D68930。
std::int32_t gLegacyBottomYCB1DD8 = 0; // 原全局：0x00CB1DD8。
std::int32_t gLegacyBottomHeightCA0014 = 0; // 原全局：0x00CA0014。
std::int32_t gLegacyBottomHeightD2DAB4 = 0; // 原全局：0x00D2DAB4。
// 仅承接九个已证W32初始化值；不是原40B结构，未知+24H尾部及指针语义不定型。
std::array<std::array<std::int32_t, 9>, 8> gLegacyBottomInitWordsD68938 {}; // 原全局：0x00D68938。
// LNW01：原0x00D68AC6零字节仅供已退休的负分类旁读；历史来源见BRU01。
constexpr wchar_t kLegacyButton58A914[] = L"按鈕設定"; // 原常量参考：0x0058A914。
constexpr wchar_t kLegacyButton58A4BC[] = L"回上一頁"; // 原常量参考：0x0058A4BC。
constexpr wchar_t kLegacyButton58A904[] = L"回到系統選單。"; // 原常量参考：0x0058A904。
constexpr wchar_t kLegacyButton58A97C[] = L"聊天文字大小"; // 原常量参考：0x0058A97C。
constexpr wchar_t kLegacyButton58A970[] = L"聊天範圍"; // 原常量参考：0x0058A970。
constexpr wchar_t kLegacyButton58A948[] = // 原常量参考：0x0058A948。
    L"將聊天文字大小按鈕變更成聊天範圍按鈕。";
constexpr wchar_t kLegacyButton58A920[] = // 原常量参考：0x0058A920。
    L"將聊天範圍按鈕變更成聊天文字大小按鈕。";

LegacyPanelRecord gLegacyLogoutPanel584F38 { // 原全局：0x00584F38。
    0, 0, 192, 304, 4, {22504, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
constexpr wchar_t kLegacyLogout58A4D8[] = L"回到城內登入點"; // 原常量参考：0x0058A4D8。
constexpr wchar_t kLegacyLogout58A4C8[] = L"伺服器選擇畫面"; // 原常量参考：0x0058A4C8。

constexpr wchar_t kLegacyLogout586E60[] = // 原常量参考：0x00586E60。
    L"登出後回到伺服器選擇畫面。";
constexpr wchar_t kLegacyLogout586E48[] = // 原常量参考：0x00586E48。
    L"登出並回到城內登入點。";
constexpr wchar_t kLegacyLogout58A4B0[] = L"放棄登出。"; // 原常量参考：0x0058A4B0。

// 原函数：0x004FC7F0。
std::int32_t Sub_4FC7F0()
{
    static_cast<void>(ui::Sub_5042D0(3));
    static_cast<void>(ui::Sub_504180(4));
    return 0;
}

// 原函数：0x004FC810。
std::int32_t Sub_4FC810()
{
    static_cast<void>(ui::Sub_5042D0(3));
    static_cast<void>(ui::Sub_504180(5));
    return 0;
}

// 原函数：0x004FC830。
std::int32_t Sub_4FC830()
{
    static_cast<void>(ui::Sub_5042D0(3));
    static_cast<void>(ui::Sub_504180(7));
    return 0;
}

// 原函数：0x004FC850。
std::int32_t Sub_4FC850()
{
    static_cast<void>(ui::Sub_5042D0(3));
    static_cast<void>(ui::Sub_504180(8));
    return 0;
}

// 原函数：0x004FC870。
std::int32_t Sub_4FC870()
{
    static_cast<void>(ui::Sub_5042D0(3));
    static_cast<void>(ui::Sub_504180(9));
    return 0;
}

// 原函数：0x004FC890。
std::int32_t Sub_4FC890()
{
    static_cast<void>(ui::Sub_5042D0(3));
    static_cast<void>(ui::Sub_504180(10));
    return 0;
}

// 原函数：0x004FC8B0。
std::int32_t Sub_4FC8B0()
{
    static_cast<void>(ui::Sub_5042D0(3));
    static_cast<void>(ui::Sub_504180(11));
    return 0;
}

// 原函数：0x004FC8D0。
std::int32_t Sub_4FC8D0()
{
    static_cast<void>(ui::Sub_5042D0(3));
    static_cast<void>(ui::Sub_504180(12));
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

// 原函数：0x004ECDF0。
std::uint32_t Sub_4ECDF0()
{
    const std::uint32_t now = platform::ReadTickCount32();
    gLegacyPointResultTickCC3E74 = now;
    return now;
}

// 原函数：0x004ED010。
std::uint32_t Sub_4ED010()
{
    const std::uint32_t now = platform::ReadTickCount32();
    gLegacyBattleResultTickD3FAB8 = now;
    return now;
}

// 原函数：0x00501F60。
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

LegacyPanelRecord gLegacyProfilePanel5857D0 { // 原全局：0x005857D0。
    0, 0, 272, 438, 2, {22501, 22515, 22980, 0, 0},
    {{{1, 7, 416, 77, 16, kLegacyProfileTab5858D8},
      {1, 97, 416, 77, 16, kLegacyProfileTab5858C0},
      {1, 187, 416, 77, 16, kLegacyProfileTab5858A8}, {}, {}}},
    0, 0, {}, {},
};

// 原[402020,402196)：两个打开阶段、忽略fgets返回的显示行计数及S32页数返回。
std::int32_t ReloadLegacyLog_402020()
{
    char path[256];
    char buffer[3000] {}; // D01/P03：只在入口定义一次原未写首轮，其后复用。
    platform::LegacyTm53B8C6* calendar = gLegacyLogCalendar593F88;
    if (calendar == nullptr) {
        static_cast<void>(platform::ReadLegacyTime32_53BA26(&gLegacyLogStorage593A00.time593E10));
        calendar = platform::LocalTime_53B8C6(&gLegacyLogStorage593A00.time593E10);
        gLegacyLogCalendar593F88 = calendar;
    }
    const std::int32_t month = calendar->month0based;
    const std::int32_t day = calendar->day;
    const std::int32_t year = calendar->yearMinus1900;
    std::sprintf(world::detail::gLogFileName554648,
        "chat_%02d%02d%02d.txt", year % 100, AddW32(month, 1), day);
    std::sprintf(path, "%s\\%s", "Log",
        world::detail::gLogFileName554648);
    platform::LegacyFile* current = std::fopen(path, "rt");
    gLegacyLogFile593F78 = current;
    if (current == nullptr) {
        gLegacyLogError593F7C = 1;
        return 0;
    }
    gLegacyLogError593F7C = 0;
    static_cast<void>(std::fclose(current));
    current = std::fopen(path, "a+t");
    gLegacyLogFile593F78 = current;
    if (current == nullptr) {
        gLegacyLogError593F7C = 1;
        return 0;
    }
    ui::detail::gLogDisplayGate593F84 = 1;
    gLegacyLogLineCount593F80 = 0;
    while (std::feof(current) == 0) {
        static_cast<void>(std::fgets(buffer, 1000, current));
        const auto length = static_cast<std::int32_t>(std::strlen(buffer));
        if (length > 65) {
            gLegacyLogLineCount593F80 = AddW32(gLegacyLogLineCount593F80, (length - 1) / 65);
        }
        const std::int32_t count = gLegacyLogLineCount593F80;
        current = gLegacyLogFile593F78;
        gLegacyLogLineCount593F80 = AddW32(count, 1);
    }
    const std::int32_t pageCount = AddW32(gLegacyLogLineCount593F80 / 16, 1);
    gLegacyLogPageCount553504 = pageCount;
    if (gLegacyLogPage553508 >= pageCount) {
        gLegacyLogPage553508 = pageCount;
    }
    return pageCount;
}

// 原函数：0x00455490。
std::int32_t Sub_455490()
{
    return Sub_4554A0();
}

// 原函数：0x004554A0。
std::int32_t Sub_4554A0()
{
    const std::int32_t width = gLegacyUiWidth556C30;
    const std::int32_t x = 640 - width;
    gLegacyUiX62B39C = x;
    return x;
}

// 原函数：0x004554C0。
std::int32_t Sub_4554C0()
{
    return Sub_4554D0();
}

// 原函数：0x004554D0。
std::int32_t Sub_4554D0()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B3B8[0], -16, 0, 8, 22581);
}

// 原函数：0x00455540。
std::int32_t Sub_455540()
{
    return Sub_455550();
}

// 原函数：0x00455550。
std::int32_t Sub_455550()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B3B8[1], -16, 0, 38, 22583);
}

// 原函数：0x004555C0。
std::int32_t Sub_4555C0()
{
    return Sub_4555D0();
}

// 原函数：0x004555D0。
std::int32_t Sub_4555D0()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B3B8[2], -16, 0, 68, 22585);
}

// 原函数：0x00455640。
std::int32_t Sub_455640()
{
    return Sub_455650();
}

// 原函数：0x00455650。
std::int32_t Sub_455650()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B3B8[3], -16, 0, 98, 22587);
}

// 原函数：0x004556C0。
std::int32_t Sub_4556C0()
{
    return Sub_4556D0();
}

// 原函数：0x004556D0。
std::int32_t Sub_4556D0()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B3B8[4], -16, 0, 128, 22589);
}

// 原函数：0x00455740。
std::int32_t Sub_455740()
{
    return Sub_455750();
}

// 原函数：0x00455750。
std::int32_t Sub_455750()
{
    return InitializeLegacyUiSquareRecord(gLegacyUiInitWords62B460, 76, -13, 158, 238544);
}

// 原函数：0x004557C0。
std::int32_t Sub_4557C0()
{
    return Sub_4557D0();
}

// 原函数：0x004557D0。
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

// 原函数：0x00455840。
std::int32_t Sub_455840()
{
    return Sub_455850();
}

// 原函数：0x00455850。
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

// 原函数：0x00455930。
std::int32_t Sub_455930()
{
    return Sub_455940();
}

// 原函数：0x00455940。
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

// 原函数：0x004D3180。
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

// 原函数：0x004D31C0。
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

// 原函数：0x004D66E0。
YobiAction* Sub_4D66E0()
{
    const std::int32_t y = gLegacySkillChoicePanel581D60.y + 133;
    const std::int32_t x = gLegacySkillChoicePanel581D60.x + 45;
    const std::int32_t image = ui::detail::WindowShopChoice_ReadSkillImage();
    YobiAction* const action = Sub_4D6640(image, x, y);
    gLegacySkillChoiceActionD67C58 = action;
    return action;
}

// 原函数：0x004D6A30。
YobiAction* Sub_4D6A30()
{
    const std::int32_t y = gLegacySkillLearningPanel581E28.y + 246;
    const std::int32_t x = gLegacySkillLearningPanel581E28.x + 469;
    const std::int32_t image = ui::detail::WindowShopChoice_ReadSkillImage();
    YobiAction* const action = Sub_4D6640(image, x, y);
    gLegacySkillChoiceActionD67C58 = action;
    return action;
}

// 原函数：0x004D6E70。
YobiAction* Sub_4D6E70()
{
    const std::int32_t y = gLegacySkillForgetPanel581EF0.y + 246;
    const std::int32_t x = gLegacySkillForgetPanel581EF0.x + 469;
    const std::int32_t image = ui::detail::WindowShopChoice_ReadSkillImage();
    YobiAction* const action = Sub_4D6640(image, x, y);
    gLegacySkillChoiceActionD67C58 = action;
    return action;
}


// 原函数：0x004D8D70。
YobiAction* Sub_4D8D70()
{
    const std::int32_t y = gLegacyShopChoicePanel5822D8.y;
    const std::int32_t x = gLegacyShopChoicePanel5822D8.x;
    const std::int32_t image = ui::detail::WindowShopChoice_ReadShopImage();
    YobiAction* const action = Sub_4F9FF0(image, x + 45, y + 133, 213);
    gLegacyShopChoiceActionD67C68 = action;
    return action;
}

// 原函数：0x004D91E0。
YobiAction* Sub_4D91E0()
{
    const std::int32_t y = gLegacyBuyPanel5823A0.y + 245;
    const std::int32_t x = gLegacyBuyPanel5823A0.x + 469;
    const std::int32_t image = gLegacyBuyImageD2DBFC;
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyBuyActionD67C6C = action;
    return action;
}

// 原函数：0x004DA400。
void Sub_4DA400(const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t page, const std::int32_t mode)
{
    if (index < 0 || index >= 20) {
        return;
    }
    const auto* const item = BorrowLegacyShopItemCF4FE0(index);
    if (item->used == 0) {
        return;
    }
    std::wstring title;
    const std::int32_t count = item->count;
    if (count <= 1) {
        title = cg::text::FromGbkWide(item->name.data());
    } else {
        title = std::format(L"{}({})", cg::text::FromGbkWide(item->name.data()), count);
    }
    const std::int32_t flags = item->flags;
    Sub_504BA0(x, y, 272, 4, 0, title, cg::text::FromGbkWide(item->memo.data()), 0, 0,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(flags)));
    for (std::int32_t row = 0; row < 7; ++row) {
        const char* const line = item->description[page * 8].data() + row * 96;
        if (std::strlen(line) != 0) {
            static_cast<void>(ui::detail::WindowItem_SplitTooltipLines(x + 10, y + 34 + 18 * row, 4, 1, 0, cg::text::FromGbkWide(line), 0, 0));
        }
    }
    if (item->allowed == 0 && mode == 0) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + 10),
            static_cast<std::int16_t>(y + 160), 4, 1, 4, kLegacyBuyUnavailableHint586BF4, 0, 0));
    }
    static_cast<void>(DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr));
}

// 原函数：0x004DAA00。
YobiAction* Sub_4DAA00()
{
    const std::int32_t y = gLegacySellPanel582530.y + 245;
    const std::int32_t x = gLegacySellPanel582530.x + 469;
    const std::int32_t image = gLegacySellImageCC3EFC;
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacySellActionD67C74 = action;
    return action;
}

// 原函数：0x004DC7A0。
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

// 原函数：0x004DDA30。
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
    Sub_504BA0(x, y, 272, 4, 0, cg::text::FromGbkWide(item + 1), cg::text::FromGbkWide(item + 0xC34), 0, 0,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(flags)));
    for (std::int32_t row = 0; row < 8; ++row) {
        const char* const line = item + 0x1E + page * 768 + row * 96;
        if (std::strlen(line) != 0) {
            static_cast<void>(ui::detail::WindowItem_SplitTooltipLines(x + 10, y + 34 + 18 * row, 4, 1, 0, cg::text::FromGbkWide(line), 0, 0));
        }
    }
    static_cast<void>(DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr));
}

// 原函数：0x004DDF80。
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

// 原函数：0x004DF210。
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
    Sub_504BA0(x, y, 272, 4, 0, cg::text::FromGbkWide(item + 1), cg::text::FromGbkWide(item + 0xC34), 0, 0,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(flags)));
    for (std::int32_t row = 0; row < 8; ++row) {
        const char* const line = item + 0x1E + page * 768 + row * 96;
        if (std::strlen(line) != 0) {
            static_cast<void>(ui::detail::WindowItem_SplitTooltipLines(x + 10, y + 34 + 18 * row, 4, 1, 0, cg::text::FromGbkWide(line), 0, 0));
        }
    }
    static_cast<void>(DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr));
}

// 原函数：0x004D36B0。
YobiAction* Sub_4D36B0()
{
    const std::int32_t y = gLegacyBarterPanel5818B0.y + 245;
    const std::int32_t x = gLegacyBarterPanel5818B0.x + 469;
    const std::int32_t image = gLegacyBarterImageCF4A0C;
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyBarterActionD67C50 = action;
    return action;
}

// 原函数：0x004D4960。
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
    const std::int32_t width = TextMeasure(cg::text::FromGbkWide(item.name.data()).c_str(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x - width / 2 + 136),
        static_cast<std::int16_t>(y + 5), 4, 0, 0, cg::text::FromGbkWide(item.name.data()).c_str(), 0, 0));
    for (std::int32_t row = 0; row < 8; ++row) {
        const char* const line = item.descriptions[8 * page + row].data();
        if (std::strlen(line) != 0) {
            static_cast<void>(ui::detail::WindowItem_SplitTooltipLines(x + 10, y + 34 + 18 * row, 4, 1, 0, cg::text::FromGbkWide(line), 0, 0));
        }
    }
    static_cast<void>(DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr));
}

// 原函数：0x004D4ED0。
YobiAction* Sub_4D4ED0()
{
    const std::int32_t y = gLegacyExchangePanel581A40.y + 133;
    const std::int32_t x = gLegacyExchangePanel581A40.x + 45;
    const std::int32_t image = ui::detail::WindowShopChoice_ReadExchangeImage();
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyExchangeActionCF4B5C = action;
    return action;
}

// 原函数：0x004D5220。
YobiAction* Sub_4D5220()
{
    const std::int32_t y = gLegacyGiveReceivePanel581B08.y + 133;
    const std::int32_t x = gLegacyGiveReceivePanel581B08.x + 45;
    const std::int32_t image = ui::detail::WindowShopChoice_ReadGiveReceiveImage();
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyGiveReceiveActionCB2228 = action;
    return action;
}

// 原函数：0x004D5580。
YobiAction* Sub_4D5580()
{
    const std::int32_t y = gLegacyGiveReceiveQuantityPanel581BD0.y + 245;
    const std::int32_t x = gLegacyGiveReceiveQuantityPanel581BD0.x + 469;
    const std::int32_t image = gLegacyGiveReceiveImageD2D814;
    YobiAction* const action = Sub_4F9FF0(image, x, y, 213);
    gLegacyGiveReceiveQuantityActionCB1DD4 = action;
    return action;
}

// 原函数：0x004D61E0。
std::int32_t Sub_4D61E0(const std::int32_t x, const std::int32_t y, const std::int32_t page)
{
    const char* const name = ui::detail::gShopDetailCC3210.name.data();
    const std::int32_t width = TextMeasure(cg::text::FromGbkWide(name).c_str(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x - width / 2 + 136),
        static_cast<std::int16_t>(y + 5), 4, 0, 0, cg::text::FromGbkWide(name).c_str(), 0, 0));
    const char* const description = ui::detail::gShopDetailCC3210.description[page * 8].data();
    for (std::int32_t row = 0; row < 8; ++row) {
        const char* const line = description + 96 * row;
        if (std::strlen(line) != 0) {
            static_cast<void>(ui::detail::WindowItem_SplitTooltipLines(x + 10, y + 34 + 18 * row, 4, 1, 0, cg::text::FromGbkWide(line), 0, 0));
        }
    }
    return DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr);
}

// 原函数：0x004E0270。
char* Sub_4E0270(const std::int32_t side)
{
    gLegacyStorageEmptySlotsCF49BC = 0;
    std::int32_t count = 0;
    auto* record = BorrowLegacyStorageItem(side, 0);
    for (std::int32_t item = 0; item < 20; ++item, ++record) {
        if (std::bit_cast<std::int16_t>(record->used) == 0) {
            ++count;
        }
    }
    gLegacyStorageEmptySlotsCF49BC = count;
    return reinterpret_cast<char*>(record); // 旧接口借用组内物品末尾地址。
}

// 原函数：0x004E0BC0。
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

// 原函数：0x004E0C70。
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

// 原函数：0x004E0D10。
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
    std::wstring buffer;
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            auto* const record = BorrowLegacyStorageItem(side, column + 5 * row);
            if (std::bit_cast<std::int16_t>(record->used) != 0) {
                if (gLegacyStorageDragSlotCC4C98 == column + 5 * row && side == 1) {
                    const std::int32_t y = gLegacyStoragePanel582B70.y;
                    const std::int32_t x = gLegacyStoragePanel582B70.x;
                    DrawGraphicHover(Low16(x + 52 * column + grid[0] + 2),
                        Low16(y + 52 * row + grid[1] + 2),
                        Low16(x + 52 * column + grid[2] - 2),
                        Low16(y + 52 * row + grid[3] - 2), 212, 254, 0);
                }
                const std::int16_t selection = record->selection;
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
                const std::int32_t image = record->imageNo;
                const std::int32_t y = gLegacyStoragePanel582B70.y;
                const std::int32_t x = gLegacyStoragePanel582B70.x;
                static_cast<void>(DrawGraphic(x + 52 * column + grid[0] + 24,
                    y + 52 * row + grid[1] + 24, 214, image, 0, &descriptor));
                const std::int32_t count = record->count;
                if (count > 0) {
                    cg::text::FormatTo(buffer, L"{:3}", count);
                    const std::int32_t countY = gLegacyStoragePanel582B70.y + 52 * row + grid[1] + 31;
                    const std::int32_t countX = gLegacyStoragePanel582B70.x + 52 * column + grid[0] + 21;
                    static_cast<void>(DrawText(static_cast<std::int16_t>(countX),
                        static_cast<std::int16_t>(countY), 2, 1, 0, buffer, 0, 0));
                }
            }
            if (record->selection == -2) {
                const std::int32_t y = gLegacyStoragePanel582B70.y;
                const std::int32_t x = gLegacyStoragePanel582B70.x;
                static_cast<void>(DrawGraphic(x + 52 * column + grid[0] + 24,
                    y + 52 * row + grid[1] + 24, 218, 23041, 0, nullptr));
            }
        }
    }
    return 20;
}

// 原函数：0x004E0FF0。
std::int32_t Sub_4E0FF0(const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t item, const std::int32_t page)
{
    if (item < 0 || item >= 20) {
        return item;
    }
    const auto* const record = BorrowLegacyStorageItem(side, item);
    if (std::bit_cast<std::int16_t>(record->used) == 0) {
        return item;
    }
    const std::int32_t flags = record->flags;
    Sub_504BA0(x, y, 272, 4, 0, cg::text::FromGbkWide(record->name.data()), cg::text::FromGbkWide(record->memo.data()), 0, 0,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(flags)));
    const char* text = record->description[page * 8].data();
    for (std::int32_t row = 0; row < 8; ++row, text += 96) {
        if (std::strlen(text) != 0) {
            static_cast<void>(ui::detail::WindowItem_SplitTooltipLines(x + 10, y + 34 + 18 * row, 4, 1, 0, cg::text::FromGbkWide(text), 0, 0));
        }
    }
    return DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr);
}

// 原函数：0x004E10F0。
std::int32_t Sub_4E10F0(const std::int32_t x, const std::int32_t y)
try {
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
        world::detail::BorrowPlayerItemsFDA8A4()[gLegacyStorageSplitSourceD3FACC + 8].count;
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
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:7}", world::detail::BorrowPlayerItemsFDA8A4()[gLegacyStorageSplitSourceD3FACC + 8].count);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 88),
        static_cast<std::int16_t>(y + 46), 4, 1, 0, buffer, 0, 0));
    cg::text::FormatTo(buffer, L"{:7}", gLegacyStorageSplitAmountCB2220);
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
catch (const cg::text::GbkStorageError&) {
    ReportLegacyTextStorageFailure();
    return 0;
}

// 原函数：0x004E18F0。
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

// 原函数：0x004E1980。
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

// 原函数：0x004E19E0。
void Sub_4E19E0(const std::int32_t side)
{
    const auto& origin = kLegacyStoragePetRows582C54[side];
    for (std::int32_t pet = 0; pet < 5; ++pet) {
        const auto* const record = BorrowLegacyStoragePet(side, pet);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            if (record->selection > 0) {
                const std::int32_t y = gLegacyStoragePanel582B70.y + origin[1] + 22 * pet;
                const std::int32_t x = gLegacyStoragePanel582B70.x + origin[0];
                DrawGraphicHover(Low16(x - 3), Low16(y - 2), Low16(x + 146),
                    Low16(y + 16), 213, 251, 0);
            }
            const char* const name = record->name.data();
            const std::int32_t width = TextMeasure(cg::text::FromGbkWide(name).c_str(), 0);
            const std::int32_t y = gLegacyStoragePanel582B70.y + origin[1] + 22 * pet;
            const std::int32_t x = gLegacyStoragePanel582B70.x + origin[0] - width / 2 + 72;
            static_cast<void>(DrawText(static_cast<std::int16_t>(x),
                static_cast<std::int16_t>(y), 2, 1, 0, cg::text::FromGbkWide(name).c_str(), 0, 0));
        }
        if (record->selection < 0) {
            const std::int32_t y = gLegacyStoragePanel582B70.y + origin[1] + 22 * pet + 8;
            const std::int32_t x = gLegacyStoragePanel582B70.x + origin[0] + 72;
            static_cast<void>(DrawGraphic(x, y, 218, 23348, 0, nullptr));
        }
    }
}

// 原函数：0x004E1B10。
std::int32_t Sub_4E1B10(const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t pet)
{
    const auto* const record = BorrowLegacyStoragePet(side, pet);
    if (std::bit_cast<std::int16_t>(record->used) == 0) {
        return 0x154 * pet;
    }
    YobiAction* action = gLegacyStoragePetActionD67C90;
    if (action != nullptr) {
        SetYobiActionFlag2(action, false);
        SetYobiActionField30(gLegacyStoragePetActionD67C90, x + 74);
        SetYobiActionField34(gLegacyStoragePetActionD67C90, y + 220);
        action = gLegacyStoragePetActionD67C90;
        const std::int32_t image = record->imageNo;
        if (GetYobiActionField2A8(action) != image) {
            const std::int32_t mapped = character_select::detail::Sub_51B740(image);
            SetYobiActionField2A8(gLegacyStoragePetActionD67C90, mapped);
        }
    }
    for (std::int32_t element = 0; element < 4; ++element) {
        const std::uint32_t color = kLegacyAttributeBars57CBB8[element][0];
        const std::int32_t value = record->elements[element];
        DrawGraphicHover(Low16(x + 191), Low16(y + 70 + 4 * element),
            Low16(x + 191 + 7 * (value / 10)), Low16(y + 73 + 4 * element), 229, color, 1);
    }
    const auto drawAt = [x, y](const std::wstring_view text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 4, font, 0, text, 0, 0));
    };
    const char* const name = record->name.data();
    const std::int32_t width = TextMeasure(cg::text::FromGbkWide(name), 0);
    drawAt(cg::text::FromGbkWide(name), 136 - width / 2, 5, 0);
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", record->level);
    drawAt(buffer, 191, 30, 1);
    const std::int32_t tribeWidth = TextMeasure(ui::detail::gTribeNames57CB54[std::bit_cast<std::int16_t>(record->tribe)], 1);
    drawAt(ui::detail::gTribeNames57CB54[std::bit_cast<std::int16_t>(record->tribe)], 227 - tribeWidth / 2, 50, 1);
    const std::int32_t maxHp = record->maxHp;
    const std::int32_t hp = record->hp;
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    drawAt(buffer, 191, 90, 1);
    const std::int32_t maxFp = record->maxFp;
    const std::int32_t fp = record->fp;
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    drawAt(buffer, 191, 110, 1);
    for (std::int32_t attribute = 0; attribute < 5; ++attribute) {
        cg::text::FormatTo(buffer, L"{:3}", record->combatStats[attribute]);
        drawAt(buffer, 191, 130 + 20 * attribute, 1);
    }
    cg::text::FormatTo(buffer, L"{:2}", record->value44);
    drawAt(buffer, 191, 230, 1);
    drawAt(L"1/2", 240, 9, 1);
    return DrawGraphic(x + 136, y + 126, 222, 23277, 0, nullptr);
}

// 原函数：0x004E1EE0。
std::int32_t Sub_4E1EE0(const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t pet)
{
    const auto* const record = BorrowLegacyStoragePet(side, pet);
    if (std::bit_cast<std::int16_t>(record->used) == 0) {
        return 0x154 * pet;
    }
    std::int32_t row = 0;
    for (std::int32_t skill = 0; skill < 10; ++skill) {
        const char* const text = record->techniqueNames[skill].data();
        if (std::strlen(text) != 0) {
            static_cast<void>(DrawText(static_cast<std::int16_t>(x + 75),
                static_cast<std::int16_t>(y + 22 * row + 29), 4, 0, 0, cg::text::FromGbkWide(text).c_str(), 0, 0));
            static_cast<void>(DrawGraphic(x + 137, y + 22 * row + 38, 223, 23281, 0, nullptr));
            ++row;
        }
    }
    const char* const name = record->name.data();
    const std::int32_t width = TextMeasure(cg::text::FromGbkWide(name).c_str(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x - width / 2 + 136),
        static_cast<std::int16_t>(y + 5), 4, 0, 0, cg::text::FromGbkWide(name).c_str(), 0, 0));
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 240),
        static_cast<std::int16_t>(y + 9), 4, 1, 0, L"2/2", 0, 0));
    return DrawGraphic(x + 136, y + 126, 222, 23280, 0, nullptr);
}

// 原函数：0x004E2030。
std::uint32_t Sub_4E2030()
try {
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
        auto* const record = BorrowLegacyStorageItem(1, right);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            const std::int16_t selection = record->selection;
            if (selection >= 0) {
                std::int32_t sound = 55;
                if (selection == 1) {
                    record->selection = 0;
                    sound = 54;
                } else if (gLegacyStoragePutCountD2DAD4 < gLegacyStorageCapacityD30404) {
                    const std::int32_t money = world::detail::ReadPlayerValueFC();
                    if (money - gLegacyStorageFeeD40B04 - gLegacyStorageMoneyD2F79C >= 1) {
                        record->selection = static_cast<std::int16_t>(selection ^ 1);
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
        auto* const target = BorrowLegacyStorageItem(1, right);
        if (source < 0) {
            if (std::bit_cast<std::int16_t>(target->used) != 0 && target->selection >= 0) {
                const std::int32_t image = target->imageNo;
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
                if (BorrowLegacyStorageItem(1, source)->count > 0
                    && std::bit_cast<std::int16_t>(target->used) == 0 && split) {
                    gLegacyStorageSplitModeCA0AA8 = 1;
                    gLegacyStorageSplitSourceD3FACC = source;
                    gLegacyStorageSplitTargetCC4D38 = right;
                    gLegacyStorageSplitAmountCB2220 = 0;
                    gLegacyStorageDragSlotCC4C98 = -1;
                    gLegacyStorageDragImageCB09D4 = 0;
                    static_cast<void>(audio::PlaySe_52C450(56, 320));
                    mouse = static_cast<std::uint8_t>(gValueD68D9C);
                } else if (std::bit_cast<std::int16_t>(target->used) == 0 || target->selection >= 0) {
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
            if (gLegacyStorageDetailPageD67C94 >= BorrowLegacyStorageItem(1, right)->pages) {
                gLegacyStorageDetailPageD67C94 = 0;
            }
        }
    }
    if (left >= 0) {
        auto* const record = BorrowLegacyStorageItem(0, left);
        if ((mouse & 0x10) != 0) {
            if (std::bit_cast<std::int16_t>(record->used) != 0) {
                const std::int16_t selection = record->selection;
                if (selection >= 0) {
                    if (selection == 1) {
                        record->selection = 0;
                        static_cast<void>(audio::PlaySe_52C450(54, 320));
                    } else if (gLegacyStorageTakeCountCF4A04 < gLegacyStorageEmptySlotsCF49BC) {
                        record->selection = static_cast<std::int16_t>(selection ^ 1);
                        static_cast<void>(audio::PlaySe_52C450(79, 320));
                    } else {
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    }
                }
            }
        } else if ((mouse & 2) != 0) {
            ++gLegacyStorageDetailPageD67C94;
            if (gLegacyStorageDetailPageD67C94 >= record->pages) {
                gLegacyStorageDetailPageD67C94 = 0;
            }
        }
    }
    const std::int32_t image = gLegacyStorageDragImageCB09D4;
    if (image != 0) {
        const std::int32_t mouseY = platform::BorrowLegacyMouseStateStorage()->mouseY.value();
        const std::int32_t mouseX = platform::BorrowLegacyMouseStateStorage()->mouseX.value();
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
    const auto drawAt = [](const std::wstring_view text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t y = gLegacyStoragePanel582B70.y + offsetY;
        const std::int32_t x = gLegacyStoragePanel582B70.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(x),
            static_cast<std::int16_t>(y), 2, font, 0, text, 0, 0));
    };
    std::wstring buffer;
    const std::int32_t message = gLegacyStorageMessageD67C98;
    if (message == 1) {
        drawAt(kLegacyStorageNoMoney58906C, 63, 305, 0);
    } else if (message == 0) {
        drawAt(kLegacyStorageFee589284, 80, 305, 0);
        cg::text::FormatTo(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
        drawAt(buffer, 139, 306, 1);
    }
    drawAt(kLegacyStorageGold589278, 358, 305, 0);
    cg::text::FormatTo(buffer, kLegacyStorageAmount588FC4, world::detail::ReadPlayerValueFC());
    drawAt(buffer, 417, 306, 1);
    const std::int32_t backgroundY = gLegacyStoragePanel582B70.y + 180;
    const std::int32_t backgroundX = gLegacyStoragePanel582B70.x + 280;
    std::uint32_t result = static_cast<std::uint32_t>(
        DrawGraphic(backgroundX, backgroundY, 212, 23341, 0, &descriptor));
    const wchar_t* hint = nullptr;
    bool leftHint = false;
    if (left >= 0) {
        result = static_cast<std::uint32_t>(0xC44 * left);
        const auto* const record = BorrowLegacyStorageItem(0, left);
        if (std::bit_cast<std::int16_t>(record->used) != 0 && record->selection >= 0) {
            leftHint = true;
            const std::int32_t flags = record->flags;
            if ((flags & 0x100) != 0) {
                ui::detail::gHoverTextD68AE8 = cg::text::FromGbkWide(record->name.data());
                ui::detail::gHoverTextD68AE8 += L"[kok]";
                result = 0;
            } else if ((flags & 0x200) != 0) {
                ui::detail::gHoverTextD68AE8 = cg::text::FromGbkWide(record->memo.data());
                ui::detail::gHoverTextD68AE8 += L"[han]";
                result = 0;
            } else {
                hint = kLegacyStorageTake589268;
                result = static_cast<std::uint32_t>(cg::text::ToGbk(hint).size() + 1);
            }
        }
    }
    if (!leftHint && right >= 0) {
        result = static_cast<std::uint32_t>(0xC44 * right);
        const auto* const record = BorrowLegacyStorageItem(1, right);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            const std::int32_t flags = record->flags;
            if ((flags & 0x100) != 0) {
                ui::detail::gHoverTextD68AE8 = cg::text::FromGbkWide(record->name.data());
                ui::detail::gHoverTextD68AE8 += L"[kok]";
                result = 0;
            } else if ((flags & 0x200) != 0) {
                ui::detail::gHoverTextD68AE8 = cg::text::FromGbkWide(record->memo.data());
                ui::detail::gHoverTextD68AE8 += L"[han]";
                result = 0;
            } else {
                const std::int16_t selection = record->selection;
                if (selection > 0) {
                    hint = kLegacyStoragePut589258;
                    result = static_cast<std::uint32_t>(cg::text::ToGbk(hint).size() + 1);
                } else if (selection < 0) {
                    hint = kLegacyStorageCannotPut5878BC;
                    result = 0;
                } else if (gLegacyStoragePutCountD2DAD4 >= gLegacyStorageCapacityD30404) {
                    hint = kLegacyStorageFull589230;
                    result = static_cast<std::uint32_t>(cg::text::ToGbk(hint).size() + 1);
                } else {
                    hint = kLegacyStorageCanPut589244;
                    result = 0;
                }
            }
        }
    }
    if (hint != nullptr) {
        ui::detail::gHoverTextD68AE8 = hint;
    }
    gLegacyStoragePreviousRight582C68 = right;
    gLegacyStoragePreviousLeft582C64 = left;
    return result;
}
catch (const cg::text::GbkStorageError&) {
    ReportLegacyTextStorageFailure();
    return 0;
}

// 原函数：0x004E28F0。
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
        auto* const record = BorrowLegacyStoragePet(0, left);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            const std::int16_t selection = record->selection;
            if (selection >= 0) {
                record->selection = selection == 1 ? 0 : static_cast<std::int16_t>(selection ^ 1);
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
    } else if (right >= 0 && (mouse & 0x10) != 0) {
        auto* const record = BorrowLegacyStoragePet(1, right);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            const std::int16_t selection = record->selection;
            if (selection >= 0) {
                if (selection == 1) {
                    record->selection = 0;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                } else {
                    const std::int32_t available = world::detail::ReadPlayerValueFC()
                        - gLegacyStorageFeeD40B04 - gLegacyStorageMoneyD2F79C;
                    if (available < world::detail::ReadPlayerLevel()) {
                        gLegacyStoragePetMessageD67CA4 = 1;
                        gLegacyStoragePetTickD67CA8 = platform::ReadTickCount32();
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    } else {
                        record->selection = static_cast<std::int16_t>(selection ^ 1);
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
    const auto drawAt = [](const std::wstring_view text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t y = gLegacyStoragePanel582B70.y + offsetY;
        const std::int32_t x = gLegacyStoragePanel582B70.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(x),
            static_cast<std::int16_t>(y), 2, font, 0, text, 0, 0));
    };
    std::wstring buffer;
    const std::int32_t message = gLegacyStoragePetMessageD67CA4;
    if (message == 1) {
        drawAt(kLegacyStorageNoMoney58906C, 63, 305, 0);
    } else if (message == 0) {
        drawAt(kLegacyStorageFee589284, 80, 305, 0);
        cg::text::FormatTo(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
        drawAt(buffer, 139, 306, 1);
    }
    drawAt(kLegacyStorageGold589278, 358, 305, 0);
    cg::text::FormatTo(buffer, kLegacyStorageAmount588FC4, world::detail::ReadPlayerValueFC());
    drawAt(buffer, 417, 306, 1);
    const std::int32_t backgroundY = gLegacyStoragePanel582B70.y + 180;
    const std::int32_t backgroundX = gLegacyStoragePanel582B70.x + 280;
    std::uint32_t result = static_cast<std::uint32_t>(
        DrawGraphic(backgroundX, backgroundY, 212, 23342, 0, nullptr));
    const wchar_t* hint = nullptr;
    if (left >= 0) {
        result = static_cast<std::uint32_t>(0x154 * left);
        const auto* const record = BorrowLegacyStoragePet(0, left);
        if (std::bit_cast<std::int16_t>(record->used) != 0 && record->selection >= 0) {
            hint = kLegacyStoragePetTake5892C4;
            result = 0;
        }
    }
    if (hint == nullptr && right >= 0) {
        result = static_cast<std::uint32_t>(0x154 * right);
        const auto* const record = BorrowLegacyStoragePet(1, right);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            const std::int16_t selection = record->selection;
            result = (result & 0xFFFF0000u) | static_cast<std::uint16_t>(selection);
            if (selection > 0) {
                hint = kLegacyStoragePetPut5892B4;
                result = static_cast<std::uint32_t>(cg::text::ToGbk(hint).size() + 1);
            } else if (selection == 0) {
                if (gLegacyStoragePetPutCountD2D600 < gLegacyStoragePetCapacityCB1DDC) {
                    hint = kLegacyStoragePetCanPut5892A0;
                    result = 0;
                } else {
                    hint = kLegacyStoragePetFull58928C;
                    result = static_cast<std::uint32_t>(cg::text::ToGbk(hint).size() + 1);
                }
            }
        }
    }
    if (hint != nullptr) {
        ui::detail::gHoverTextD68AE8 = hint;
    }
    gLegacyStoragePetPreviousRight582C70 = right;
    gLegacyStoragePetPreviousLeft582C6C = left;
    return result;
}

// 原函数：0x004E2D90。
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
        return gLegacyStorageDepositDraftCC4B58;
    };
    const auto writeDraft = [](const std::int32_t value) {
        gLegacyStorageDepositDraftCC4B58 = value;
    };
    const std::int32_t side = ui::detail::WindowBank_ReadSide();
    // 非0/1模式取原var_21C已写几何值，不以默认上限或补零替换。
    std::int32_t maximum = x + 404;
    if (side == 0 || side == 1) {
        const std::int32_t limit = world::detail::ReadPlayerValue15BD0(side + 1);
        const std::int32_t amount = ui::detail::gItemDetailStorageCC4D58[side].amount;
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
    const auto drawAt = [](const std::wstring_view text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t top = gLegacyStoragePanel582B70.y + offsetY;
        const std::int32_t left = gLegacyStoragePanel582B70.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, font, 0, text, 0, 0));
    };
    std::wstring buffer;
    const std::int32_t displaySide = ui::detail::WindowBank_ReadSide();
    const std::int32_t balance = ui::detail::gItemDetailStorageCC4D58[displaySide].amount;
    cg::text::FormatTo(buffer, L"{:7}", balance);
    drawAt(buffer, 146, 150, 0);
    cg::text::FormatTo(buffer, L"{:7}", maximum);
    drawAt(buffer, 421, 118, 0);
    cg::text::FormatTo(buffer, L"{:7}", readDraft());
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
        cg::text::FormatTo(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
        drawAt(buffer, 139, 306, 1);
    }
    drawAt(kLegacyStorageGold589278, 358, 305, 0);
    cg::text::FormatTo(buffer, kLegacyStorageAmount588FC4, world::detail::ReadPlayerValueFC());
    drawAt(buffer, 417, 306, 1);
    const wchar_t* hint;
    std::uint32_t result = 0;
    if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        const std::int32_t currentMoney = world::detail::ReadPlayerValueFC();
        const std::int32_t draft = readDraft();
        const std::int32_t fee = gLegacyStorageFeeD40B04;
        if (currentMoney - draft < fee) {
            hint = kLegacyStorageCannotDeposit5892E8;
        } else {
            hint = kLegacyStorageDeposit5892F4;
            result = static_cast<std::uint32_t>(cg::text::ToGbk(hint).size() + 1);
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
            result = static_cast<std::uint32_t>(cg::text::ToGbk(hint).size() + 1);
        } else {
            hint = kLegacyStorageAmountLimit5892D4;
        }
    }
    ui::detail::gHoverTextD68AE8 = hint;
    return result;
}

// 原函数：0x004E3750。
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
    const std::int32_t side = ui::detail::WindowBank_ReadSide();
    const std::int32_t limit = world::detail::ReadPlayerValue15BD0(0);
    const std::int32_t money = world::detail::ReadPlayerValueFC();
    std::int32_t maximum = limit - money;
    std::int32_t hoveredDigit = -1;
    const std::int32_t balance = ui::detail::gItemDetailStorageCC4D58[side].amount;
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
    const auto drawAt = [](const std::wstring_view text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        const std::int32_t top = gLegacyStoragePanel582B70.y + offsetY;
        const std::int32_t left = gLegacyStoragePanel582B70.x + offsetX;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, font, 0, text, 0, 0));
    };
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:7}", world::detail::ReadPlayerValueFC());
    drawAt(buffer, 423, 151, 0);
    cg::text::FormatTo(buffer, L"{:7}", maximum);
    drawAt(buffer, 143, 118, 0);
    cg::text::FormatTo(buffer, L"{:7}", gLegacyStorageWithdrawDraftD2EA98);
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
    const std::int32_t displaySide = ui::detail::WindowBank_ReadSide();
    const std::int32_t currentBalance = ui::detail::gItemDetailStorageCC4D58[displaySide].amount;
    cg::text::FormatTo(buffer, kLegacyStorageAmount588FC4, currentBalance);
    drawAt(buffer, 139, 306, 1);
    drawAt(kLegacyStorageFee589284, 358, 305, 0);
    cg::text::FormatTo(buffer, kLegacyStorageAmount588FC4, gLegacyStorageFeeD40B04);
    drawAt(buffer, 417, 306, 1);
    const wchar_t* hint;
    std::uint32_t result = 0;
    if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = kLegacyStorageWithdraw589304;
        result = static_cast<std::uint32_t>(cg::text::ToGbk(hint).size() + 1);
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
            result = static_cast<std::uint32_t>(cg::text::ToGbk(hint).size() + 1);
        } else {
            hint = kLegacyStorageAmountLimit5892D4;
        }
    }
    ui::detail::gHoverTextD68AE8 = hint;
    return result;
}

// 原函数：0x004E6CC0。
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

// 原函数：0x004E6D70。
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

// 原函数：0x004E6E10。
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
    std::wstring buffer;
    for (std::int32_t row = 0; row < 4; ++row) {
        for (std::int32_t column = 0; column < 5; ++column) {
            auto* const record = BorrowLegacyTradeItem(side, column + 5 * row);
            if (std::bit_cast<std::int16_t>(record->used) != 0) {
                if (ui::detail::gTradeValueCC3EC4 == column + 5 * row && side == 1) {
                    const std::int32_t y = gLegacyTradePanel582F00.y;
                    const std::int32_t x = gLegacyTradePanel582F00.x;
                    DrawGraphicHover(Low16(x + 52 * column + grid[0] + 2),
                        Low16(y + 52 * row + grid[1] + 2),
                        Low16(x + 52 * column + grid[2] - 2),
                        Low16(y + 52 * row + grid[3] - 2), 212, 254, 0);
                }
                const std::int16_t selection = record->selection;
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
                const std::int32_t image = record->imageNo;
                const std::int32_t y = gLegacyTradePanel582F00.y;
                const std::int32_t x = gLegacyTradePanel582F00.x;
                static_cast<void>(DrawGraphic(x + 52 * column + grid[0] + 24,
                    y + 52 * row + grid[1] + 24, 214, image, 0, &descriptor));
                const std::int32_t count = record->count;
                if (count > 0) {
                    cg::text::FormatTo(buffer, L"{:3}", count);
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

// 原函数：0x004E70B0。
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

// 原函数：0x004E7140。
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

// 原函数：0x004E71A0。
void Sub_4E71A0(const std::int32_t side)
{
    const auto& origin = kLegacyTradePetRows582EEC[side];
    for (std::int32_t pet = 0; pet < 5; ++pet) {
        const auto* const record = BorrowLegacyTradePet(side, pet);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            if (record->selection > 0) {
                const std::int32_t y = gLegacyTradePanel582F00.y + origin[1] + 22 * pet;
                const std::int32_t x = gLegacyTradePanel582F00.x + origin[0];
                DrawGraphicHover(Low16(x - 3), Low16(y - 2), Low16(x + 146),
                    Low16(y + 16), 213, 251, 0);
            }
            const char* const name = record->name.data();
            const std::int32_t width = TextMeasure(cg::text::FromGbkWide(name).c_str(), 0);
            const std::int32_t y = gLegacyTradePanel582F00.y + origin[1] + 22 * pet;
            const std::int32_t x = gLegacyTradePanel582F00.x + origin[0] - width / 2 + 72;
            static_cast<void>(DrawText(static_cast<std::int16_t>(x),
                static_cast<std::int16_t>(y), 2, 1, 0, cg::text::FromGbkWide(name).c_str(), 0, 0));
        }
    }
}

// 原函数：0x004E7290。
std::int32_t Sub_4E7290(const std::int32_t x, const std::int32_t y,
    const std::int32_t item, const std::int32_t page)
{
    if (item < 0 || item >= 20) {
        return item;
    }
    const auto* const record = BorrowLegacyTradeItem(0, item);
    if (std::bit_cast<std::int16_t>(record->used) == 0) {
        return item;
    }
    const std::int32_t flags = record->flags;
    Sub_504BA0(x, y, 272, 4, 0, cg::text::FromGbkWide(record->name.data()), cg::text::FromGbkWide(record->memo.data()), 0, 0,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(flags)));
    const char* text = record->description[page * 8].data();
    for (std::int32_t row = 0; row < 8; ++row, text += 96) {
        if (std::strlen(text) != 0) {
            static_cast<void>(ui::detail::WindowItem_SplitTooltipLines(x + 10, y + 34 + 18 * row, 4, 1, 0, cg::text::FromGbkWide(text), 0, 0));
        }
    }
    return DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr);
}

// 原函数：0x004E7380。
std::int32_t Sub_4E7380(const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t pet)
{
    const auto* const record = BorrowLegacyTradePet(side, pet);
    if (std::bit_cast<std::int16_t>(record->used) == 0) {
        return pet + 5 * side;
    }
    YobiAction* action = gLegacyTradePetActionD67CB4;
    if (action != nullptr) {
        SetYobiActionFlag2(action, false);
        SetYobiActionField30(gLegacyTradePetActionD67CB4, x + 74);
        SetYobiActionField34(gLegacyTradePetActionD67CB4, y + 220);
        action = gLegacyTradePetActionD67CB4;
        const std::int32_t image = record->imageNo;
        if (GetYobiActionField2A8(action) != image) {
            const std::int32_t mapped = character_select::detail::Sub_51B740(image);
            SetYobiActionField2A8(gLegacyTradePetActionD67CB4, mapped);
        }
    }
    for (std::int32_t element = 0; element < 4; ++element) {
        const std::uint32_t color = kLegacyAttributeBars57CBB8[element][0];
        const std::int32_t value = record->elements[element];
        DrawGraphicHover(Low16(x + 191), Low16(y + 70 + 4 * element),
            Low16(x + 191 + 7 * (value / 10)), Low16(y + 73 + 4 * element), 229, color, 1);
    }
    const auto drawAt = [x, y](const std::wstring_view text, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::int8_t font) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 4, font, 0, text, 0, 0));
    };
    const char* const name = record->name.data();
    const std::int32_t width = TextMeasure(cg::text::FromGbkWide(name), 0);
    drawAt(cg::text::FromGbkWide(name), 136 - width / 2, 5, 0);
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", record->level);
    drawAt(buffer, 191, 30, 1);
    const std::int32_t tribeWidth = TextMeasure(ui::detail::gTribeNames57CB54[std::bit_cast<std::int16_t>(record->tribe)], 1);
    drawAt(ui::detail::gTribeNames57CB54[std::bit_cast<std::int16_t>(record->tribe)], 227 - tribeWidth / 2, 50, 1);
    const std::int32_t maxHp = record->maxHp;
    const std::int32_t hp = record->hp;
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    drawAt(buffer, 191, 90, 1);
    const std::int32_t maxFp = record->maxFp;
    const std::int32_t fp = record->fp;
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    drawAt(buffer, 191, 110, 1);
    for (std::int32_t attribute = 0; attribute < 5; ++attribute) {
        cg::text::FormatTo(buffer, L"{:3}", record->baseStats[attribute]);
        drawAt(buffer, 191, 130 + 20 * attribute, 1);
    }
    cg::text::FormatTo(buffer, L"{:2}", record->value5C);
    drawAt(buffer, 191, 230, 1);
    drawAt(L"1/2", 240, 9, 1);
    return DrawGraphic(x + 136, y + 126, 222, 23277, 0, nullptr);
}

// 原函数：0x004E7740。
std::int32_t Sub_4E7740(const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t pet)
{
    const auto* const record = BorrowLegacyTradePet(side, pet);
    if (std::bit_cast<std::int16_t>(record->used) == 0) {
        return pet + 5 * side;
    }
    // 每次绘制后重读技能数；未使用槽也占一行并绘底图。
    for (std::int32_t skill = 0; skill < record->value5C; ++skill) {
        const auto* const entry = &record->techniques[skill];
        if (std::bit_cast<std::int16_t>(entry->used) != 0) {
            static_cast<void>(DrawText(static_cast<std::int16_t>(x + 75),
                static_cast<std::int16_t>(y + 22 * skill + 29), 4, 0, 0, cg::text::FromGbkWide(entry->name.data()).c_str(), 0, 0));
        }
        static_cast<void>(DrawGraphic(x + 137, y + 22 * skill + 38, 223, 23281, 0, nullptr));
    }
    const char* const name = record->name.data();
    const std::int32_t width = TextMeasure(cg::text::FromGbkWide(name).c_str(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x - width / 2 + 136),
        static_cast<std::int16_t>(y + 5), 4, 0, 0, cg::text::FromGbkWide(name).c_str(), 0, 0));
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 240),
        static_cast<std::int16_t>(y + 9), 4, 1, 0, L"2/2", 0, 0));
    return DrawGraphic(x + 136, y + 126, 222, 23280, 0, nullptr);
}

// 原函数：0x004E7870。
std::int32_t Sub_4E7870(const std::int32_t x, const std::int32_t y)
try {
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
        world::detail::BorrowPlayerItemsFDA8A4()[gLegacyTradeSplitSourceD45F90 + 8].count;
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
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:7}", world::detail::BorrowPlayerItemsFDA8A4()[gLegacyTradeSplitSourceD45F90 + 8].count);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 88),
        static_cast<std::int16_t>(y + 46), 4, 1, 0, buffer, 0, 0));
    cg::text::FormatTo(buffer, L"{:7}", gLegacyTradeSplitAmountD2D490);
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
catch (const cg::text::GbkStorageError&) {
    ReportLegacyTextStorageFailure();
    return 0;
}

// 原函数：0x004E7FC0。
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
            if (gLegacyTradeRemoteItemPageD67CC0 >= world::detail::BorrowPlayerItemsFDA8A4()[hovered + 8].pages) {
                gLegacyTradeRemoteItemPageD67CC0 = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        Sub_4E6D70(0, hovered, gValueD69768);
        const std::int32_t page = gLegacyTradeRemoteItemPageD67CC0;
        const std::int32_t y = gLegacyTradePanel582F00.y + 104;
        const std::int32_t x = gLegacyTradePanel582F00.x + 283;
        static_cast<void>(Sub_4E7290(x, y, hovered, page));
        const auto* const record = BorrowLegacyTradeItem(0, hovered);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            const std::int32_t flags = record->flags;
            if ((flags & 0x100) != 0) {
                ui::detail::gHoverTextD68AE8 = cg::text::FromGbkWide(record->name.data());
                ui::detail::gHoverTextD68AE8 += L"[kok]";
            } else if ((flags & 0x200) != 0) {
                ui::detail::gHoverTextD68AE8 = cg::text::FromGbkWide(record->memo.data());
                ui::detail::gHoverTextD68AE8 += L"[han]";
            } else {
                ui::detail::gHoverTextD68AE8 = kLegacyTradeRemoteItem5894DC;
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

// 原函数：0x004E81B0。
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
        if (std::bit_cast<std::int16_t>(BorrowLegacyTradePet(0, hovered)->used) != 0) {
            ui::detail::gHoverTextD68AE8 = kLegacyTradeRemotePet5894F4;
        }
    }
    Sub_4E71A0(0);
    const std::int32_t y = gLegacyTradePanel582F00.y + 180;
    const std::int32_t x = gLegacyTradePanel582F00.x + 141;
    const std::int32_t result = DrawGraphic(x, y, 212, 23233, 0, nullptr);
    gLegacyTradeRemotePetPrevious582FC8 = hovered;
    return result;
}

// 原函数：0x004E82F0。
std::int32_t Sub_4E82F0()
{
    const std::int32_t y = gLegacyTradePanel582F00.y;
    const std::int32_t x = gLegacyTradePanel582F00.x;
    if (Sub_5049A0(x + 115, y + 146, x + 235, y + 170, -1) != 0) {
        ui::detail::gHoverTextD68AE8 = kLegacyTradeRemoteMoney58950C;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:7}", ui::detail::gTradeValueCF4688);
    const std::int32_t textY = gLegacyTradePanel582F00.y + 150;
    const std::int32_t textX = gLegacyTradePanel582F00.x + 146;
    static_cast<void>(DrawText(static_cast<std::int16_t>(textX),
        static_cast<std::int16_t>(textY), 2, 0, 0, buffer, 0, 0));
    const std::int32_t backgroundY = gLegacyTradePanel582F00.y + 180;
    const std::int32_t backgroundX = gLegacyTradePanel582F00.x + 141;
    return DrawGraphic(backgroundX, backgroundY, 212, 23234, 0, nullptr);
}

// 原函数：0x004E83D0。
std::int32_t Sub_4E83D0()
try {
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
        auto* const record = BorrowLegacyTradeItem(1, hovered);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            const std::int16_t selection = record->selection;
            if (selection >= 0) {
                static_cast<void>(audio::PlaySe_52C450(selection != 0 ? 54 : 79, 320));
                const std::int32_t phase = ui::detail::gTradeValueD2E2B0;
                const std::int16_t toggled = static_cast<std::int16_t>(record->selection ^ 1);
                std::memcpy(&record->selection, &toggled, sizeof(toggled));
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
        auto* const target = BorrowLegacyTradeItem(1, hovered);
        if (source < 0) {
            if (std::bit_cast<std::int16_t>(target->used) != 0 && target->selection >= 0) {
                const std::int32_t image = target->imageNo;
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
            if (BorrowLegacyTradeItem(1, source)->count > 0
                && std::bit_cast<std::int16_t>(target->used) == 0 && split) {
                ui::detail::gTradeValueCC2344 = 1;
                gLegacyTradeSplitSourceD45F90 = source;
                gLegacyTradeSplitTargetD2D668 = hovered;
                gLegacyTradeSplitAmountD2D490 = 0;
                ui::detail::gTradeValueCC3EC4 = -1;
                ui::detail::gTradeValueD66E08 = 0;
                static_cast<void>(audio::PlaySe_52C450(56, 320));
            } else if (std::bit_cast<std::int16_t>(target->used) != 0 && target->selection < 0) {
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
        if (gLegacyTradeDetailPageD67CC8 >= world::detail::BorrowPlayerItemsFDA8A4()[hovered + 8].pages) {
            gLegacyTradeDetailPageD67CC8 = 0;
        }
    }
    // 原分支分别跳往拖图尾/公共尾，不能统一在清槽后重新判定。
    if (drawDrag && ui::detail::gTradeValueD66E08 != 0) {
        gLegacyTradeMouseXCF3FEC = platform::BorrowLegacyMouseStateStorage()->mouseX.value();
        const std::int32_t mouseY = platform::BorrowLegacyMouseStateStorage()->mouseY.value();
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
        const auto* const record = BorrowLegacyTradeItem(1, hovered);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            const std::int32_t flags = record->flags;
            if ((flags & 0x100) != 0) {
                ui::detail::gHoverTextD68AE8 = cg::text::FromGbkWide(record->name.data());
                ui::detail::gHoverTextD68AE8 += L"[kok]";
            } else if ((flags & 0x200) != 0) {
                ui::detail::gHoverTextD68AE8 = cg::text::FromGbkWide(record->memo.data());
                ui::detail::gHoverTextD68AE8 += L"[han]";
            } else {
                const std::int16_t selection = record->selection;
                const wchar_t* const hint = selection > 0 ? kLegacyTradeSelected589554
                    : (selection < 0 ? kLegacyTradeUnavailable58953C : kLegacyTradeAvailable589524);
                ui::detail::gHoverTextD68AE8 = hint;
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
catch (const cg::text::GbkStorageError&) {
    ReportLegacyTextStorageFailure();
    return 0;
}

// 原函数：0x004E8920。
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
            auto* const record = BorrowLegacyTradePet(1, hovered);
            if (std::bit_cast<std::int16_t>(record->used) != 0) {
                const std::int16_t selection = record->selection;
                if (selection >= 0) {
                    const std::int16_t toggled = static_cast<std::int16_t>(selection ^ 1);
                    std::memcpy(&record->selection, &toggled, sizeof(toggled));
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
        const auto* const record = BorrowLegacyTradePet(1, hovered);
        if (std::bit_cast<std::int16_t>(record->used) != 0) {
            const wchar_t* const hint = record->selection != 0
                ? kLegacyTradePetSelected589580 : kLegacyTradePetAvailable589568;
            ui::detail::gHoverTextD68AE8 = hint;
        }
    }
    Sub_4E71A0(1);
    const std::int32_t y = gLegacyTradePanel582F00.y + 180;
    const std::int32_t x = gLegacyTradePanel582F00.x + 419;
    const std::int32_t result = DrawGraphic(x, y, 212, 23243, 0, nullptr);
    gLegacyTradeLocalPetPrevious582FD0 = hovered;
    return result;
}

// 原函数：0x004E8AD0。
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
    const auto drawAt = [](const std::wstring_view text, const std::int32_t offsetY) {
        const std::int32_t top = gLegacyTradePanel582F00.y + offsetY;
        const std::int32_t left = gLegacyTradePanel582F00.x + 421;
        static_cast<void>(DrawText(static_cast<std::int16_t>(left),
            static_cast<std::int16_t>(top), 2, 0, 0, text, 0, 0));
    };
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:7}", money);
    drawAt(buffer, 118);
    cg::text::FormatTo(buffer, L"{:7}", ui::detail::gTradeValueD046D0);
    drawAt(buffer, 158);
    for (auto& digit : digits) {
        Sub_5035E0(&digit, 213, 1, gValueD69768, 0);
    }
    Sub_5035E0(&backspace, 213, 1, gValueD69768, 0);
    Sub_5035E0(&all, 213, 1, gValueD69768, 0);
    Sub_5035E0(&clear, 213, 1, gValueD69768, 0);
    const wchar_t* hint = nullptr;
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
        ui::detail::gHoverTextD68AE8 = hint;
    }
    const std::int32_t backgroundY = gLegacyTradePanel582F00.y + 180;
    const std::int32_t backgroundX = gLegacyTradePanel582F00.x + 419;
    return DrawGraphic(backgroundX, backgroundY, 212, 23244, 0, nullptr);
}

// 原函数：0x004E9280。
void Sub_4E9280()
{
    gLegacyEngravingPageC8AA9C = 0;
    gLegacyEngravingTargetCC1A14 = -1;
}

// 原函数：0x004E97C0。
void Sub_4E97C0()
{
    gLegacyBurialPageCC4B64 = 0;
    gLegacyBurialSelectedCC4214 = -1;
}

// 原函数：0x004E9C60。
void Sub_4E9C60()
{
    gLegacyMailGiftPageCC2E58 = 0;
    gLegacyMailGiftSelectedCC1A2C = -1;
}

// 原函数：0x004EA130。
void Sub_4EA130()
{
    const std::wstring preserved =
        input::ContextText(input::gInputContextCB03F0);
    const std::int32_t y = gLegacyMailComposePanel583230.y;
    const std::int32_t x = gLegacyMailComposePanel583230.x;
    input::InitializeContext_449CF0(&input::gInputContextCB03F0,
        x + 10, y + 219, 2, 0, 0, preserved, 10, 28, 20, 0);
    input::SetContext_44B330(&input::gInputContextCB03F0);
}

// 原函数：0x004EC1D0。
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

// 原函数：0x004EAA50。
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

// 原函数：0x004EB120。
std::int32_t Sub_4EB120()
{
    const std::wstring preserved =
        input::ContextText(input::gInputContextD66CA8);
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

// 原函数：0x0052D790。
void Sub_52D790()
{
    gLegacyGuildGiftPage1050DAC = 0;
    gLegacyGuildGiftSelected1050DA8 = -1;
}

// 原函数：0x0052DC60。
void Sub_52DC60()
{
    const std::wstring preserved =
        input::ContextText(input::gInputContext1050B80);
    const std::int32_t y = gLegacyGuildComposePanel58EA18.y;
    const std::int32_t x = gLegacyGuildComposePanel58EA18.x;
    input::InitializeContext_449CF0(&input::gInputContext1050B80,
        x + 10, y + 219, 2, 0, 0, preserved, 10, 28, 20, 0);
    input::SetContext_44B330(&input::gInputContext1050B80);
}

// 原函数：0x0052E600。
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

// 原函数：0x0052ECB0。
std::int32_t Sub_52ECB0()
{
    const std::wstring preserved =
        input::ContextText(input::gInputContext1051348);
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

// 原函数：0x0052FCF0。
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


// 原函数：0x00530B70。
void Sub_530B70()
{
    if (ReadGuildKey10514A8() == -1) {
        gLegacyGuildEditAllowed1050F14 = 0;
        return;
    }
    gLegacyGuildEditAllowed1050F14 = ReadGuildTextIndex10514B0() == 0 ? 1 : 0;
}

// 原函数：0x00532430。
std::int32_t Sub_532430()
{
    const std::int32_t rank = ReadGuildTextIndex10514B0();
    const std::int32_t allowed = rank == 0 ? 1 : 0;
    gLegacyGuildEditAllowed1050F14 = allowed;
    return allowed;
}

// 原函数：0x004F24A0。
std::int32_t Sub_4F24A0()
{
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    const std::wstring request = gLegacyItemWorkDraftCC31E0;
    const std::string source = cg::text::ToGbk(gLegacyItemWorkDraftCC31E0);
    if (source.size() >= 64) {
        throw cg::text::GbkStorageError(122, std::system_category(), "名称超过GBK字段容量");
    }
    NullSub_509080();
    const std::int32_t item = gLegacyItemWorkSelection584394 + 8;
    const std::int32_t level = ui::detail::WindowSkill_ReadRequestFlag();
    const std::uintptr_t socket = platform::gConnectionSocket;
    static_cast<void>(network::Sub_511EF0(socket, skill, level, item, request));
    if (!source.empty()) {
        gLegacyItemWorkNameCC40D8 = gLegacyItemWorkDraftCC31E0;
    } else {
        gLegacyItemWorkNameCC40D8 = world::detail::BorrowPlayerItemsFDA8A4()[gLegacyItemWorkSelection584394 + 8].name;
    }
    const std::int32_t selected = gLegacyItemWorkSelection584394;
    const std::uint32_t panelId = gLegacyItemWorkPanel5842D0.id;
    gLegacyItemWorkImageCC320C = world::detail::BorrowPlayerItemsFDA8A4()[selected + 8].imageNo;
    return ui::Sub_5042D0(panelId);
}

// 原函数：0x004F25C0。
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

// 原函数：0x004F2650。
std::int32_t Sub_4F2650()
{
    const std::int32_t gate = gLegacyItemWorkInitGateD67CE8;
    if (gate != 0) {
        return gate;
    }
    const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
    static_cast<void>(ui::detail::WindowProfile_SaveOrigin(x, y));
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    gLegacyItemWorkSelection584394 = -1;
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    const std::int32_t skillId = world::detail::gSkillsF1BAFC[skill].skillId;
    ui::detail::WindowCraft_WriteSkill(skillId);
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

// 原函数：0x004F2720。
std::int32_t Sub_4F2720(const std::int32_t cost, const std::int32_t skill)
{
    if (world::detail::IsCostGreaterThanPlayerFp(cost)) {
        gLegacyItemWorkPhaseCC4220 = 3;
        return 1;
    }
    const std::int32_t skillId = ui::detail::WindowCraft_ReadSkill();
    if (skillId == 219 || skillId == 2033) {
        const auto* const item = &world::detail::BorrowPlayerItemsFDA8A4()[gLegacyItemWorkSelection584394 + 8];
        if (item->hasDescription != 0) {
            gLegacyItemWorkPhaseCC4220 = 4;
            return 1;
        }
        if (item->level > world::detail::gSkillsF1BAFC[skill].level) {
            gLegacyItemWorkPhaseCC4220 = 12;
            return 1;
        }
    } else if (skillId == 217 || skillId == 2031 || skillId == 218 || skillId == 2032) {
        const auto* item = &world::detail::BorrowPlayerItemsFDA8A4()[gLegacyItemWorkSelection584394 + 8];
        if (item->hasDescription == 0) {
            gLegacyItemWorkPhaseCC4220 = 5;
            return 1;
        }
        const std::int32_t rejectedKind = skillId == 217 || skillId == 2031 ? 2 : 1;
        if (ui::detail::WindowCraft_ClassifyWork(item->itemType) == rejectedKind) {
            gLegacyItemWorkPhaseCC4220 = 10;
            return 1;
        }
        item = &world::detail::BorrowPlayerItemsFDA8A4()[gLegacyItemWorkSelection584394 + 8];
        if (ui::detail::WindowCraft_ClassifyWork(item->itemType) == 0) {
            gLegacyItemWorkPhaseCC4220 = 11;
            return 1;
        }
        item = &world::detail::BorrowPlayerItemsFDA8A4()[gLegacyItemWorkSelection584394 + 8];
        if (item->level > world::detail::gSkillsF1BAFC[skill].level) {
            gLegacyItemWorkPhaseCC4220 = 13;
            return 1;
        }
    } else if (skillId == 220) {
        const auto* const item = &world::detail::BorrowPlayerItemsFDA8A4()[gLegacyItemWorkSelection584394 + 8];
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

// 原函数：0x004FB940。
void Sub_4FB940(
    const std::int32_t x, const std::int32_t y, const std::int32_t index, const std::int32_t page)
{
    if (index < 0 || index >= 28) {
        return;
    }
    const auto* const item = &world::detail::BorrowPlayerItemsFDA8A4()[index];
    if (item->used == 0) {
        return;
    }
    Sub_504BA0(x, y, 272, 4, 0, item->name, item->memo,
        0, 0, std::bit_cast<std::int16_t>(item->flags));
    for (std::int32_t row = 0; row < 8; ++row) {
        const std::wstring& text = item->description[page * 8 + row];
        if (!text.empty()) {
            static_cast<void>(ui::detail::WindowItem_SplitTooltipLines(x + 10, y + 34 + 18 * row, 4, 1, 0, text, 0, 0));
        }
    }
    static_cast<void>(DrawGraphic(x + 136, y + 92, 222, 22541, 0, nullptr));
}

// 原函数：0x00504BA0。
void Sub_504BA0(
    const std::int32_t x, const std::int32_t y, const std::int32_t width, const std::uint8_t layer,
    const std::int32_t font, const std::wstring_view name, const std::wstring_view memo,
    const std::int32_t, const std::int32_t, const std::int16_t flags)
{
    std::int32_t textWidth = TextMeasure(name, font);
    std::wstring_view text = name;
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

// 原函数：0x004F4470。
std::int32_t Sub_4F4470()
{
    const std::int32_t pair = std::bit_cast<std::int8_t>(ui::detail::gSkillPairIndexD67AD8);
    const std::int32_t skill = std::bit_cast<std::int16_t>(world::detail::gSkillPairsF63370[pair].index);
    const std::int32_t skillId = world::detail::gSkillsF1BAFC[skill].skillId;
    for (std::int32_t first = 0; first < 20; ++first) {
        if (gLegacyCraftMarksC8AA7C[first] != 0) {
            for (std::int32_t second = 0; second < 20; ++second) {
                if (first != second && gLegacyCraftMarksC8AA7C[second] != 0
                    && world::detail::BorrowPlayerItemsFDA8A4()[first + 8].itemId
                        == world::detail::BorrowPlayerItemsFDA8A4()[second + 8].itemId) {
                    return 2;
                }
            }
        }
    }
    if (skillId != 215 && skillId != 216) {
        bool foundSpecial = false;
        for (std::int32_t slot = 0; slot < 20; ++slot) {
            if (gLegacyCraftMarksC8AA7C[slot] != 0
                && ui::detail::WindowCraft_ClassifySpecialMaterial(world::detail::BorrowPlayerItemsFDA8A4()[slot + 8].itemType, skillId) != 0) {
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
        const auto* const item = &world::detail::BorrowPlayerItemsFDA8A4()[slot + 8];
        if (skillId != 215 && skillId != 216) {
            const std::int32_t special = ui::detail::WindowCraft_ClassifySpecialMaterial(item->itemType, skillId);
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
                const auto* const item = &world::detail::BorrowPlayerItemsFDA8A4()[slot + 8];
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

// 原函数：0x004F4690。
void Sub_4F4690()
{
    std::int32_t normalCount = 0;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        if (gLegacyCraftMarksC8AA7C[slot] != 0) {
            const auto* const item = &world::detail::BorrowPlayerItemsFDA8A4()[slot + 8];
            const bool special = ui::detail::WindowCraft_ClassifySpecialMaterial(item->itemType, 0) != 0;
            const std::int32_t itemId = item->itemId;
            const std::int32_t image = item->imageNo;
            const std::int32_t destination = special ? 5 : normalCount;
            ui::detail::gSelectionImagesCF3FF4[destination] = image;
            const std::int32_t count = ui::detail::WindowCraft_CountRecipeMaterial(itemId);
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

// 原函数：0x004F4720。
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

// 原函数：0x004F4740。
std::int32_t Sub_4F4740()
{
    gLegacyCraftMarksC8AA7C.fill(0);
    return 0;
}

// 原函数：0x004F4760。
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

// 原函数：0x004F50D0。
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

// 原函数：0x004F5170。
std::int32_t Sub_4F5170()
{
    ui::detail::gSelectedSlotsCA0A90.fill(-1);
    ui::detail::gSelectionCountsD66E0C.fill(-1);
    ui::detail::gSelectionImagesCF3FF4.fill(-1);
    return -1;
}

// 原函数：0x004F51A0。
std::int32_t Sub_4F51A0()
{
    static_cast<void>(Sub_4F5170());
    gLegacyCraftPhaseD67D00 = 0;
    const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
    return ui::detail::WindowProfile_SaveOrigin(x, y);
}

// 原函数：0x00504EE0。
std::int32_t Sub_504EE0()
{
    gLegacyGemDragSlotD68AA8 = -1;
    ui::detail::gItemMarksD68A90.fill(0);
    gLegacyGemPhaseD68A8C = 0;
    gLegacyGemDragImageD68AA4 = 0;
    return 0;
}

// 原函数：0x00504FA0。
std::int32_t Sub_504FA0()
{
    bool foundNormal = false;
    bool foundSpecial = false;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        if (ui::detail::gItemMarksD68A90[slot] == 0) {
            continue;
        }
        const auto* const item = &world::detail::BorrowPlayerItemsFDA8A4()[slot + 8];
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

// 原函数：0x00505BC0。
std::int32_t Sub_505BC0()
{
    const std::int32_t playerY = world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t playerX = world::detail::BorrowPlayerXStorage()->value();
    static_cast<void>(ui::detail::WindowProfile_SaveOrigin(playerX, playerY));
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

// 原函数：0x005062A0。
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

// 原函数：0x00519640。
std::int32_t Sub_519640(const std::int32_t skill)
{
    std::int32_t count = 0;
    while (count < 56 && world::detail::gSkillsF1BAFC[skill].recipes8E8[count].itemId >= 0) {
        ++count;
    }
    return count == 0 ? 1 : (count - 1) / 8 + 1;
}

// 原函数：0x004F61C0。
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

// 原函数：0x004F6BF0。
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

// 原函数：0x004F6C60。
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

// 原函数：0x004F7140。
std::int32_t Sub_4F7140()
{
    gLegacyPetStateOpenD67D18 = 0;
    gLegacyPetStatePreviousD2D48C = 0;
    return 0;
}

// 原[4F7870,4F8336)完整634条；唯一4F7150 caller弃返回，沿已证void合同。
void Sub_4F7870(const std::int32_t x, const std::int32_t y)
try {
    std::int32_t hoveredRow = -1; // D01/P06：仅入口补原var_228。
    std::int32_t hoveredState = -1; // D01/P06：仅入口补原var_224。
    LegacyPanelButtonRecord confirm {AddW32(x, 48), AddW32(y, 228),
        AddW32(x, 10), AddW32(y, 410), 76, 18, 22920, 22921};
    LegacyPanelButtonRecord cancel {AddW32(x, 48), AddW32(y, 228),
        AddW32(x, 10), AddW32(y, 431), 76, 18, 22922, 22923};
    std::array<LegacyPanelButtonRecord, 15> buttons;
    constexpr std::array<std::int32_t, 3> images {22915, 22913, 22911};
    for (std::int32_t row = 0; row < 5; ++row) {
        for (std::int32_t option = 0; option < 3; ++option) {
            buttons[3 * row + option] = {AddW32(x, 48), AddW32(y, 228 + 75 * row),
                AddW32(x, 10), AddW32(y, 78 - 21 * option + 75 * row),
                76, 18, images[option], images[option] + 1};
        }
    }
    if (gLegacyPetStateOpenD67D18 == 1 && gLegacyPetStatePreviousD2D48C == 0) {
        std::int16_t battleCount = 0;
        std::int16_t standbyCount = 0;
        gLegacyPetBattleCountD45F8C = 0;
        gLegacyPetStandbyCountCC4D18 = 0;
        for (std::int32_t index = 0; index < 5; ++index) {
            const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[index];
            gLegacyPetStateValuesCF4E38[index] = pet.used != 0
                ? std::bit_cast<std::int16_t>(pet.flags6AA) : 0;
            const std::int16_t state = gLegacyPetStateValuesCF4E38[index];
            if (state == 1) {
                ++standbyCount;
            } else if (state == 2) {
                ++battleCount;
            }
        }
        gLegacyPetBattleCountD45F8C = battleCount;
        gLegacyPetStandbyCountCC4D18 = standbyCount;
        static_cast<void>(audio::PlaySe_52C450(56, 320));
    }
    if ((QueryPanelButtonState(&confirm) & 1) != 0) {
        const std::int32_t fifth = gLegacyPetStateValuesCF4E38[4];
        const std::int32_t fourth = gLegacyPetStateValuesCF4E38[3];
        const std::int32_t third = gLegacyPetStateValuesCF4E38[2];
        const std::int32_t second = gLegacyPetStateValuesCF4E38[1];
        const std::int32_t first = gLegacyPetStateValuesCF4E38[0];
        const std::uintptr_t socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_511AE0(socket, first, second, third, fourth, fifth));
        gLegacyPetStateOpenD67D18 = 0;
        static_cast<void>(audio::PlaySe_52C450(78, 320));
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        gLegacyPetStateOpenD67D18 = 0;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else {
        hoveredRow = -1; // 原正常扫描分支仍写一次，不外推入口适配。
        hoveredState = -1;
        bool clicked = false;
        for (std::int32_t row = 0; row < 5 && !clicked; ++row) {
            if (world::detail::gPetPairsDE0B08[row].used == 0) {
                continue;
            }
            for (std::int32_t option = 0; option < 3; ++option) {
                LegacyPanelButtonRecord* const button = &buttons[3 * row + option];
                if ((QueryPanelButtonState(button) & 0x40) != 0) {
                    hoveredState = option;
                    hoveredRow = row;
                }
                if ((QueryPanelButtonState(button) & 1) == 0) {
                    continue;
                }
                clicked = true;
                const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
                const std::int16_t state = gLegacyPetStateValuesCF4E38[index];
                if (option == 2) {
                    if (state != 2) {
                        const std::int16_t battleCount = gLegacyPetBattleCountD45F8C;
                        const std::int16_t standbyCount = gLegacyPetStandbyCountCC4D18;
                        if (battleCount == 0 && (standbyCount <= 2 || (standbyCount == 3 && state == 1))) {
                            if (state == 1) {
                                --gLegacyPetStandbyCountCC4D18;
                            }
                            gLegacyPetStateValuesCF4E38[index] = 2;
                            gLegacyPetBattleCountD45F8C = 1;
                            static_cast<void>(audio::PlaySe_52C450(51, 320));
                        } else if (standbyCount > 2 || battleCount != 1) {
                            static_cast<void>(audio::PlaySe_52C450(55, 320));
                        } else {
                            std::int32_t existing = 0;
                            while (existing < 5 && gLegacyPetStateValuesCF4E38[existing] != 2) {
                                ++existing;
                            }
                            if (existing < 5) {
                                gLegacyPetStateValuesCF4E38[existing] = state;
                            }
                            gLegacyPetStateValuesCF4E38[index] = 2;
                        }
                    }
                } else if (option == 1) {
                    if (state != 1) {
                        const std::int32_t standbyCount = gLegacyPetStandbyCountCC4D18;
                        const std::int32_t battleCount = gLegacyPetBattleCountD45F8C;
                        if (standbyCount + battleCount <= 2 || state == 2) {
                            if (state == 2) {
                                --gLegacyPetBattleCountD45F8C;
                            }
                            ++gLegacyPetStandbyCountCC4D18;
                            gLegacyPetStateValuesCF4E38[index] = 1;
                            static_cast<void>(audio::PlaySe_52C450(51, 320));
                        } else {
                            static_cast<void>(audio::PlaySe_52C450(55, 320));
                        }
                    }
                } else if (state != 0) {
                    if (state == 2) {
                        --gLegacyPetBattleCountD45F8C;
                    } else if (state == 1) {
                        --gLegacyPetStandbyCountCC4D18;
                    }
                    gLegacyPetStateValuesCF4E38[index] = 0;
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
                break;
            }
        }
    }
    for (std::int32_t row = 0; row < 5; ++row) {
        if (world::detail::gPetPairsDE0B08[row].used == 0) {
            continue;
        }
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
        for (std::int32_t option = 0; option < 3; ++option) {
            const std::int32_t selected = gLegacyPetStateValuesCF4E38[index] == option ? 1 : 0;
            Sub_5038B0(&buttons[3 * row + option], 212, 1, gValueD69768, selected);
        }
    }
    Sub_5035E0(&confirm, 212, 1, gValueD69768, 0);
    Sub_5035E0(&cancel, 212, 1, gValueD69768, 0);
    static_cast<void>(DrawGraphic(AddW32(x, 48), AddW32(y, 228), 212, 22910, 1, nullptr));
    const wchar_t* hint = nullptr;
    if ((QueryPanelButtonState(&confirm) & 0x40) != 0) {
        hint = kLegacyPetStateConfirm58A1F0;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyPetStateCancel58A1D8;
    } else if (hoveredRow >= 0) {
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[hoveredRow].index);
        if (hoveredState == 2) {
            const std::int16_t state = gLegacyPetStateValuesCF4E38[index];
            if (state == 2) {
                hint = kLegacyPetStateBattleCurrent586C5C;
            } else {
                const std::int16_t battleCount = gLegacyPetBattleCountD45F8C;
                const std::int16_t standbyCount = gLegacyPetStandbyCountCC4D18;
                if (battleCount == 0 && (standbyCount <= 2 || (standbyCount == 3 && state == 1))) {
                    hint = kLegacyPetStateBattle586C40;
                } else if (standbyCount > 2 || battleCount != 1) {
                    hint = kLegacyPetStateBattleRejected58A194;
                } else if (state == 1 || state == 0) {
                    hint = kLegacyPetStateSwap58A1B0;
                }
            }
        } else if (hoveredState == 1) {
            const std::int16_t state = gLegacyPetStateValuesCF4E38[index];
            if (state == 1) {
                hint = kLegacyPetStateStandbyCurrent586C94;
            } else {
                const std::int32_t standbyCount = gLegacyPetStandbyCountCC4D18;
                const std::int32_t battleCount = gLegacyPetBattleCountD45F8C;
                hint = standbyCount + battleCount <= 2 || state == 2
                    ? kLegacyPetStateStandby586C78 : kLegacyPetStateStandbyRejected58A174;
            }
        } else if (hoveredState == 0) {
            hint = gLegacyPetStateValuesCF4E38[index] != 0
                ? kLegacyPetStateRest58A158 : kLegacyPetStateRestCurrent58A140;
        }
    }
    if (hint != nullptr) {
        ui::detail::gHoverTextD68AE8 = hint;
    }
}
catch (const cg::text::GbkStorageError&) {
    ReportLegacyTextStorageFailure();
    return;
}

// 原函数：0x004F8340。
YobiAction* Sub_4F8340()
{
    YobiAction* action = gLegacyPetDetailActionD67AE0;
    if (action == nullptr) {
        const std::int32_t y = gLegacyPetDetailPanel584918.y + 160;
        const std::int32_t position = gLegacyPetSelectionD67ADC;
        const std::int32_t x = gLegacyPetDetailPanel584918.x + 202;
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[position].index);
        const std::int32_t image = (*world::detail::BorrowPetRuntimeF633F8Storage())[index].imageNo6A0;
        action = Sub_4F9FF0(image, x, y, 221);
        gLegacyPetDetailActionD67AE0 = action;
        if (action != nullptr) {
            SetYobiActionField2B8(action, 6);
        }
    }
    return action;
}

// 原函数：0x004F8550。
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
            static_cast<void>(ui::Sub_504380(22));
        }
        static_cast<void>(ui::Sub_504180(22));
    }
    const auto index = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLegacyPetSelectionD67ADC].index);
    auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[index];
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
    const auto drawAt = [x, y](const std::wstring_view text, const std::int32_t offsetX,
        const std::int32_t offsetY) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 2, 1, 0, text, 0, 0));
    };
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", pet.value08);
    drawAt(buffer, 62, 44);
    cg::text::FormatTo(buffer, L"{:9}", pet.values0C[4].value());
    drawAt(buffer, 55, 66);
    if (pet.value08 < 130) {
        cg::text::FormatTo(buffer, L"{:9}", pet.values0C[5].value());
        drawAt(buffer, 55, 89);
    } else {
        drawAt(kLegacyPetLevelLimit58A214, 55, 89);
    }
    const std::int32_t maxHp = pet.values0C[1].value();
    const std::int32_t hp = pet.values0C[0].value();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    drawAt(buffer, 55, 113);
    const std::int32_t maxFp = pet.values0C[3].value();
    const std::int32_t fp = pet.values0C[2].value();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    drawAt(buffer, 55, 136);
    for (std::int32_t field = 0; field < 6; ++field) {
        cg::text::FormatTo(buffer, L"{:3}", pet.values70[5 + field]);
        drawAt(buffer, 62, 159 + 23 * field);
    }
    cg::text::FormatTo(buffer, L"{:2}", pet.value6A4);
    drawAt(buffer, 70, 297);
    buffer = ui::detail::gTribeNames57CB54[std::bit_cast<std::int16_t>(pet.value04)];
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
    cg::text::FormatTo(buffer, L"{}/{}", position + 1, count);
    const std::int32_t pageY = gLegacyPetDetailPanel584918.y + 382;
    const std::int32_t pageX = gLegacyPetDetailPanel584918.x + 180;
    static_cast<void>(DrawText(static_cast<std::int16_t>(pageX),
        static_cast<std::int16_t>(pageY), 2, 1, 0, buffer, 0, 0));
    const std::int32_t healthImage = ui::detail::WindowPet_ReadHealthImage(pet.values70[25]);
    const std::int32_t healthY = gLegacyPetDetailPanel584918.y + 222;
    const std::int32_t healthX = gLegacyPetDetailPanel584918.x + 184;
    static_cast<void>(DrawGraphic(healthX, healthY, 212, healthImage, 0, nullptr));
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    Sub_5035E0(&rename, 212, 1, gValueD69768, 0);
    const wchar_t* hint = nullptr;
    if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        hint = kLegacyPetBack58A204;
    } else if ((QueryPanelButtonState(&rename) & 0x40) != 0) {
        hint = kLegacyPetRename586D38;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0 || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    }
    if (hint != nullptr) {
        ui::detail::gHoverTextD68AE8 = hint;
    }
    return 0;
}

// 原函数：0x004F8E40。
std::int32_t Sub_4F8E40(const std::int32_t x, const std::int32_t y)
try {
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
    auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[index];
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
    const auto drawAt = [x, y](const std::wstring_view text, const std::int32_t offsetX,
        const std::int32_t offsetY) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 2, 1, 0, text, 0, 0));
    };
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", pet.value08);
    drawAt(buffer, 62, 36);
    const std::int32_t maxHp = pet.values0C[1].value();
    const std::int32_t hp = pet.values0C[0].value();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    drawAt(buffer, 55, 59);
    const std::int32_t maxFp = pet.values0C[3].value();
    const std::int32_t fp = pet.values0C[2].value();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    drawAt(buffer, 55, 82);
    for (std::int32_t field = 0; field < 5; ++field) {
        cg::text::FormatTo(buffer, L"{:3}", pet.values70[5 + field]);
        drawAt(buffer, 62, 105 + 23 * field);
    }
    for (std::int32_t field = 0; field < 6; ++field) {
        cg::text::FormatTo(buffer, L"{:+4}", pet.values70[11 + field]);
        drawAt(buffer, 69, 243 + 23 * field);
    }
    for (std::int32_t field = 0; field < 5; ++field) {
        cg::text::FormatTo(buffer, L"{:3}", pet.values70[field]);
        drawAt(buffer, 192, 82 + 23 * field);
    }
    cg::text::FormatTo(buffer, L"{:3}", pet.values70[10]);
    drawAt(buffer, 186, 202);
    cg::text::FormatTo(buffer, L"{:2}", pet.value6A4);
    drawAt(buffer, 194, 227);
    for (std::int32_t field = 0; field < 4; ++field) {
        cg::text::FormatTo(buffer, L"{:+4}", pet.values70[17 + field]);
        drawAt(buffer, 193, 273 + 23 * field);
    }
    const std::int32_t position = gLegacyPetSelectionD67ADC;
    const std::int32_t count = world::detail::gPetCountDE0B00;
    cg::text::FormatTo(buffer, L"{}/{}", position + 1, count);
    const std::int32_t pageY = gLegacyPetDetailPanel584918.y + 388;
    const std::int32_t pageX = gLegacyPetDetailPanel584918.x + 180;
    static_cast<void>(DrawText(static_cast<std::int16_t>(pageX),
        static_cast<std::int16_t>(pageY), 2, 1, 0, buffer, 0, 0));
    cg::text::FormatTo(buffer, L"{:3}", pet.lvFlag6C);
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
    const wchar_t* hint = nullptr;
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
        ui::detail::gHoverTextD68AE8 = hint;
    }
    return 0;
}
catch (const cg::text::GbkStorageError&) {
    ReportLegacyTextStorageFailure();
    return 0;
}

// 原函数：0x004F9B10。
std::int32_t Sub_4F9B10(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t panelX = gLegacyPetDetailPanel584918.x;
    const std::int32_t panelY = gLegacyPetDetailPanel584918.y;
    LegacyPanelButtonRecord back {panelX + 70, panelY + 393, panelX + 30, panelY + 383, 80, 20, 22830, 22831};
    LegacyPanelButtonRecord previous {panelX + 155, panelY + 393, panelX + 141, panelY + 383, 28, 20, 22832, 22833};
    LegacyPanelButtonRecord next {panelX + 229, panelY + 393, panelX + 215, panelY + 383, 28, 20, 22834, 22835};
    const auto index = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLegacyPetSelectionD67ADC].index);
    const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[index];
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
    std::wstring buffer;
    for (std::int32_t row = 0; row < slots; ++row) {
        const auto& pair = pet.techniquePairs650[row];
        if (pair.used != 0) {
            const auto skill = std::bit_cast<std::int16_t>(pair.index);
            const auto& technique = pet.techniquesD8[skill];
            static_cast<void>(DrawText(static_cast<std::int16_t>(x + 39),
                static_cast<std::int16_t>(y + 37 + 24 * row), 2, 0, 0, cg::text::FromGbkWide(technique.name.data()).c_str(), 0, 0));
            cg::text::FormatTo(buffer, L"{:4}", technique.fp);
            static_cast<void>(DrawText(static_cast<std::int16_t>(x + 211),
                static_cast<std::int16_t>(y + 37 + 24 * row), 2, 1, 0, buffer, 0, 0));
        }
        static_cast<void>(DrawGraphic(x + 136, y + 46 + 24 * row, 212, 22825, 0, nullptr));
    }
    if (hovered >= 0 && pet.techniquePairs650[hovered].used != 0) {
        const auto skill = std::bit_cast<std::int16_t>(pet.techniquePairs650[hovered].index);
        const char* const description = pet.techniquesD8[skill].description.data();
        for (std::int32_t line = 0; line < 4; ++line) {
            if (input::ExtractGbkTextLine(buffer, description, line, 26) != 0) {
                static_cast<void>(DrawText(static_cast<std::int16_t>(x + 19),
                    static_cast<std::int16_t>(y + 294 + 19 * line), 2, 0, 0, buffer, 0, 0));
            }
        }
    }
    const std::int32_t position = gLegacyPetSelectionD67ADC;
    const std::int32_t count = world::detail::gPetCountDE0B00;
    cg::text::FormatTo(buffer, L"{}/{}", position + 1, count);
    const std::int32_t pageY = gLegacyPetDetailPanel584918.y + 386;
    const std::int32_t pageX = gLegacyPetDetailPanel584918.x + 180;
    static_cast<void>(DrawText(static_cast<std::int16_t>(pageX),
        static_cast<std::int16_t>(pageY), 2, 1, 0, buffer, 0, 0));
    Sub_5035E0(&back, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    const wchar_t* hint = nullptr;
    if ((QueryPanelButtonState(&back) & 0x40) != 0) {
        hint = kLegacyPetBack58A204;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0 || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    }
    if (hint != nullptr) {
        ui::detail::gHoverTextD68AE8 = hint;
    }
    return 0;
}

std::int32_t Sub_503BB0()
{
    return ui::Sub_504460(2);
}

std::int32_t Sub_503D00()
{
    world::detail::Sub_4FA8E0(0);
    return platform::Rand16();
}

std::int32_t Sub_503D10()
{
    world::detail::Sub_4FA8E0(2);
    return platform::Rand16();
}

std::int32_t Sub_503D20()
{
    world::detail::Sub_4FA8E0(4);
    return platform::Rand16();
}

std::int32_t Sub_503D30()
{
    world::detail::Sub_4FA8E0(6);
    return platform::Rand16();
}

std::int32_t Sub_503D40()
{
    world::detail::Sub_4FA8E0(8);
    return platform::Rand16();
}

std::int32_t Sub_503D50()
{
    world::detail::Sub_4FA8E0(10);
    return platform::Rand16();
}

std::int32_t Sub_503D60()
{
    world::detail::Sub_4FA8E0(1);
    return platform::Rand16();
}

std::int32_t Sub_503D70()
{
    world::detail::Sub_4FA8E0(3);
    return platform::Rand16();
}

std::int32_t Sub_503D80()
{
    world::detail::Sub_4FA8E0(7);
    return platform::Rand16();
}

std::int32_t Sub_503D90()
{
    world::detail::Sub_4FA8E0(5);
    return platform::Rand16();
}

std::int32_t Sub_503DA0()
{
    world::detail::Sub_4FA8E0(9);
    return platform::Rand16();
}

std::int32_t Sub_503DB0()
{
    world::detail::Sub_4FA8E0(12);
    return platform::Rand16();
}

std::uintptr_t Sub_503DC0()
{
    const std::int32_t selected = platform::Rand16() % 3;
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

// 原函数：0x00402B30。
std::int32_t Sub_402B30()
{
    if (ui::Sub_5043D0(2) != 0) {
        static_cast<void>(ui::Sub_5042D0(100));
        return audio::PlaySe_52C450(57, 320);
    }
    return ui::Sub_504180(98);
}

// 原函数：0x00503B70。
std::int32_t Sub_503B70()
{
    if (ui::Sub_5043D0(2) != 0) {
        static_cast<void>(ui::Sub_5042D0(100));
        return audio::PlaySe_52C450(57, 320);
    }
    return ui::Sub_504180(3);
}

// 原函数：0x00503BC0。
std::int32_t Sub_503BC0()
{
    static_cast<void>(ui::Sub_504460(18));
    return ui::Sub_5042D0(31);
}

// 原函数：0x00503BE0。
std::int32_t Sub_503BE0()
{
    if (ui::Sub_504410(16) != 0) {
        static_cast<void>(ui::Sub_5042D0(16));
        return audio::PlaySe_52C450(57, 320);
    }
    if (ui::Sub_504410(15) != 0) {
        static_cast<void>(ui::Sub_5042D0(15));
        return audio::PlaySe_52C450(57, 320);
    }
    return ui::Sub_504460(14);
}

// 原函数：0x00503C30。
std::int32_t Sub_503C30()
{
    return ui::Sub_504460(42);
}

// 原函数：0x00503C40。
std::int32_t Sub_503C40()
{
    return ui::Sub_504460(23);
}

// 原函数：0x00503C50。
std::int32_t Sub_503C50()
{
    return ui::Sub_504460(1);
}

// 原函数：0x00503C60。
std::int32_t Sub_503C60()
{
    return ui::Sub_504460(20);
}

// 原函数：0x00503C70。
std::int32_t Sub_503C70()
{
    return ui::Sub_504460(17);
}

// 原函数：0x004FD690。
std::int32_t Sub_4FD690()
{
    gLegacyPresetPanel5850C8.selected = 0;
    gLegacyPresetRowD30408 = 0;
    return 0;
}

// 原函数：0x004FAED0。
std::int32_t Sub_4FAED0()
{
    const std::int32_t result = ui::detail::WindowItem_ResetItemDrag();
    gLegacyInventoryPageD67D28 = 0;
    gLegacyInventoryHoverC8AF78 = -1;
    return result;
}

// 原函数：0x004FBB30。
void Sub_4FBB30()
{
    gLegacyMoneyAmountD2D618 = 0;
}

// 原函数：0x005008C0。
std::int32_t Sub_5008C0(const std::int32_t x, const std::int32_t y)
try {
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

    std::wstring buffer;
    const auto versionY = [](const std::int32_t oldY, const std::int32_t newY) {
        return character_select::state::gPukVersion >= 7 ? newY : oldY;
    };
    const auto drawAt = [&](const std::wstring_view text, const std::int32_t offsetX,
        const std::int32_t offsetY) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 2, 1, 0, text, 0, 0));
    };
    const auto drawNumber = [&](const std::int32_t value, const std::int32_t offsetX,
        const std::int32_t oldY, const std::int32_t newY, const std::wstring_view format) {
        cg::text::FormatTo(buffer, format, value);
        drawAt(buffer, offsetX, versionY(oldY, newY));
    };
    const std::int32_t tribeWidth = TextMeasure(
        ui::detail::gTribeNames57CB54[world::detail::ReadPlayerTribe()], 1);
    const std::int32_t tribe = world::detail::ReadPlayerTribe();
    const std::int32_t tribeY = versionY(168, 185);
    drawAt(ui::detail::gTribeNames57CB54[tribe], 220 - tribeWidth / 2, tribeY);
    drawNumber(world::detail::ReadPlayerDuelPoint(), 185, 145, 162, L"{:9}");
    const std::int32_t title = world::detail::ReadPlayerTitleIndex();
    if (title >= 0) {
        const std::int32_t width = TextMeasure(cg::text::FromGbkWide(world::detail::gTitlesFF0508[title].text.data()).c_str(), 1);
        const std::int32_t titleY = versionY(75, 68);
        const std::int32_t currentTitle = world::detail::ReadPlayerTitleIndex();
        drawAt(cg::text::FromGbkWide(world::detail::gTitlesFF0508[currentTitle].text.data()).c_str(), 198 - width / 2, titleY);
    }
    const char* const ownTitle = world::detail::BorrowPlayerDisplayText(2);
    if (std::strlen(ownTitle) != 0) {
        const std::int32_t width = TextMeasure(cg::text::FromGbkWide(ownTitle).c_str(), 1);
        const std::int32_t titleY = versionY(98, 91);
        drawAt(cg::text::FromGbkWide(world::detail::BorrowPlayerDisplayText(2)).c_str(), 198 - width / 2, titleY);
    }
    if (character_select::state::gPukVersion >= 7) {
        const std::int32_t rankIndex = ReadGuildTextIndex10514B0();
        const char* const rank = BorrowGuildRankText1051518(rankIndex);
        if (std::strlen(rank) != 0) {
            const std::int32_t width = TextMeasure(cg::text::FromGbkWide(rank).c_str(), 1);
            const std::int32_t currentRank = ReadGuildTextIndex10514B0();
            drawAt(cg::text::FromGbkWide(BorrowGuildRankText1051518(currentRank)).c_str(), 198 - width / 2, 114);
        }
    }
    const char* const job = world::detail::BorrowPlayerJobName();
    if (std::strlen(job) != 0) {
        const std::int32_t width = TextMeasure(cg::text::FromGbkWide(job).c_str(), 1);
        const std::int32_t jobY = versionY(52, 45);
        drawAt(cg::text::FromGbkWide(world::detail::BorrowPlayerJobName()).c_str(), 198 - width / 2, jobY);
    }
    drawNumber(world::detail::ReadPlayerLevel(), 62, 122, 140, L"{:3}");
    const std::int32_t maxHp = world::detail::ReadPlayerMaxHpOnce();
    const std::int32_t hp = world::detail::ReadPlayerHpOnce();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    drawAt(buffer, 55, versionY(191, 206));
    const std::int32_t maxFp = world::detail::ReadPlayerMaxFpOnce();
    const std::int32_t fp = world::detail::ReadPlayerFpOnce();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    drawAt(buffer, 55, versionY(214, 228));
    drawNumber(world::detail::ReadPlayerValue78(8), 62, 237, 250, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(9), 62, 260, 272, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(10), 62, 283, 294, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(11), 62, 306, 316, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(12), 62, 329, 338, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(13), 62, 352, 360, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(5), 193, 306, 316, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(6), 193, 329, 338, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(7), 193, 352, 360, L"{:3}");
    drawNumber(world::detail::ReadPlayerExperienceOnce(), 55, 145, 162, L"{:9}");
    std::wstring_view nextExperience = L"   ---   ";
    if (world::detail::ReadPlayerLevel() < 130) {
        const std::int32_t next = world::detail::ReadPlayerNextExperienceOnce();
        cg::text::FormatTo(buffer, L"{:9}", next);
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
    const std::int32_t injuryImage = ui::detail::WindowPet_ReadHealthImage(world::detail::ReadPlayerInjury());
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
        drawNumber(world::detail::ReadPlayerPenalty(), 241, 122, 139, L"x{}");
    }
    const std::int32_t hoveredPosition = std::bit_cast<std::int16_t>(world::detail::ReadPlayerValue1A0());
    if ((QueryPanelButtonState(&buttons[hoveredPosition]) & 0x40) != 0) {
        const wchar_t* const hint = world::detail::ReadPlayerValue1A0() != 0
            ? kLegacyProfileRear586344 : kLegacyProfileFront586360;
        ui::detail::gHoverTextD68AE8 = hint;
    }
    return 0;
}
catch (const cg::text::GbkStorageError&) {
    ReportLegacyTextStorageFailure();
    return 0;
}

// 原函数：0x005014A0。
std::int32_t Sub_5014A0(const std::int32_t x, const std::int32_t y)
try {
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
    std::wstring buffer;
    const auto drawNumber = [&](const std::int32_t value, const std::int32_t offsetX,
        const std::int32_t offsetY, const std::wstring_view format, const std::int8_t color = 0) {
        cg::text::FormatTo(buffer, format, value);
        static_cast<void>(DrawText(static_cast<std::int16_t>(x + offsetX),
            static_cast<std::int16_t>(y + offsetY), 2, 1, color, buffer, 0, 0));
    };
    drawNumber(world::detail::ReadPlayerLevel(), 62, 38, L"{:3}");
    const std::int32_t maxHp = world::detail::ReadPlayerMaxHpOnce();
    const std::int32_t hp = world::detail::ReadPlayerHpOnce();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 55),
        static_cast<std::int16_t>(y + 61), 2, 1, 0, buffer, 0, 0));
    const std::int32_t maxFp = world::detail::ReadPlayerMaxFpOnce();
    const std::int32_t fp = world::detail::ReadPlayerFpOnce();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 55),
        static_cast<std::int16_t>(y + 84), 2, 1, 0, buffer, 0, 0));
    drawNumber(world::detail::ReadPlayerValue78(8), 62, 107, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(9), 62, 130, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(10), 62, 153, L"{:3}");
    drawNumber(world::detail::ReadPlayerValueB0(0), 62, 176, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(11), 62, 199, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(12), 62, 222, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(13), 62, 245, L"{:3}");
    drawNumber(world::detail::ReadPlayerValueB0(1), 62, 269, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(5), 186, 206, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(6), 186, 228, L"{:3}");
    drawNumber(world::detail::ReadPlayerValue78(7), 186, 252, L"{:3}");
    drawNumber(world::detail::ReadPlayerValueD4(0), 194, 298, L"{:+4}");
    drawNumber(world::detail::ReadPlayerValueD4(1), 194, 321, L"{:+4}");
    drawNumber(world::detail::ReadPlayerValueD4(2), 194, 344, L"{:+4}");
    drawNumber(world::detail::ReadPlayerValueD4(3), 194, 367, L"{:+4}");
    drawNumber(world::detail::gValueD2F788, 220, 61, L"{:4}");
    const std::int8_t color = world::detail::ReadPlayerPenalty() != 0 ? 6 : 0;
    drawNumber(world::detail::ReadPlayerValue78(0), 192, 84, L"{:3}", color);
    drawNumber(world::detail::ReadPlayerValue78(1), 192, 107, L"{:3}", color);
    drawNumber(world::detail::ReadPlayerValue78(2), 192, 130, L"{:3}", color);
    drawNumber(world::detail::ReadPlayerValue78(3), 192, 153, L"{:3}", color);
    drawNumber(world::detail::ReadPlayerValue78(4), 192, 176, L"{:3}", color);
    drawNumber(world::detail::ReadPlayerValueBC(0), 69, 268, L"{:+4}");
    drawNumber(world::detail::ReadPlayerValueBC(1), 69, 291, L"{:+4}");
    drawNumber(world::detail::ReadPlayerValueBC(2), 69, 314, L"{:+4}");
    drawNumber(world::detail::ReadPlayerValueBC(3), 69, 337, L"{:+4}");
    drawNumber(world::detail::ReadPlayerValueBC(4), 69, 360, L"{:+4}");
    drawNumber(world::detail::ReadPlayerValueBC(5), 69, 383, L"{:+4}");
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
    const wchar_t* hint;
    switch (hovered) {
    case -2: hint = kLegacyAttributeLimit58A220; break;
    case 0: hint = kLegacyAttributeHint58AA08; break;
    case 1: hint = kLegacyAttributeHint58A274; break;
    case 2: hint = kLegacyAttributeHint58A9F4; break;
    case 3: hint = kLegacyAttributeHint58A24C; break;
    case 4: hint = kLegacyAttributeHint58A9E0; break;
    default: return 0;
    }
    ui::detail::gHoverTextD68AE8 = hint;
    return 0;
}
catch (const cg::text::GbkStorageError&) {
    ReportLegacyTextStorageFailure();
    return 0;
}

// 原函数：0x00501FD0。
std::int32_t Sub_501FD0(const std::int32_t x, const std::int32_t y)
try {
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
    std::wstring buffer;
    std::wstring text;
    if (gLegacyProfileInputD6885C == nullptr) {
        text = cg::text::FromGbkWide(world::detail::BorrowPlayerDisplayText(2));
        input::gInputContextD2E2E0.clearText();
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
            if (cg::text::FromGbkWide(world::detail::BorrowPlayerDisplayText(2)) != input::ContextText(*context)) {
                const std::wstring escapedText = input::EscapeTextAndNotify(input::ContextText(*context), 128);
                static_cast<void>(network::Sub_512360(platform::gConnectionSocket, escapedText));
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
                static_cast<std::int16_t>(y + row * 30 + 136), 2, 0, color, cg::text::FromGbkWide(title->text.data()), 0, 0));
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
    cg::text::FormatTo(buffer, L"{:2}/{:2}", page + 1, pages);
    static_cast<void>(DrawText(static_cast<std::int16_t>(x + 166),
        static_cast<std::int16_t>(y + 101), 2, 0, 0, buffer, 0, 0));
    Sub_5035E0(&submit, 212, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 212, 1, gValueD69768, 0);
    Sub_5035E0(&next, 212, 1, gValueD69768, 0);
    const std::int32_t deleteMode = gLegacyTitleDeleteCF4678;
    Sub_5038B0(&remove, 212, 1, gValueD69768, deleteMode);
    Sub_5035E0(&clear, 212, 1, gValueD69768, 0);
    const wchar_t* hint;
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
    ui::detail::gHoverTextD68AE8 = hint;
    return 0;
}
catch (const cg::text::GbkStorageError&) {
    ReportLegacyTextStorageFailure();
    return 0;
}

// 原函数：0x004FC420。
std::int32_t Sub_4FC420()
{
    return Sub_4FC430();
}

// 原函数：0x004FC430。
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

// 原函数：0x004FCB90。
std::int32_t Sub_4FCB90()
{
    return Sub_4FCBA0();
}

// 原函数：0x004FCBA0。
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

// 原函数：0x004FD9F0。
std::int32_t Sub_4FD9F0()
{
    return Sub_4FDA00();
}

// 原4FDA00：一次X/Y取样，按原交错顺序发布字段，保留+23H未写字节。
std::int32_t Sub_4FDA00()
{
    const std::int32_t x = gLegacyMusicSettings585190.x;
    const std::int32_t y = gLegacyMusicSettings585190.y;
    auto& records = gLegacyMusicRecordsD68150;
    records[0].label.textX = x + 20;
    records[1].label.textX = x + 20;
    records[2].label.textX = x + 124;
    records[1].label.textY = y + 60;
    records[2].label.textY = y + 60;
    records[2].label.top = y + 60;
    records[3].label.textY = y + 60;
    records[3].label.top = y + 60;
    records[0].label.textY = y + 40;
    records[2].label.left = x + 123;
    records[3].label.textX = x + 156;
    records[0].reserved21[0] = std::byte {0xFF};
    records[1].reserved21[0] = std::byte {0xFF};
    records[4].label.textX = x + 62;
    records[2].label.height = 18;
    records[3].label.height = 18;
    records[4].label.height = 18;
    records[0].label.left = 0;
    records[0].label.top = 0;
    records[0].label.width = 0;
    records[0].label.height = 0;
    records[0].label.text = kLegacyAudio58A6D4;
    records[0].callback = nullptr;
    records[0].color = 4;
    records[0].reserved21[1] = std::byte {0x00};
    records[0].hint = nullptr;
    records[1].label.left = 0;
    records[1].label.top = 0;
    records[1].label.width = 0;
    records[1].label.height = 0;
    records[1].label.text = kLegacyChat58A500;
    records[1].callback = nullptr;
    records[1].color = 0;
    records[1].reserved21[1] = std::byte {0x00};
    records[1].hint = nullptr;
    records[2].label.width = 19;
    records[2].label.text = kLegacyChat58A574;
    records[2].callback = &Sub_4FDD70;
    records[2].color = 0;
    records[2].reserved21[0] = std::byte {0x01};
    records[2].reserved21[1] = std::byte {0xF9};
    records[2].hint = nullptr;
    records[3].label.left = x + 155;
    records[3].label.width = 19;
    records[3].label.text = kLegacyChat58A570;
    records[3].callback = &Sub_4FDDC0;
    records[3].color = 0;
    records[3].reserved21[0] = std::byte {0x01};
    records[3].reserved21[1] = std::byte {0xF9};
    records[3].hint = nullptr;
    records[4].label.textY = y + 260;
    records[4].label.left = x + 60;
    records[4].label.top = y + 260;
    records[4].label.width = 69;
    records[4].label.text = kLegacyChat58A4F8;
    records[4].callback = &Sub_4FE3E0;
    records[4].color = 0;
    records[4].reserved21[0] = std::byte {0x01};
    records[4].reserved21[1] = std::byte {0xF9};
    records[4].hint = kLegacyChat58A4E8;
    return 0;
}


// 原函数：0x004FDE10。
std::int32_t Sub_4FDE10()
{
    return Sub_4FDE20();
}

// 原4FDE20：一次X/Y取样，按原交错顺序发布字段，保留+23H未写字节。
std::int32_t Sub_4FDE20()
{
    const std::int32_t x = gLegacySoundSettings585258.x;
    const std::int32_t y = gLegacySoundSettings585258.y;
    auto& records = gLegacySoundRecordsD68218;
    records[1].label.textY = y + 60;
    records[2].label.textX = x + 124;
    records[2].label.textY = y + 60;
    records[2].label.top = y + 60;
    records[3].label.textX = x + 156;
    records[3].label.textY = y + 60;
    records[3].label.top = y + 60;
    records[0].label.textY = y + 40;
    records[2].label.left = x + 123;
    records[3].label.left = x + 155;
    records[4].label.textY = y + 80;
    records[0].label.textX = x + 20;
    records[0].label.left = 0;
    records[0].label.top = 0;
    records[0].label.width = 0;
    records[0].label.height = 0;
    records[0].label.text = kLegacyAudio58A6D4;
    records[0].callback = nullptr;
    records[0].color = 4;
    records[0].reserved21[0] = std::byte {0xFF};
    records[0].reserved21[1] = std::byte {0x00};
    records[0].hint = nullptr;
    records[1].label.textX = x + 20;
    records[1].label.left = 0;
    records[1].label.top = 0;
    records[1].label.width = 0;
    records[1].label.height = 0;
    records[1].label.text = kLegacyChat58A500;
    records[1].callback = nullptr;
    records[1].color = 0;
    records[1].reserved21[0] = std::byte {0xFF};
    records[1].reserved21[1] = std::byte {0x00};
    records[1].hint = nullptr;
    records[2].label.width = 19;
    records[2].label.height = 18;
    records[2].label.text = kLegacyChat58A574;
    records[2].callback = &Sub_4FE320;
    records[2].color = 0;
    records[2].reserved21[0] = std::byte {0x01};
    records[2].reserved21[1] = std::byte {0xF9};
    records[2].hint = nullptr;
    records[3].label.width = 19;
    records[3].label.height = 18;
    records[3].label.text = kLegacyChat58A570;
    records[3].callback = &Sub_4FE370;
    records[3].color = 0;
    records[3].reserved21[0] = std::byte {0x01};
    records[3].reserved21[1] = std::byte {0xF9};
    records[3].hint = nullptr;
    records[4].label.textX = x + 20;
    records[4].label.left = 0;
    records[4].label.top = 0;
    records[4].label.width = 0;
    records[4].label.height = 0;
    records[4].label.text = kLegacyAudio58A720;
    records[4].callback = nullptr;
    records[4].color = 4;
    records[4].reserved21[0] = std::byte {0xFF};
    records[4].reserved21[1] = std::byte {0x00};
    records[4].hint = nullptr;
    records[5].label.textX = x + 20;
    records[5].label.textY = y + 100;
    records[5].label.left = 0;
    records[6].label.textX = x + 120;
    records[6].label.textY = y + 120;
    records[6].label.top = y + 120;
    records[6].label.left = x + 119;
    records[7].label.textX = x + 62;
    records[6].label.height = 18;
    records[7].label.height = 18;
    records[6].reserved21[1] = std::byte {0xF9};
    records[7].reserved21[1] = std::byte {0xF9};
    records[5].label.top = 0;
    records[5].label.width = 0;
    records[5].label.height = 0;
    records[5].label.text = kLegacyChat58A500;
    records[5].callback = nullptr;
    records[5].color = 0;
    records[5].reserved21[0] = std::byte {0xFF};
    records[5].reserved21[1] = std::byte {0x00};
    records[5].hint = nullptr;
    records[6].label.width = 54;
    records[6].label.text = kLegacyChat58A54C;
    records[6].callback = &Sub_4FE3B0;
    records[6].color = 0;
    records[6].reserved21[0] = std::byte {0x01};
    records[6].hint = nullptr;
    records[7].label.textY = y + 260;
    records[7].label.left = x + 60;
    records[7].label.top = y + 260;
    records[7].label.width = 69;
    records[7].label.text = kLegacyChat58A4F8;
    records[7].callback = &Sub_4FE3E0;
    records[7].color = 0;
    records[7].reserved21[0] = std::byte {0x01};
    records[7].hint = kLegacyChat58A4E8;
    return 0;
}


// 原函数：0x004FE490。
std::int32_t Sub_4FE490()
{
    return Sub_4FE4A0();
}

// 原函数：0x004FE4A0。
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

// 原函数：0x004FEB00。
std::int32_t Sub_4FEB00()
{
    return Sub_4FEB10();
}

// 原函数：0x004FEB10。
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

// 原函数：0x004FF020。
std::int32_t Sub_4FF020()
{
    return Sub_4FF030();
}

// 原函数：0x004FF030。
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

// 原函数：0x004FF340。
std::int32_t Sub_4FF340()
{
    return Sub_4FF350();
}

// 原函数：0x004FF350。
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

// 原函数：0x00502930。
std::int32_t Sub_502930()
{
    return Sub_502940();
}

// 原函数：0x00502940。
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

// 原函数：0x00502B80。
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

// 原函数：0x00502C30。
std::int32_t Sub_502C30()
{
    return Sub_502C40();
}

// 原函数：0x00502C40。
std::int32_t Sub_502C40()
{
    const std::int32_t height = gLegacyBottomHeight5858A4;
    const std::int32_t y = 480 - height;
    gLegacyBottomYCB1DD8 = y;
    return y;
}

// 原函数：0x00502C60。
std::int32_t Sub_502C60()
{
    return Sub_502C70();
}

// 原函数：0x00502C70。
std::int32_t Sub_502C70()
{
    const std::int32_t height = gLegacyBottomHeight5858A4;
    gLegacyBottomHeightCA0014 = height;
    return height;
}

// 原函数：0x00502C80。
std::int32_t Sub_502C80()
{
    return Sub_502C90();
}

// 原函数：0x00502C90。
std::int32_t Sub_502C90()
{
    const std::int32_t height = gLegacyBottomHeightCA0014;
    gLegacyBottomHeightD2DAB4 = height;
    return height;
}

// 原函数：0x00502CA0。
std::int32_t Sub_502CA0()
{
    return Sub_502CB0();
}

// 原函数：0x00502CB0。
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

// 原函数：0x00502D20。
std::int32_t Sub_502D20()
{
    return Sub_502D30();
}

// 原函数：0x00502D30。
std::int32_t Sub_502D30()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[1], 80, 22552);
}

// 原函数：0x00502DA0。
std::int32_t Sub_502DA0()
{
    return Sub_502DB0();
}

// 原函数：0x00502DB0。
std::int32_t Sub_502DB0()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[2], 160, 22553);
}

// 原函数：0x00502E20。
std::int32_t Sub_502E20()
{
    return Sub_502E30();
}

// 原函数：0x00502E30。
std::int32_t Sub_502E30()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[3], 240, 22554);
}

// 原函数：0x00502EA0。
std::int32_t Sub_502EA0()
{
    return Sub_502EB0();
}

// 原函数：0x00502EB0。
std::int32_t Sub_502EB0()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[4], 320, 22555);
}

// 原函数：0x00502F20。
std::int32_t Sub_502F20()
{
    return Sub_502F30();
}

// 原函数：0x00502F30。
std::int32_t Sub_502F30()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[5], 400, 22556);
}

// 原函数：0x00502FA0。
std::int32_t Sub_502FA0()
{
    return Sub_502FB0();
}

// 原函数：0x00502FB0。
std::int32_t Sub_502FB0()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[6], 480, 22557);
}

// 原函数：0x00503020。
std::int32_t Sub_503020()
{
    return Sub_503030();
}

// 原函数：0x00503030。
std::int32_t Sub_503030()
{
    return InitializeLegacyBottomBarPrefix(gLegacyBottomInitWordsD68938[7], 560, 22558);
}

// 原函数：0x005030A0。
std::int32_t Sub_5030A0(LegacyPanelTextButtonRecord* const records, const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        LegacyPanelTextButtonRecord* const record = &records[index];
        const std::int32_t state = character_select::detail::Sub_503990(&record->label);
        if ((state & 0x40) != 0) {
            const wchar_t* const hint = record->hint;
            if (hint != nullptr) {
                ui::detail::gHoverTextD68AE8 = hint;
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

// 原函数：0x00503120。
void Sub_503120(LegacyPanelTextButtonRecord* const records, const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        const std::uint32_t hoverImage = gValueD69768;
        const std::int8_t color = std::bit_cast<std::int8_t>(records[index].color);
        static_cast<void>(character_select::detail::Sub_503A20(
            &records[index].label, 2, color, 1, hoverImage, 214));
    }
}

// 原函数：0x00503160。
std::int32_t Sub_503160(
    const std::int32_t opened, LegacyPanelRecord* const panel,
    void (*const initialize)(), const std::int32_t flags)
{
    const std::int32_t initialX = panel->x;
    const std::int32_t initialWidth = panel->width;
    const std::int32_t right = initialX + initialWidth;
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
    const std::int32_t index = ui::Sub_5044C0(id);
    if (index < 0) {
        return 0;
    }
    YobiAction*& actionSlot = ui::BorrowLegacyPanelAction57C374(index);
    if (opened == 0) {
        YobiAction* const actionToDestroy = actionSlot;
        if (actionToDestroy != nullptr) {
            static_cast<void>(MarkDeath(actionToDestroy));
            actionSlot = nullptr;
        }
        std::uint8_t& dirty = ui::BorrowLegacyPanelDirty57C378(index);
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
        actionSlot = CreateModalAction_5044E0(x, y, width, height, style, 1);
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
                const wchar_t* const hint = tab->hint;
                if (hint != nullptr) {
                    ui::detail::gHoverTextD68AE8 = hint;
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
        static_cast<void>(ui::Sub_5042D0(panel->id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

// 原函数：0x00503440。
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

// 原函数：0x00503590。
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

// 原函数：0x005035E0。
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

// 原函数：0x005038B0。
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

// 原函数：0x0045F100。
std::int32_t Sub_45F100(const void* const uiRecord)
{
    return QueryLegacyUiRecordState_45F100(uiRecord);
}

// 原函数：0x0045EFD0。
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

// 原函数：0x0045F100。
std::int32_t Sub_45F100(
    const CharacterSelectUiRecord* const uiRecord)
{
    return QueryLegacyUiRecordState_45F100(uiRecord);
}

// 原函数：0x0045EFD0。
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
