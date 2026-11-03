#pragma once

#include "WindowRuntimeState.hpp"

// 各窗口具名原型、跨窗口共享载荷与回调；单窗口定义留在对应实现文件。
namespace cg::ui::detail {

// 原函数：0x004A5650。
void WindowMailCompose_Release(WindowInstance&);

// 71槽注册表直接引用这些原型；窗口36／37共用Dialog。
extern WindowPrototype gWindowPrototypeStatusBar; // 原全局：0x0054E2A0。
extern WindowPrototype gWindowPrototypeCompactStatusBar; // 原全局：0x0054E2D0。
extern WindowPrototype gWindowPrototypeCollapsedStatusBar; // 原全局：0x0054E300。
extern WindowPrototype gWindowPrototypeShortcutBar; // 原全局：0x0054E240。
extern WindowPrototype gWindowPrototypeCollapsedShortcutBar; // 原全局：0x0054E270。
extern WindowPrototype gWindowPrototypeBottomBar; // 原全局：0x0054E330。
extern WindowPrototype gWindowPrototypePlayerProfile; // 原全局：0x0054E360。
extern WindowPrototype gWindowPrototypePlayerAttributes; // 原全局：0x0054E390。
extern WindowPrototype gWindowPrototypePlayerTitles; // 原全局：0x0054E3C0。
extern WindowPrototype gWindowPrototypeSkill; // 原全局：0x0054E420。
extern WindowPrototype gWindowPrototypeCraft; // 原全局：0x0054E450。
extern WindowPrototype gWindowPrototypeGather; // 原全局：0x0054E480。
extern WindowPrototype gWindowPrototypeSkillResult; // 原全局：0x0054E4B0。
extern WindowPrototype gWindowPrototypeInventory; // 原全局：0x0054E4E0。
extern WindowPrototype gWindowPrototypePetList; // 原全局：0x0054E540。
extern WindowPrototype gWindowPrototypePetDetail; // 原全局：0x0054E570。
extern WindowPrototype gWindowPrototypePetAttributes; // 原全局：0x0054E5A0。
extern WindowPrototype gWindowPrototypePetSkills; // 原全局：0x0054E5D0。
extern WindowPrototype gWindowPrototypePetEquipment; // 宠物装备页原型。
extern WindowGraphicData gPetEquipmentPageGraphic; // 四个宠物详情页共用的装备页签原型。
extern WindowPrototype gWindowPrototypeAlbumList; // 原全局：0x0054E600。
extern WindowPrototype gWindowPrototypeAlbumDetail; // 原全局：0x0054E630。
extern WindowPrototype gWindowPrototypeContacts; // 原全局：0x0054E990。
extern WindowPrototype gWindowPrototypeMailCompose; // 原全局：0x0054EA20。
extern WindowPrototype gWindowPrototypePetMailCompose; // 原全局：0x0054EA50。
extern WindowPrototype gWindowPrototypeSettings; // 原全局：0x0054E660。
extern WindowPrototype gWindowPrototypeHotkeys; // 原全局：0x0054E690。
extern WindowPrototype gWindowPrototypeBattleCommand; // 原全局：0x0054E720。
extern WindowPrototype gWindowPrototypeBattleItems; // 原全局：0x0054E7E0。
extern WindowPrototype gWindowPrototypeBattlePets; // 原全局：0x0054E810。
extern WindowPrototype gWindowPrototypeSpectator; // 原全局：0x0054E750。
extern WindowPrototype gWindowPrototypeBattleResult; // 原全局：0x0054E780。
extern WindowPrototype gWindowPrototypeDpResult; // 原全局：0x0054E7B0。
extern WindowPrototype gWindowPrototypeBattleStateNotice; // 原全局：0x0054E840。
extern WindowPrototype gWindowPrototypeMiniMap; // 原全局：0x0054E6C0。
extern WindowPrototype gWindowPrototypeEmote; // 原全局：0x0054E6F0。
extern WindowPrototype gWindowPrototypeChat; // 原全局：0x0054EAB0。
extern WindowPrototype gWindowPrototypeDialog; // 原全局：0x0054E870。
extern WindowPrototype gWindowPrototypeItemStorage; // 原全局：0x0054EB40。
extern WindowPrototype gWindowPrototypeTimerNotice; // 原全局：0x0054E8A0。
extern WindowPrototype gWindowPrototypeMailRead; // 原全局：0x0054E9F0。
extern WindowPrototype gWindowPrototypeBank; // 原全局：0x0054EB10。
extern WindowPrototype gWindowPrototypeQuantity; // 原全局：0x0054E510。
extern WindowPrototype gWindowPrototypeTrade; // 原全局：0x0054EBD0。
extern WindowPrototype gWindowPrototypeTradeTarget; // 原全局：0x0054E8D0。
extern WindowPrototype gWindowPrototypeTextTarget; // 原全局：0x0054E900。
extern WindowPrototype gWindowPrototypeAttributeTarget; // 原全局：0x0054E930。
extern WindowPrototype gWindowPrototypeConfirmation; // 原全局：0x0054EA80。
extern WindowPrototype gWindowPrototypeGuildDetails; // 原全局：0x0054EBA0。
extern WindowPrototype gWindowPrototypeGroupSelect; // 原全局：0x0054EC00。
extern WindowPrototype gWindowPrototypeMessageBoard; // 原全局：0x0054EC30。
extern WindowPrototype gWindowPrototypeTextList; // 原全局：0x0054EB70。
extern WindowPrototype gWindowPrototypeShopChoice; // 原全局：0x0054E960。
extern WindowPrototype gWindowPrototypeShop; // 原全局：0x0054EC60。
extern WindowPrototype gWindowPrototypePetSkillChoice; // 原全局：0x0054EC90。
extern WindowPrototype gWindowPrototypeGuildRank; // 原全局：0x0054E9C0。
extern WindowPrototype gWindowPrototypeAppearance; // 原全局：0x0054ED20。
extern WindowPrototype gWindowPrototypeCompactChat; // 原全局：0x0054EAE0。
extern WindowPrototype gWindowPrototypeOwnProfile; // 原全局：0x0054E3F0。
extern WindowPrototype gWindowPrototypeOwnProfileCategory; // 原全局：0x0054ED60。
extern WindowPrototype gWindowPrototypeProfileCategory; // 原全局：0x0054ED90。
extern WindowPrototype gWindowPrototypeProfileSearch; // 原全局：0x0054EDC0。
extern WindowPrototype gWindowPrototypeProfileView; // 原全局：0x0054EDF0。
extern WindowPrototype gWindowPrototypeMiniMail; // 原全局：0x0054EE20。
extern WindowPrototype gWindowPrototypeProfileReply; // 原全局：0x0054EE50。
extern WindowPrototype gWindowPrototypeBlacklist; // 原全局：0x0054EE80。
extern WindowPrototype gWindowPrototypeAdventureLog; // 原全局：0x0054EEB0。
extern WindowPrototype gWindowPrototypeOwnStall; // 原全局：0x0054EF58。
extern WindowPrototype gWindowPrototypeStallBrowse; // 原全局：0x0054EF88。
extern WindowPrototype gWindowPrototypeCrystalShop; // 原全局：0x0054ECC0。
extern WindowPrototype gWindowPrototypeCrystalExchangeShop; // 原全局：0x0054ECF0。
extern WindowPrototype gWindowPrototypeBigMap; // BMWP01：大地图原生原型。

// 跨窗口共享载荷；本头只声明，中央原型文件保留唯一定义。
extern WindowGraphicData gWindowGraphic570160; // 原全局：0x00570160。
extern WindowGraphicData gWindowGraphic570190; // 原全局：0x00570190。
extern WindowTextPrototype gWindowText5701F0; // 原全局：0x005701F0。 // 原常量参考：0x00585EA4。
extern WindowGraphicData gWindowGraphic565608; // 原全局：0x00565608。
extern WindowGraphicData gWindowGraphic565620; // 原全局：0x00565620。
extern WindowGraphicData gWindowGraphic565638; // 原全局：0x00565638。
extern WindowGraphicData gWindowGraphic565650; // 原全局：0x00565650。
extern WindowGraphicData gWindowGraphic5656B0; // 原全局：0x005656B0。
extern WindowGraphicData gWindowGraphic5656C8; // 原全局：0x005656C8。
extern WindowGraphicData gWindowGraphic5656E0; // 原全局：0x005656E0。
extern WindowGraphicData gWindowGraphic565878; // 原全局：0x00565878。
extern WindowGraphicData gWindowGraphic565890; // 原全局：0x00565890。
extern WindowGraphicData gWindowGraphic5658C0; // 原全局：0x005658C0。
extern WindowTextPrototype gWindowText565950; // 原全局：0x00565950。 // 原常量参考：0x00586230。
extern WindowTextPrototype gWindowText565988; // 原全局：0x00565988。 // 原常量参考：0x0058600C。
extern WindowTextPrototype gWindowText562C70; // 原全局：0x00562C70。 // 原常量参考：0x00586054。
extern WindowTextPrototype gWindowText562C78; // 原全局：0x00562C78。 // 原常量参考：0x00586054。
extern WindowGraphicData gWindowGraphicD678E0; // 原全局：0x00D678E0。

// 原函数：0x004A4210；原型释放回调共用的返回值适配。
void WindowRuntime_ReleaseDefault(WindowInstance& window);

} // namespace cg::ui::detail
