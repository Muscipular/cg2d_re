#pragma once

// UI 实现内部的共享依赖、类型与声明；窗口专属定义位于 windows/。
#include "WindowPrototypeDependencies.hpp"
#include "cg/session/ClientSession.hpp"

#include "../scene/character_select/CharacterSelectSceneDependencies.hpp"
#include "../scene/character_select/CharacterSelectState.hpp"
#include "../scene/login/LoginInputDependencies.hpp"
#include "../scene/login/LoginMessageDependencies.hpp"
#include "../scene/login/LoginSceneDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "../platform/StartupNative.hpp" // ERROR_INSUFFICIENT_BUFFER的Win32声明。
#include "../platform/imeImpl.hpp"
#include "../platform/WindowProcedureState.hpp"
#include "../world/MailState.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/world/Blacklist.hpp"
#include "cg/input/Input.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/world/GuildState.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/scene/login/LoginState.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/GameTime.hpp"
#include "cg/world/SceneState.hpp"

#include <ctime>
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

// CTM01：本地CRT时间槽显式__time64_t，原地址注释仅保历史身份。
static_assert(sizeof(__time64_t) == sizeof(std::int64_t));
static_assert(alignof(__time64_t) == alignof(std::int64_t));

namespace cg::ui {
namespace detail {

// PSF01：切页完成坐标复制后登记，在旧页本帧绘制位置刷新并绘制新页。
void WindowRuntime_QueuePagePresentation(const WindowInstance& source, const WindowInstance& target);

// 悬停宠物预览分页：银行／交易含装备共3页，其他默认2页；identity仅比较借用记录身份。
std::int32_t WindowPetSkills_GetPreviewPage(session::ClientSession& session, const void* identity, std::int32_t pageCount = 2);


inline constexpr std::array<std::array<std::int32_t, 2>, 6> kCraftAlternateActionImages {{
    {0x186A0, 0x18A87}, {0x19A28, 0x1A130}, {0x197D0, 0x19848},
    {0x1D524, 0x1D553}, {0x1D55C, 0x1D563}, {0x1AF0E, 0x1AF25},
}};

inline constexpr std::int16_t kCraftPreviewOffsetX57D7D0 = 30; // 原常量参考：0x0057D7D0。

inline constexpr std::int16_t kCraftPreviewOffsetY57D7D2 = -45; // 原常量参考：0x0057D7D2。

inline constexpr wchar_t kCraftLevelUp58661C[] = L"等级提升！"; // 原常量参考：0x0058661C。

inline constexpr wchar_t kCraftExperience58660C[] = L"技能经验值 +{}"; // 原常量参考：0x0058660C。

inline constexpr wchar_t kCraftEndurance586600[] = L"　耐力 +{}"; // 原常量参考：0x00586600。

inline constexpr wchar_t kCraftDexterity5865F4[] = L"　灵巧 +{}"; // 原常量参考：0x005865F4。

inline constexpr wchar_t kCraftIntelligence5865E8[] = L"　智力 +{}"; // 原常量参考：0x005865E8。

inline constexpr wchar_t kCraftExperience589820[] = L"技能经验值 {:+}"; // 原常量参考：0x00589820。

inline constexpr wchar_t kCraftLevelUp589814[] = L"等级上升！\n"; // 原常量参考：0x00589814。

inline constexpr wchar_t kCraftEndurance589808[] = L"耐力 {:+}"; // 原常量参考：0x00589808。

inline constexpr wchar_t kCraftDexterity5897FC[] = L"灵巧 {:+}"; // 原常量参考：0x005897FC。

inline constexpr wchar_t kCraftIntelligence5897F0[] = L"智力 {:+}"; // 原常量参考：0x005897F0。

inline constexpr std::array<std::int8_t, 16> kSkillRebirthRates57D7A4 { // 原常量参考：0x0057D7A4。
    100, 95, 90, 85, 80, 75, 65, 60, 55, 50, 45, 40, 35, 30, 25, 0,
};

inline constexpr std::array<std::int8_t, 16> kSkillCrystalRates57D7B4 { // 原常量参考：0x0057D7B4。
    100, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
};

inline constexpr wchar_t kWindow12Unknown554590[] = L"???"; // 原常量参考：0x00554590。

inline constexpr wchar_t kWindow12NoFp5864F0[] = L"魔力不足。"; // 原常量参考：0x005864F0。

inline constexpr wchar_t kWindow13Text5868EC[] = L"关闭此视窗"; // 原常量参考：0x005868EC。

inline constexpr wchar_t kMiniMailCostHint588588[] = // 原常量参考：0x00588588。
    L"使用迷你邮件发信需要花费魔币{}G";


inline constexpr std::array<const wchar_t*, 3> kShopHints57E6C4 { // 原常量参考：0x0057E6C4。
    L"在这家店买物品。",
    L"在这家店卖物品。",
    L"还是算了。",
};


inline constexpr std::array<std::int32_t, 5> gHealthImages57E6A4 {243220, 243221, 243222, 243223, 243224}; // 原全局：0x0057E6A4。

inline constexpr wchar_t kSettingsClose55331C[] = L"关闭这个视窗。"; // 原常量参考：0x0055331C。

// 数值字段只借用；文本快照供当次窗口刷新，公会记录不强转为另一种C++记录。
struct MailContactView {
    const std::uint16_t* used;
    const std::byte* statusBytes;
    const std::int32_t* number;
    const std::int32_t* imageNo;
    std::wstring name;
    std::wstring text21; // 原成员偏移：+0x21。
    std::wstring text32; // 原成员偏移：+0x32。

    std::int16_t ReadStatus() const
    {
        std::int16_t value;
        std::memcpy(&value, statusBytes, sizeof(value));
        return value;
    }
};

inline constexpr wchar_t kReplyMailHint587730[] = // 原常量参考：0x00587730。
    L"对已选择的对象寄送邮件。";

inline constexpr wchar_t kSkillEnableRebirth5864A0[] = // 原常量参考：0x005864A0。
    L"进行「精灵变身」。";

inline constexpr wchar_t kSkillDisableRebirth5864B4[] = // 原常量参考：0x005864B4。
    L"解除「精灵变身」。";

inline constexpr wchar_t kStatusGuildUnavailable586270[] = L"无法使用家族功能。"; // 原常量参考：0x00586270。

inline constexpr wchar_t kPetPlaceHint586C14[] = L"将此宠物放在地上。"; // 原常量参考：0x00586C14。

inline constexpr wchar_t kPetSelectHint586D2C[] = L"切换页数。"; // 原常量参考：0x00586D2C。

inline constexpr wchar_t kPetRenameHint586D38[] = L"变更宠物名字。"; // 原常量参考：0x00586D38。


// 原函数：0x004A6D00。
std::int32_t WindowRuntime_MatchesRuntimePrefix(
    const WindowRuntimePrefixD2F0B8 first, const WindowRuntimePrefixD2F0B8 second);



// 原519680：零单独处理，其余按有符号26/51/76分档，不钳制负值。
// 原函数：0x00519680。
std::int32_t WindowItem_HealthImageLevel(const std::int32_t health);

void WindowRuntime_FillTwoTextLines(WindowTextData* const first, WindowTextData* const second, const std::wstring_view contents);

void WindowMail_InitializeDeleteConfirmation(session::ClientSession& session, const WindowElementCallback callback);

MailContactView WindowMail_MakeContactView(const world::detail::ContactRuntimeRecord& record);

MailContactView WindowMail_MakeContactView(const cg::world::OrderedRecord1051B14& record);

MailContactView WindowMail_MakeContactView(const world::detail::MiniMailContactRecord& record);

void WindowMail_WriteGuildStatus(session::ClientSession& session, WindowTextData* const data, const std::int32_t status);

void WindowMail_WriteContactStatus(session::ClientSession& session,
    const MailContactView& record, WindowTextData* const data, const std::int16_t status);

void WindowMail_SetUnreadGraphic(WindowInstance* callbackWindow, const std::int32_t index);

// MC01-C8：两邮件索引域共用std::sort；三向COMP<0表示在前，同键等价。
void WindowMail_SortIndices(session::ClientSession& session,
    std::int32_t* const first, const std::uint32_t count,
    std::int32_t (*compare)(session::ClientSession&, const std::int32_t*, const std::int32_t*));

// 4A5970各成功发送路径分别取时，不跨收件人复用时间或Input正文。
void WindowMail_FormatTimestamp(session::ClientSession& session, std::wstring& destination);

// 原函数：0x00492260。
void WindowPetDetail_ResetDraftFlag(session::ClientSession& session);

// 原函数：0x00492270。
std::int32_t WindowPetDetail_ReadDraftFlag(session::ClientSession& session);

// 原函数：0x00492280。
void WindowPetDetail_InitializeDraft(WindowInstance* callbackWindow);

// 原函数：0x00492350。
std::int32_t WindowPetDetail_SavePetSequences(session::ClientSession& session);

// 原函数：0x00492390。
std::int32_t WindowPetDetail_HasPetSequenceChanges(session::ClientSession& session);

static_assert(offsetof(WindowMailGroupSlotsCC1A30, indices) == 8);

static_assert(offsetof(WindowMailGroupSlotsCC1A30, selected) == 0x328);

static_assert(offsetof(WindowMailGroupSlotsCC1A30, initialize) == 0x648);

static_assert(sizeof(WindowMailGroupSlotsCC1A30) == 0x64C);

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
