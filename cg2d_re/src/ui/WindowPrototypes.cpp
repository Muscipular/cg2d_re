#include "WindowPrototypeDependencies.hpp"
#include "cg/text/Encoding.hpp"
#include <string>
#include "../world/WorldUpdateDependencies.hpp"
#include "cg/Input.hpp"

namespace cg::ui {
namespace detail {

// 控件载荷、窗口状态、组件表与原型。

WindowGraphicData gWindowGraphic570160 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570160。

WindowGraphicData gWindowGraphic570190 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570190。

WindowTextPrototype gWindowText5701F0 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"測試人員"}; // 原全局：0x005701F0。 // 原常量参考：0x00585EA4。

WindowGraphicData gWindowGraphic565608 {0x3BAEC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565608。

WindowGraphicData gWindowGraphic565620 {0x3BAED, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00565620。

WindowGraphicData gWindowGraphic565638 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565638。

WindowGraphicData gWindowGraphic565650 {0x3B56E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565650。

WindowGraphicData gWindowGraphic5656B0 {0x3BAF5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005656B0。

WindowGraphicData gWindowGraphic5656C8 {0x3B584, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005656C8。

WindowGraphicData gWindowGraphic5656E0 {0x3BAF2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005656E0。

WindowGraphicData gWindowGraphic565878 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565878。

WindowGraphicData gWindowGraphic565890 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565890。

WindowGraphicData gWindowGraphic5658C0 {0x3BAF7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005658C0。

WindowTextPrototype gWindowText565950 { // 原全局：0x00565950。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};

WindowTextPrototype gWindowText565988 { // 原全局：0x00565988。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"1/5", // 原常量参考：0x0058600C。
};

WindowTextPrototype gWindowText562C70 { // 原全局：0x00562C70。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};

WindowTextPrototype gWindowText562C78 { // 原全局：0x00562C78。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};

WindowGraphicData gWindowGraphicD678E0 {}; // 原全局：0x00D678E0。

static_assert(sizeof(WindowGraphicData) == 0x18);

static_assert(offsetof(WindowGraphicData, height) == 0x10);

// 各窗口唯一原型对象；71槽索引仍由WindowPrototypes.cpp统一绑定。
// 原全局：0x0054EFB8。
// 原型：窗口0；原全局：0x0054EFB8。
static WindowPrototype gWindowPrototypeNone {0, 0, 0, 0, 0, 0, 0x00000000u, 0, nullptr, nullptr, nullptr, nullptr}; // 0: 54EFB8

// 71个原生指针；原36/37同源，不因窗口编号不同复制对象。
std::array<WindowPrototype*, kWindowCount> gWindowPrototypes57C130 { // 原全局：0x0057C130。
    &gWindowPrototypeNone, // 0
    &gWindowPrototypeStatusBar,
    &gWindowPrototypeCompactStatusBar,
    &gWindowPrototypeCollapsedStatusBar,
    &gWindowPrototypeShortcutBar,
    &gWindowPrototypeCollapsedShortcutBar,
    &gWindowPrototypeBottomBar,
    &gWindowPrototypePlayerProfile,
    &gWindowPrototypePlayerAttributes,
    &gWindowPrototypePlayerTitles,
    &gWindowPrototypeSkill, // 10
    &gWindowPrototypeCraft,
    &gWindowPrototypeGather,
    &gWindowPrototypeSkillResult,
    &gWindowPrototypeInventory,
    &gWindowPrototypePetList,
    &gWindowPrototypePetDetail,
    &gWindowPrototypePetAttributes,
    &gWindowPrototypePetSkills,
    &gWindowPrototypeAlbumList,
    &gWindowPrototypeAlbumDetail, // 20
    &gWindowPrototypeContacts,
    &gWindowPrototypeMailCompose,
    &gWindowPrototypePetMailCompose,
    &gWindowPrototypeSettings,
    &gWindowPrototypeHotkeys,
    &gWindowPrototypeBattleCommand,
    &gWindowPrototypeBattleItems,
    &gWindowPrototypeBattlePets,
    &gWindowPrototypeSpectator,
    &gWindowPrototypeBattleResult, // 30
    &gWindowPrototypeDpResult,
    &gWindowPrototypeBattleStateNotice,
    &gWindowPrototypeMiniMap,
    &gWindowPrototypeEmote,
    &gWindowPrototypeChat,
    &gWindowPrototypeDialog,
    &gWindowPrototypeDialog, // 37
    &gWindowPrototypeItemStorage,
    &gWindowPrototypeTimerNotice,
    &gWindowPrototypeMailRead, // 40
    &gWindowPrototypeBank,
    &gWindowPrototypeQuantity,
    &gWindowPrototypeTrade,
    &gWindowPrototypeTradeTarget,
    &gWindowPrototypeTextTarget,
    &gWindowPrototypeAttributeTarget,
    &gWindowPrototypeConfirmation,
    &gWindowPrototypeGuildDetails,
    &gWindowPrototypeGroupSelect,
    &gWindowPrototypeMessageBoard, // 50
    &gWindowPrototypeTextList,
    &gWindowPrototypeShopChoice,
    &gWindowPrototypeShop,
    &gWindowPrototypePetSkillChoice,
    &gWindowPrototypeGuildRank,
    &gWindowPrototypeAppearance,
    &gWindowPrototypeCompactChat,
    &gWindowPrototypeOwnProfile,
    &gWindowPrototypeOwnProfileCategory,
    &gWindowPrototypeProfileCategory, // 60
    &gWindowPrototypeProfileSearch,
    &gWindowPrototypeProfileView,
    &gWindowPrototypeMiniMail,
    &gWindowPrototypeProfileReply,
    &gWindowPrototypeBlacklist,
    &gWindowPrototypeAdventureLog,
    &gWindowPrototypeOwnStall,
    &gWindowPrototypeStallBrowse,
    &gWindowPrototypeCrystalShop,
    &gWindowPrototypeCrystalExchangeShop, // 70
};

} // namespace detail

} // namespace cg::ui
