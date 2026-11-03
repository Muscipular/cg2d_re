#pragma once

#include <cstdint>

namespace cg::network {
class CgConnection;
enum class SendResult : std::int32_t;
}
namespace cg::session { class ClientSession; }

#include <array>
#include <cstddef>
#include <string>
#include <type_traits>
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/world/PlayerItemRecord.hpp"
#include "cg/world/PlayerRuntimeSlots.hpp"
#include "cg/world/PetRuntimeRecord.hpp"
#include "cg/world/PlayerSlots.hpp"
#include "PetTechniqueRecord.hpp"

// 玩家、技能、宠物与称号的共享声明。

namespace cg::world {

// 原[509650,509B96)，玩家属性掩码回复，socket未用。
std::int32_t HandlePlayerPrimaryStatsReply_509650(cg::network::CgConnection& networkConnection, const char* payload); // MPR01：aPc掩码主属性，六CryptInt原序/金额和双尾字段，真实S32分界。
// 原函数：0x00509BA0。
void HandlePlayerSecondaryStatsReply_509BA0(cg::network::CgConnection& networkConnection, const char* payload); // MPR01：UU掩码附加属性/三金额上限、原步速修正和action发布，void副作用/完整ABI待。
// 原函数：0x00509E30。
void HandlePlayerProfessionReply_509E30(cg::network::CgConnection& networkConnection, std::int32_t value, char* text); // 先写低U8，再原位反转义职业名。
const std::wstring& GetPlayerJobName() noexcept; // F1BAE0职业名，同源28B字段只读借用。

// 原技能+3C的94H技术记录；50A120生产，数值语义未证的字段保持地址命名。
struct SkillTechniqueRecord {
    std::wstring name; // 原成员偏移：+0x00。
    std::wstring description; // 原成员偏移：+0x19。
    std::array<std::byte, 2> bytes7A; // 原成员偏移：+0x7A。
    std::int32_t value7C; // 原成员偏移：+0x7C。
    std::int32_t value80; // 原成员偏移：+0x80。
    std::int32_t value84; // 原成员偏移：+0x84。
    std::uint8_t used88; // 原成员偏移：+0x88。
    std::array<std::byte, 3> bytes89; // 原成员偏移：+0x89。
    std::int32_t value8C; // 原成员偏移：+0x8C。
    std::int32_t value90; // 原成员偏移：+0x90。
};

// 原技能配方材料0x28字节，50C580生产、519950只清itemId、4F4630查count。
struct SkillRecipeItem {
    std::int32_t itemId;
    std::int32_t count; // 原成员偏移：+0x04。
    std::wstring name; // 原成员偏移：+0x08。
    std::array<std::byte, 3> bytes25; // 原成员偏移：+0x25。
};

struct SkillRecipeRecord {
    std::int32_t itemId;
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t level; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::int32_t value10; // 原成员偏移：+0x10。
    std::wstring name; // 原成员偏移：+0x14。
    std::wstring description; // 原成员偏移：+0x31。
    std::array<std::byte, 2> bytes6A; // 原成员偏移：+0x6A。
    std::array<SkillRecipeItem, 5> items6C; // 原成员偏移：+0x6C。
};

// 原F1BAFC起的技能记录；未消费字段不计最终业务映射。
struct SkillRuntimeRecord {
    std::wstring name;
    std::array<std::uint8_t, 2> requiredSlots19; // 原成员偏移：+0x19。 回复写低U8；4740F0读S16，窗口54读低S8。
    std::byte byte1B; // 原成员偏移：+0x1B。
    std::int32_t level; // 原成员偏移：+0x1C。
    std::int32_t maxLevel; // 原成员偏移：+0x20。
    std::int32_t experience; // 原成员偏移：+0x24。
    std::int32_t nextExperience; // 原成员偏移：+0x28。
    std::int32_t canRebirth; // 原成员偏移：+0x2C。
    std::int32_t skillId; // 原成员偏移：+0x30。
    std::int32_t fpReduce; // 原成员偏移：+0x34。
    std::int32_t sequence; // 原成员偏移：+0x38。
    std::array<SkillTechniqueRecord, kTechniqueSlotCount> techniques3C; // 原成员偏移：+0x3C；现扩为30栏。
    std::array<SkillRecipeRecord, 56> recipes8E8; // 原成员偏移：+0x8E8。
    std::int32_t rebirthState4C48; // 原F20744；不是旧文档的+744。
};

extern std::array<SkillRuntimeRecord, kSkillSlotCount>& gSkillsF1BAFC; // 原全局：0x00F1BAFC。 同一第四清零块成员别名。
extern std::array<SkillPairRecord, kSkillSlotCount>& gSkillPairsF63370; // 原全局：0x00F63370。
// 原479FBE按S8索引读取U16；按用户确认，-1现表示未选中，不再读取原末条技能+4C44。
std::uint16_t ReadSkillPairUsed(std::int8_t pairIndex);
extern std::uint16_t gSkillReorderPending_DA3B20; // 原全局：0x00DA3B20。 原技能拖放门禁，509EB0尾部清零。
// 原函数：0x00505040。
std::int32_t IsItemEligibleForDecoration_505040(session::ClientSession& session, std::int32_t slot); // 说明/类别/flag800及制造配方条件。
// 原函数：0x005050B0。
std::int32_t IsItemEligibleForPetDecoration_5050B0(session::ClientSession& session, std::int32_t slot); // 类别80或flag800与601..605技能配方条件。
// 原函数：0x00505110。
std::int32_t IsItemEligibleForRefining_505110(session::ClientSession& session, std::int32_t slot); // 说明、两段物品类别及flag1000条件。
// 原函数：0x00509EB0。
void HandlePlayerSkillsReply_509EB0(cg::network::CgConnection& networkConnection, const char* payload);
// 原函数：0x0050A120。
void HandlePlayerSkillTechniquesReply_50A120(cg::network::CgConnection& networkConnection, std::int32_t skillIndex, const char* payload); // 清技术区后按六字段生产。
// 原函数：0x0050C580。
void HandleSkillRecipesReply_50C580(cg::network::CgConnection& networkConnection, std::int32_t index, const char* payload); // 固定56行、每行五材料。

// 原函数：0x0050BFD0。
void HandleFullPlayerInventoryReply_50BFD0(cg::network::CgConnection& networkConnection, const char* payload); // 固定28槽全量回复。
// 原函数：0x0050C260。
void HandlePlayerInventoryUpdatesReply_50C260(cg::network::CgConnection& networkConnection, const char* payload); // 带槽号的14字段增量回复。
// 原函数：0x0050C900。
void HandleInventorySlotSwapReply_50C900(cg::network::CgConnection& networkConnection, std::int32_t first, std::int32_t second); // 玩家/UI/拖动/副本按序换位。
// 原函数：0x0051B5F0。
std::int32_t CountInventoryItemQuantity_51B5F0(session::ClientSession& session, std::int32_t itemId); // 仅背包8..27槽，完整W16占用/匹配S32编号后累加数量。
// 原51B630：玩家/宠物容器间整条换位，不改装备标志或同步UI。
void SwapPlayerOrPetItemSlots_51B630(
    session::ClientSession& session,
    std::int32_t firstGroup, std::int32_t firstIndex,
    std::int32_t secondGroup, std::int32_t secondIndex);

// 原F7C950的已消费AC字节前缀；510490生产，4C75A0等UI读取，非完整业务布局。
struct ReplyRecordF7C950 {
    std::int32_t used; // 原成员偏移：+0x00。
    std::wstring text04; // 原成员偏移：+0x04。
    std::uint8_t hasText24; // 原成员偏移：+0x24。 第2文本不等于单空格；空串同样为1。
    std::wstring text25; // 原成员偏移：+0x25。
    std::array<std::byte, 3> bytes45; // 原成员偏移：+0x45。
    std::int32_t value48; // 原成员偏移：+0x48。
    std::array<std::int32_t, 24> values4C; // 原成员偏移：+0x4C。
};

extern ReplyRecordF7C950 gReplyRecordF7C950; // 原全局：0x00F7C950。

// 原函数：0x0052E1D0。
std::int32_t HasPetMailTechnique_52E1D0(session::ClientSession& session, std::int32_t index); // 四个邮件技能按原顺序逐次查询，任一命中即1。
// 原函数：0x0052E180。
std::int32_t HasPetTechnique_52E180(
    session::ClientSession& session,
    std::int32_t index, std::int32_t techniqueId); // 原十个真实技能记录，非排序索引表。

// 原510150：宠物物品带槽号回复；level重复解析图号字段，不同步UI。
void HandlePetInventoryUpdatesReply_510150(cg::network::CgConnection& networkConnection, std::int32_t petIndex, const char* payload);

// 原函数：0x0050A6B0。
void HandlePetPrimaryStatsReply_50A6B0(cg::network::CgConnection& networkConnection, std::int32_t index, const char* payload);
// 原函数：0x0050AC20。
void HandlePetSecondaryStatsReply_50AC20(cg::network::CgConnection& networkConnection, std::int32_t index, const char* payload);
// 原函数：0x0050B0D0。
void HandlePetTechniquesReply_50B0D0(cg::network::CgConnection& networkConnection, std::int32_t index, const char* payload); // 技能回复，无CryptInt门禁。
// 原函数：0x005185C0。
void BuildPetTechniqueDisplayEntries_5185C0(
    const detail::PetTechniqueRecord* source, SkillPairRecord* destination,
    std::int32_t count); // 保留全部used和sequence，按源序写低W16索引，不排序。
// 原函数：0x0050CF70。
std::int32_t HandlePetAttributePointsReply_50CF70(cg::network::CgConnection& networkConnection, std::int32_t index, std::int32_t value); // 仅写宠物+6C并返回index。
// 原函数：0x0043C4B0。
std::int32_t FindBattlePetIndex_43C4B0(session::ClientSession& session); // 首个used严格1且flags6AA严格2的宠物槽，未命中-1。

// 原 FF0508/DE03F8 两张称号表共用记录；地址不存入数值字段。
struct TitleRecord {
    std::int32_t id; // 原 +0；负值表示无称号。
    std::int32_t value; // 原 +4；50A300第三字段。
    std::wstring text; // 原 +8；内部称号文本，协议入口保留原限制。
};

extern std::array<TitleRecord, 64> gSortedTitlesDE03F8; // 原全局：0x00DE03F8。

// 原函数：0x0050A300。
void HandlePlayerTitlesReply_50A300(cg::network::CgConnection& networkConnection, const char* payload); // 原称号回复；socket未用。
// 原函数：0x00519840。
std::int32_t FindTitleIndexById(std::int32_t id); // 原表首匹配索引，未找到返回-1。
extern std::int32_t gInitialPlayerDuelPoints_FDA6F8; // 原全局：0x00FDA6F8。 4631D0确认已写、5199A0同源W32读取；不是联系人索引的一过尾元素。
// 原函数：0x0050BFA0。
std::uint32_t HandlePlayerOptionsReply_50BFA0(cg::network::CgConnection& networkConnection, std::int32_t value); // 玩家选项回复，清bit2后写U16。
// 原函数：0x0050BFC0。
std::uint16_t HandlePlayerBattleRowReply_50BFC0(cg::network::CgConnection& networkConnection, std::int32_t value); // 玩家+1A0低W16写入；原返回仅定义AX。
// 原函数：0x00472F40。
network::SendResult ToggleDuelPermission_472F40(session::ClientSession& session); // 同源玩家选项切换04后发送。
std::uint8_t ReadPlayerOptions194LowByte(session::ClientSession& session) noexcept; // 原FDA894低U8，状态栏提示与图号用同一快照。
// 原函数：0x00472F70。
network::SendResult TogglePartyChat_472F70(session::ClientSession& session); // 切换08后发送。
// 原函数：0x00472FA0。
network::SendResult TogglePartyJoinPermission_472FA0(session::ClientSession& session); // 切换01后发送。
// 原函数：0x00472FD0。
network::SendResult ToggleContactCardExchangePermission_472FD0(session::ClientSession& session); // 切换10后发送。
// 原函数：0x00473000。
network::SendResult ToggleTradePermission_473000(session::ClientSession& session); // 切换20后发送。
// 原函数：0x00473030。
network::SendResult ToggleGuildInvitePermission_473030(session::ClientSession& session); // 切换40后发送。
// YobiRuntime借用同一SceneState状态，不公开玩家首块或五槽记录布局。
std::uint32_t ReadPlayerFlags18A(session::ClientSession& session) noexcept; // 原FDA88A跨W32读取。
std::uint8_t ReadPlayerActionFlagsByte(session::ClientSession& session) noexcept; // 原FDA88E单次W8；读取flags18C第2字节，保留既有W16接口。
std::uint16_t ReadPlayerActionFlags(session::ClientSession& session) noexcept; // 原FDA88E，与flags18C高W16同源。
void ClearPlayerActionFlags(session::ClientSession& session, std::uint16_t mask) noexcept; // 只清FDA88E，不改FDA88C低W16。
void SetPlayerActionFlags(session::ClientSession& session, std::uint16_t mask) noexcept; // 同源FDA88E置位，保留其他字节。
void CopyPlayerBarValues(
    session::ClientSession& session,
    platform::LegacyCryptInt* hp, platform::LegacyCryptInt* maxHp,
    platform::LegacyCryptInt* fp, platform::LegacyCryptInt* maxFp);
const wchar_t* GetTitleText(std::int32_t index) noexcept; // 合法索引读取称号表；未装备／无效索引返回空文本。
const wchar_t* GetPlayerDisplayText(session::ClientSession& session, std::int16_t mode) noexcept; // 模式1..6宽文本，只读借用。
// 原函数：0x00504B10。
std::int32_t FindAttributeAllocationHintTarget_504B10(session::ClientSession& session); // SHP01：玩家正加点返5；五S16配对首个宠物非零加点位置，无则6。
// 原函数：0x00504B60。
std::int32_t FindFirstInjuredPet_504B60(session::ClientSession& session); // G04：五S16配对中首个used且伤势>0的宠物原编号，无则-1。
std::int16_t ReadPlayerTribe(session::ClientSession& session) noexcept; // 原FDA700，窗口7按S16读取。
cg::yobi::YobiAction* ReadPetMapAction(std::int16_t index) noexcept;
std::uint16_t ReadPetMapFlags(std::int16_t index) noexcept; // 原FF02E0五槽W16占用，名单按完整非零判断。
std::uint32_t ReadPetMapObjectIndex(std::int16_t index) noexcept;
// 原函数：0x00519EC0。
std::uint32_t SetPlayerMapCoordinates_519EC0(std::int32_t x, std::int32_t y); // 透传468330。
// 原函数：0x0051BEE0。
void SuspendPlayerForObservation_51BEE0(session::ClientSession& session); // 观察处理置FDA88E的0x80，复用flags18C高W16。
// 原函数：0x0051C0D0。
std::int32_t ResolveRidingCharacterImage_51C0D0(std::int32_t imageNo); // 原骑乘图号映射，未命中原值返回。
// 原函数：0x0051BEC0。
std::int32_t GetPlayerStatusIcon_51BEC0(session::ClientSession& session); // 原 51BEC0：按动作 bit1 返回同一动作值或0。
// 原函数：0x0051BE50。
std::int8_t RequestPlayerStatusIcon_51BE50(session::ClientSession& session, std::int32_t requested); // 原 51BE50：去重、klF 发送及低 S8 返回。

namespace detail {
// 原函数：0x00504F10。
std::int32_t HasSkillRecipeForItem_504F10(std::int32_t value, std::int32_t mode); // 原序查询recipe.value10，含索引表末次别名读取。
// 原函数：0x00519950。
void ResetSkillRecipeSlots_519950(std::int32_t index); // 仅重置技能尾槽及56行配方编号。
// 原函数：0x00518600。
void BuildSortedSkillDisplayEntries_518600(
    const SkillRuntimeRecord* source, SkillPairRecord* destination, std::int32_t count);
// 原函数：0x00518660。
std::int32_t CompareSkillDisplayEntries_518660(const SkillPairRecord* left, const SkillPairRecord* right);
// 原函数：0x0051B530。
void SwapPlayerInventorySlots_51B530(session::ClientSession& session, std::int32_t first, std::int32_t second); // 整条C5C交换，装备位标志及UI迁移。
// 原函数：0x005196B0。
void SetPlayerItemDescription_5196B0(session::ClientSession& session, std::int32_t index, const char* text); // 32行描述与最多4页的原回扫。
// 原519770：同源宠物物品32行描述与前行回扫，保持独立页数写序。
void SetPetItemDescription_519770(session::ClientSession& session, std::int32_t petIndex, std::int32_t itemIndex, const char* text);
// 原函数：0x00519920。
std::int32_t RefreshPetCount_519920(session::ClientSession& session); // 先清计数，再扫描占用并发布。
// 原函数：0x00518580。
void BuildPetDisplayEntries_518580(
    const PetRuntimeRecord* source, PetPairRecord* destination, std::int32_t count);
// 原函数：0x00492250。
void MarkPetDetailDraftChanged_492250(session::ClientSession& session); // 原[492250,49225B)，置刷新标志。
extern std::array<TitleRecord, 64> gTitlesFF0508; // 原全局：0x00FF0508。
// 原函数：0x005198D0。
void SortTitleRecords_5198D0(TitleRecord* records, std::uint32_t count); // 原CRT次序的称号排序。

// 原 50A300 唯一调用的 28 字节记录前向复制叶。
// 原函数：0x005198A0。
void CopyTitleRecords_5198A0(
    TitleRecord* destination, const TitleRecord* source,
    std::int32_t count); // 原 [0x5198A0,0x5198CB)，内部对象按值复制。

// MC01-C8：5198D0的qsort回调，比较首S32；非负在前，同键/负id组返回0。
// 原函数：0x005198F0。
std::int32_t CompareTitleRecordsById_5198F0(const void* left, const void* right);
// 原函数：0x005199A0。
void InitializePlayerRuntimeState_5199A0(network::CgConnection& connection); // 六块同源重置及设置/称号/配方恢复；唯一caller不消费原残留EAX。
// 原函数：0x0051A4B0。
void CopyPlayerIdentityToMapAction_51A4B0(session::ClientSession& session); // 玩家初始姓名/称号/等级/HP同步。
// 原函数：0x0051A5E0。
void MarkPlayerAsPartyMember_51A5E0(session::ClientSession& session); // 玩家flags18A置200。
// 原函数：0x0051A5F0。
void ClearPlayerPartyMembership_51A5F0(session::ClientSession& session); // 仅清玩家flags18A的200。
// 原函数：0x0051B080。
cg::yobi::YobiAction* ClearPartyMembershipState_51B080(session::ClientSession& session); // 解散地图五槽；返回51A5B0的非拥有T*。
// 原函数：0x0051B0F0。
void DetachPartyMapActions_51B0F0(); // 仅清地图五槽的非拥有action，不释放或清占用。
// 原函数：0x00519A80。
void CreatePlayerMapPresence_519A80(session::ClientSession& session); // 原普通场景玩家创建/身份发布、HP复制与宠物地图槽匹配。
// 原函数：0x0051A090。
cg::yobi::YobiAction* SetPlayerMapIdentity_51A090(
    session::ClientSession& session,
    std::wstring_view name, const std::array<std::wstring, 4>& titles, std::int32_t level,
    std::int32_t value188, std::int32_t flag40, std::int32_t flag80);
// 协议与场景共用的队伍状态。
extern std::uint16_t gPartyActive_FF0D36; // 原全局：0x00FF0D36。 原跟随gate；协议和场景共享，不默认启用。
// 原函数：0x0051B3E0。
std::uint32_t RequestAdjacentWorldInteraction_51B3E0(session::ClientSession& session); // NFI01：完整邻格fIM父，原EAX位型/两tick保持；现普通空格明文。
std::int16_t ReadPetFollowIndex(std::size_t position) noexcept;
// 519EE0 每个原读取点借用唯一 FF02B4，不暴露可写槽或对象布局。
// 原函数：0x00519CD0。
void CreateOrUpdatePlayerMapActor_519CD0(
    session::ClientSession& session, std::int32_t imageNo, std::int32_t x, std::int32_t y,
    std::int32_t direction); // 原 519CD0：创建并发布玩家，或更新已有玩家。
// 原函数：0x00519E50。
void SetPlayerMapAppearance_519E50(session::ClientSession& session, std::int32_t imageNo, std::int32_t direction);
// 原函数：0x0051A040。
std::int32_t SetPlayerFacingDirection_51A040(session::ClientSession& session, std::int32_t direction); // 同源方向发布，返回完整输入。
// 原函数：0x0051A060。
void SetPlayerAnimationState_51A060(session::ClientSession& session, std::int32_t characterAction); // 动作1不重启，其余动作请求重启。
// 原函数：0x0051A840。
void ApplyPlayerActionCommand_51A840(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::int32_t direction,
    std::int32_t characterAction, std::int32_t option1, std::int32_t option2,
    std::int32_t option3, std::int32_t option4); // 完整玩家动作回复分支。
// 原函数：0x0051A600。
void Sub_51A600(session::ClientSession& session); // flags18A置0x4000。
// 原函数：0x0051A610。
void Sub_51A610(session::ClientSession& session); // flags18A清0x4000。
// 原函数：0x0051A620。
void Sub_51A620(session::ClientSession& session); // flags18A置0x8000。
// 原函数：0x0051A630。
void Sub_51A630(session::ClientSession& session); // flags18A清0x8000。
// 原函数：0x0051BDB0。
void ShowPlayerPersistentStatusIcon_51BDB0(session::ClientSession& session); // FDA88E置1，复用flags18C高W16。
// 原函数：0x0051BDC0。
void HidePlayerPersistentStatusIcon_51BDC0(session::ClientSession& session); // FDA88E清1。
// 原函数：0x0051BE10。
void ShowPlayerTimedStatusIcon_51BE10(session::ClientSession& session); // FDA88E置0x24。
// 原函数：0x0051BE20。
void ShowPlayerPetPersistentStatusIcon_51BE20(session::ClientSession& session); // FDA88E置8。
// 原函数：0x0051BE30。
void HidePlayerPetPersistentStatusIcon_51BE30(session::ClientSession& session); // FDA88E清8。
// 原函数：0x0051BE40。
void ShowPlayerPetTimedStatusIcon_51BE40(session::ClientSession& session); // FDA88E置0x50。
// 原函数：0x00519D50。
std::int8_t DestroyPlayerMapPresence_519D50(session::ClientSession& session);                  // 原 0x519D50：附件清理、重读玩家标死与动作尾链，返回低 S8。
// 519D50 尾链及 51A840 状态生产共用的玩家位槽，不复制 FDA88E 动作标志。
// 原函数：0x0051A5A0。
void MarkPlayerAsPartyLeader_51A5A0(session::ClientSession& session); // 原 51A5A0：先置 FDA88A bit8，再只写 FDA88C 低 W8。
// 原函数：0x0051A5B0。
cg::yobi::YobiAction* ClearPlayerLeaderMarker_51A5B0(session::ClientSession& session); // 原 51A5B0：条件清标志并返回入口借用指针。
// 原函数：0x0051BDD0。
std::int32_t SetPlayerStatusIcon_51BDD0(session::ClientSession& session, std::int32_t value); // 原 51BDD0：发布动作值，透传输入。
// 原函数：0x0051BDF0。
void ClearPlayerStatusIcon_51BDF0(session::ClientSession& session); // 原 51BDF0：清动作 bit1 后清动作值，不发送。

} // namespace detail

} // namespace cg::world
