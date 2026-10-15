#pragma once

#include "WindowRuntimeState.hpp"

namespace cg::ui::detail {

// 原57C130指向的元素数组；实际初值和回调绑定位于windows/各窗口文件；共享载荷与索引表位于WindowPrototypes.cpp，本头声明供窗口调用侧使用。
extern WindowElementPrototype gWindowElements55ED60[11]; // 原全局：0x0055ED60。
extern WindowElementPrototype gWindowElements55EEC0[2]; // 原全局：0x0055EEC0。
extern WindowElementPrototype gWindowElements55F0A0[24]; // 原全局：0x0055F0A0。
extern WindowElementPrototype gWindowElements55F3A0[7]; // 原全局：0x0055F3A0。
extern WindowElementPrototype gWindowElements55F480[2]; // 原全局：0x0055F480。
extern WindowElementPrototype gWindowElements55F5C0[14]; // 原全局：0x0055F5C0。
extern WindowElementPrototype gWindowElements55FD60[73]; // 原全局：0x0055FD60。
extern WindowElementPrototype gWindowElements560840[43]; // 原全局：0x00560840。
extern WindowElementPrototype gWindowElements560F50[37]; // 原全局：0x00560F50。
extern WindowElementPrototype gWindowElements561558[34]; // 原全局：0x00561558。
extern WindowElementPrototype gWindowElements561B78[30]; // 原全局：0x00561B78。
extern WindowElementPrototype gWindowElements5620E0[31]; // 原全局：0x005620E0。
extern WindowElementPrototype gWindowElements562620[15]; // 原全局：0x00562620。
extern WindowElementPrototype gWindowElements562930[14]; // 原全局：0x00562930。
extern WindowElementPrototype gWindowElements562CD8[24]; // 原全局：0x00562CD8。
extern WindowElementPrototype gWindowElements5630F0[8]; // 原全局：0x005630F0。
extern WindowElementPrototype gWindowElements563500[259]; // 原全局：0x00563500。
extern WindowElementPrototype gWindowElements5659C0[66]; // 原全局：0x005659C0。
extern WindowElementPrototype gWindowElements566200[46]; // 原全局：0x00566200。
extern WindowElementPrototype gWindowElements566810[51]; // 原全局：0x00566810。
extern WindowElementPrototype gWindowElements567028[71]; // 原全局：0x00567028。
extern WindowElementPrototype gWindowElements567AE8[82]; // 原全局：0x00567AE8。
extern WindowElementPrototype gWindowElements5689C8[32]; // 原全局：0x005689C8。
extern WindowElementPrototype gWindowElements568F78[25]; // 原全局：0x00568F78。
extern WindowElementPrototype gWindowElements56AF50[7]; // 原全局：0x0056AF50。
extern WindowElementPrototype gWindowElements56B2B8[20]; // 原全局：0x0056B2B8。
extern WindowElementPrototype gWindowElements56B710[14]; // 原全局：0x0056B710。
extern WindowElementPrototype gWindowElements56B8D8[2]; // 原全局：0x0056B8D8。
extern WindowElementPrototype gWindowElements56B9B8[66]; // 原全局：0x0056B9B8。
extern WindowElementPrototype gWindowElements56C2C8[4]; // 原全局：0x0056C2C8。
extern WindowElementPrototype gWindowElements56C430[22]; // 原全局：0x0056C430。
extern WindowElementPrototype gWindowElements56C800[8]; // 原全局：0x0056C800。
extern WindowElementPrototype gWindowElements56C9B8[2]; // 原全局：0x0056C9B8。
extern WindowElementPrototype gWindowElements56CA88[19]; // 原全局：0x0056CA88。
extern WindowElementPrototype gWindowElements56CCD0[1]; // 原全局：0x0056CCD0。
extern WindowElementPrototype gWindowElements56CDB0[19]; // 原全局：0x0056CDB0。
extern WindowElementPrototype gWindowElements56D0B0[19]; // 原全局：0x0056D0B0。
extern WindowElementPrototype gWindowElements56D3E8[34]; // 原全局：0x0056D3E8。
extern WindowElementPrototype gWindowElements56D838[3]; // 原全局：0x0056D838。
extern WindowElementPrototype gWindowElements56DB38[111]; // 原全局：0x0056DB38。
extern WindowElementPrototype gWindowElements56F860[29]; // 原全局：0x0056F860。
extern WindowElementPrototype gWindowElements56FD70[21]; // 原全局：0x0056FD70。
extern WindowElementPrototype gWindowElements5701F8[9]; // 原全局：0x005701F8。
extern WindowElementPrototype gWindowElements570318[13]; // 原全局：0x00570318。
extern WindowElementPrototype gWindowElements5706D0[34]; // 原全局：0x005706D0。
extern WindowElementPrototype gWindowElements570CB0[20]; // 原全局：0x00570CB0。
extern WindowElementPrototype gWindowElements570FD8[2]; // 原全局：0x00570FD8。
extern WindowElementPrototype gWindowElements571160[38]; // 原全局：0x00571160。
extern WindowElementPrototype gWindowElements571740[6]; // 原全局：0x00571740。
extern WindowElementPrototype gWindowElements5719E8[26]; // 原全局：0x005719E8。
extern WindowElementPrototype gWindowElements571F18[76]; // 原全局：0x00571F18。
extern WindowElementPrototype gWindowElements572AF8[20]; // 原全局：0x00572AF8。
extern WindowElementPrototype gWindowElements572F90[37]; // 原全局：0x00572F90。
extern WindowElementPrototype gWindowElements5735C8[36]; // 原全局：0x005735C8。
extern WindowElementPrototype gWindowElements573D80[66]; // 原全局：0x00573D80。
extern WindowElementPrototype gWindowElements5748E8[107]; // 原全局：0x005748E8。
extern WindowElementPrototype gWindowElements575988[70]; // 原全局：0x00575988。
extern WindowElementPrototype gWindowElements576588[70]; // 原全局：0x00576588。
extern WindowElementPrototype gWindowElements576F90[15]; // 原全局：0x00576F90。
extern WindowElementPrototype gWindowElements577298[32]; // 原全局：0x00577298。
extern WindowElementPrototype gWindowElements5777A8[162]; // 原全局：0x005777A8。
extern WindowElementPrototype gWindowElements578CC8[71]; // 原全局：0x00578CC8。
extern WindowElementPrototype gWindowElements579710[22]; // 原全局：0x00579710。
extern WindowElementPrototype gWindowElements579B28[16]; // 原全局：0x00579B28。
extern WindowElementPrototype gWindowElements57A070[21]; // 原全局：0x0057A070。
extern WindowElementPrototype gWindowElements57A450[71]; // 原全局：0x0057A450。
extern WindowElementPrototype gWindowElements57AEC0[25]; // 原全局：0x0057AEC0。
extern WindowElementPrototype gWindowElements57B508[48]; // 原全局：0x0057B508。
extern WindowElementPrototype gWindowElements57BB08[43]; // 原全局：0x0057BB08。

// 主原型中的真实回调依赖；本声明只约束窗口调用侧，不替代各本体取证。
// 原函数：0x0046FBA0。
std::int32_t WindowShortcutBar_Update(std::int32_t value);
// 原函数：0x00470A80。
// 关联原入口：0x00470A9F（共享尾已并入本函数正文）。
std::int32_t WindowStatusBar_Update(std::int32_t value);
// 原函数：0x00470AB0。
std::int32_t WindowCompactStatusBar_Update(std::int32_t value);
// HWT01：窗口6按原显示/复制阶段消费共享悬停文本，颜色只读低BYTE，尾部清槽并返S32 1。
// 原函数：0x004731E0。
std::int32_t WindowBottomBar_Update(std::int32_t value);
// 原函数：0x00474310。
void WindowPlayerProfile_Release();
// 原函数：0x00474360。
void WindowPlayerAttributes_Release();
// 原函数：0x004744A0。
void WindowPlayerTitles_Release();
// 原函数：0x004744F0。
void WindowOwnProfile_Release();
// 原函数：0x00474540。
std::int32_t WindowPlayerProfile_Update(std::int32_t value);
// 原函数：0x00475DF0。
std::int32_t WindowPlayerAttributes_Update(std::int32_t value);
// 原函数：0x004774C0。
// 关联原入口：0x0047752C（共享尾已并入本函数正文）。
std::int32_t WindowPlayerTitles_Update(std::int32_t value);
// 原函数：0x00478320。
std::int32_t WindowOwnProfile_Update(std::int32_t value);
// 原函数：0x0047A620。
std::int32_t WindowSkill_Update(std::int32_t value);
// 原函数：0x0047A7A0。
std::int32_t WindowSkill_Draw(std::int32_t value);
// 原函数：0x0047B0E0。
void WindowSkill_Release();
// 原函数：0x00481010。
void WindowCraft_Release();
// 原函数：0x004810A0。
std::int32_t WindowCraft_Update(std::int32_t value);
// 原函数：0x00482000。
std::int32_t WindowCraft_Draw(std::int32_t value);
// 原函数：0x00485690。
void WindowGather_Release();
// 原函数：0x00486040。
std::int32_t WindowGather_Update(std::int32_t value);
// 原函数：0x00487660。
void WindowSkillResult_Release();
// 原函数：0x00487AE0。
std::int32_t WindowSkillResult_Update(std::int32_t value);
// 原函数：0x00487B80。
std::int32_t WindowSkillResult_Draw(std::int32_t value);
// 原函数：0x00488510。
void WindowInventory_Release();
// 原函数：0x00488990。
std::int32_t WindowInventory_Update(std::int32_t value);
// 原函数：0x00488B10。
std::int32_t WindowInventory_Draw(std::int32_t value);
// 原函数：0x0048A8C0。
std::int32_t WindowQuantity_Update(std::int32_t value);
// 原函数：0x0048C1B0。
// 关联原入口：0x0048C1CF（共享尾已并入本函数正文）。
std::int32_t WindowPetList_Update(std::int32_t value);
// 原函数：0x0048D9A0。
std::int32_t WindowPetDetail_Update(std::int32_t value);
// 原函数：0x0048DAB0。
std::int32_t WindowPetAttributes_Update(std::int32_t value);
// 原函数：0x0048DBB0。
std::int32_t WindowPetSkills_Update(std::int32_t value);
// 原函数：0x0048DCB0。
std::int32_t WindowPetSkills_Draw(std::int32_t value);
// 原函数：0x0048DCC0。
void WindowPetDetail_Release();
// 原函数：0x0048DD00。
void WindowPetAttributes_Release();
// 原函数：0x0048DD40。
void WindowPetSkills_Release();
// 原函数：0x004923F0。
std::int32_t WindowAlbumList_Update(std::int32_t value);
// 原函数：0x00492410。
std::int32_t WindowAlbumDetail_Update(std::int32_t value);
// 原函数：0x00493D60。
std::int32_t WindowSettings_Update(std::int32_t value);
// 原函数：0x00493DB0。
// 关联原入口：0x00493DF9（共享尾已并入本函数正文）。
std::int32_t WindowSettings_Draw(std::int32_t value);
// 原函数：0x00493E10。
void WindowSettings_Release();
// 原函数：0x00495130。
std::int32_t WindowHotkeys_Update(std::int32_t value);
// 原函数：0x004951C0。
std::int32_t WindowHotkeys_Draw(std::int32_t value);
// 原函数：0x00495D70。
std::int32_t WindowMiniMap_Update(std::int32_t value);
// 原函数：0x00495F50。
std::int32_t WindowEmote_Update(std::int32_t value);
// 原函数：0x00496B90。
std::int32_t WindowBattleCommand_Update(std::int32_t value);
// 原函数：0x00496D50。
std::int32_t WindowBattleCommand_Draw(std::int32_t value);
// 原函数：0x004991C0。
std::int32_t WindowBattleResult_Update(std::int32_t value);
// 原函数：0x004996C0。
std::int32_t WindowBattleResult_Draw(std::int32_t value);
// 原函数：0x00499790。
std::int32_t WindowDpResult_Update(std::int32_t value);
// 原函数：0x00499990。
std::int32_t WindowBattleItems_Update(std::int32_t value);
// 原函数：0x004999D0。
std::int32_t WindowBattleItems_Draw(std::int32_t value);
// 原函数：0x0049BD40。
std::int32_t WindowBattleStateNotice_Update(std::int32_t value);
// 原函数：0x0049C710。
std::int32_t WindowDialog_Update(std::int32_t value);
// 原函数：0x0049CAD0。
std::int32_t WindowDialog_Draw(std::int32_t value);
// 原函数：0x0049CDF0。
void WindowDialog_Release();
// 原函数：0x0049FD20。
std::int32_t WindowShopChoice_Update(std::int32_t value);
// 原函数：0x0049FE40。
std::int32_t WindowShopChoice_Draw(std::int32_t value);
// 原函数：0x0049FF70。
void WindowShopChoice_Release();
// 原函数：0x004A0970。
std::int32_t WindowContacts_Update(std::int32_t value);
// 原函数：0x004A09B0。
std::int32_t WindowContacts_Draw(std::int32_t value);
// 原函数：0x004A4180。
std::int32_t WindowGuildRank_Update(std::int32_t value);
// 原函数：0x004A41A0。
std::int32_t WindowGuildRank_Draw(std::int32_t value);
// 原函数：0x004A4620。
std::int32_t WindowMailRead_Update(std::int32_t value);
// 原函数：0x004A4650。
std::int32_t WindowMailRead_Draw(std::int32_t value);
// 原函数：0x004A53F0。
std::int32_t WindowMailCompose_Update(std::int32_t value);
// 原函数：0x004A5650。
void WindowMailCompose_Release();
// 原函数：0x004A56F0。
std::int32_t WindowPetMailCompose_Update(std::int32_t value);
// 原函数：0x004A6DD0。
std::int32_t WindowChat_Update(std::int32_t value); // CWC01：见聊天窗口控制完整合同。
// 原函数：0x004A8240。
std::int32_t WindowCompactChat_Update(std::int32_t value);
// CFD01：窗口35/57共用C9A96C绘制门禁与原返1，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
// 原函数：0x004A82C0。
std::int32_t WindowChat_Draw(std::int32_t value);
// 原函数：0x004A83D0。
void WindowBank_Release();
// 原函数：0x004A9020。
std::int32_t WindowBank_Update(std::int32_t value);
// 原函数：0x004A9160。
std::int32_t WindowBank_Draw(std::int32_t value);
// 原函数：0x004AC0F0。
void WindowItemStorage_Release();
// 原函数：0x004AC140。
std::int32_t WindowItemStorage_Update(std::int32_t value);
// 原函数：0x004AC1B0。
// 关联原入口：0x004AC1EC（共享尾已并入本函数正文）。
std::int32_t WindowItemStorage_Draw(std::int32_t value);
// 原函数：0x004AD780。
void WindowTextList_Release();
// 原函数：0x004AD800。
std::int32_t WindowTextList_Update(std::int32_t value);
// 原函数：0x004AD9F0。
std::int32_t WindowTextList_Draw(std::int32_t value);
// 原函数：0x004AE5F0。
std::int32_t WindowConfirmation_Update(std::int32_t value);
// 原函数：0x004AEAE0。
std::int32_t WindowGuildDetails_Update(std::int32_t value);
// 原函数：0x004AEC10。
std::int32_t WindowGuildDetails_Draw(std::int32_t value);
// 原函数：0x004B4B00。
std::int32_t WindowGroupSelect_Update(std::int32_t value);
// 原函数：0x004B4B70。
void WindowGroupSelect_Release();
// 原函数：0x004B4B90。
std::int32_t WindowGroupSelect_Draw(std::int32_t value);
// 原函数：0x004B5AD0。
std::int32_t WindowMessageBoard_Update(std::int32_t value);
// 原函数：0x004B5B50。
void WindowMessageBoard_Release();
// 原函数：0x004B6DC0。
void WindowShop_Release();
// 原函数：0x004B9310。
std::int32_t WindowShop_Update(std::int32_t value);
// 原函数：0x004B99D0。
std::int32_t WindowShop_Draw(std::int32_t value);
// 原函数：0x004BD500。
void WindowPetSkillChoice_Release();
// 原函数：0x004BF7C0。
std::int32_t WindowPetSkillChoice_Update(std::int32_t value);
// 原函数：0x004BF950。
std::int32_t WindowPetSkillChoice_Draw(std::int32_t value);
// 原函数：0x004C1390。
void WindowCrystalShop_Release();
// 原函数：0x004C1C50。
std::int32_t WindowCrystalShop_Update(std::int32_t value);
// 原函数：0x004C1EC0。
std::int32_t WindowCrystalShop_Draw(std::int32_t value);
// 原函数：0x004C3530。
void WindowCrystalExchangeShop_Release();
// 原函数：0x004C3F00。
std::int32_t WindowCrystalExchangeShop_Update(std::int32_t value);
// 原函数：0x004C4180。
std::int32_t WindowCrystalExchangeShop_Draw(std::int32_t value);
// 原函数：0x004C5850。
std::int32_t WindowAppearance_Update(std::int32_t value);
// 原函数：0x004C5B70。
std::int32_t WindowAppearance_Draw(std::int32_t value);
// 原函数：0x004C5C00。
void WindowAppearance_Release();
// 原函数：0x004C94B0。
// 关联原入口：0x004C94D6（共享尾已并入本函数正文）。
std::int32_t WindowOwnProfileCategory_Update(std::int32_t value);
// 原函数：0x004CBD90。
std::int32_t WindowAdventureLog_Update(std::int32_t value);
// 原函数：0x004CC6C0。
std::int32_t WindowBlacklist_Update(std::int32_t value);


// 各窗口唯一原型对象；71槽索引仍由WindowPrototypes.cpp统一绑定。
extern WindowPrototype gWindowPrototypeNone; // 原全局：0x0054EFB8。
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
void WindowRuntime_ReleaseDefault();

} // namespace cg::ui::detail
