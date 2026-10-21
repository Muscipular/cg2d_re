#pragma once

#include "base.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

// 状态模式／类型宏见 doc/cg2d_yobi_value_naming_spec.md；保留有符号整数常量和完整S32字段。
// YVN01：statusEffectMode13D8的解释域；RESET仅表示清理／基态，不保证类型值为空。
#define CG_BATTLE_STATUS_MODE_RESET 0 // 清理／基态。
#define CG_BATTLE_STATUS_MODE_PARAMETER 1 // 能力变化。
#define CG_BATTLE_STATUS_MODE_DEBUFF 2 // 普通减益输入。
#define CG_BATTLE_STATUS_MODE_EXTENDED 3 // 扩展状态。

// ANR清理请求的临时类型值；动作142随后可回写现有减益类型。
#define CG_BATTLE_STATUS_TYPE_CLEAR_REQUEST (-1)

// 普通减益类型：动作141及NewBattleDebuffEffect共用；与FDC位掩码不同。
#define CG_BATTLE_DEBUFF_NONE 0 // 无普通减益类型。
#define CG_BATTLE_DEBUFF_POISON 1 // 中毒。
#define CG_BATTLE_DEBUFF_SLEEP 2 // 睡眠。
#define CG_BATTLE_DEBUFF_STONE 3 // 石化。
#define CG_BATTLE_DEBUFF_INEBRIETY 4 // 醉酒。
#define CG_BATTLE_DEBUFF_CONFUSION 5 // 混乱。
#define CG_BATTLE_DEBUFF_FORGET 6 // 遗忘。

// mode=1的能力变化类型；NONE不代表其他模式的0。
#define CG_BATTLE_PARAMETER_EFFECT_NONE 0 // 不创建新的能力状态图。
#define CG_BATTLE_PARAMETER_EFFECT_DEFENSE_UP 1 // 防御上升。
#define CG_BATTLE_PARAMETER_EFFECT_DEFENSE_DOWN 2 // 防御下降。
#define CG_BATTLE_PARAMETER_EFFECT_ATTACK_UP 3 // 攻击上升。
#define CG_BATTLE_PARAMETER_EFFECT_ATTACK_DOWN 4 // 攻击下降。
#define CG_BATTLE_PARAMETER_EFFECT_AGILITY_UP 5 // 敏捷上升。
#define CG_BATTLE_PARAMETER_EFFECT_AGILITY_DOWN 6 // 敏捷下降。
#define CG_BATTLE_PARAMETER_EFFECT_RECOVERY_UP 7 // 回复能力上升。

// mode=3的扩展类型：宏值是状态类型而非技能号；保留来源编号，相同图号不合并技能。
#define CG_BATTLE_EXT_EFFECT_SKILL_2006 0 // 集中（用户确认名称）；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2010 4 // 元素增幅（用户确认名称）；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2012 5 // 防御结界（用户确认名称）；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2019 7 // 独舞（用户确认名称）；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2015 8 // 伺机而动（用户确认名称）；当前生产不安排动作143常驻图。
#define CG_BATTLE_EXT_EFFECT_SKILL_2014 9 // 战斗号令（用户确认名称）；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2027 10 // 肾上腺素（用户确认名称）；当前生产不安排动作143常驻图。
#define CG_BATTLE_EXT_EFFECT_SKILL_2058_2059 11 // 技能2058／2059；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2060_2061 12 // 技能2060／2061；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2062_2063 13 // 技能2062／2063；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2064_2065 14 // 技能2064／2065；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2066_2067 15 // 技能2066／2067；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2092_2093 16 // 技能2092／2093；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2094_2095 17 // 技能2094／2095；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2091 18 // 技能2091；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2089 19 // 技能2089；动作141特殊图号。
#define CG_BATTLE_EXT_EFFECT_SKILL_2090 20 // 技能2090；动作141特殊图号。
#define CG_BATTLE_EXT_EFFECT_SKILL_2050 30 // 技能2050；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2051 31 // 技能2051；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2052 32 // 技能2052；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3002 33 // 技能3002；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3003 34 // 技能3003；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3004 35 // 技能3004；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3005 36 // 技能3005；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3006 37 // 技能3006；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3007 38 // 技能3007；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3008 39 // 技能3008；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3009 40 // 技能3009；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3010 41 // 技能3010；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3011 42 // 技能3011；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3012 43 // 技能3012；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3013 44 // 技能3013；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3014 45 // 技能3014；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3015 46 // 技能3015；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3016 47 // 技能3016；动作143的扩展选择值。

#define CG_BATTLE_DEBUFF_TYPE_COUNT 6 // 技能32～49循环使用的六种普通减益。
#define CG_BATTLE_EXT_SKILL_2019_IMAGE_COUNT 3 // 13E0的防御／敏捷／攻击图像变体数。

namespace cg::yobi {

namespace detail {
// BSI01：客户端分派技能号；用于gBattleMSkill5AAA78、castSkill103C及skill7C的已归一化消费。
// 保持各字段为有符号整数，允许未知／Lua技能；不用于原始BattleSkillMeta::skillId注册键。
// data技术号与所属技能号不一致时取techId / 100；不改变现有205000报文归一化阈值。
// Strong／Super仅对应data的强力／超强名称；每个成员的业务／用户来源见注释。
enum BattleSkillDispatchId : std::int32_t {
    BattleSkill_ContinuationAttack = 0, // 连击；TECH_ContinuationAttack。
    BattleSkill_Parameter_DoubleEdgedBlade = 1, // 诸刃；TECH_Parameter，名称译名区分共用处理函数。
    BattleSkill_Parameter_AllOrNothing = 3, // 乾坤一掷；TECH_Parameter，名称译名区分共用处理函数。
    BattleSkill_SpiracleShot = 4, // 气功弹；TECH_SpiracleShot。
    BattleSkill_GuardBreak = 5, // 崩击；TECH_GuardBreak／GuardBreakEx。
    BattleSkill_Forcecut = 6, // 战栗袭心；TECH_Forcecut。
    BattleSkill_SpecialGuard = 8, // 圣盾；TECH_SpecialGuard。
    BattleSkill_CrossCounter = 11, // 反击；TECH_CrossCounter。
    BattleSkill_Consentration = 12, // 明镜止水；保留TECH_Consentration来源拼写，与扩展集中技能不同。
    BattleSkill_AttackMagicEarth = 19, // 陨石／地魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWater = 20, // 冰冻／水魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicFire = 21, // 火焰／火魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWind = 22, // 风刃／风魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicEarthStrong = 23, // 强力陨石／地魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWaterStrong = 24, // 强力冰冻／水魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicFireStrong = 25, // 强力火焰／火魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWindStrong = 26, // 强力风刃／风魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicEarthSuper = 27, // 超强陨石／地魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWaterSuper = 28, // 超强冰冻／水魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicFireSuper = 29, // 超强火焰／火魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWindSuper = 30, // 超强风刃／风魔法；TECH_Attack_Magic。
    BattleSkill_Dorain = 31, // 吸血魔法；保留TECH_Dorain来源拼写。
    BattleSkill_StatusChangePoison = 32, // 中毒魔法；TECH_StatusChange。
    BattleSkill_StatusChangeSleep = 33, // 昏睡魔法；TECH_StatusChange。
    BattleSkill_StatusChangeStone = 34, // 石化魔法；TECH_StatusChange。
    BattleSkill_StatusChangeInebriety = 35, // 酒醉魔法；TECH_StatusChange。
    BattleSkill_StatusChangeConfusion = 36, // 混乱魔法；TECH_StatusChange。
    BattleSkill_StatusChangeForget = 37, // 遗忘魔法；TECH_StatusChange。
    BattleSkill_StatusChangePoisonStrong = 38, // 强力中毒魔法；TECH_StatusChange。
    BattleSkill_StatusChangeSleepStrong = 39, // 强力昏睡魔法；TECH_StatusChange。
    BattleSkill_StatusChangeStoneStrong = 40, // 强力石化魔法；TECH_StatusChange。
    BattleSkill_StatusChangeInebrietyStrong = 41, // 强力酒醉魔法；TECH_StatusChange。
    BattleSkill_StatusChangeConfusionStrong = 42, // 强力混乱魔法；TECH_StatusChange。
    BattleSkill_StatusChangeForgetStrong = 43, // 强力遗忘魔法；TECH_StatusChange。
    BattleSkill_StatusChangePoisonSuper = 44, // 超强中毒魔法；TECH_StatusChange。
    BattleSkill_StatusChangeSleepSuper = 45, // 超强昏睡魔法；TECH_StatusChange。
    BattleSkill_StatusChangeStoneSuper = 46, // 超强石化魔法；TECH_StatusChange。
    BattleSkill_StatusChangeInebrietySuper = 47, // 超强酒醉魔法；TECH_StatusChange。
    BattleSkill_StatusChangeConfusionSuper = 48, // 超强混乱魔法；TECH_StatusChange。
    BattleSkill_StatusChangeForgetSuper = 49, // 超强遗忘魔法；TECH_StatusChange。
    BattleSkill_TreatTypeEarth = 50, // 大地的祈祷；TECH_Treat_Type。
    BattleSkill_TreatTypeWater = 51, // 海洋的祈祷；TECH_Treat_Type。
    BattleSkill_TreatTypeFire = 52, // 火焰的祈祷；TECH_Treat_Type。
    BattleSkill_TreatTypeWind = 53, // 云群的祈祷；TECH_Treat_Type。
    BattleSkill_ReverseType = 54, // 属性反转；TECH_Reverse_Type。
    BattleSkill_ReflectionPhysics = 55, // 攻击反弹；TECH_Reflection_Physics。
    BattleSkill_ReflectionMagic = 56, // 魔法反弹；TECH_Reflection_Magic。
    BattleSkill_AbsorbPhysics = 57, // 攻击吸收；TECH_Absorb_Physics。
    BattleSkill_AbsorbMagic = 58, // 魔法吸收；TECH_Absorb_Magic。
    BattleSkill_IneffectivePhysics = 59, // 攻击无效；TECH_Ineffective_Physics。
    BattleSkill_IneffectiveMagic = 60, // 魔法无效；TECH_Ineffective_Magic。
    BattleSkill_HealMagic = 61, // 补血魔法；TECH_Heal_Magic。
    BattleSkill_HealMagicStrong = 62, // 强力补血魔法；TECH_Heal_Magic。
    BattleSkill_HealMagicSuper = 63, // 超强补血魔法；TECH_Heal_Magic。
    BattleSkill_LpRecovery = 64, // 恢复魔法；TECH_LpRecovery。
    BattleSkill_LpRecoveryStrong = 65, // 强力恢复魔法；TECH_LpRecovery。
    BattleSkill_LpRecoverySuper = 66, // 超强恢复魔法；TECH_LpRecovery。
    BattleSkill_StatusRecover = 67, // 洁净魔法；TECH_StatusRecover。
    BattleSkill_Revive = 68, // 气绝回复；TECH_Revive。
    BattleSkill_NormalAttack = 73, // 攻击；TECH_NormalAttack。
    BattleSkill_StatusAttackPoison = 75, // 中毒攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_StatusAttackSleep = 76, // 昏睡攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_StatusAttackStone = 77, // 石化攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_StatusAttackInebriety = 78, // 酒醉攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_StatusAttackConfusion = 79, // 混乱攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_StatusAttackForget = 80, // 遗忘攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_PanicAttack = 94, // 混乱攻击；TECH_PanicAttack，随机异常，与79固定混乱不同。
    BattleSkill_RandomShot = 95, // 乱射；TECH_RandomShot。
    BattleSkill_Assassin = 96, // 暗杀；TECH_Assassin。
    BattleSkill_EarthQuake = 97, // 大地之怒；TECH_EarthQuake。
    BattleSkill_Dance = 98, // 跳舞；TECH_Dance。
    BattleSkill_DefUp = 99, // 捷舞；TECH_DefUp，防御上升。
    BattleSkill_DefDown = 100, // 霹雳舞；TECH_DefDown，防御下降。
    BattleSkill_AtkUp = 101, // 黏巴达舞；TECH_AtkUp，攻击上升。
    BattleSkill_AtkDown = 102, // 梭舞／灵魂舞；TECH_AtkDown，攻击下降。
    BattleSkill_AglUp = 103, // 啪啦啪啦舞；TECH_AglUp，敏捷上升。
    BattleSkill_AglDown = 104, // 浩室舞；TECH_AglDown，敏捷下降。
    BattleSkill_Ranbu = 105, // 飞燕乱舞；TECH_Ranbu，用户指定105保留Ranbu；挑衅另用2008。
    BattleSkill_UltimateAttack = 112, // 狮子吼；TECH_UltimateAttack，所属107的技术11201按整数除100。
    BattleSkill_RecoveryUp = 114, // 回复能力上升；服务器RCVUP宏与客户端独立114路径，当前data缺记录。
    BattleSkill_DelayAttack257 = 257, // 戒骄戒躁；TECH_DelayAttack，技术25700／100；子类保留编号，所属技能1001。
    BattleSkill_DelayAttack258 = 258, // 一击必中；TECH_DelayAttack，技术25800／100；子类保留编号，所属技能1002。
    BattleSkill_StatusAttackPoisonStrike = 259, // 毒击；TECH_StatusAttack，技术25900／100；PUK2_SKILL下默认中毒，与75不合并。
    BattleSkill_Wattack = 260, // 一石二鸟；TECH_Wattack，技术26000／100，所属技能1004。
    BattleSkill_FirstAttack = 262, // 迅速果断；TECH_FirstAttack，技术26200／100，所属技能1006。
    BattleSkill_Copy = 263, // 羊头狗肉；TECH_Copy，技术26300／100，所属技能1007。
    BattleSkill_Madmax = 266, // 因果报应；TECH_Madmax，技术26600／100，所属技能1010。
    BattleSkill_MagicMissile = 267, // 精神冲击波；TECH_MagicMissile，技术26700／100，所属技能1011。
    BattleSkill_MonWind = 400, // 突风；TECH_MonWind，技术40030／100，所属技能257。
    BattleSkill_Acid = 401, // 酸性攻击；TECH_Acid，技术40130／100，data名称为アシッド。
    BattleSkill_Barrier = 402, // 屏障；TECH_Barrier，技术40230／100，data名称为バリア。
    BattleSkill_RiderBreakAttack = 404, // 人宠合击ATK；TECH_RiderBreak，技术40430／100。
    BattleSkill_LvSet = 405, // 等级控制；TECH_LvSet，技术40530／100。
    BattleSkill_RiderBreakMind = 406, // 人宠合击MND；TECH_RiderBreak，技术40630／100。
    BattleSkill_Concentration = 2006, // 集中；用户确认名称，英文为项目译名。
    BattleSkill_Recklessness = 2007, // 鲁莽；用户确认名称，英文为项目译名。
    BattleSkill_Taunt = 2008, // 挑衅；用户确认名称，英文为项目译名。用户指定挑衅按2008；105不命名为Taunt。
    BattleSkill_ElementalAmplification = 2010, // 元素增幅；用户确认名称，英文为项目译名。
    BattleSkill_Blessing = 2011, // 祝福；用户确认名称，英文为项目译名。
    BattleSkill_DefensiveBarrier = 2012, // 防御结界；用户确认名称，英文为项目译名。
    BattleSkill_VoodooCurse = 2013, // 巫毒诅咒；用户确认名称，英文为项目译名。
    BattleSkill_BattleCommand = 2014, // 战斗号令；用户确认名称，英文为项目译名。
    BattleSkill_BideTime = 2015, // 伺机而动；用户确认名称，英文为项目译名。
    BattleSkill_ShortStrike = 2016, // 短击；用户确认名称，英文为项目译名。
    BattleSkill_ShadowHand = 2017, // 暗影之手；用户确认名称，英文为项目译名。
    BattleSkill_SoloDance = 2019, // 独舞；用户确认名称，英文为项目译名。
    BattleSkill_MercyStrike = 2021, // 手下留情；用户确认名称，英文为项目译名。
    BattleSkill_KarmaCycle = 2022, // 因果轮回；用户确认名称，英文为项目译名。
    BattleSkill_Adrenaline = 2027, // 肾上腺素；用户确认名称，英文为项目译名。
    BattleSkill_EmergencyTransfusion = 2028, // 紧急输血；用户确认名称，英文为项目译名。
    BattleSkill_StatusAttackPoison2096 = 2096, // 独立中毒异常攻击；图号110075及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_StatusAttackSleep2097 = 2097, // 独立昏睡异常攻击；图号110076及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_StatusAttackStone2098 = 2098, // 独立石化异常攻击；图号110077及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_StatusAttackInebriety2099 = 2099, // 独立酒醉异常攻击；图号110078及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_StatusAttackConfusion2100 = 2100, // 独立混乱异常攻击；图号110079及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_StatusAttackForget2101 = 2101, // 独立遗忘异常攻击；图号110080及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_ConfusionShockwave = 3029, // 混乱冲击波；用户确认名称，英文为项目译名。
    BattleSkill_SwallowSlash = 3100, // 燕斩；用户确认名称，英文为项目译名。
    BattleSkill_BladeSoul = 3101, // 锋上魂；用户确认名称，英文为项目译名。
    BattleSkill_WindRendingStrike = 4003, // 裂风击；用户确认名称，英文为项目译名。
};

// BSI01未命名项仍保留原数字：不能仅凭共享处理入口或图像推导同一业务。
// 72、81～88、93、106～111、113、117、2002、2005：尚未完成本编号域的名称对应核实。
// 2050～2052、2058～2067、2089～2095、3002～3016：仅确认独立技能／图号，未提供业务名。
// 2055～2057、2068～2079、3001、3019、3037、3038、4002：已有分支不足以确定名称。
// 4004仅作为现有比较的区间上界保留，不据此定义技能名称。
// 2009鹰眼为用户提供名称，本轮未找到独立消费分支，暂不定义无消费者的枚举项。

// BAF01：BC及相关角色记录的协议位；与BM命中标志、服务器CHAR_BATTLEFLG分别定义。
// 数值来源及客户端承接差异见 doc/cg2d_battle_actor_flags_spec.md。
enum BattleCharacterFlag : std::uint32_t {
    BattleCharacterFlag_Appear = 0x00000001u,       // 协议要求播放入场；FDC运行位反向承接。
    BattleCharacterFlag_Dead = 0x00000002u,         // 倒地／死亡。
    BattleCharacterFlag_Player = 0x00000004u,       // 玩家类别；部分骑乘宠物也按此类别编码。
    BattleCharacterFlag_Pet = 0x00000008u,          // 宠物类别，服务端BC_FLAG_MON。
    BattleCharacterFlag_Poison = 0x00000010u,       // 中毒。
    BattleCharacterFlag_Sleep = 0x00000020u,        // 睡眠。
    BattleCharacterFlag_Stone = 0x00000040u,        // 石化。
    BattleCharacterFlag_Inebriety = 0x00000080u,    // 醉酒，对应ModDrunk。
    BattleCharacterFlag_Confusion = 0x00000100u,    // 混乱。
    BattleCharacterFlag_Forget = 0x00000200u,       // 遗忘。
    BattleCharacterFlag_TwoActions = 0x00000400u,   // 双动。
    BattleCharacterFlag_ReverseType = 0x00000800u,  // 属性反转。
    BattleCharacterFlag_DamageUp = 0x00001000u,     // 伤害增强。
    BattleCharacterFlag_Morph = 0x00002000u,        // 变身。
    BattleCharacterFlag_DefenseUp = 0x00004000u,    // 防御上升。
    BattleCharacterFlag_DefenseDown = 0x00008000u,  // 防御下降。
    BattleCharacterFlag_AttackUp = 0x00010000u,     // 攻击上升。
    BattleCharacterFlag_AttackDown = 0x00020000u,   // 攻击下降。
    BattleCharacterFlag_AgilityUp = 0x00040000u,    // 敏捷上升。
    BattleCharacterFlag_AgilityDown = 0x00080000u,  // 敏捷下降。
    BattleCharacterFlag_RecoveryUp = 0x00100000u,   // 回复能力上升。
    BattleCharacterFlag_Earth = 0x01000000u,        // 地属性。
    BattleCharacterFlag_Water = 0x02000000u,        // 水属性。
    BattleCharacterFlag_Fire = 0x04000000u,         // 火属性。
    BattleCharacterFlag_Wind = 0x08000000u,         // 风属性。
    BattleCharacterFlag_Rebirth = 0x10000000u,     // REB形态，不是倒地后的复活动作。
};

// BattleActorPayload::flagsFDC的运行位；存储仍为uint32_t，保留组合位及未知位。
enum BattleActorFlag : std::uint32_t {
    BattleActorFlag_EntryComplete = 0x00000001u, // 入场／归位完成；不能直接当作协议Appear。
    BattleActorFlag_Dead = 0x00000002u,         // 倒地／死亡。
    BattleActorFlag_Player = 0x00000004u,       // 玩家类别。
    BattleActorFlag_Pet = 0x00000008u,          // 宠物类别。
    BattleActorFlag_Poison = 0x00000010u,       // 中毒，特效110075。
    BattleActorFlag_Sleep = 0x00000020u,        // 睡眠，特效110076。
    BattleActorFlag_Stone = 0x00000040u,        // 石化，特效110077。
    BattleActorFlag_Inebriety = 0x00000080u,    // 醉酒，特效110078。
    BattleActorFlag_Confusion = 0x00000100u,    // 混乱，特效110079。
    BattleActorFlag_Forget = 0x00000200u,       // 遗忘，特效110080。
    BattleActorFlag_TwoActions = 0x00000400u,   // 双动；BC路径仅更新附着效果，不补写此位。
    BattleActorFlag_ReverseType = 0x00000800u,  // 属性反转，特效110152。
    BattleActorFlag_DamageUp = 0x00001000u,     // 伤害增强；放大／缩小动作完成时更新。
    BattleActorFlag_Morph = 0x00002000u,        // 变身，用于绘制与挂件处理。
    BattleActorFlag_DefenseUp = 0x00004000u,    // 防御上升，特效103018。
    BattleActorFlag_DefenseDown = 0x00008000u,  // 防御下降，特效103019。
    BattleActorFlag_AttackUp = 0x00010000u,     // 攻击上升，特效103020。
    BattleActorFlag_AttackDown = 0x00020000u,   // 攻击下降，特效103021。
    BattleActorFlag_AgilityUp = 0x00040000u,    // 敏捷上升，特效103022。
    BattleActorFlag_AgilityDown = 0x00080000u,  // 敏捷下降，特效103023。
    BattleActorFlag_RecoveryUp = 0x00100000u,   // 回复能力上升，103033分支使用103018显示。
    BattleActorFlag_Earth = 0x01000000u,        // 地属性，fields288[0]。
    BattleActorFlag_Water = 0x02000000u,        // 水属性，fields288[1]。
    BattleActorFlag_Fire = 0x04000000u,         // 火属性，fields288[2]。
    BattleActorFlag_Wind = 0x08000000u,         // 风属性，fields288[3]。
    BattleActorFlag_Rebirth = 0x10000000u,     // REB形态及其持续特效。
};

// 仅替换既有组合掩码；睡眠／石化播放门禁不代表全部不可行动状态。
inline constexpr std::uint32_t BattleActorSleepOrStoneMask = BattleActorFlag_Sleep | BattleActorFlag_Stone;
inline constexpr std::uint32_t BattleActorPlayerOrPetMask = BattleActorFlag_Player | BattleActorFlag_Pet;
inline constexpr std::uint32_t BattleActorAbnormalMask = BattleActorFlag_Poison | BattleActorFlag_Sleep
    | BattleActorFlag_Stone | BattleActorFlag_Inebriety | BattleActorFlag_Confusion | BattleActorFlag_Forget;
// 原0xFFF03FFF只清六个能力升降位，不能扩大到RecoveryUp。
inline constexpr std::uint32_t BattleActorParameterChangeMask = BattleActorFlag_DefenseUp | BattleActorFlag_DefenseDown
    | BattleActorFlag_AttackUp | BattleActorFlag_AttackDown | BattleActorFlag_AgilityUp | BattleActorFlag_AgilityDown;

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
    std::int32_t actionFrameCountFCC; // 原+FCC；动作回调帧计数，用于阶段等待和移动阈值。
    std::int32_t valueFD0; // 原+FD0；共用等待倒计时、一次性启动状态及碰撞边界编号，不能只叫计时器，保留原名。
    std::int32_t valueFD4; // 原+FD4；攻击等待帧与REB进入／退出阶段共用，单一含义不能覆盖，保留原名。
    std::int32_t busyFD8; // 原成员偏移：+0xFD8。
    std::uint32_t flagsFDC; // 原成员偏移：+0xFDC；BattleActorFlag位集合，保留未知位和原消息承接。
    std::uint32_t stateFE0; // 原成员偏移：+0xFE0。
    std::int32_t effectFlagsFE4; // 原成员偏移：+0xFE4。
    std::int32_t moveTargetXFE8; // 412870按目标坐标侧写入；与00/08/10坐标来源独立。
    std::int32_t moveTargetYFEC;
    std::int32_t valueFF0; // 原+FF0；共用周期／碰撞计数、延时和保存的phase，无法统一为计时器或阶段，保留原名。
    std::int32_t attackModeFF4; // 原成员偏移：+0xFF4。 426FC0写入原完整攻击模式。
    std::int32_t aliveFF8; // 原成员偏移：+0xFF8。
    std::array<std::byte, 4> bytesFFC; // 原成员偏移：+0xFFC。 原FFC..FFF：随整个载荷清零/浅复制，用途及地址属性未定。
    std::int32_t flashCounter1000; // 原成员偏移：+0x1000。 404DB0残影节拍；438290沿通用分配零化，不额外写入。
    std::int32_t flash1004; // 原成员偏移：+0x1004。 416DD0的bit100门禁，仅首次写1并播放音效。
    std::int32_t fpUsed1008; // 原成员偏移：+0x1008。
    std::int32_t targetPosition100C; // 原成员偏移：+0x100C。 4242B0三方序列写入原目标。
    std::int32_t reactionImage1010; // 原成员偏移：+0x1010。
    std::int32_t image1014; // 原成员偏移：+0x1014。 427470装备消息写51B740转换后的图号，40C480消费。
    std::int32_t soundId1018; // 原+1018；BM／技能链生产，动作80传给PlaySe的音效编号。
    std::int32_t soundId101C; // 原成员偏移：+0x101C。 40CCB0音效编号；427470技能98模式6已写入，424620已接，其他生产沿对应父合同。
    std::int32_t soundId1020; // 原成员偏移：+0x1020。 40CCE0音效编号；与1018/101C独立，不是地址。
    std::int32_t effectPlaybackState1024; // 原+1024；关联特效置1、完成置2，部分动作续用3／4；保持完整S32状态。
    std::int32_t image1028; // 原成员偏移：+0x1028。 427470技能链生产图号；41CD00的type34用作图号及音效/坐标重读源。
    std::int32_t image102C; // 原成员偏移：+0x102C。 426FC0按技术索引和目标阵营选表。
    std::int32_t value1030; // 原+1030；共用HP／FP数值、选图参数和动作模式，不能仅命名为伤害值，保留原名；非指针。
    std::int32_t numberDisplayType1034; // 原成员偏移：+0x1034。 427470状态/技能链写入，4107C0/410B00消费的显示及数值类型。
    std::int32_t value1038; // 原+1038；409680作强制命中门禁，40E560兼作动作阶段；统一业务含义未定，保留原名。
    std::int32_t castSkill103C; // 原+0x103C；分派技能号，已证常量见BattleSkillDispatchId，保留未知整数。
    std::int32_t castLevel1040; // 原成员偏移：+0x1040。 与1044的技术索引独立。
    std::int32_t castSkillLevel1044; // 原成员偏移：+0x1044。
    std::int32_t skill98Mode1048; // 原成员偏移：+0x1048。 42CFE5/42E90A写S32，141的技能98按图号偏移消费。
    std::array<std::byte, 4> bytes104C; // 原成员偏移：+0x104C。 原104C..104F：仅保留存储，不按邻接值猜图号或计数。
    std::int32_t attackDebuffType1050; // 原成员偏移：+0x1050。 424F64保存技能94的M整数；409680用于减益图号偏移。
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
    std::int32_t queuedTargetIndex13A4; // 原+13A4；queuedTargets1084读取游标，由43B830递增。
    std::int32_t queuedTargetCount13A8; // 原成员偏移：+0x13A8。
    std::int32_t baseImage13AC; // 原成员偏移：+0x13AC。 413550保存原图号，413750/413980恢复，独立于13E4。
    std::int32_t appearanceOverrideActive13B0; // 原成员偏移：+0x13B0。 413550写0/1，413750/413980恢复后清零，4131B0消费。
    YobiAction* gear13B4; // 原成员偏移：+0x13B4。 422C20主附件，非拥有引用。
    std::array<YobiAction*, 2> auxiliaryGear13B8; // 原成员偏移：+0x13B8。 原13B8/13BC，422C90的0/1槽。
    std::array<std::int32_t, 2> auxiliaryGearTypes13C0; // 原成员偏移：+0x13C0。 原13C0/13C4，清附件不清类型。
    YobiAction* rider13C8; // 原成员偏移：+0x13C8。 4222B0保存4221F0创建的非拥有引用，移除时MarkDeath再清槽。
    std::int32_t debuffState13CC; // 原成员偏移：+0x13CC。 41F660分配前写2，失败不回滚。
    std::int32_t debuffType13D0; // 原成员偏移：+0x13D0。 41F660六图号对应类型1至6。
    std::array<std::byte, 4> bytes13D4; // 原成员偏移：+0x13D4。 原13D4..13D7：仅保留存储，不解释为减益状态或指针。
    std::int32_t statusEffectMode13D8; // 原+13D8；CG_BATTLE_STATUS_MODE_*解释域，427470生产、41FE00消费完整W32。
    std::int32_t statusEffectType13DC; // 原+13DC；按模式使用CG_BATTLE_DEBUFF_*／PARAMETER_EFFECT_*／EXT_EFFECT_*，保留清理哨兵及未知值。
    std::int32_t statusEffectImageIndex13E0; // 原+13E0；扩展技能2019按S32查54D4C0，变体数见CG_BATTLE_EXT_SKILL_2019_IMAGE_COUNT。
    std::int32_t image13E4; // 原成员偏移：+0x13E4。 427470技能链生产图号；41CD00的type43从重取的源载荷读取。
    BattleSkillMeta skillMeta; // E01：本机具名扩展，分配及完整克隆统一按 sizeof(BattleActorPayload)。
};

static_assert(sizeof(BattleEntryEffectRecord) == 16);

static_assert(std::is_trivially_copyable_v<BattleActorPayload>);
static_assert(std::is_trivially_copy_assignable_v<BattleActorPayload>);

static_assert(offsetof(BattleEntryEffectRecord, position4) == 12);

// 原场景180B载荷；标量保留宽度，B0场地特效引用按原生T*拓宽。
struct BattleScenePayload {
    std::int32_t completedEntryCount00; // 原+00，41A9E0/41AF00完成指定分类且场景phase8时递增。
    std::int32_t entryCount04; // 原成员偏移：+0x04。 427470/CMB按S32读写的参与者数量。
    std::array<std::int32_t, 20> entryPositions08; // 原成员偏移：+0x08。 原08..57的位置列表，非对象地址。
    std::int32_t packetSubState58; // 原成员偏移：+0x58。
    std::int32_t remainingMessages5C; // 原成员偏移：+0x5C。
    std::int32_t eventSlot60; // 原成员偏移：+0x60。
    std::int32_t packetOffset64; // 原成员偏移：+0x64。
    std::int32_t groupBase68; // 原成员偏移：+0x68。
    std::int32_t casterPosition6C; // 原成员偏移：+0x6C。 427750恢复、437A29回存的同源执行快照，非新端字节偏移。
    std::int32_t value70; // 原+70；gBattleMValue5ABA7C快照，既存角色位置又作累计技能重复标记，不能统一叫施法者位置，保留原名。
    std::int32_t defenderPosition74; // 原成员偏移：+0x74。
    std::int32_t targetPosition78; // 原成员偏移：+0x78。
    std::int32_t skill7C; // 原+0x7C；当前场景技能号；原生分派见BattleSkillDispatchId，Lua路径仍承接原始ID。
    std::int32_t technique80; // 原成员偏移：+0x80。
    std::int32_t itemMode84; // 原+84；ITM消息模式，执行状态保存／恢复使用。
    std::int32_t skill98Mode88; // 原+88；技能98模式，执行状态保存／恢复使用。
    std::int32_t feedbackSourcePosition8C; // 原+8C；反射／累计回馈的来源角色位置快照。
    std::uint32_t resultFlags90; // 原成员偏移：+0x90。
    std::uint32_t flags94; // 原成员偏移：+0x94。
    std::int32_t feedbackEffectImage98; // 原+98；反射／累计回馈的首个附加图号快照。
    std::uint32_t flags9C; // 原成员偏移：+0x9C。 恢复为函数局部量，公共尾不回存。
    std::int32_t feedbackAlternateImageA0; // 原成员偏移：+0xA0。 同上，不与messageValueA4合并。
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
void UpdateBattleDirectionFromAngle_405CB0(YobiAction* action); // 角度只校正一圈，按八个不等宽区间写方向。
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
void FaceBattleActorTowardFormationPosition_406260(YobiAction* action); // 载荷反向坐标差转角度，再更新方向。
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
