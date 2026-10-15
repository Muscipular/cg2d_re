#include "WindowPrototypeDependencies.hpp"
#include "cg/text/Encoding.hpp"
#include <string>

#include "../world/WorldUpdateDependencies.hpp"
#include "cg/Input.hpp"


namespace cg::ui::detail {

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

namespace {

static_assert(sizeof(WindowGraphicData) == 0x18);

static_assert(offsetof(WindowGraphicData, height) == 0x10);

// 原型记录下标；窗口36/37共用Dialog，窗口38起不能直接用WindowId索引。
enum WindowPrototypeIndex : std::size_t {
    WindowPrototypeIndex_None = 0,
    WindowPrototypeIndex_StatusBar = 1,
    WindowPrototypeIndex_CompactStatusBar = 2,
    WindowPrototypeIndex_CollapsedStatusBar = 3,
    WindowPrototypeIndex_ShortcutBar = 4,
    WindowPrototypeIndex_CollapsedShortcutBar = 5,
    WindowPrototypeIndex_BottomBar = 6,
    WindowPrototypeIndex_PlayerProfile = 7,
    WindowPrototypeIndex_PlayerAttributes = 8,
    WindowPrototypeIndex_PlayerTitles = 9,
    WindowPrototypeIndex_Skill = 10,
    WindowPrototypeIndex_Craft = 11,
    WindowPrototypeIndex_Gather = 12,
    WindowPrototypeIndex_SkillResult = 13,
    WindowPrototypeIndex_Inventory = 14,
    WindowPrototypeIndex_PetList = 15,
    WindowPrototypeIndex_PetDetail = 16,
    WindowPrototypeIndex_PetAttributes = 17,
    WindowPrototypeIndex_PetSkills = 18,
    WindowPrototypeIndex_AlbumList = 19,
    WindowPrototypeIndex_AlbumDetail = 20,
    WindowPrototypeIndex_Contacts = 21,
    WindowPrototypeIndex_MailCompose = 22,
    WindowPrototypeIndex_PetMailCompose = 23,
    WindowPrototypeIndex_Settings = 24,
    WindowPrototypeIndex_Hotkeys = 25,
    WindowPrototypeIndex_BattleCommand = 26,
    WindowPrototypeIndex_BattleItems = 27,
    WindowPrototypeIndex_BattlePets = 28,
    WindowPrototypeIndex_Spectator = 29,
    WindowPrototypeIndex_BattleResult = 30,
    WindowPrototypeIndex_DpResult = 31,
    WindowPrototypeIndex_BattleStateNotice = 32,
    WindowPrototypeIndex_MiniMap = 33,
    WindowPrototypeIndex_Emote = 34,
    WindowPrototypeIndex_Chat = 35,
    WindowPrototypeIndex_Dialog = 36, // 窗口36/37共用。
    WindowPrototypeIndex_ItemStorage = 37,
    WindowPrototypeIndex_TimerNotice = 38,
    WindowPrototypeIndex_MailRead = 39,
    WindowPrototypeIndex_Bank = 40,
    WindowPrototypeIndex_Quantity = 41,
    WindowPrototypeIndex_Trade = 42,
    WindowPrototypeIndex_TradeTarget = 43,
    WindowPrototypeIndex_TextTarget = 44,
    WindowPrototypeIndex_AttributeTarget = 45,
    WindowPrototypeIndex_Confirmation = 46,
    WindowPrototypeIndex_GuildDetails = 47,
    WindowPrototypeIndex_GroupSelect = 48,
    WindowPrototypeIndex_MessageBoard = 49,
    WindowPrototypeIndex_TextList = 50,
    WindowPrototypeIndex_ShopChoice = 51,
    WindowPrototypeIndex_Shop = 52,
    WindowPrototypeIndex_PetSkillChoice = 53,
    WindowPrototypeIndex_GuildRank = 54,
    WindowPrototypeIndex_Appearance = 55,
    WindowPrototypeIndex_CompactChat = 56,
    WindowPrototypeIndex_OwnProfile = 57,
    WindowPrototypeIndex_OwnProfileCategory = 58,
    WindowPrototypeIndex_ProfileCategory = 59,
    WindowPrototypeIndex_ProfileSearch = 60,
    WindowPrototypeIndex_ProfileView = 61,
    WindowPrototypeIndex_MiniMail = 62,
    WindowPrototypeIndex_ProfileReply = 63,
    WindowPrototypeIndex_Blacklist = 64,
    WindowPrototypeIndex_AdventureLog = 65,
    WindowPrototypeIndex_OwnStall = 66,
    WindowPrototypeIndex_StallBrowse = 67,
    WindowPrototypeIndex_CrystalShop = 68,
    WindowPrototypeIndex_CrystalExchangeShop = 69,
    WindowPrototypeIndex_Count = 70,
};

} // namespace

// 原型：窗口0；原全局：0x0054EFB8。
WindowPrototype gWindowPrototypeNone {0, 0, 0, 0, 0, 0, 0x00000000u, 0, nullptr, nullptr, nullptr, nullptr}; // 0: 54EFB8

namespace {

// 70个原型对象的编译期指针索引；对象定义位于各窗口文件，记录编号保持。
constexpr std::array<WindowPrototype*, WindowPrototypeIndex_Count> gWindowPrototypeRecords {{
    &gWindowPrototypeNone,
    &gWindowPrototypeStatusBar,
    &gWindowPrototypeCompactStatusBar,
    &gWindowPrototypeCollapsedStatusBar,
    &gWindowPrototypeShortcutBar,
    &gWindowPrototypeCollapsedShortcutBar,
    &gWindowPrototypeBottomBar,
    &gWindowPrototypePlayerProfile,
    &gWindowPrototypePlayerAttributes,
    &gWindowPrototypePlayerTitles,
    &gWindowPrototypeSkill,
    &gWindowPrototypeCraft,
    &gWindowPrototypeGather,
    &gWindowPrototypeSkillResult,
    &gWindowPrototypeInventory,
    &gWindowPrototypePetList,
    &gWindowPrototypePetDetail,
    &gWindowPrototypePetAttributes,
    &gWindowPrototypePetSkills,
    &gWindowPrototypeAlbumList,
    &gWindowPrototypeAlbumDetail,
    &gWindowPrototypeContacts,
    &gWindowPrototypeMailCompose,
    &gWindowPrototypePetMailCompose,
    &gWindowPrototypeSettings,
    &gWindowPrototypeHotkeys,
    &gWindowPrototypeBattleCommand,
    &gWindowPrototypeBattleItems,
    &gWindowPrototypeBattlePets,
    &gWindowPrototypeSpectator,
    &gWindowPrototypeBattleResult,
    &gWindowPrototypeDpResult,
    &gWindowPrototypeBattleStateNotice,
    &gWindowPrototypeMiniMap,
    &gWindowPrototypeEmote,
    &gWindowPrototypeChat,
    &gWindowPrototypeDialog,
    &gWindowPrototypeItemStorage,
    &gWindowPrototypeTimerNotice,
    &gWindowPrototypeMailRead,
    &gWindowPrototypeBank,
    &gWindowPrototypeQuantity,
    &gWindowPrototypeTrade,
    &gWindowPrototypeTradeTarget,
    &gWindowPrototypeTextTarget,
    &gWindowPrototypeAttributeTarget,
    &gWindowPrototypeConfirmation,
    &gWindowPrototypeGuildDetails,
    &gWindowPrototypeGroupSelect,
    &gWindowPrototypeMessageBoard,
    &gWindowPrototypeTextList,
    &gWindowPrototypeShopChoice,
    &gWindowPrototypeShop,
    &gWindowPrototypePetSkillChoice,
    &gWindowPrototypeGuildRank,
    &gWindowPrototypeAppearance,
    &gWindowPrototypeCompactChat,
    &gWindowPrototypeOwnProfile,
    &gWindowPrototypeOwnProfileCategory,
    &gWindowPrototypeProfileCategory,
    &gWindowPrototypeProfileSearch,
    &gWindowPrototypeProfileView,
    &gWindowPrototypeMiniMail,
    &gWindowPrototypeProfileReply,
    &gWindowPrototypeBlacklist,
    &gWindowPrototypeAdventureLog,
    &gWindowPrototypeOwnStall,
    &gWindowPrototypeStallBrowse,
    &gWindowPrototypeCrystalShop,
    &gWindowPrototypeCrystalExchangeShop,
}};

} // namespace

// 71个原生指针；原36/37同源，不因窗口编号不同复制对象。
std::array<WindowPrototype*, kWindowCount> gWindowPrototypes57C130 { // 原全局：0x0057C130。
    gWindowPrototypeRecords[WindowPrototypeIndex_None], // 0
    gWindowPrototypeRecords[WindowPrototypeIndex_StatusBar],
    gWindowPrototypeRecords[WindowPrototypeIndex_CompactStatusBar],
    gWindowPrototypeRecords[WindowPrototypeIndex_CollapsedStatusBar],
    gWindowPrototypeRecords[WindowPrototypeIndex_ShortcutBar],
    gWindowPrototypeRecords[WindowPrototypeIndex_CollapsedShortcutBar],
    gWindowPrototypeRecords[WindowPrototypeIndex_BottomBar],
    gWindowPrototypeRecords[WindowPrototypeIndex_PlayerProfile],
    gWindowPrototypeRecords[WindowPrototypeIndex_PlayerAttributes],
    gWindowPrototypeRecords[WindowPrototypeIndex_PlayerTitles],
    gWindowPrototypeRecords[WindowPrototypeIndex_Skill], // 10
    gWindowPrototypeRecords[WindowPrototypeIndex_Craft],
    gWindowPrototypeRecords[WindowPrototypeIndex_Gather],
    gWindowPrototypeRecords[WindowPrototypeIndex_SkillResult],
    gWindowPrototypeRecords[WindowPrototypeIndex_Inventory],
    gWindowPrototypeRecords[WindowPrototypeIndex_PetList],
    gWindowPrototypeRecords[WindowPrototypeIndex_PetDetail],
    gWindowPrototypeRecords[WindowPrototypeIndex_PetAttributes],
    gWindowPrototypeRecords[WindowPrototypeIndex_PetSkills],
    gWindowPrototypeRecords[WindowPrototypeIndex_AlbumList],
    gWindowPrototypeRecords[WindowPrototypeIndex_AlbumDetail], // 20
    gWindowPrototypeRecords[WindowPrototypeIndex_Contacts],
    gWindowPrototypeRecords[WindowPrototypeIndex_MailCompose],
    gWindowPrototypeRecords[WindowPrototypeIndex_PetMailCompose],
    gWindowPrototypeRecords[WindowPrototypeIndex_Settings],
    gWindowPrototypeRecords[WindowPrototypeIndex_Hotkeys],
    gWindowPrototypeRecords[WindowPrototypeIndex_BattleCommand],
    gWindowPrototypeRecords[WindowPrototypeIndex_BattleItems],
    gWindowPrototypeRecords[WindowPrototypeIndex_BattlePets],
    gWindowPrototypeRecords[WindowPrototypeIndex_Spectator],
    gWindowPrototypeRecords[WindowPrototypeIndex_BattleResult], // 30
    gWindowPrototypeRecords[WindowPrototypeIndex_DpResult],
    gWindowPrototypeRecords[WindowPrototypeIndex_BattleStateNotice],
    gWindowPrototypeRecords[WindowPrototypeIndex_MiniMap],
    gWindowPrototypeRecords[WindowPrototypeIndex_Emote],
    gWindowPrototypeRecords[WindowPrototypeIndex_Chat],
    gWindowPrototypeRecords[WindowPrototypeIndex_Dialog],
    gWindowPrototypeRecords[WindowPrototypeIndex_Dialog], // 37
    gWindowPrototypeRecords[WindowPrototypeIndex_ItemStorage],
    gWindowPrototypeRecords[WindowPrototypeIndex_TimerNotice],
    gWindowPrototypeRecords[WindowPrototypeIndex_MailRead], // 40
    gWindowPrototypeRecords[WindowPrototypeIndex_Bank],
    gWindowPrototypeRecords[WindowPrototypeIndex_Quantity],
    gWindowPrototypeRecords[WindowPrototypeIndex_Trade],
    gWindowPrototypeRecords[WindowPrototypeIndex_TradeTarget],
    gWindowPrototypeRecords[WindowPrototypeIndex_TextTarget],
    gWindowPrototypeRecords[WindowPrototypeIndex_AttributeTarget],
    gWindowPrototypeRecords[WindowPrototypeIndex_Confirmation],
    gWindowPrototypeRecords[WindowPrototypeIndex_GuildDetails],
    gWindowPrototypeRecords[WindowPrototypeIndex_GroupSelect],
    gWindowPrototypeRecords[WindowPrototypeIndex_MessageBoard], // 50
    gWindowPrototypeRecords[WindowPrototypeIndex_TextList],
    gWindowPrototypeRecords[WindowPrototypeIndex_ShopChoice],
    gWindowPrototypeRecords[WindowPrototypeIndex_Shop],
    gWindowPrototypeRecords[WindowPrototypeIndex_PetSkillChoice],
    gWindowPrototypeRecords[WindowPrototypeIndex_GuildRank],
    gWindowPrototypeRecords[WindowPrototypeIndex_Appearance],
    gWindowPrototypeRecords[WindowPrototypeIndex_CompactChat],
    gWindowPrototypeRecords[WindowPrototypeIndex_OwnProfile],
    gWindowPrototypeRecords[WindowPrototypeIndex_OwnProfileCategory],
    gWindowPrototypeRecords[WindowPrototypeIndex_ProfileCategory], // 60
    gWindowPrototypeRecords[WindowPrototypeIndex_ProfileSearch],
    gWindowPrototypeRecords[WindowPrototypeIndex_ProfileView],
    gWindowPrototypeRecords[WindowPrototypeIndex_MiniMail],
    gWindowPrototypeRecords[WindowPrototypeIndex_ProfileReply],
    gWindowPrototypeRecords[WindowPrototypeIndex_Blacklist],
    gWindowPrototypeRecords[WindowPrototypeIndex_AdventureLog],
    gWindowPrototypeRecords[WindowPrototypeIndex_OwnStall],
    gWindowPrototypeRecords[WindowPrototypeIndex_StallBrowse],
    gWindowPrototypeRecords[WindowPrototypeIndex_CrystalShop],
    gWindowPrototypeRecords[WindowPrototypeIndex_CrystalExchangeShop], // 70
};

} // namespace cg::ui::detail
