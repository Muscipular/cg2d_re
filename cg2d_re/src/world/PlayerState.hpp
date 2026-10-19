#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "PetTechniqueRecord.hpp"

// 玩家、技能、宠物与称号的共享声明。

namespace cg::world {

// 角色技能及每项技能的技术栏位容量；协议索引仍从0开始。
inline constexpr std::int32_t kSkillSlotCount = 30;
inline constexpr std::int32_t kTechniqueSlotCount = 30;
static_assert(kSkillSlotCount <= 32); // 现有战斗P封包以32位掩码表示技能可用状态。

// 原[509650,509B96)，玩家属性掩码回复，socket未用。
std::int32_t HandlePlayerPrimaryStatsReply_509650(std::uintptr_t socket, const char* payload); // MPR01：aPc掩码主属性，六CryptInt原序/金额和双尾字段，真实S32分界。
// 原函数：0x00509BA0。
void HandlePlayerSecondaryStatsReply_509BA0(std::uintptr_t unusedSocket, const char* payload); // MPR01：UU掩码附加属性/三金额上限、原步速修正和action发布，void副作用/完整ABI待。
// 原函数：0x00509E30。
void HandlePlayerProfessionReply_509E30(std::uintptr_t unusedSocket, std::int32_t value, char* text); // 先写低U8，再原位反转义职业名。
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

struct SkillPairRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t index; // 原成员偏移：+0x02。 原技能/宠物技术表下标，UI按S16消费，不是地址。
    std::int32_t sequence; // 原成员偏移：+0x04。
};

// IWT01：以上为内部对象，文本宽化后不再断言旧字节布局；SkillPair仍为纯数值记录。
static_assert(offsetof(SkillPairRecord, used) == 0);
static_assert(offsetof(SkillPairRecord, index) == 2);
static_assert(offsetof(SkillPairRecord, sequence) == 4);
static_assert(sizeof(SkillPairRecord) == 8);

extern std::array<SkillRuntimeRecord, kSkillSlotCount>& gSkillsF1BAFC; // 原全局：0x00F1BAFC。 同一第四清零块成员别名。
extern std::array<SkillPairRecord, kSkillSlotCount>& gSkillPairsF63370; // 原全局：0x00F63370。
// 原479FBE按S8索引读取U16；按用户确认，-1现表示未选中，不再读取原末条技能+4C44。
std::uint16_t ReadSkillPairUsed(std::int8_t pairIndex);
extern std::uint16_t gSkillReorderPending_DA3B20; // 原全局：0x00DA3B20。 原技能拖放门禁，509EB0尾部清零。
// 原函数：0x00505040。
std::int32_t IsItemEligibleForDecoration_505040(std::int32_t slot); // 说明/类别/flag800及制造配方条件。
// 原函数：0x005050B0。
std::int32_t IsItemEligibleForPetDecoration_5050B0(std::int32_t slot); // 类别80或flag800与601..605技能配方条件。
// 原函数：0x00505110。
std::int32_t IsItemEligibleForRefining_505110(std::int32_t slot); // 说明、两段物品类别及flag1000条件。
// 原函数：0x00509EB0。
void HandlePlayerSkillsReply_509EB0(std::uintptr_t socket, const char* payload);
// 原函数：0x0050A120。
void HandlePlayerSkillTechniquesReply_50A120(
    std::uintptr_t unusedSocket, std::int32_t skillIndex, const char* payload); // 清技术区后按六字段生产。
// 原函数：0x0050C580。
void HandleSkillRecipesReply_50C580(
    std::uintptr_t unusedSocket, std::int32_t index, const char* payload); // 固定56行、每行五材料。

// 原FDA8A4起的28条玩家物品；NIO01在网络字段解析后持有UTF-16，注释偏移仅作原端参考。
struct PlayerItemRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::wstring name; // 原成员偏移：+0x02；输入GBK上限28B。
    std::wstring memo; // 原成员偏移：+0x1F；输入GBK上限16B。
    std::array<std::wstring, 32> description; // 原成员偏移：+0x30；32行按原GBK列规则解析。
    std::int32_t pages; // 原成员偏移：+0xC30。
    std::int32_t color; // 原成员偏移：+0xC34。
    std::int32_t imageNo; // 原成员偏移：+0xC38。
    std::int32_t level; // 原成员偏移：+0xC3C。
    std::int32_t itemId; // 原成员偏移：+0xC40。
    std::int32_t count; // 原成员偏移：+0xC44。
    std::int32_t itemType; // 原成员偏移：+0xC48。
    std::uint16_t canUseInField; // 原成员偏移：+0xC4C。
    std::uint16_t battleUseFlag; // 原成员偏移：+0xC4E。
    std::uint16_t target; // 原成员偏移：+0xC50。
    std::uint16_t flags; // 原成员偏移：+0xC52。
    std::uint16_t hasDescription; // 原成员偏移：+0xC54。
    std::array<std::byte, 6> bytesC56; // 原成员偏移：+0xC56。
};

// 原首块 [FDA700,FF02E0) 已证玩家槽；不将此原生结构当作原连续内存。
struct PlayerRuntimeSlots {
    // 原0x005184A0：成员正序构造及失败前缀清理由C++承接。
    PlayerRuntimeSlots() = default;
    // 原0x00518410：成员自动逆序析构。
    ~PlayerRuntimeSlots() = default;
    void ClearRaw() noexcept; // 原主数据整块清零，不消费六次构造RNG。
    std::uint16_t value00; // 原成员偏移：+0x00。 原FDA700；509BA0仅写低W16。
    std::int32_t level; // 原 FDA704。
    std::int32_t value08; // 原成员偏移：+0x08。 原FDA708。
    std::int32_t timerRemaining0C; // 原成员偏移：+0x0C。 原FDA70C；458F40按帧扣减的S32。
    std::int32_t timerMode10; // 原成员偏移：+0x10。 原FDA710；保留完整模式，非零才扣减。
    std::uint64_t timerTick14; // 原成员偏移：+0x14。 原FDA714；50C7A0与458F40同源。
    platform::LegacyCryptInt hp; // 原 FDA718；与 action +170 的载荷同步。
    platform::LegacyCryptInt maxHp; // 原 FDA728。
    platform::LegacyCryptInt fp; // 原 FDA738；与宠物数据独立。
    platform::LegacyCryptInt maxFp; // 原 FDA748。
    platform::LegacyCryptInt value58; // 原成员偏移：+0x58。 原 FDA758；5184A0第五构造对象。
    platform::LegacyCryptInt value68; // 原成员偏移：+0x68。 原 FDA768；5184A0第六构造对象。
    std::array<std::int32_t, 14> values78; // 原成员偏移：+0x78。 原FDA778..FDA7AC，509650两段数值。
    std::array<std::int32_t, 2> valuesB0; // 原成员偏移：+0xB0。 原FDA7B0/FDA7B4，509BA0尾部两值。
    std::int32_t valueB8; // 原成员偏移：+0xB8。 原FDA7B8。
    std::array<std::int32_t, 6> valuesBC; // 原成员偏移：+0xBC。 原FDA7BC..FDA7D0。
    std::array<std::int32_t, 4> valuesD4; // 原成员偏移：+0xD4。 原FDA7D4..FDA7E0。
    std::array<std::int32_t, 4> valuesE4; // 原成员偏移：+0xE4。 原FDA7E4..FDA7F0。
    std::int32_t valueF4; // 原成员偏移：+0xF4。 原FDA7F4。
    std::int32_t valueF8; // 原成员偏移：+0xF8。 原FDA7F8。
    std::int32_t valueFC; // 原成员偏移：+0xFC。 原FDA7FC；与末尾15BCC共用回复门禁。
    std::int32_t titleIndex100; // 原成员偏移：+0x100。 原FDA800，直接索引FF0508原表。
    std::int32_t value104; // 原成员偏移：+0x104。 原FDA804。
    std::array<char, 17> name; // 原 FDA808；旧编码字节。
    std::array<char, 17> ownTitle; // 原 FDA819。
    std::array<char, 33> text12A; // 原成员偏移：+0x12A。 原 FDA82A。
    std::array<char, 33> text14B; // 原成员偏移：+0x14B。 原 FDA84B。
    std::int32_t value16C; // 原成员偏移：+0x16C。 原FDA86C。
    std::int32_t walkSpeed; // 原 FDA870；独立于通用 action 的初始步速。
    std::int32_t value174; // 原成员偏移：+0x174。 原 FDA874；46A700仅在到达时按S32递减。
    std::int32_t imageNo; // 原 FDA878，不是 FDA87C。
    std::int32_t value17C; // 原成员偏移：+0x17C。 原FDA87C，不与imageNo合并。
    std::uint32_t objectIndex; // 原0x00FDA880；玩家对象索引。
    std::int32_t direction; // 原 FDA884；其他消费者可读取低 S8。
    std::uint16_t value188; // 原成员偏移：+0x188。 原 FDA888；519A80 符号扩展读取。
    std::uint16_t flags18A; // 原成员偏移：+0x18A。 原FDA88A W16；与flags18C低W16组成跨字段W32观察。
    std::uint32_t flags18C; // 原成员偏移：+0x18C。 承载原 [FDA88C,FDA890)；18C 低 W8、18E 高 W16 共享字节。
    std::int32_t value190; // 原成员偏移：+0x190。 原 FDA890；51BDD0/51BE50 写入的完整动作值。
    std::uint16_t options194; // 原成员偏移：+0x194。 原FDA894低W16；选项切换/回复与聊天门禁共用。
    std::uint16_t value1A0; // 原成员偏移：+0x1A0。 原FDA8A0；50BFC0写入低W16。
    std::array<PlayerItemRecord, 28> items; // 原FDA8A4..FF02B4，唯一物品owner。
    cg::yobi::YobiAction* action;
    std::array<char, 17> text15BB8; // 原成员偏移：+0x15BB8。 原 FF02B8，不紧接新端的指针字节布局。
    std::int32_t value15BCC; // 原成员偏移：+0x15BCC。 原FF02CC；509650尾字段。
    std::array<std::int32_t, 3> values15BD0; // 原成员偏移：+0x15BD0。 原FF02D0/FF02D4/FF02D8。
};

static_assert(offsetof(PlayerRuntimeSlots, flags18C)
    == offsetof(PlayerRuntimeSlots, flags18A) + sizeof(std::uint16_t));

extern PlayerRuntimeSlots gPlayerRuntimeSlots; // 原全局：0x00FDA700；中央有序构造。

// 原函数：0x0050BFD0。
void HandleFullPlayerInventoryReply_50BFD0(std::uintptr_t unusedSocket, const char* payload); // 固定28槽全量回复。
// 原函数：0x0050C260。
void HandlePlayerInventoryUpdatesReply_50C260(std::uintptr_t unusedSocket, const char* payload); // 带槽号的14字段增量回复。
// 原函数：0x0050C900。
void HandleInventorySlotSwapReply_50C900(
    std::uintptr_t unusedSocket, std::int32_t first, std::int32_t second); // 玩家/UI/拖动/副本按序换位。
// 原函数：0x0051B5F0。
std::int32_t CountInventoryItemQuantity_51B5F0(std::int32_t itemId); // 仅背包8..27槽，完整W16占用/匹配S32编号后累加数量。
// 原51B630：玩家/宠物容器间整条换位，不改装备标志或同步UI。
void SwapPlayerOrPetItemSlots_51B630(
    std::int32_t firstGroup, std::int32_t firstIndex,
    std::int32_t secondGroup, std::int32_t secondIndex);

// 原F633F8五条宠物数据，不是FF02E0地图槽；未消费间隔不计最终类型冻结。
struct PetRuntimeRecord {
    // 原0x004CD360：成员正序构造及失败前缀清理由C++承接。
    PetRuntimeRecord() = default;
    // 原0x004CD2D0：成员自动逆序析构。
    ~PetRuntimeRecord() = default;
    PetRuntimeRecord(const PetRuntimeRecord&) = default;
    PetRuntimeRecord& operator=(const PetRuntimeRecord&) = default;
    void ClearRaw() noexcept; // 原零字节回复／场景清理，不构造整数或消费RNG。
    std::uint16_t used; // 原+0占用。
    std::uint16_t value02; // 原成员偏移：+0x02。 主回复掩码200001。
    std::uint16_t value04; // 原成员偏移：+0x04。 主回复掩码5。
    std::array<std::byte, 2> bytes06; // 原成员偏移：+0x06。
    std::int32_t value08; // 原成员偏移：+0x08。 主回复掩码201。
    std::array<platform::LegacyCryptInt, 6> values0C; // HP/最大HP/FP/最大FP/+4C/+5C。
    std::int32_t lvFlag6C; // 原成员偏移：+0x6C。 原F63464；50CF70写入完整W32。
    std::array<std::int32_t, 26> values70; // 原+70..+D4，两回复共享。
    std::array<detail::PetTechniqueRecord, kPetTechniqueSlotCount> techniquesD8; // 原成员偏移：+0xD8。
    std::array<SkillPairRecord, kPetTechniqueSlotCount> techniquePairs650; // 原成员偏移：+0x650。 5185C0只生成索引，不排序。
    std::int32_t imageNo6A0; // 原成员偏移：+0x6A0。
    std::int32_t value6A4; // 原成员偏移：+0x6A4。
    std::array<std::byte, 2> bytes6A8; // 原成员偏移：+0x6A8。
    std::uint16_t flags6AA; // 原成员偏移：+0x6AA。 不含跟随bit0x10的状态。
    std::array<char, 17> name6AC; // 原成员偏移：+0x6AC。
    std::array<char, 17> name6BD; // 原成员偏移：+0x6BD。
    std::array<std::byte, 2> bytes6CE; // 原成员偏移：+0x6CE。
    std::int32_t value6D0; // 原成员偏移：+0x6D0。 518580复制到索引对。
    std::int32_t follow6D4; // 原成员偏移：+0x6D4。
    std::array<std::int32_t, 3> values6D8; // 原成员偏移：+0x6D8。
    std::array<PlayerItemRecord, 6> items6E4; // 原成员偏移：+0x6E4。 原51B630复用的六条C5C物品。
    std::uint8_t updated510C; // 原成员偏移：+0x510C。 仅50A6B0尾部置1。
    std::array<std::byte, 3> bytes510D; // 原成员偏移：+0x510D。
};

static_assert(!std::is_trivially_destructible_v<PetRuntimeRecord>);
static_assert(offsetof(PetRuntimeRecord, used) == 0);
static_assert(offsetof(PetRuntimeRecord, value02) == 2);
static_assert(offsetof(PetRuntimeRecord, value04) == 4);
static_assert(offsetof(PetRuntimeRecord, value08) == 8);
static_assert(offsetof(PetRuntimeRecord, values0C) == 0x0C);
static_assert(offsetof(PetRuntimeRecord, lvFlag6C) == 0x6C);
static_assert(offsetof(PetRuntimeRecord, values70) == 0x70);
static_assert(offsetof(PetRuntimeRecord, techniquesD8) == 0xD8);
// 原0x650／0x6A0之后的成员随两张技能表扩容，旧偏移保留在成员注释中。
static_assert(offsetof(PetRuntimeRecord, techniquePairs650) == 0xD8 + sizeof(detail::PetTechniqueRecord) * kPetTechniqueSlotCount);
static_assert(offsetof(PetRuntimeRecord, imageNo6A0) == offsetof(PetRuntimeRecord, techniquePairs650) + sizeof(SkillPairRecord) * kPetTechniqueSlotCount);
static_assert(offsetof(PetRuntimeRecord, value6A4) == offsetof(PetRuntimeRecord, imageNo6A0) + 4);
static_assert(offsetof(PetRuntimeRecord, flags6AA) == offsetof(PetRuntimeRecord, imageNo6A0) + 0xA);
static_assert(offsetof(PetRuntimeRecord, name6AC) == offsetof(PetRuntimeRecord, imageNo6A0) + 0xC);
static_assert(offsetof(PetRuntimeRecord, name6BD) == offsetof(PetRuntimeRecord, imageNo6A0) + 0x1D);
static_assert(offsetof(PetRuntimeRecord, value6D0) == offsetof(PetRuntimeRecord, imageNo6A0) + 0x30);
static_assert(offsetof(PetRuntimeRecord, follow6D4) == offsetof(PetRuntimeRecord, imageNo6A0) + 0x34);
static_assert(offsetof(PetRuntimeRecord, values6D8) == offsetof(PetRuntimeRecord, imageNo6A0) + 0x38);

// 原F7C950的已消费AC字节前缀；510490生产，4C75A0等UI读取，非完整业务布局。
struct ReplyRecordF7C950 {
    std::int32_t used; // 原成员偏移：+0x00。
    std::array<char, 32> text04; // 原成员偏移：+0x04。
    std::uint8_t hasText24; // 原成员偏移：+0x24。 第2文本不等于单空格；空串同样为1。
    std::array<char, 32> text25; // 原成员偏移：+0x25。
    std::array<std::byte, 3> bytes45; // 原成员偏移：+0x45。
    std::int32_t value48; // 原成员偏移：+0x48。
    std::array<std::int32_t, 24> values4C; // 原成员偏移：+0x4C。
};

static_assert(std::is_standard_layout_v<ReplyRecordF7C950>);
static_assert(std::is_trivially_copyable_v<ReplyRecordF7C950>);
static_assert(offsetof(ReplyRecordF7C950, used) == 0);
static_assert(offsetof(ReplyRecordF7C950, text04) == 4);
static_assert(offsetof(ReplyRecordF7C950, hasText24) == 0x24);
static_assert(offsetof(ReplyRecordF7C950, text25) == 0x25);
static_assert(offsetof(ReplyRecordF7C950, bytes45) == 0x45);
static_assert(offsetof(ReplyRecordF7C950, value48) == 0x48);
static_assert(offsetof(ReplyRecordF7C950, values4C) == 0x4C);
static_assert(sizeof(ReplyRecordF7C950) == 0xAC);
extern ReplyRecordF7C950 gReplyRecordF7C950; // 原全局：0x00F7C950。

// 原DE0B08，每项8B；索引是数值，不是地址。
struct PetPairRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t index; // 原成员偏移：+0x02。
    std::int32_t value; // 原成员偏移：+0x04。
};

static_assert(offsetof(PetPairRecord, used) == 0);
static_assert(offsetof(PetPairRecord, index) == 2);
static_assert(offsetof(PetPairRecord, value) == 4);
static_assert(sizeof(PetPairRecord) == 8);
extern std::array<PetRuntimeRecord, 5> gPetRuntimeRecords; // 原全局：0x00F633F8。
extern std::array<PetPairRecord, 5> gPetPairsDE0B08; // 原全局：0x00DE0B08。
// 原函数：0x0052E1D0。
std::int32_t HasPetMailTechnique_52E1D0(std::int32_t index); // 四个邮件技能按原顺序逐次查询，任一命中即1。
// 原函数：0x0052E180。
std::int32_t HasPetTechnique_52E180(
    std::int32_t index, std::int32_t techniqueId); // 原十个真实技能记录，非排序索引表。
extern std::uint32_t gPetDetailDraftChanged_D67B28; // 原全局：0x00D67B28。 492250置1，UI后续复用。

// 原510150：宠物物品带槽号回复；level重复解析图号字段，不同步UI。
void HandlePetInventoryUpdatesReply_510150(
    std::uintptr_t unusedSocket, std::int32_t petIndex, const char* payload);

// 原函数：0x0050A6B0。
void HandlePetPrimaryStatsReply_50A6B0(std::uintptr_t socket, std::int32_t index, const char* payload);
// 原函数：0x0050AC20。
void HandlePetSecondaryStatsReply_50AC20(std::uintptr_t socket, std::int32_t index, const char* payload);
// 原函数：0x0050B0D0。
void HandlePetTechniquesReply_50B0D0(
    std::uintptr_t unusedSocket, std::int32_t index, const char* payload); // 技能回复，无CryptInt门禁。
// 原函数：0x005185C0。
void BuildPetTechniqueDisplayEntries_5185C0(
    const detail::PetTechniqueRecord* source, SkillPairRecord* destination,
    std::int32_t count); // 保留全部used和sequence，按源序写低W16索引，不排序。
// 原函数：0x0050CF70。
std::int32_t HandlePetAttributePointsReply_50CF70(
    std::uintptr_t socket, std::int32_t index, std::int32_t value); // 仅写宠物+6C并返回index。
// 原函数：0x0043C4B0。
std::int32_t FindBattlePetIndex_43C4B0(); // 首个used严格1且flags6AA严格2的宠物槽，未命中-1。

// 原 FF0508/DE03F8 两张称号表共用记录；地址不存入数值字段。
struct TitleRecord {
    std::int32_t id; // 原 +0；负值表示无称号。
    std::int32_t value; // 原 +4；50A300第三字段。
    std::array<char, 20> text; // 原 +8；原编码字节，不增加复制限长。
};

static_assert(std::is_standard_layout_v<TitleRecord>);
static_assert(std::is_trivially_copyable_v<TitleRecord>);
static_assert(offsetof(TitleRecord, id) == 0);
static_assert(offsetof(TitleRecord, value) == 4);
static_assert(offsetof(TitleRecord, text) == 8);
static_assert(sizeof(TitleRecord) == 0x1C);
extern std::array<TitleRecord, 64> gSortedTitlesDE03F8; // 原全局：0x00DE03F8。

// 原函数：0x0050A300。
void HandlePlayerTitlesReply_50A300(std::uintptr_t socket, const char* payload); // 原称号回复；socket未用。
// 原函数：0x00519840。
std::int32_t FindTitleIndexById(std::int32_t id); // 原表首匹配索引，未找到返回-1。
extern std::int32_t gInitialPlayerDuelPoints_FDA6F8; // 原全局：0x00FDA6F8。 4631D0确认已写、5199A0同源W32读取；不是联系人索引的一过尾元素。
// 原函数：0x0050BFA0。
std::uint32_t HandlePlayerOptionsReply_50BFA0(std::uintptr_t socket, std::int32_t value); // 玩家选项回复，清bit2后写U16。
// 原函数：0x0050BFC0。
std::uint16_t HandlePlayerBattleRowReply_50BFC0(
    std::uintptr_t socket, std::int32_t value); // 玩家+1A0低W16写入；原返回仅定义AX。
// 原函数：0x00472F40。
std::int32_t ToggleDuelPermission_472F40(); // 同源玩家选项切换04后发送。
std::uint8_t ReadPlayerOptions194LowByte() noexcept; // 原FDA894低U8，状态栏提示与图号用同一快照。
// 原函数：0x00472F70。
std::int32_t TogglePartyChat_472F70(); // 切换08后发送。
// 原函数：0x00472FA0。
std::int32_t TogglePartyJoinPermission_472FA0(); // 切换01后发送。
// 原函数：0x00472FD0。
std::int32_t ToggleContactCardExchangePermission_472FD0(); // 切换10后发送。
// 原函数：0x00473000。
std::int32_t ToggleTradePermission_473000(); // 切换20后发送。
// 原函数：0x00473030。
std::int32_t ToggleGuildInvitePermission_473030(); // 切换40后发送。
// YobiRuntime借用同一SceneState状态，不公开玩家首块或五槽记录布局。
std::uint32_t ReadPlayerFlags18A() noexcept; // 原FDA88A跨W32读取。
std::uint8_t ReadPlayerActionFlagsByte() noexcept; // 原FDA88E单次W8；读取flags18C第2字节，保留既有W16接口。
std::uint16_t ReadPlayerActionFlags() noexcept; // 原FDA88E，与flags18C高W16同源。
void ClearPlayerActionFlags(std::uint16_t mask) noexcept; // 只清FDA88E，不改FDA88C低W16。
void SetPlayerActionFlags(std::uint16_t mask) noexcept; // 同源FDA88E置位，保留其他字节。
void CopyPlayerBarValues(
    platform::LegacyCryptInt* hp, platform::LegacyCryptInt* maxHp,
    platform::LegacyCryptInt* fp, platform::LegacyCryptInt* maxFp);
const char* GetTitleText(std::int32_t index) noexcept; // 合法索引读取称号表；未装备／无效索引返回空文本。
const char* GetPlayerDisplayText(std::int16_t mode) noexcept; // 模式1..6原文本，不复制。
extern std::uint32_t gActorBarsEnabled5AC490; // 原全局：0x005AC490。 原虚拟零区，442F70与后续设置UI共享。
// 原函数：0x00504B10。
std::int32_t FindAttributeAllocationHintTarget_504B10(); // SHP01：玩家正加点返5；五S16配对首个宠物非零加点位置，无则6。
// 原函数：0x00504B60。
std::int32_t FindFirstInjuredPet_504B60(); // G04：五S16配对中首个used且伤势>0的宠物原编号，无则-1。
std::int16_t ReadPlayerTribe() noexcept; // 原FDA700，窗口7按S16读取。
cg::yobi::YobiAction* ReadPetMapAction(std::int16_t index) noexcept;
std::uint16_t ReadPetMapFlags(std::int16_t index) noexcept; // 原FF02E0五槽W16占用，名单按完整非零判断。
std::uint32_t ReadPetMapObjectIndex(std::int16_t index) noexcept;
// 原函数：0x00519EC0。
std::uint32_t SetPlayerMapCoordinates_519EC0(std::int32_t x, std::int32_t y); // 透传468330。
// 原函数：0x0051BEE0。
void SuspendPlayerForObservation_51BEE0(); // 观察处理置FDA88E的0x80，复用flags18C高W16。
// 原函数：0x0051C0D0。
std::int32_t ResolveRidingCharacterImage_51C0D0(std::int32_t imageNo); // 原骑乘图号映射，未命中原值返回。
// 原函数：0x0051BEC0。
std::int32_t GetPlayerStatusIcon_51BEC0(); // 原 51BEC0：按动作 bit1 返回同一动作值或0。
// 原函数：0x0051BE50。
std::int8_t RequestPlayerStatusIcon_51BE50(std::int32_t requested); // 原 51BE50：去重、klF 发送及低 S8 返回。

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
void SwapPlayerInventorySlots_51B530(std::int32_t first, std::int32_t second); // 整条C5C交换，装备位标志及UI迁移。
// 原函数：0x005196B0。
void SetPlayerItemDescription_5196B0(std::int32_t index, const char* text); // 32行描述与最多4页的原回扫。
extern std::int32_t gPetCountDE0B00; // 原全局：0x00DE0B00。
// 原519770：同源宠物物品32行描述与前行回扫，保持独立页数写序。
void SetPetItemDescription_519770(std::int32_t petIndex, std::int32_t itemIndex, const char* text);
// 原函数：0x00519920。
std::int32_t RefreshPetCount_519920(); // 先清计数，再扫描占用并发布。
// 原函数：0x00518580。
void BuildPetDisplayEntries_518580(
    const PetRuntimeRecord* source, PetPairRecord* destination, std::int32_t count);
// 原函数：0x00492250。
void MarkPetDetailDraftChanged_492250(); // 原[492250,49225B)，置刷新标志。
extern std::array<TitleRecord, 64> gTitlesFF0508; // 原全局：0x00FF0508。
// 原函数：0x005198D0。
void SortTitleRecords_5198D0(TitleRecord* records, std::uint32_t count); // 原CRT次序的称号排序。

// 原 50A300 唯一调用的 28 字节记录前向复制叶。
// 原函数：0x005198A0。
void CopyTitleRecords_5198A0(
    std::uint8_t* destination, const std::uint8_t* source,
    std::int32_t count);                    // 原 [0x5198A0,0x5198CB)，逐记录复制 7 个 W32。

// MC01-C8：5198D0的qsort回调，比较首S32；非负在前，同键/负id组返回0。
// 原函数：0x005198F0。
std::int32_t CompareTitleRecordsById_5198F0(const void* left, const void* right);
// 原函数：0x005199A0。
void InitializePlayerRuntimeState_5199A0(); // 六块同源重置及设置/称号/配方恢复；唯一caller不消费原残留EAX。
// 原函数：0x0051A4B0。
void CopyPlayerIdentityToMapAction_51A4B0(); // 玩家初始姓名/称号/等级/HP同步。
// 原函数：0x0051A5E0。
void MarkPlayerAsPartyMember_51A5E0(); // 玩家flags18A置200。
// 原函数：0x0051A5F0。
void ClearPlayerPartyMembership_51A5F0(); // 仅清玩家flags18A的200。
// 原函数：0x0051B080。
cg::yobi::YobiAction* ClearPartyMembershipState_51B080(); // 解散地图五槽；返回51A5B0的非拥有T*。
// 原函数：0x0051B0F0。
void DetachPartyMapActions_51B0F0(); // 仅清地图五槽的非拥有action，不释放或清占用。
// 原函数：0x00519A80。
void CreatePlayerMapPresence_519A80(); // 原普通场景玩家创建/身份发布、HP复制与宠物地图槽匹配。
// 原函数：0x0051A090。
cg::yobi::YobiAction* SetPlayerMapIdentity_51A090(
    const char* name, const char* titles, std::int32_t level,
    std::int32_t value188, std::int32_t flag40, std::int32_t flag80);
// 协议与场景共用的队伍状态、邻格交互节流时间。
extern std::uint16_t gPartyActive_FF0D36; // 原全局：0x00FF0D36。 原跟随gate；协议和场景共享，不默认启用。
extern std::uint64_t gLastAdjacentInteractionTick_FF0D38; // 原全局：0x00FF0D38。 原51B3E0邻格请求的独立时间槽。
// 原函数：0x0051B3E0。
std::uint32_t RequestAdjacentWorldInteraction_51B3E0(); // NFI01：完整邻格fIM父，原EAX位型/两tick保持；现普通空格明文。
std::int16_t ReadPetFollowIndex(std::size_t position) noexcept;
// 519EE0 每个原读取点借用唯一 FF02B4，不暴露可写槽或对象布局。
// 原函数：0x00519CD0。
void CreateOrUpdatePlayerMapActor_519CD0(
    std::int32_t imageNo, std::int32_t x, std::int32_t y,
    std::int32_t direction); // 原 519CD0：创建并发布玩家，或更新已有玩家。
// 原函数：0x00519E50。
void SetPlayerMapAppearance_519E50(std::int32_t imageNo, std::int32_t direction);
// 原函数：0x0051A040。
std::int32_t SetPlayerFacingDirection_51A040(std::int32_t direction); // 同源方向发布，返回完整输入。
// 原函数：0x0051A060。
void SetPlayerAnimationState_51A060(std::int32_t characterAction); // 动作1不重启，其余动作请求重启。
// 原函数：0x0051A840。
void ApplyPlayerActionCommand_51A840(
    std::int32_t x, std::int32_t y, std::int32_t direction,
    std::int32_t characterAction, std::int32_t option1, std::int32_t option2,
    std::int32_t option3, std::int32_t option4); // 完整玩家动作回复分支。
// 原函数：0x0051A600。
void Sub_51A600(); // flags18A置0x4000。
// 原函数：0x0051A610。
void Sub_51A610(); // flags18A清0x4000。
// 原函数：0x0051A620。
void Sub_51A620(); // flags18A置0x8000。
// 原函数：0x0051A630。
void Sub_51A630(); // flags18A清0x8000。
// 原函数：0x0051BDB0。
void ShowPlayerPersistentStatusIcon_51BDB0(); // FDA88E置1，复用flags18C高W16。
// 原函数：0x0051BDC0。
void HidePlayerPersistentStatusIcon_51BDC0(); // FDA88E清1。
// 原函数：0x0051BE10。
void ShowPlayerTimedStatusIcon_51BE10(); // FDA88E置0x24。
// 原函数：0x0051BE20。
void ShowPlayerPetPersistentStatusIcon_51BE20(); // FDA88E置8。
// 原函数：0x0051BE30。
void HidePlayerPetPersistentStatusIcon_51BE30(); // FDA88E清8。
// 原函数：0x0051BE40。
void ShowPlayerPetTimedStatusIcon_51BE40(); // FDA88E置0x50。
// 原函数：0x00519D50。
std::int8_t DestroyPlayerMapPresence_519D50();                  // 原 0x519D50：附件清理、重读玩家标死与动作尾链，返回低 S8。
// 519D50 尾链及 51A840 状态生产共用的玩家位槽，不复制 FDA88E 动作标志。
// 原函数：0x0051A5A0。
void MarkPlayerAsPartyLeader_51A5A0(); // 原 51A5A0：先置 FDA88A bit8，再只写 FDA88C 低 W8。
// 原函数：0x0051A5B0。
cg::yobi::YobiAction* ClearPlayerLeaderMarker_51A5B0(); // 原 51A5B0：条件清标志并返回入口借用指针。
// 原函数：0x0051BDD0。
std::int32_t SetPlayerStatusIcon_51BDD0(std::int32_t value); // 原 51BDD0：发布动作值，透传输入。
// 原函数：0x0051BDF0。
void ClearPlayerStatusIcon_51BDF0(); // 原 51BDF0：清动作 bit1 后清动作值，不发送。

} // namespace detail

} // namespace cg::world
