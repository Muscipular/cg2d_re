#pragma once

#include "base.hpp"

#include <array>
#include <cstdint>

namespace cg::world { struct BattleSceneState; }

namespace cg::yobi {

namespace detail {

// 原43BF30读取模式；对象字段与预扫描各消费8整数，未知模式保持不读取。
enum BattleMAuxGraphicsReadMode : std::int32_t {
    BattleMAuxGraphicsReadMode_ReadObjectFields = 0, // 读对象游标，输出6项映射图号及2项值。
    BattleMAuxGraphicsReadMode_SkipPreviewFields = 1, // 仅推进预扫描游标，不解引用输出。
};

// 原553E94头表索引；439B30预扫描与427470事件phase共用，不是角色动作或技能mode。
enum BattleMHeaderKind : std::int32_t {
    BattleMHeader_SKL = 0, // 技能头；内部技能／mode与本枚举独立。
    BattleMHeader_ESC = 1, // 逃跑头，原Transition分类。
    BattleMHeader_CLR = 2, // CLR头，原Clear分类。
    BattleMHeader_SUP = 3, // SUP头，原Support分类。
    BattleMHeader_POS = 4, // 位置头。
    BattleMHeader_MON = 5, // MON头，原实体进出处理。
    BattleMHeader_EQU = 6, // 装备头。
    BattleMHeader_ITM = 7, // 物品头，内部物品mode独立。
    BattleMHeader_CMB = 8, // 合击头，预扫描保留原FF终止位置。
    BattleMHeader_PSN = 9, // PSN头，原StatusValue分类。
    BattleMHeader_ANR = 10, // ANR头，原Debuff分类。
    BattleMHeader_PUE = 11, // PUE头，原Power分类。
    BattleMHeader_ACT = 12, // 动作头。
    BattleMHeader_CFP = 13, // FP消耗头。
    BattleMHeader_CHT = 14, // 聊天头。
    BattleMHeader_PNO = 15, // PNO头，原PetNotice分类。
    BattleMHeader_PLV = 16, // PLV头，原PetLeave分类。
    BattleMHeader_CBC = 17, // CBC头，原ChangeBody分类。
    BattleMHeader_PDN = 18, // PDN头，原PetDown分类。
    BattleMHeader_REB = 19, // REB头，原Rebirth分类。
    BattleMHeader_BST = 20, // BST头，原QueuedValue分类及聊天外计时处理。
    BattleMHeader_GMN = 21, // GMN头，全位置参与预扫描，独立业务名未证。
    BattleMHeader_RID = 22, // 骑乘头，试读／实际读取差异保持。
    BattleMHeader_DSK = 23, // 追加技能头，预扫描贯穿END。
    BattleMHeader_END = 24, // 结束头。
};

inline constexpr std::array<const char*, 25> kBattleMHeaders553E94 { // 原常量参考：0x00553E94。
    "SKL", "ESC", "CLR", "SUP", "POS", "MON", "EQU", "ITM", "CMB", "PSN",
    "ANR", "PUE", "ACT", "CFP", "CHT", "PNO", "PLV", "CBC", "PDN", "REB",
    "BST", "GMN", "RID", "DSK", "END",
};

// 共享原状态，只在本组实现中定义；原全局：0x005AAA64。
extern std::int32_t gBattleMCasterPosition5AAA64;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A20。
extern std::uint32_t gBattleMResultFlags5A4A20;

// 共享原状态，只在本组实现中定义；原全局：0x005ABA7C。
extern std::int32_t gBattleMValue5ABA7C;

// 共享原状态，只在本组实现中定义；原全局：0x005AAA40。
extern std::int32_t gBattleMTargetPosition5AAA40;

// 共享原状态，只在本组实现中定义；原全局：0x005AAA78。
extern std::int32_t gBattleMSkill5AAA78;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A38。
extern std::int32_t gBattleMTechnique5A4A38;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A34。
extern std::int32_t gBattleMSkill98Mode5A4A34;

// 共享原状态，只在本组实现中定义；原全局：0x005ABA90。
extern std::int32_t gBattleMFeedbackSourcePosition5ABA90;

// 共享原状态，只在本组实现中定义；原全局：0x005ABA84。
extern std::uint32_t gBattleMFlags5ABA84;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A28。
extern std::int32_t gBattleMFeedbackEffectImage5A4A28;

// 共享原状态，只在本组实现中定义；原全局：0x0059FA14。
extern std::int32_t gBattleMMessageValue59FA14;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A2C。
extern std::int32_t gBattleMSkillLevel5A4A2C;

// 共享原状态，只在本组实现中定义；原全局：0x005A3A18。
extern std::int32_t gBattleMSkillLevelIndex5A3A18;

enum class BattleMDataHeader : std::int32_t {
    Action = BattleMHeader_ACT, ConsumeFp = BattleMHeader_CFP, Chat = BattleMHeader_CHT, ChangeBody = BattleMHeader_CBC, AppendSkill = BattleMHeader_DSK, End = BattleMHeader_END,
};

enum class BattleMStatusHeader : std::int32_t {
    Equipment = BattleMHeader_EQU, StatusValue = BattleMHeader_PSN, Debuff = BattleMHeader_ANR, Power = BattleMHeader_PUE, Rebirth = BattleMHeader_REB, QueuedValue = BattleMHeader_BST,
};


// 原函数：0x00439070。
std::int32_t GetBattleMHeader_439070(char* destination); // 独立头游标，读3B并补NUL，结束返回1。
// 原函数：0x00438F70。
std::int32_t TakeBattleMInteger_438F70(YobiAction* action); // 对象载荷64游标读取大写十六进制。
// 原函数：0x00438FE0。
void TakeBattleMString_438FE0(YobiAction* action, char* destination, std::int32_t capacity);
// 原函数：0x00439600。
std::int32_t TakeBattleMPreviewInteger_439600(); // 局部扫描，仅结束时发布5AAA60。
// 原函数：0x00439670。
void TakeBattleMPreviewString_439670(char* destination, std::int32_t capacity);
// 原函数：0x00439700。
std::int32_t GetBattleMPreviewHeader_439700(char* destination); // 预扫描头，不推进5A8A3C。
// 原函数：0x0043B940。
std::int32_t TakeBattleMIntegerAt_43B940(std::int32_t* offset); // 显式游标，逐字节发布。
// 原函数：0x0043B8C0。
void TakeBattleMStringAt_43B8C0(std::int32_t* offset, char* destination, std::int32_t capacity);
// 原函数：0x00439770。
void ScanBattleMRideParticipants_439770(std::uint8_t* participants); // 22槽计数，RID试读成功才推进游标。
// 原函数：0x00439920。
std::int32_t ScanBattleMAttackParticipants_439920(std::int32_t skill, std::uint8_t* participants);
// 原函数：0x00439B30。
void ScanBattleMParticipants_439B30(
    std::int32_t kind, std::uint8_t* participants, std::int32_t* source, std::int32_t* flag);
// 原43B290：冲突返回0，保留此前槽写入和预扫描副作用；成功OR参与位，不清调用方mask。
std::int32_t CheckBattleMParticipants_43B290(
    YobiAction* scene, std::int32_t* source, std::int32_t previousSource, std::uint32_t* mask, std::int32_t* flag);
// 原函数：0x00427470。
void UpdateBattleEvent_427470(YobiAction* scene); // 完整分派与恢复/回存；424620本体已接，完整战斗另验。
// 原函数：0x0043B860。
std::int32_t PeekBattleMInteger_43B860(YobiAction* action); // 只预读，不推进对象或全局游标。
// 原函数：0x00423960。
std::int32_t GetBattleMObjectHeader_423960(YobiAction* action, char* destination);
// 原函数：0x004239D0。
void ParseBattleMRide_4239D0(YobiAction* action); // RID预读/实际消费差异，双新对象交226恢复发布。
// 原函数：0x00438450。
std::int32_t TakeBattleCInteger_438450(world::BattleSceneState& battle); // C会话游标，大写十六进制读取。
// 原函数：0x004384C0。
void TakeBattleCString_4384C0(world::BattleSceneState& battle, char* destination, std::int32_t capacity); // 原双字节截断，尾部反转义/空叶。
// 原43B9B0：只推进局部游标，flags&1010h非零且位置匹配时返回次位置，否则-1。
std::int32_t FindBattleMTarget_43B9B0(YobiAction* scene, std::int32_t position, std::int32_t skip);
// 原43BF30：mode1只消费预扫描8整数；mode0读对象字段，五输出均为S32非拥有指针。
void TakeBattleMAuxGraphics_43BF30(
    YobiAction* action, std::int32_t mode, std::int32_t* firstImage, std::int32_t* secondImage,
    std::int32_t* firstImages, std::int32_t* secondImages, std::int32_t* values);

} // namespace detail

} // namespace cg::yobi
