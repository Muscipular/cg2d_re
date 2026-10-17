#pragma once

#include "base.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::yobi {

namespace detail {

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

} // namespace detail

} // namespace cg::yobi
