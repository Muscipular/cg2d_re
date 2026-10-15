#pragma once

// UI 实现内部的共享依赖、类型与声明；窗口专属定义位于 windows/。
#include "WindowPrototypeDependencies.hpp"

#include "../CharacterSelectSceneDependencies.hpp"
#include "../CharacterSelectState.hpp"
#include "../LoginInputDependencies.hpp"
#include "../LoginMessageDependencies.hpp"
#include "../LoginSceneDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "../platform/StartupNative.hpp" // ERROR_INSUFFICIENT_BUFFER的Win32声明。
#include "../platform/LegacyStdio.hpp"
#include "../platform/ImeLifecycle.hpp"
#include "../platform/WindowProcedureState.hpp"
#include "../world/MailState.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/Blacklist.hpp"
#include "cg/Input.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/LegacyRecordOrder.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/LoginState.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/GameTime.hpp"
#include "cg/world/SceneState.hpp"

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B226、0x0053B2F1、0x0053BC41、0x0053BCCD、0x0053C680；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304、0x0053D21D；直接沿MSVC CRT。
#include <cstdlib>
#include <cwchar>
#include <cstring> // 原CRT接口：0x0053BE40、0x0053C2F0、0x0053D26E、0x0053BEC0、0x0053C649、0x0053C0A9、0x0053C3AC；原参数／返回合同由直接调用承接。
#include <fcntl.h>
#include <io.h>
#include <optional>
#include <string>
#include <format>
#include <sys/stat.h>
#include <type_traits>
#include <cstdint>

namespace cg::ui {
namespace detail {


// 原CF4B78五条独立战斗数值快照；只含S32，不与World宠物CryptInt存储合并。
struct BattlePetVitals {
    std::int32_t hp;
    std::int32_t maxHp;
    std::int32_t fp;
    std::int32_t maxFp;
};

inline constexpr std::array<std::array<std::int32_t, 2>, 6> kCraftAlternateActionImages {{
    {0x186A0, 0x18A87}, {0x19A28, 0x1A130}, {0x197D0, 0x19848},
    {0x1D524, 0x1D553}, {0x1D55C, 0x1D563}, {0x1AF0E, 0x1AF25},
}};

inline constexpr std::int16_t kCraftPreviewOffsetX57D7D0 = 30; // 原常量参考：0x0057D7D0。

inline constexpr std::int16_t kCraftPreviewOffsetY57D7D2 = -45; // 原常量参考：0x0057D7D2。

inline constexpr wchar_t kCraftLevelUp58661C[] = L"等級提升！"; // 原常量参考：0x0058661C。

inline constexpr wchar_t kCraftExperience58660C[] = L"技能經驗值 +{}"; // 原常量参考：0x0058660C。

inline constexpr wchar_t kCraftEndurance586600[] = L"　耐力 +{}"; // 原常量参考：0x00586600。

inline constexpr wchar_t kCraftDexterity5865F4[] = L"　靈巧 +{}"; // 原常量参考：0x005865F4。

inline constexpr wchar_t kCraftIntelligence5865E8[] = L"　智力 +{}"; // 原常量参考：0x005865E8。

inline constexpr wchar_t kCraftExperience589820[] = L"技能經驗值 {:+}"; // 原常量参考：0x00589820。

inline constexpr wchar_t kCraftLevelUp589814[] = L"等級上升！\n"; // 原常量参考：0x00589814。

inline constexpr wchar_t kCraftEndurance589808[] = L"耐力 {:+}"; // 原常量参考：0x00589808。

inline constexpr wchar_t kCraftDexterity5897FC[] = L"靈巧 {:+}"; // 原常量参考：0x005897FC。

inline constexpr wchar_t kCraftIntelligence5897F0[] = L"智力 {:+}"; // 原常量参考：0x005897F0。

inline constexpr std::array<std::int8_t, 16> kSkillRebirthRates57D7A4 { // 原常量参考：0x0057D7A4。
    100, 95, 90, 85, 80, 75, 65, 60, 55, 50, 45, 40, 35, 30, 25, 0,
};

inline constexpr std::array<std::int8_t, 16> kSkillCrystalRates57D7B4 { // 原常量参考：0x0057D7B4。
    100, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
};

inline constexpr wchar_t kWindow12Unknown554590[] = L"???"; // 原常量参考：0x00554590。

inline constexpr wchar_t kWindow12NoFp5864F0[] = L"魔力不足。"; // 原常量参考：0x005864F0。

inline constexpr wchar_t kWindow13Text5868EC[] = L"關閉此視窗"; // 原常量参考：0x005868EC。

// 原105F508至category起点；尾段只保留已识别字节跨度，不作为输入正文。
struct MiniMailComposeState {
    std::int32_t target;
    std::array<char, 256> name;
    std::array<char, 0x404> text104; // 原成员偏移：+0x104。
};

inline constexpr wchar_t kMiniMailCostHint588588[] = // 原常量参考：0x00588588。
    L"使用迷你郵件發信需要花費魔幣{}G";

struct WindowListRecordD3FAD4 {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::array<std::array<char, 0x100>, 4> texts08; // 原成员偏移：+0x08。
    std::int32_t value408; // 原成员偏移：+0x408。
};

struct WindowChoiceSlotsCC4CA0 {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::array<std::int32_t, 5> values04; // 原成员偏移：+0x04。
    std::array<std::array<std::int32_t, 2>, 5> pairs18; // 原成员偏移：+0x18。
};

// 仅承接D2F0B8已消费前缀，不视为完整对象或最终x64布局。
struct WindowRuntimePrefixD2F0B8 {
    std::int32_t value00; // 4A2610先写+10再写此槽，后续由邮件窗口消费。
    std::int32_t value04; // 原成员偏移：+0x04。 4A3C70写排序后的记录索引，不是地址。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::int32_t value10; // 原成员偏移：+0x10。 原W32方式值，不是地址。
    std::array<std::byte, 0x258> bytes14; // 原成员偏移：+0x14。
    std::uint32_t value26C; // 原成员偏移：+0x26C。
};

// 原CC1A30..CC207B，窗口22消费、窗口49生产；两个数组各200个数值槽。
struct WindowMailGroupSlotsCC1A30 {
    std::int32_t count;
    std::int32_t mode;
    std::array<std::int32_t, 200> indices;
    std::array<std::int32_t, 200> selected;
    std::int32_t initialize;
};

// 各原地址的唯一UI存储；此聚合的整体内存顺序不是原端连续布局。

struct LoginWindowRuntimeSlots {
    std::int32_t valueCF4A00; // 原全局：0x00CF4A00。
    std::int32_t valueC8A95C; // 原全局：0x00C8A95C。
    std::int32_t valueCB2200; // 原全局：0x00CB2200。
    std::uint32_t valueD3FAD0; // 原全局：0x00D3FAD0。
    std::array<WindowListRecordD3FAD4, 4> recordsD3FAD4; // 原全局：0x00D3FAD4。
    WindowChoiceSlotsCC4CA0 choicesCC4CA0; // 原全局：0x00CC4CA0。
    WindowRuntimePrefixD2F0B8 prefixD2F0B8; // 原全局：0x00D2F0B8。
    WindowRuntimePrefixD2F0B8 mailDraftCF4BC8; // 原全局：0x00CF4BC8。 4A5650只更新五字段和正文，保留其余字节。
    WindowMailGroupSlotsCC1A30 mailGroupCC1A30; // 原全局：0x00CC1A30。
    std::int32_t mailGroupScrollD67B88; // 原全局：0x00D67B88。
    std::int32_t mailGroupSortCategoryCB0600; // 原全局：0x00CB0600。
    std::array<std::int32_t, 200> mailGroupOrderCC2E6C; // 原全局：0x00CC2E6C。 原排序索引，末地址CC318C，不与技能HP区重叠。
    std::int32_t statusHasPointsD66E24; // 原全局：0x00D66E24。 470AC0生产，4723D0两次独立读取。
    std::int32_t statusHasInjuryD046C8; // 原全局：0x00D046C8。 伤势提示门禁，不是玩家伤势副本。
    std::int32_t statusHasMailCC4B5C; // 原全局：0x00CC4B5C。 470AC0的518DA0结果，按钮固定入口快照。
    std::int32_t statusMailCategoryD66E34; // 原全局：0x00D66E34。 4723D0传给518F00的同源输出。
    std::int32_t statusMailIndexD2E2D8; // 原全局：0x00D2E2D8。
    std::int32_t statusBlinkPhaseCF4FBC; // 原全局：0x00CF4FBC。 470AC0递增并在>60时回零，开窗不重置。
    std::int32_t valueD673D0; // 原全局：0x00D673D0。
    std::int32_t selectedPetCC3E5C; // 原全局：0x00CC3E5C。
    std::int32_t previousPetCC4888; // 原全局：0x00CC4888。 详情页记排序位置，不是实际宠物编号。
    std::array<std::int32_t, 5> petSequencesCAFEB0; // 原全局：0x00CAFEB0。
    std::wstring petNameDraftD45E74; // 原全局：0x00D45E74。
    std::int32_t petTechniqueD2D66C; // 原全局：0x00D2D66C。 原物理技能下标，生产见48F620/48F690/48F6C0。
    std::int32_t petTechniqueValueD2F784; // 原全局：0x00D2F784。 原拖动关联槽，48D9A0置-1，48F620/48F6C0后续写。
    std::int32_t petTechniqueDragD460CC; // 原全局：0x00D460CC。 4920F0绘制门禁，初始化由48D9A0负责。
    std::array<std::int32_t, 5> valuesCC1A18; // 原全局：0x00CC1A18。
    std::array<std::int32_t, 5> valuesCC4CE8; // 原全局：0x00CC4CE8。
    std::int32_t countCC4CFC; // 原全局：0x00CC4CFC。 48D460每帧重建的状态1计数，48C150不清此槽。
    std::array<std::int32_t, 3> valuesD2EAA0; // 原全局：0x00D2EAA0。
    std::array<std::int32_t, 2> valuesCF4E50; // 原全局：0x00CF4E50。
    std::array<std::int32_t, 7> valuesD67B40; // 原全局：0x00D67B40。 两组三类滚动位置，末槽为D67B58类别。
    std::uint32_t mailSortModeD67B3C; // 原全局：0x00D67B3C。 4A0970从存档live排序模式复制。
    std::int32_t mailBlinkPhaseD67B5C; // 原全局：0x00D67B5C。 4A32B0有符号余数相位。
    std::int32_t mailBlinkFlagD67B60; // 原全局：0x00D67B60。 每16帧异或1，开窗不清零。
    std::array<std::int32_t, 200> mailIndicesC9FCE4; // 原全局：0x00C9FCE4。 窗口21三类名单共用的数值索引，不与CA0004重叠。
    std::array<std::uint8_t, 2> mailGuildColorsD6784C; // 原全局：0x00D6784C。 公会状态文本两W8颜色，保留原虚拟零存储。
    std::int32_t mailMemberIndexCB1D18; // 原全局：0x00CB1D18。 4A38C0按编号查得，窗口55的4A4540消费。
    std::int32_t mailMemberKeyD45F88; // 原全局：0x00D45F88。 独立于D45F78金额文本；4A4370消费编号。
    std::int32_t mailRankCF4A14; // 原全局：0x00CF4A14。 4A38C0/4A4220生产，4A4370发送时读取。
    std::int32_t mailRankScrollCA0474; // 原全局：0x00CA0474。 窗口55滚动位置，初始化不清所选职级。
    std::int32_t mailCategoryD67B64; // 原全局：0x00D67B64。 4A2B00发布，4A47C0/4A48A0消费。
    std::int32_t mailCategoryCC4D50; // 原全局：0x00CC4D50。 4A2B00随后发布，4A49B0/4A4FC0消费。
    std::int32_t mailDeleteIndexD2DA8C; // 原全局：0x00D2DA8C。 删除确认与后续列表父链共用的记录索引。
    std::int32_t valueCC31A4; // 原全局：0x00CC31A4。
    std::int32_t valueD2E444; // 原全局：0x00D2E444。
    std::int32_t blacklistDeleteModeD67BC4; // 原全局：0x00D67BC4。 只由65删除按钮切换，初始化不重置。
    std::int32_t valueCB0604; // 原全局：0x00CB0604。
    std::uint32_t valueD67618; // 原全局：0x00D67618。
    platform::LegacyTm53B8C6* calendarTimeD67BAC; // 原全局：0x00D67BAC。 借用platform同一可变localtime对象。
    std::int32_t timeCB1DF0; // 原全局：0x00CB1DF0。
    std::int32_t valueD67BB0; // 原全局：0x00D67BB0。
    std::int32_t logViewerMaximumD67BB4; // 原全局：0x00D67BB4。 原计数上限，含EOF空读取，不减可见行数。
    platform::LegacyFile* logViewerFileD67BB8; // 原全局：0x00D67BB8。 借用当前打开的MSVC FILE，关闭后不得重复消费。
    std::int32_t logViewerErrorD67BBC; // 原全局：0x00D67BBC。
    std::int32_t dayD67BC0; // 原全局：0x00D67BC0。
};

// 原CC4240..CC466C确认上下文；函数指针原生化，不沿用x86字段偏移。
struct Window47RuntimeSlots {
    std::int32_t visibleMaskCC4240; // 原全局：0x00CC4240。
    std::int32_t widthUnitsCC4244; // 原全局：0x00CC4244。
    std::uint32_t valueCC4248; // 原全局：0x00CC4248。
    std::uint32_t valueCC424C; // 原全局：0x00CC424C。
    WindowCallback initializeCC4250; // 原全局：0x00CC4250。
    std::array<WindowElementCallback, 6> actionsCC4254; // 原全局：0x00CC4254。
    std::wstring textCC426C; // 原全局：0x00CC426C。
};

inline constexpr std::array<const wchar_t*, 3> kShopHints57E6C4 { // 原常量参考：0x0057E6C4。
    L"在這家店買物品。",
    L"在這家店賣物品。",
    L"還是算了。",
};



inline constexpr std::array<std::int32_t, 5> gHealthImages57E6A4 {243220, 243221, 243222, 243223, 243224}; // 原全局：0x0057E6A4。

inline constexpr wchar_t kSettingsClose55331C[] = L"關閉這個視窗。"; // 原常量参考：0x0055331C。

// 只借各原字段；公会记录的+8前缀不强转为另一种C++记录。
struct MailContactView {
    const std::uint16_t* used;
    const std::byte* statusBytes;
    const std::int32_t* number;
    const std::int32_t* imageNo;
    const char* name;
    const char* text21; // 原成员偏移：+0x21。
    const char* text32; // 原成员偏移：+0x32。

    std::int16_t ReadStatus() const
    {
        std::int16_t value;
        std::memcpy(&value, statusBytes, sizeof(value));
        return value;
    }
};

inline constexpr wchar_t kReplyMailHint587730[] = // 原常量参考：0x00587730。
    L"對已選擇的對象寄送郵件。";

inline constexpr wchar_t kSkillEnableRebirth5864A0[] = // 原常量参考：0x005864A0。
    L"進行「精靈變身」。";

inline constexpr wchar_t kSkillDisableRebirth5864B4[] = // 原常量参考：0x005864B4。
    L"解除「精靈變身」。";

inline constexpr wchar_t kStatusGuildUnavailable586270[] = L"無法使用家族功能。"; // 原常量参考：0x00586270。

inline constexpr wchar_t kPetPlaceHint586C14[] = L"將此寵物放在地上。"; // 原常量参考：0x00586C14。

inline constexpr wchar_t kPetSelectHint586D2C[] = L"切換頁數。"; // 原常量参考：0x00586D2C。

inline constexpr wchar_t kPetRenameHint586D38[] = L"變更寵物名字。"; // 原常量参考：0x00586D38。

extern std::uint16_t gShopPriceTailD67420; // 原D673EC+0x34：第五行价格末两位的非文本别名。

extern std::uint8_t gItemSelectionModeCC40D4; // 原全局：0x00CC40D4。

extern std::array<BattlePetVitals, 5> gBattlePetVitalsCF4B78; // 原全局：0x00CF4B78。

// 原CF4FC4表情选择区；4BAA50的-1商品映射复用首字节。
extern std::array<std::uint8_t, 18> gEmoteSelectionCF4FC4;

extern std::int32_t gSkillPetImageCB0A18; // 原全局：0x00CB0A18。

extern std::int32_t gCraftSkillIndexCC2368; // 原全局：0x00CC2368。

extern std::int32_t gCraftRecipeIndexD2D638; // 原全局：0x00D2D638。

extern std::int32_t gCraftRequestValueD30400; // 原全局：0x00D30400。 制造发送保留完整S32值。

extern std::uint8_t gSkillRequestByteD64E13; // 原全局：0x00D64E13。 其他加工发送按S8读取。

extern std::uint32_t gSkillPreviewTickCF46C8; // 原全局：0x00CF46C8。 多个技能窗口共用的另一计时槽。

extern std::uint8_t gWindow12SkillIndexCA0470; // 原全局：0x00CA0470。 原低U8发布，索引消费按S8。

extern std::uint32_t gSkillExtraDelayCF46C4; // 原全局：0x00CF46C4。

extern std::wstring gProfileSearchKeyword1075988; // 原全局：0x01075988。

extern MiniMailComposeState gMiniMailCompose105F508; // 原全局：0x0105F508。

// 原函数：0x004A6D00。
std::int32_t WindowRuntime_MatchesRuntimePrefix(
    const WindowRuntimePrefixD2F0B8 first, const WindowRuntimePrefixD2F0B8 second);

extern LoginWindowRuntimeSlots gLoginWindowRuntime;

extern Window47RuntimeSlots gWindow47Runtime;

// CC24D4首行标题和CC2525起正文必须同源，其他NPC窗口也消费这一区域。
extern std::array<WindowDialogTextLine, 30> gNpcDialogLinesCC24D4; // 原全局：0x00CC24D4。

// 原519680：零单独处理，其余按有符号26/51/76分档，不钳制负值。
// 原函数：0x00519680。
std::int32_t WindowItem_HealthImageLevel(const std::int32_t health);

void WindowRuntime_FillTwoTextLines(WindowTextData* const first, WindowTextData* const second, const std::wstring_view contents);

void WindowMail_InitializeDeleteConfirmation(const WindowElementCallback callback);

MailContactView WindowMail_MakeContactView(const world::detail::ContactRuntimeRecord& record);

MailContactView WindowMail_MakeContactView(const login::detail::OrderedRecord1051B14& record);

MailContactView WindowMail_MakeContactView(const world::detail::MiniMailContactRecord& record);

void WindowMail_WriteGuildStatus(WindowTextData* const data, const std::int32_t status);

void WindowMail_WriteContactStatus(
    const MailContactView& record, WindowTextData* const data, const std::int16_t status);

void WindowMail_SetUnreadGraphic(const std::int32_t index);

// MC01-C8：两邮件索引域共用std::sort；三向COMP<0表示在前，同键等价。
void WindowMail_SortIndices(
    std::int32_t* const first, const std::uint32_t count,
    std::int32_t (*compare)(const std::int32_t*, const std::int32_t*));

// 4A5970各成功发送路径分别取时，不跨收件人复用时间或Input正文。
void WindowMail_FormatTimestamp(char* const destination);

// 原函数：0x00492260。
void WindowPetDetail_ResetDraftFlag();

// 原函数：0x00492270。
std::int32_t WindowPetDetail_ReadDraftFlag();

// 原函数：0x00492280。
void WindowPetDetail_InitializeDraft();

// 原函数：0x00492350。
std::int32_t WindowPetDetail_SavePetSequences();

// 原函数：0x00492390。
std::int32_t WindowPetDetail_HasPetSequenceChanges();

static_assert(sizeof(BattlePetVitals) == 0x10);

static_assert(offsetof(MiniMailComposeState, name) == 4);

static_assert(offsetof(MiniMailComposeState, text104) == 0x104);

static_assert(sizeof(MiniMailComposeState) == 0x508);

static_assert(offsetof(WindowMailGroupSlotsCC1A30, indices) == 8);

static_assert(offsetof(WindowMailGroupSlotsCC1A30, selected) == 0x328);

static_assert(offsetof(WindowMailGroupSlotsCC1A30, initialize) == 0x648);

static_assert(sizeof(WindowMailGroupSlotsCC1A30) == 0x64C);

static_assert(offsetof(WindowListRecordD3FAD4, value00) == 0);

static_assert(offsetof(WindowListRecordD3FAD4, value04) == 4);

static_assert(offsetof(WindowListRecordD3FAD4, texts08) == 8);

static_assert(offsetof(WindowListRecordD3FAD4, value408) == 0x408);

static_assert(sizeof(WindowListRecordD3FAD4) == 0x40C);

static_assert(offsetof(WindowChoiceSlotsCC4CA0, values04) == 4);

static_assert(offsetof(WindowChoiceSlotsCC4CA0, pairs18) == 0x18);

static_assert(sizeof(WindowChoiceSlotsCC4CA0) == 0x40);

static_assert(offsetof(WindowRuntimePrefixD2F0B8, value00) == 0);

static_assert(offsetof(WindowRuntimePrefixD2F0B8, value04) == 4);

static_assert(offsetof(WindowRuntimePrefixD2F0B8, value08) == 8);

static_assert(offsetof(WindowRuntimePrefixD2F0B8, value0C) == 0x0C);

static_assert(offsetof(WindowRuntimePrefixD2F0B8, value10) == 0x10);

static_assert(offsetof(WindowRuntimePrefixD2F0B8, value26C) == 0x26C);

static_assert(sizeof(WindowRuntimePrefixD2F0B8) == 0x270);

} // namespace detail

} // namespace cg::ui
