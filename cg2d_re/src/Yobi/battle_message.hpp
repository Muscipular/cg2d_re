#pragma once

#include "base.hpp"

#include <array>
#include <cstdint>

namespace cg::yobi {

namespace detail {

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
extern std::int32_t gBattleMValue5A4A34;

// 共享原状态，只在本组实现中定义；原全局：0x005ABA90。
extern std::int32_t gBattleMValue5ABA90;

// 共享原状态，只在本组实现中定义；原全局：0x005ABA84。
extern std::uint32_t gBattleMFlags5ABA84;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A28。
extern std::int32_t gBattleMValue5A4A28;

// 共享原状态，只在本组实现中定义；原全局：0x0059FA14。
extern std::int32_t gBattleMMessageValue59FA14;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A2C。
extern std::int32_t gBattleMSkillLevel5A4A2C;

// 共享原状态，只在本组实现中定义；原全局：0x005A3A18。
extern std::int32_t gBattleMSkillLevelIndex5A3A18;

enum class BattleMDataHeader : std::int32_t {
    Action = 12, ConsumeFp = 13, Chat = 14, ChangeBody = 17, AppendSkill = 23, End = 24,
};

enum class BattleMStatusHeader : std::int32_t {
    Equipment = 6, StatusValue = 9, Debuff = 10, Power = 11, Rebirth = 19, QueuedValue = 20,
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
    std::int32_t* source, std::int32_t previousSource, std::uint32_t* mask, std::int32_t* flag);
// 原函数：0x00427470。
void UpdateBattleEvent_427470(YobiAction* scene); // 完整分派与恢复/回存；424620本体已接，完整战斗另验。
// 原函数：0x0043B860。
std::int32_t PeekBattleMInteger_43B860(YobiAction* action); // 只预读，不推进对象或全局游标。
// 原函数：0x00423960。
std::int32_t GetBattleMObjectHeader_423960(YobiAction* action, char* destination);
// 原函数：0x004239D0。
void ParseBattleMRide_4239D0(YobiAction* action); // RID预读/实际消费差异，双新对象交226恢复发布。
// 原函数：0x00438450。
std::int32_t TakeBattleCInteger_438450(); // C独立全局游标，大写十六进制读取。
// 原函数：0x004384C0。
void TakeBattleCString_4384C0(char* destination, std::int32_t capacity); // 原双字节截断，尾部反转义/空叶。
// 原43B9B0：只推进局部游标，flags&1010h非零且位置匹配时返回次位置，否则-1。
std::int32_t FindBattleMTarget_43B9B0(YobiAction* scene, std::int32_t position, std::int32_t skip);
// 原43BF30：mode1只消费预扫描8整数；mode0读对象字段，五输出均为S32非拥有指针。
void TakeBattleMAuxGraphics_43BF30(
    YobiAction* action, std::int32_t mode, std::int32_t* firstImage, std::int32_t* secondImage,
    std::int32_t* firstImages, std::int32_t* secondImages, std::int32_t* values);

} // namespace detail

} // namespace cg::yobi
