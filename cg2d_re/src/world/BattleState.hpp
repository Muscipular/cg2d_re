#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include "PlayerState.hpp"
#include "cg/Yobi/YobiRuntime.hpp"

// 战斗状态、协议结果与场景绘制的共享声明。

namespace cg::world {

// 原 0x5ABB70/0x5ABB6C/0x553F90/0x553F84：43C8F0 顺序重置的独立 S32 槽。
extern std::int32_t gValue5ABB70; // 原全局：0x005ABB70。
extern std::int32_t gValue5ABB6C; // 原全局：0x005ABB6C。
extern std::int32_t gValue553F90; // 原全局：0x00553F90。
extern std::int32_t gValue553F84; // 原全局：0x00553F84。
// 原函数：0x0050D120。
void HandleBattleEntryReply_50D120(
    std::uintptr_t socket, std::int32_t type, std::int32_t imageNo); // 完整战斗进入回复，socket未用。
// 原函数：0x0050D3C0。
void HandleBattleStreamReply_50D3C0(std::uintptr_t socket, const char* text); // C/P/A/第二字节U/M完整分派。
// 原FF0C14四条结果记录；50D500按两次W16、一次S32的顺序发布。
struct BattleResultRecord {
    std::uint16_t value00; // 原成员偏移：+0x00。
    std::uint16_t value02; // 原成员偏移：+0x02。
    std::int32_t value04; // 原成员偏移：+0x04。
};
static_assert(offsetof(BattleResultRecord, value00) == 0);
static_assert(offsetof(BattleResultRecord, value02) == 2);
static_assert(offsetof(BattleResultRecord, value04) == 4);
static_assert(sizeof(BattleResultRecord) == 8);

// 原FF0C8C十五条技能结果；+2两字节不由50D500写入，末条止FF0D04。
struct BattleSkillResultRecord {
    std::int16_t value00; // 原成员偏移：+0x00。
    std::array<std::byte, 2> bytes02; // 原成员偏移：+0x02。
    std::int32_t value04; // 原成员偏移：+0x04。
};
static_assert(offsetof(BattleSkillResultRecord, value00) == 0);
static_assert(offsetof(BattleSkillResultRecord, bytes02) == 2);
static_assert(offsetof(BattleSkillResultRecord, value04) == 4);
static_assert(sizeof(BattleSkillResultRecord) == 8);

// 窗口30只读借用50D500唯一结果存储，各字段仍按原指令时机读取。
extern std::array<BattleResultRecord, 4> gBattleResultsFF0C14; // 原全局：0x00FF0C14。
extern std::array<BattleSkillResultRecord, kSkillSlotCount> gBattleSkillResultsFF0C8C; // 原全局：0x00FF0C8C。
extern std::array<std::array<char, 29>, 3> gBattleResultNamesFF0C34; // 原全局：0x00FF0C34。
extern std::array<std::int32_t, 3> gBattleResultGraphicsFF0D0C; // 原全局：0x00FF0D0C。
// 原函数：0x0050D500。
void HandleBattleResultsReply_50D500(std::uintptr_t socket, const char* payload); // 三阶段战斗结果生产，socket未用。
extern std::int32_t gDpResultDelta_FF0D04; // 原全局：0x00FF0D04。 既有DP回复变化值，窗口31直接读取。
extern std::int32_t gDpResultTotal_FF0D08; // 原全局：0x00FF0D08。 既有DP回复总量，与变化值分别采样。
void ConsumeBattlePlayerTimer(std::int32_t seconds); // 原43BBCE..43BBF2，先扣毫秒再按S32负值清零。
// 原 59F9F8，战斗背景 W32 图号；重建与战斗绘制/协议生产共用，非地址。
extern std::int32_t gBattleBackgroundImage59F9F8;
extern std::uint8_t gBattleAttributeEffectFlags59F9E0; // 原全局：0x0059F9E0。 4209C0门禁；43CBB0按称号value生产0/5。
extern std::int32_t gBattleType5ABFC8; // 原全局：0x005ABFC8。 50D120写完整S32类型。
extern std::uint32_t gBattleModeFlag5ABFCC; // 原全局：0x005ABFCC。 仅类型2写1。
extern std::uint32_t gValue5ABB1C; // 原全局：0x005ABB1C。
extern std::uint32_t gValue5ABB20; // 原全局：0x005ABB20。 原W32初值FFFFFFFF，4383F0清零，418B50/43CBB0置1。
extern std::uint32_t gValue5ABB28; // 原全局：0x005ABB28。
extern std::array<char, 0x1000> gBattleCText5A3A20; // 原全局：0x005A3A20。 场景从C队列复制的活动原字节文本。
extern std::int32_t gBattleCOffset5ABA80; // 原全局：0x005ABA80。 438550先写2；438450/4384C0消费，与C队列索引独立。
extern std::uint32_t gBattleFieldFlags5ABB58; // 原全局：0x005ABB58。 438550发布完整W32，43CBB0随后检查bit20。
extern std::array<char, 0x1000> gBattleMText5AAA7C; // 原全局：0x005AAA7C。
extern std::int32_t gBattleMScanOffset5A8A3C; // 原全局：0x005A8A3C。 M头扫描S32游标，非队列索引或对象载荷64游标。
extern std::int32_t gBattleMPreviewOffset5AAA60; // 原全局：0x005AAA60。 439600/439700预扫描游标，由43B290复制头游标。
extern std::uint32_t gBattleActiveEventMask5AAA74; // 原全局：0x005AAA74。 32事件槽位图，43B6C0清当前位。
extern std::array<cg::yobi::YobiAction*, 32> gBattleEventActions5ABA9C; // 原全局：0x005ABA9C。 非拥有事件引用；4383F0清表，43B610按当前eventSlot发布。
extern const std::array<std::int32_t, 5> gBattleValueThresholds54D93C; // 原全局：0x0054D93C。
extern std::array<std::uint8_t, 22> gBattlePreviewParticipants5AAA48; // 原全局：0x005AAA48。 调度累计、43B290逐槽冲突判定。
extern std::array<std::array<std::int32_t, 2>, 22> gBattleEffectPairs5A8A40; // 原全局：0x005A8A40。 仅映射439B30已消费位置，40F2B1同源读取。
struct BattleNamedEffectRecord {
    std::array<char, 32> name; // 原编码名称，首字节0表示空槽。
    std::int32_t value; // 原+20，439B30按名称更新。
};
extern std::array<BattleNamedEffectRecord, 20> gBattleNamedEffects5ABCE0; // 原全局：0x005ABCE0。
extern cg::yobi::YobiAction* gBattleSceneAction5ABA94; // 原全局：0x005ABA94。 4383F0发布的非拥有场景对象，43CBB0已调用初始化。
extern std::array<cg::yobi::YobiAction*, 22> gBattleAllEntries59E980; // 原全局：0x0059E980。 含20/21特殊槽的唯一非拥有对象表。
extern const std::span<cg::yobi::YobiAction*, 20> gBattleEntries59E980; // 原全局：0x0059E980。 常规路径仅遍历前20槽。
extern const std::array<std::int32_t, 22> gBattlePointIndices5536FC; // 原全局：0x005536FC。
// R03：窗口初始化调整这一份阵位，战斗动作／效果继续借用。
void CenterBattleLayoutToWindow();
extern std::array<std::array<std::int32_t, 4>, 22> gBattleGraphicPoints553758; // 原全局：0x00553758。 两组原S32坐标。
extern std::int32_t gBattleCurrentPosition5ABCA4; // 原全局：0x005ABCA4。 实际gMyPostion；不同于P回复的5ABCA8。
extern std::int32_t gBattlePlayerActionState5ABC74; // 原全局：0x005ABC74。 UI发布动作完成状态，非5ABB74回合数。
extern std::array<std::int32_t, 9> gBattleSelectionModes5ABC7C; // 原全局：0x005ABC7C。 496F10清九个W32，含94/98/9C。
extern std::int32_t gBattleSelectedSkill5ABB5C; // 原全局：0x005ABB5C。 4974C0写技能槽，4975A0/498520重取。
extern std::int32_t gBattleSelectedTechnique5ABC78; // 原全局：0x005ABC78。 4974C0写技术槽，不是UI列表索引。
extern std::int32_t gBattleCommandState5ABC3C; // 原全局：0x005ABC3C。 技能/宠物命令完成状态，498520消费。
extern std::uint8_t gBattleRebirthDisabled5ABC40; // 原全局：0x005ABC40。 497C80先读的独立U8门禁，498520后续写。
extern std::int32_t gBattleSkillActionCount5ABBF4; // 原全局：0x005ABBF4。
extern std::int32_t gBattlePetActionCount5ABBBC; // 原全局：0x005ABBBC。
extern std::int32_t gBattleCountdownActive5ABCB8; // 原全局：0x005ABCB8。 43C4E0置1，4982C0到期清零。
extern std::uint64_t gBattleCommandDeadline5ABBF8; // 原全局：0x005ABBF8。 原32位tick截止值，不是地址。
extern std::uint32_t gBattleCommandWaitMs; // G01：BtWt命令等待毫秒，默认五分钟。
extern bool gBattleShowEnemyHpBars; // G02：战斗追加血条及原第一处悬停信息门禁共享。
extern std::int32_t gBattleCountdownSecond5ABB80; // 原全局：0x005ABB80。 低于十秒时的独立提示秒数。
extern std::int32_t gBattleSnapshotPetIndex5ABBFC; // 原全局：0x005ABBFC。 4965D0每次解码后重取，非553F90。
extern std::int32_t gBattleCommandX553F78; // 原全局：0x00553F78。 映像490，43C4E0覆盖493。
extern std::int32_t gBattleCommandY553F7C; // 原全局：0x00553F7C。 映像-104，43C4E0覆盖-106。
extern std::int32_t gBattleCommandValue553F80; // 原全局：0x00553F80。 映像及初始化17，43C3E0读取。
extern std::int32_t gBattlePetSkillGraphicIndex553F8C; // 原全局：0x00553F8C。 原绘制返回索引，初值-1，不是对象地址。
extern std::array<std::int32_t, 9> gBattleResetValues5ABB84; // 原全局：0x005ABB84。 43C490只写-2的独立九槽，非选择标志。
extern cg::yobi::YobiAction* gBattleEntryNoticeAction5ABFB8; // 原全局：0x005ABFB8。 43C950返回的非拥有窗口动画，49BD40及场景清槽。
extern std::int32_t gBattleSelectionMode5ABC70; // 原全局：0x005ABC70。 与九个选择槽分别存储。
extern std::int32_t gBattleSkillUsed5ABC68; // 原全局：0x005ABC68。 497000/4975A0读取完整W32。
extern std::int32_t gBattlePetCommandPhase553F88; // 原全局：0x00553F88。 原.data初值1，宠物命令写0/2。
extern std::int32_t gBattleValue5ABCC0; // 原全局：0x005ABCC0。 两个技能开启入口清零，不猜未消费语义。
extern cg::yobi::YobiAction* gBattleCommandAction5ABC00; // 原全局：0x005ABC00。 原非拥有对象引用，MarkDeath后清槽。
extern std::uint32_t gBattleTargetFlags5ABC44; // 原全局：0x005ABC44。 43C560先发布完整目标位模式，43C120后续消费。
extern std::int32_t gBattleSelectedItem5ABC48; // 原全局：0x005ABC48。 499C30发布真实物品槽，498520后续消费。
extern std::int32_t gBattleItemTargetPending5ABCC4; // 原全局：0x005ABCC4。 499C30请求选目标，497100/497770处理。
extern std::int32_t gBattleBombLevelD67B2C; // 原全局：0x00D67B2C。 仅炸弹hover发布，41D4FE动画读取。
// 原函数：0x00405B40。
std::int32_t GetBattlePetPosition_405B40(std::int32_t position);
// 原函数：0x00518C60。
std::int32_t CountFreeInventorySlots_518C60(); // BEI01：玩家slot8..27逐used W16统计空槽，S32返回；十九调用点仅来源/本父两消费有据，全owner/运行待。
extern std::uint32_t gBattlePlayerFlags5ABCAC; // 原全局：0x005ABCAC。
extern std::uint32_t gBattleSkillFlags5ABC04; // 原全局：0x005ABC04。
extern std::uint32_t gBattlePetSkillFlags5ABB78; // 原全局：0x005ABB78。
extern std::uint32_t gBattleRidePet5ABC38; // 原全局：0x005ABC38。
extern std::uint32_t gBattleEndBits5ABBC0; // 原全局：0x005ABBC0。
extern std::uint32_t gBattleTurn5ABB74; // 原全局：0x005ABB74。
extern std::uint32_t gBattleTurnSnapshot5ABC08; // 原全局：0x005ABC08。

namespace detail {
// 原函数：0x0043CB10。
void CaptureBattleBackgroundSnapshot_43CB10();
// 原函数：0x004ED670。
std::int32_t ShouldPlayBattleResultSound_4ED670(); // 四角色/宠物与十五技能结果字段恰1检查，完整扫描不提前返回。
// 原函数：0x00504A50。
void OpenPendingBattleResultWindow_504A50(); // S16结果模式分派30/31或既有BGM控制，原唯一caller弃返回。
// 原函数：0x0043C8F0。
std::int32_t ResetBattleCommandSelection_43C8F0();                 // 原 [0x43C8F0,0x43C90A)，四槽顺序写入并返回 -1。
extern std::uint32_t gBattleFirstTurnPending5ABC6C; // 原全局：0x005ABC6C。 50D3C0的A消息严格1时消费。
extern std::int32_t gBattleCWriteIndex5AAA6C; // 原全局：0x005AAA6C。
extern std::int32_t gBattleCReadIndex5AAA44; // 原全局：0x005AAA44。
extern std::int32_t gBattleMWriteIndex5A3A1C; // 原全局：0x005A3A1C。
extern std::int32_t gBattleMReadIndex5AAA68; // 原全局：0x005AAA68。
extern std::array<char, 0x4000> gBattleCQueue5A4A3C; // 原全局：0x005A4A3C。 四槽，每槽1000H；50D120只清各首字节。
extern std::array<char, 0x4000> gBattleMQueue59FA18; // 原全局：0x0059FA18。
extern std::uint32_t gBattlePosition5ABCA8; // 原全局：0x005ABCA8。
// 原函数：0x0043C4E0。
void BeginBattleCommandSelection_43C4E0(); // 完整命令计时初始化、对象标志与技能选择同步。
extern std::uint32_t gBattleRebirthLevel5ABB7C; // 原全局：0x005ABB7C。
extern std::uint32_t gValue5ABCB0; // 原全局：0x005ABCB0。
extern std::int32_t gBattleBackgroundSnapshot5ABCD8; // 原全局：0x005ABCD8。 43CB10末尾写，DrawBattleScene比较。
// 原函数：0x0043CBB0。
void DrawBattleScene_43CBB0(); // 完整十状态及绘制/退出尾；427470/完整事件寿命与画面运行门禁仍未闭合。

} // namespace detail

} // namespace cg::world
