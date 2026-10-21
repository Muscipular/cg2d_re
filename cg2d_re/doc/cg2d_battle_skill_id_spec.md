# 战斗技能编号命名分析（BSI01）

日期：2026-10-21。状态：data／服务器源码／客户端编号边界分析完成；未定义或替换技能枚举，未改运行源码。

## 1. 数据来源与解析边界

- 用户指定 `H:\Games\cgmsv_21.2a\gmsv\data`，并明确编码为GBK。skill.txt与tech.txt严格按GBK读取，分别得到169条技能、1736条技术记录；skilllv.txt、jobs.txt用于79／94职业限制核对。只读，不修改游戏目录。
- 最初尝试的仓库服务器data未通过UTF-8解码，已停止使用；新目录的techfuncname.txt也未通过确认的GBK解码，不切换编码猜读，不将其内容作为依据。
- 服务器参考为 `D:\Source\cgmsv\cgmsv_server` 的现有源码，显式UTF-8；与游戏目录数据不是同一运行版本的证明，不宣称本次验证了部署一致性。
- include/skill.h与skill/skill.c确认：按TAB分列，skill.txt第1列名称、第2列SKILL_ID，第4列类别；不能把行号或名称当ID。
- include/tech.h与skill/tech.c确认：tech.txt第1列名称、第2列TECH_FUNCNAME、第3列TECH_OPTION、第4列TECH_ID、第6列TECH_SKILLID。加载时按TECH_SKILLID关联技能表，按函数名绑定函数及战斗命令。

## 2. 三种编号必须区分

1. **数据技能ID**：skill.txt的SKILL_ID和tech.txt的TECH_SKILLID，表示技术所属技能；名称及函数可作为业务候选证据。
2. **报文原始skillId／techId**：BattleSkillMeta和Lua注册使用的双ID。客户端ReadBattleMSkillHeader在归一化前保存两者；Lua允许完整整数注册，不能通过枚举列举来拒绝自定义ID。
3. **客户端分派技能号**：gBattleMSkill5AAA78及相应castSkill103C消费。客户端输入techId<205000时按techId/100获得；>=205000时保留报文skillId。这里的techId必须是实际收到的报文字段，不能直接拿磁盘TECH_ID替代。

服务器还可改写显示技术号：battle_skill.c的BATTLE_StatusChange读取SK:／MD:选项，生成displayCom，再将它作为SKL首个技术号字段发送。当前源码1910～1933行明确标注fake techId。因此磁盘TECH_ID/100只能作为候选线索，最终枚举要沿实际消息生产路径核对。

## 3. 数据与处理函数可确认的候选

下表ID首先表示data技能ID；客户端分派位置要按上一节核对。英文名是候选常量后缀，不改游戏显示名；同一处理函数不代表相同技能效果。

| ID | data名称／类别 | tech处理函数 | 候选后缀 |
| --- | --- | --- | --- |
| 0 | 连击 | TECH_ContinuationAttack | ContinuationAttack |
| 1 | 诸刃 | TECH_Parameter，TR／DR／MR参数 | DoubleEdgedAttack |
| 3 | 乾坤一掷 | TECH_Parameter，HR／DD参数 | RecklessAttack |
| 4 | 气功弹 | TECH_SpiracleShot | SpiracleShot |
| 5 | 崩击 | TECH_GuardBreak／GuardBreakEx | GuardBreak |
| 6 | 战栗袭心 | TECH_Forcecut | ForceCut |
| 7 | 护卫 | TECH_BodyGuard | BodyGuard |
| 8 | 圣盾 | TECH_SpecialGuard | SpecialGuard |
| 9 | 阳炎 | TECH_Mikawashi | EvasiveAttack |
| 10 | 防御魔法攻击 | TECH_MagicGuard | MagicGuard |
| 11 | 反击 | TECH_CrossCounter | CrossCounter |
| 12 | 明镜止水 | TECH_Consentration | Concentration |
| 19～22 | 陨石／冰冻／火焰／风刃魔法 | TECH_Attack_Magic | 按元素分别命名，不合并ID |
| 23～26、27～30 | 上述四系的强力／超强魔法 | TECH_Attack_Magic | 分别保留范围级别 |
| 31 | 吸血魔法 | TECH_Dorain | DrainMagic |
| 32～37 | 中毒／昏睡／石化／酒醉／混乱／遗忘魔法 | TECH_StatusChange | 六类状态分别命名 |
| 38～43、44～49 | 上述六类强力／超强魔法 | TECH_StatusChange | 分别保留范围级别 |
| 50～53 | 地／水／火／风祈祷 | TECH_Treat_Type | 按四属性分别命名 |
| 54 | 属性反转 | TECH_Reverse_Type | ReverseAttribute |
| 55／56 | 攻击／魔法反弹 | TECH_Reflection_Physics／Magic | ReflectPhysical／ReflectMagic |
| 57／58 | 攻击／魔法吸收 | TECH_Absorb_Physics／Magic | AbsorbPhysical／AbsorbMagic |
| 59／60 | 攻击／魔法无效 | TECH_Ineffective_Physics／Magic | NullifyPhysical／NullifyMagic |
| 61～63 | 补血／强力补血／超强补血 | TECH_Heal_Magic | 按范围命名治疗技能 |
| 64～66 | 恢复／强力恢复／超强恢复 | TECH_LpRecovery | 与即时补血技能分开 |
| 67 | 洁净魔法 | TECH_StatusRecover | StatusRecovery |
| 68 | 气绝回复 | TECH_Revive | Revive |
| 73／74 | 攻击／防御 | TECH_NormalAttack／NormalGuard | NormalAttack／NormalGuard |
| 75～80 | 六类异常状态攻击 | TECH_StatusAttack | 按固定异常类型分别命名 |
| 94 | 混乱攻击 | TECH_PanicAttack | PanicAttack，见下一节 |
| 95 | 乱射 | TECH_RandomShot | RandomShot |
| 96 | 暗杀 | TECH_Assassin | Assassinate |
| 97 | 大地之怒 | TECH_EarthQuake | EarthQuake |
| 98 | 跳舞 | TECH_Dance | Dance |
| 99 | 捷舞 | TECH_DefUp | DefenseUp |
| 100 | 霹雳舞 | TECH_DefDown | DefenseDown |
| 101 | 黏巴达舞 | TECH_AtkUp | AttackUp |
| 102 | skill表为梭舞，tech表为灵魂舞 | TECH_AtkDown | AttackDown，不按单一显示名定英文 |
| 103 | 啪啦啪啦舞 | TECH_AglUp | AgilityUp |
| 104 | 浩室舞 | TECH_AglDown | AgilityDown |

99～104还由服务器include/battle.h的BATTLE_PARAMETER_SKILL_*和客户端状态类型生产共同佐证。114在本data缺失，但服务器RCVUP宏与客户端独立技能114路径可佐证回复能力上升，来源等级与表内技能不同。

## 4. 79与94的同名区别

两者在skill.txt均名为“混乱攻击”，但不能归入同一个枚举项，也不是“79格斗、94属性攻击”。

- **79：固定混乱异常攻击。** tech.txt的7900～7951系列使用TECH_StatusAttack，所查该技能记录没有MD:覆盖。服务器tech.c注册到BATTLE_COM_M_STATUSATTACK；battle_event.c的BATTLE_StatusAttackCheck在case79默认选择异常类型4，BATTLE_checkStatusChange的type4读取Confusion／更新混乱状态。SK:／MD:及套装覆盖仍属于原有可配置行为，不能据此新增固定限制。候选名ConfusionAttack。
- **94：格斗系可学的随机异常攻击。** tech.txt的9400～9458系列使用TECH_PanicAttack；9409显示名“狮子奋迅”。服务器注册到BATTLE_COM_P_PANIC，命中后调用BATTLE_PanicCheck，并把返回的异常类型附加到战斗消息；客户端94另读该类型并选择减益图像。候选名PanicAttack，可注释“格斗混乱攻击／随机异常攻击”。
- **当前服务器的随机集合必须按可达代码解释。** BATTLE_PanicCheck的17439行使用RAND(0,4)，0直接返回；1／2／3／4分别对应睡眠／醉酒／混乱／遗忘。虽保留case5中毒、case6石化，但当前抽取范围不会到达，不能说当前实现随机六种异常。
- **职业证据**：skilllv.txt第38209～38215行给职业141～147配置技能94等级上限；jobs.txt确认141为见习格斗士、142为格斗士、147为拳圣。94同时也配置给士兵等职业，不能命名为“格斗专属”。79在这些格斗职业的表项上限为0，但有超级职等其他非零记录，不能断言仅宠物／怪物能用。
- 这里讨论的是异常状态附加，不是地、水、火、风属性攻击；“属性攻击”若用于指这类技能，容易与属性相克混淆。

## 5. 技术号与技能号不一致的实际样例

最后一列仅按客户端公式对磁盘技术号作静态换算，前提是服务器将该技术号原样发出；不代表本轮已确认实际报文一定如此。

| data技能ID／名称 | TECH_ID样例 | 处理函数 | 技术号直传时的分派值 |
| --- | --- | --- | --- |
| 1001 戒骄戒躁 | 25700 | TECH_DelayAttack | 257 |
| 1002 一击必中 | 25800 | TECH_DelayAttack | 258 |
| 1003 毒击 | 25900 | TECH_StatusAttack | 259 |
| 1004 一石二鸟 | 26000 | TECH_Wattack | 260 |
| 1005 骑士之誉 | 26100 | TECH_FullBodyGuard | 261 |
| 1006 迅速果断 | 26200 | TECH_FirstAttack | 262 |
| 1007 羊头狗肉 | 26300 | TECH_Copy | 263 |
| 1008 虚死为上 | 26400 | TECH_DeadRun | 264 |
| 1009 骑乘 | 26500 | TECH_Rider | 265 |
| 1010 因果报应 | 26600 | TECH_Madmax | 266 |
| 1011 精神冲击波 | 26700 | TECH_MagicMissile | 267 |
| 257 突风 | 40030 | TECH_MonWind | 400 |
| 258 アシッド | 40130 | TECH_Acid | 401 |
| 259 バリア | 40230 | TECH_Barrier | 402 |
| 260 座骑 | 40330 | TECH_RiderPet | 403 |
| 261 人宠合击ATK | 40430 | TECH_RiderBreak | 404 |
| 262 等级控制 | 40530 | TECH_LvSet | 405 |
| 263 人宠合击MND | 40630 | TECH_RiderBreak | 406 |
| 2008 挑衅 | 10500 | TECH_Taunt | 105，与data技能105飞燕乱舞不是同一个概念 |
| 107 洞察的附加技术“狮子吼” | 11201 | TECH_UltimateAttack | 112 |
| 9909 G咒术技能 | 7589 | TECH_StatusAttack，MD:0 | 75 |

生产技能还存在500／501技术前缀对应不同所属技能的情况，不纳入本次战斗常量替换范围。由这些样例可见，不能把data名称直接绑定到所有gBattleMSkill数值。

## 6. 未覆盖与后续实施边界

- 本data缺少此前扩展状态映射中的2006、2010、2012、2014、2015、2019、2027、2050～2052、2058～2067、2089～2095和3002～3016。现有CG_BATTLE_EXT_EFFECT_SKILL_*来源宏不能据此改为猜测的业务名称。
- 服务器宏名也需交叉核实：例如94的历史宏名含RANDOMDANCE，但data函数及当前服务器命中处理为PanicAttack；不直接复制历史命名作为业务结论。
- 后续若使用枚举，原始技能域与客户端分派域应明确区分，例如BattleSkillId与BattleSkillDispatchId。只为实际需要替换且证据充分的常量定义成员；原始skillId、Lua注册键、协议字段及跨域存储仍保留有符号整数，不限制自定义ID。
- 普通静态编号用函数／行为语义命名，并注释data显示名、实际来源和编号域；有归一化冲突的项先核完整消息生产链；未确认项保留数字并写明原因。与statusEffectType13DC的状态类型宏保持独立。
- 本轮仅建立分析依据，不自动创建枚举或修改业务。只做显式编码读取、源码／表格静态核对；未编译、运行客户端、计算摘要或新增测试。
