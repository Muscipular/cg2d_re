#pragma once

#include "base.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::yobi {

namespace detail {
// 原1C起的16B数值记录；按动作解释字段，见BMSA06控制/附加记录合同，不含对象指针。
struct BattleEntryEffectRecord {
    std::int32_t position; // 原记录+00；命中目标、245特效类型或-1队列哨兵。
    std::uint32_t flags; // 原记录+04，保持完整W32标志。
    std::int32_t position3; // 原记录+08；数值/附加图号，245用作寿命输入。
    std::int32_t position4; // 原记录+0C；图号/附加值，三字段4393A0不写此字段。
};


// 4239D0/427470骑乘拆分原18h载荷；指针原生拓宽，末8B仅保留原分配范围，不解释业务。
struct BattleRideCarrierPayload {
    YobiAction* first00; // 原成员偏移：+0x00。 42958F/424179写首个438290结果，非拥有引用，226恢复角色槽。
    YobiAction* second04; // 原成员偏移：+0x04。 429923/42417B写第二个438290结果，非拥有引用，226恢复后清槽。
    std::uint32_t flags08; // 原成员偏移：+0x08。 42912D写M整数、414820恢复FDC；4239D0/RID不写此字段。
    std::int32_t gearImage0C; // 原成员偏移：+0x0C。 428FEC/42933E写51B740结果，恢复时严格>0才建主装备。
    std::array<std::byte, 8> bytes10; // 原成员偏移：+0x10。
};

static_assert(sizeof(BattleRideCarrierPayload) == 2 * sizeof(YobiAction*) + 16);

// E01：SKL 归一化前的技能标识与 E10 节点绑定，随战斗载荷复制／清理。
struct BattleSkillMeta {
    std::int32_t techId; // 未归一化的报文技术编号，YAML图号匹配键。
    std::int32_t skillId; // 未归一化的报文技能编号，与techId组成Lua注册键。
    std::int32_t runtimeGeneration; // E10运行代次，非地址。
    std::int32_t runtimeNodeId; // E10运行表节点编号，非拥有引用。
    std::int32_t flags; // 位0标识Lua技能绑定，沿参考保存。
    std::int32_t reserved; // 参考保留字段，初始化为零并随载荷复制。
};

// 战斗专用原生载荷；保留四段用途未定字节，后缀仅标原位置；完整字段/生产仍待证。
struct BattleActorPayload {
    std::int32_t x00; // 原成员偏移：+0x00。 原00坐标槽；40CAA0还会递增caster载荷同槽，不另建计数副本。
    std::int32_t y04; // 原成员偏移：+0x04。
    std::int32_t x08; // 原成员偏移：+0x08。
    std::int32_t y0C; // 原成员偏移：+0x0C。 43C9A0按(08-00,0C-04)计算朝向，不与对象坐标合并。
    std::int32_t x10; // 原成员偏移：+0x10。 416DD0恢复目标对象的F32坐标来源。
    std::int32_t y14; // 原成员偏移：+0x14。
    std::int32_t battlePosition18; // 4221F0/422A30/422B00复制到附件+1D4。
    std::array<BattleEntryEffectRecord, 200> effects1C; // 原成员偏移：+0x1C。 原16B效果记录，重置只写position。
    std::int32_t effectRunningC9C; // 原成员偏移：+0xC9C。
    std::int32_t effectCountCA0; // 原成员偏移：+0xCA0。
    std::array<std::int32_t, 200> castListCA4; // 原成员偏移：+0xCA4。 FC4前的200项动作队列。
    std::int32_t castPlayIndexFC4; // 原成员偏移：+0xFC4。
    std::int32_t castListIndexFC8; // 原成员偏移：+0xFC8。
    std::int32_t valueFCC; // 原成员偏移：+0xFCC。
    std::int32_t valueFD0; // 原成员偏移：+0xFD0。
    std::int32_t valueFD4; // 原成员偏移：+0xFD4。
    std::int32_t busyFD8; // 原成员偏移：+0xFD8。
    std::uint32_t flagsFDC; // 原成员偏移：+0xFDC。
    std::uint32_t stateFE0; // 原成员偏移：+0xFE0。
    std::int32_t effectFlagsFE4; // 原成员偏移：+0xFE4。
    std::int32_t moveTargetXFE8; // 412870按目标坐标侧写入；与00/08/10坐标来源独立。
    std::int32_t moveTargetYFEC;
    std::int32_t valueFF0; // 原成员偏移：+0xFF0。
    std::int32_t attackModeFF4; // 原成员偏移：+0xFF4。 426FC0写入原完整攻击模式。
    std::int32_t aliveFF8; // 原成员偏移：+0xFF8。
    std::array<std::byte, 4> bytesFFC; // 原成员偏移：+0xFFC。 原FFC..FFF：随整个载荷清零/浅复制，用途及地址属性未定。
    std::int32_t flashCounter1000; // 原成员偏移：+0x1000。 404DB0残影节拍；438290沿通用分配零化，不额外写入。
    std::int32_t flash1004; // 原成员偏移：+0x1004。 416DD0的bit100门禁，仅首次写1并播放音效。
    std::int32_t fpUsed1008; // 原成员偏移：+0x1008。
    std::int32_t targetPosition100C; // 原成员偏移：+0x100C。 4242B0三方序列写入原目标。
    std::int32_t reactionImage1010; // 原成员偏移：+0x1010。
    std::int32_t image1014; // 原成员偏移：+0x1014。 427470装备消息写51B740转换后的图号，40C480消费。
    std::int32_t value1018; // 原成员偏移：+0x1018。
    std::int32_t value101C; // 原成员偏移：+0x101C。 40CCB0音效编号；427470技能98模式6已写入，424620已接，其他生产沿对应父合同。
    std::int32_t value1020; // 原成员偏移：+0x1020。 40CCE0音效编号；与1018/101C独立，不是地址。
    std::int32_t value1024; // 原成员偏移：+0x1024。
    std::int32_t image1028; // 原成员偏移：+0x1028。 427470技能链生产图号；41CD00的type34用作图号及音效/坐标重读源。
    std::int32_t image102C; // 原成员偏移：+0x102C。 426FC0按技术索引和目标阵营选表。
    std::int32_t value1030; // 原成员偏移：+0x1030。 41CD00的type18按2000/3500选图，非指针。
    std::int32_t value1034; // 原成员偏移：+0x1034。 427470状态/技能链写入，4107C0/410B00消费的显示及数值类型。
    std::int32_t value1038; // 原成员偏移：+0x1038。
    std::int32_t castSkill103C; // 原成员偏移：+0x103C。
    std::int32_t castLevel1040; // 原成员偏移：+0x1040。 与1044的技术索引独立。
    std::int32_t castSkillLevel1044; // 原成员偏移：+0x1044。
    std::int32_t value1048; // 原成员偏移：+0x1048。 42CFE5/42E90A写S32，141的技能98按图号偏移消费。
    std::array<std::byte, 4> bytes104C; // 原成员偏移：+0x104C。 原104C..104F：仅保留存储，不按邻接值猜图号或计数。
    std::int32_t value1050; // 原成员偏移：+0x1050。 424F64保存技能94的M整数；409680用于减益图号偏移。
    YobiAction* effect1054; // 原成员偏移：+0x1054。 41F660发布；416DD0/4072E0先标死再清槽，非拥有引用。
    YobiAction* attachedEffect1058; // 原成员偏移：+0x1058。 41FBE0创建并发布的非拥有附属特效引用。
    YobiAction* effect105C; // 原成员偏移：+0x105C。 41F860发布；4131B0/422370标死并清槽，非拥有引用。
    YobiAction* effect1060; // 原成员偏移：+0x1060。 非拥有克隆/骑乘/缩放特效；4067F0骑乘路径payload为BattleRideCarrierPayload。
    YobiAction* centeredEffect1064; // 原成员偏移：+0x1064。 41FA00发布的非拥有居中特效；41F950随父对象移动。
    YobiAction* effect1068; // 原成员偏移：+0x1068。 41FE00发布、41FCD0更新；4131B0标死并清槽，非拥有引用。
    YobiAction* rebirthEffect106C; // 原成员偏移：+0x106C。 4114B0/438CAF发布419CF0结果；497C80只检查空值，79标死后清槽。
    YobiAction* caster1070; // 原成员偏移：+0x1070。 4392A0记录动作发起者，非拥有引用。
    std::array<YobiAction*, 3> rebirthAuxiliaryEffects1074; // 原成员偏移：+0x1074。 原1074/1078/107C三槽，非拥有；79按原顺序清理。
    std::int8_t rebirthEffectType1080; // 原成员偏移：+0x1080。 4113A0按低U8加1发布/退出清零，4208F0按S8选图；其他生产仍缺。
    std::array<std::byte, 3> bytes1081; // 原成员偏移：+0x1081。 原1081..1083：用途未定字节，不以原生隐式对齐代替承载。
    std::array<std::int32_t, 200> queuedTargets1084; // 原成员偏移：+0x1084。 原[1084,13A4)，43B7F0追加值及哨兵。
    std::int32_t value13A4; // 原成员偏移：+0x13A4。
    std::int32_t queuedTargetCount13A8; // 原成员偏移：+0x13A8。
    std::int32_t baseImage13AC; // 原成员偏移：+0x13AC。 413550保存原图号，413750/413980恢复，独立于13E4。
    std::int32_t value13B0; // 原成员偏移：+0x13B0。 413550写0/1，413750/413980恢复后清零，4131B0消费。
    YobiAction* gear13B4; // 原成员偏移：+0x13B4。 422C20主附件，非拥有引用。
    std::array<YobiAction*, 2> auxiliaryGear13B8; // 原成员偏移：+0x13B8。 原13B8/13BC，422C90的0/1槽。
    std::array<std::int32_t, 2> auxiliaryGearTypes13C0; // 原成员偏移：+0x13C0。 原13C0/13C4，清附件不清类型。
    YobiAction* rider13C8; // 原成员偏移：+0x13C8。 4222B0保存4221F0创建的非拥有引用，移除时MarkDeath再清槽。
    std::int32_t debuffState13CC; // 原成员偏移：+0x13CC。 41F660分配前写2，失败不回滚。
    std::int32_t debuffType13D0; // 原成员偏移：+0x13D0。 41F660六图号对应类型1至6。
    std::array<std::byte, 4> bytes13D4; // 原成员偏移：+0x13D4。 原13D4..13D7：仅保留存储，不解释为减益状态或指针。
    std::int32_t value13D8; // 原成员偏移：+0x13D8。 427470状态/技能链生产；41FE00按完整W32与3比较。
    std::int32_t value13DC; // 原成员偏移：+0x13DC。 427470状态/技能链生产141/143状态图号选择值。
    std::int32_t value13E0; // 原成员偏移：+0x13E0。 427470技能链生产索引；143的13DC==7按S32查54D4C0。
    std::int32_t image13E4; // 原成员偏移：+0x13E4。 427470技能链生产图号；41CD00的type43从重取的源载荷读取。
    BattleSkillMeta skillMeta; // E01：本机具名扩展，分配及完整克隆统一按 sizeof(BattleActorPayload)。
};

static_assert(sizeof(BattleEntryEffectRecord) == 16);

static_assert(std::is_trivially_copyable_v<BattleActorPayload>);
static_assert(std::is_trivially_copy_assignable_v<BattleActorPayload>);

static_assert(offsetof(BattleEntryEffectRecord, position4) == 12);

// 原场景180B载荷；标量保留宽度，B0场地特效引用按原生T*拓宽。
struct BattleScenePayload {
    std::int32_t value00; // 原+00，41A9E0/41AF00完成指定分类且场景phase8时递增。
    std::int32_t entryCount04; // 原成员偏移：+0x04。 427470/CMB按S32读写的参与者数量。
    std::array<std::int32_t, 20> entryPositions08; // 原成员偏移：+0x08。 原08..57的位置列表，非对象地址。
    std::int32_t packetSubState58; // 原成员偏移：+0x58。
    std::int32_t remainingMessages5C; // 原成员偏移：+0x5C。
    std::int32_t eventSlot60; // 原成员偏移：+0x60。
    std::int32_t packetOffset64; // 原成员偏移：+0x64。
    std::int32_t groupBase68; // 原成员偏移：+0x68。
    std::int32_t casterPosition6C; // 原成员偏移：+0x6C。 427750恢复、437A29回存的同源执行快照，非新端字节偏移。
    std::int32_t value70; // 原成员偏移：+0x70。
    std::int32_t defenderPosition74; // 原成员偏移：+0x74。
    std::int32_t targetPosition78; // 原成员偏移：+0x78。
    std::int32_t skill7C; // 原成员偏移：+0x7C。
    std::int32_t technique80; // 原成员偏移：+0x80。
    std::int32_t value84; // 原成员偏移：+0x84。
    std::int32_t value88; // 原成员偏移：+0x88。
    std::int32_t value8C; // 原成员偏移：+0x8C。
    std::uint32_t resultFlags90; // 原成员偏移：+0x90。
    std::uint32_t flags94; // 原成员偏移：+0x94。
    std::int32_t value98; // 原成员偏移：+0x98。
    std::uint32_t flags9C; // 原成员偏移：+0x9C。 恢复为函数局部量，公共尾不回存。
    std::int32_t valueA0; // 原成员偏移：+0xA0。 同上，不与messageValueA4合并。
    std::int32_t messageValueA4; // 原成员偏移：+0xA4。
    std::uint32_t entryMaskA8; // 原成员偏移：+0xA8。 4394A0参与对象位图，含特殊槽20/21。
    std::int32_t waitCountAC; // 原成员偏移：+0xAC。 忙状态每次检查递增，超时逐对象恢复后清零。
    YobiAction* fieldEffectB0; // 原成员偏移：+0xB0。 41FFE0发布的非拥有场地特效；438550无场地标志时标死并重取载荷清槽。
};

static_assert(offsetof(BattleScenePayload, entryMaskA8) == 0xA8);

static_assert(offsetof(BattleScenePayload, fieldEffectB0) == 0xB0);

// 原函数：0x004237E0。
std::int32_t GetBattlePosition_4237E0(std::int32_t position); // 50/51转20/21，其余原值。
// 原函数：0x0043B7F0。
void AppendBattleQueuedTarget_43B7F0(YobiAction* action, std::int32_t value);
// 原函数：0x0043B830。
std::int32_t AdvanceBattleQueuedTarget_43B830(YobiAction* action); // 先推进13A4，再判断新槽是否为-1。
// 原函数：0x0043BF00。
std::int32_t GetBattleValueBand_43BF00(std::int32_t value); // B22H01：原1..15按五个S32阈值计档1..5，范围外-1。
// 原函数：0x00404D90。
std::int32_t SelectIntValue_404D90(const std::int32_t* values, std::int32_t count, std::int32_t index);
// 原函数：0x004393A0。
void AppendBattleEffectList_4393A0(
    YobiAction* action, std::int32_t position, std::uint32_t flags, std::int32_t position3);
// 原函数：0x00439400。
void AppendBattleEffect_439400(YobiAction* action, BattleEntryEffectRecord effect);
// 原函数：0x0043BC10。
void AppendBattleAuxEffects_43BC10(
    YobiAction* action, std::int32_t position, std::uint32_t flags,
    std::int32_t value3, std::int32_t value4, std::int32_t value5);
// 原439470：先推进C9C，返回新项position是否为-1；空对象返回0。
std::int32_t AdvanceBattleEffect_439470(YobiAction* action);
// 原43BC90：三次推进、四输出原序写入并分别重读游标，返回末次推进结果。
std::int32_t TakeBattleAuxEffects_43BC90(
    YobiAction* action, std::int32_t* position, std::int32_t* value3,
    std::int32_t* value4, std::int32_t* value5);
// 原406390：固定当前flags/position，五块独立执行；附加块消费四输出，逐调用重取对象。
void ApplyBattleEffectFlags_406390(std::int32_t sourcePosition, YobiAction* action);
// 原函数：0x00416DB0。
void ApplyBattleCastEffects_416DB0(YobiAction* action); // 动作235：先应用效果，再推进动作队列。

// 原函数：0x0043BD00。
std::uint8_t IsBattleAppendEffectSkill_43BD00(std::int32_t skill); // 原54项技能准入集合，返回0/1；不查询链，见追加效果链SPEC。
// 原函数：0x0043BE00。
void AddBattleAppendEffect_43BE00(
    std::int32_t skill, std::int32_t technique, std::int32_t position); // 匹配首节点替换，否则尾插。
// 原函数：0x0043BEA0。
std::int32_t FindBattleAppendEffectTechnique_43BEA0(
    std::int32_t skill, std::int32_t position); // 按位置/技能查首节点，未命中-1。
// 原函数：0x00405CB0。
void Sub_405CB0(YobiAction* action); // 角度只校正一圈，按八个不等宽区间写方向。
// 原函数：0x00405E60。
void NormalizeBattleAngle_405E60(YobiAction* action); // BSTL01：上限首样/第二角度持样/负值重取，一次加减360。
// 原函数：0x00405EA0。
void MoveBattleByAngle_405EA0(YobiAction* action); // X存回F32后才计算Y，角度/速度按原序读取。
// 原函数：0x00405ED0。
std::int8_t GetBattleRelativeDirection_405ED0(
    float sourceX, float sourceY, float targetX, float targetY,
    float tolerance); // 第二组坐标作基准，原负基准两次相减及三闭区间保持独立。
// 原函数：0x00406060。
double GetBattleAngleDifference_406060(YobiAction* source, YobiAction* target); // 空引用返回0，未舍入角差只校正一圈。
// 原函数：0x004060D0。
void TurnBattleTowardPosition_4060D0(
    YobiAction* action, float x, float y, std::int32_t updateDirection,
    float step); // 无空指针门禁；角差转S32取绝对值后限步，严格1才更新方向。
// 原函数：0x00406190。
void TurnBattleToward_406190(
    YobiAction* action, YobiAction* target, std::int32_t updateDirection, float step);
// 原函数：0x004062A0。
std::int32_t GetBattleDistanceToPosition_4062A0(YobiAction* action, float x, float y); // BRCP01：先X后Y求差，平方和存F32/正常S32返回。
// 原函数：0x00406300。
std::int32_t GetBattleDistance_406300(YobiAction* first, YobiAction* second); // 平方和先舍入F32再开方。
// 原函数：0x00406360。
std::int32_t CountBattleActiveEntries_406360(); // BHRC01：22槽含20/21，FF8完整S32严格==1，无新payload保护。
// 原函数：0x004065A0。
std::int32_t SeedBattleRandom_4065A0(std::int32_t value); // 前20槽非空数乘活动M字节长度与输入，重播同源随机种子。
// 原函数：0x00406260。
void Sub_406260(YobiAction* action); // 载荷反向坐标差转角度，再更新方向。
// 原函数：0x004390E0。
void InitBattleCastList_4390E0(std::int32_t index); // 22槽索引，保留禁用/phase13门禁。
// 原函数：0x004391E0。
void ResetBattleCastList_4391E0(std::int32_t index); // 恢复重置，不清忙状态、技能或转向。
// 原函数：0x004392A0。
void AppendBattleCast_4392A0(YobiAction* caster, YobiAction* actor, std::int32_t state);
// 原函数：0x004392F0。
void AppendBattleCastAt_4392F0(
    YobiAction* caster, YobiAction* actor, std::int32_t state, std::int32_t index);
// 原函数：0x00439340。
void AdvanceBattleCastState_439340(YobiAction* action); // 先推进FC4，到200或-1哨兵写phase236。
// 原函数：0x0040CAC0。
void AdvanceBattleLinkedCast_40CAC0(YobiAction* action); // 动作73：当前效果目标先推进，再推进自身。
// 原函数：0x0040CAF0。
void AdvanceBattleEffectAndCast_40CAF0(YobiAction* action); // 动作74：推进效果后总是推进动作。
// 原函数：0x00439380。
void MoveBattleCastState_439380(YobiAction* action); // 播放索引对应项写phase，不推进索引。
// 原函数：0x004394A0。
std::int32_t CheckBattleTurnReady_4394A0(YobiAction* parent, std::int32_t enable); // 22槽就绪及超时恢复。

} // namespace detail

} // namespace cg::yobi
