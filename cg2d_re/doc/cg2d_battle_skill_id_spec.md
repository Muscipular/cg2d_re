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

服务器还可改写显示技术号：battle_skill.c的BATTLE_StatusChange读取SK:／MD:选项，生成displayCom，再将它作为SKL首个技术号字段发送。当前源码1910～1933行明确标注fake techId。此事实用于区分数据映射与实际报文，不改变下述用户确认的枚举取值规则。

**用户确认的编号规则（2026-10-21）**：技术号与所属技能号不一致的项，命名表及相应分派枚举按 `skillId = techId // 100` 取值；`//` 表示整数除法，C++整数表达式写作 `techId / 100`。data的TECH_SKILLID仅保留为所属技能来源，不能用它覆盖换算后的枚举值。此规则用于本次常量命名，不改写原始报文skillId、BattleSkillMeta或Lua注册键，也不修改现有205000阈值分支；后续替换时按消费位置所属编号域使用。

## 3. 数据与处理函数可确认的候选

下表ID首先表示data技能ID；客户端分派位置要按上一节核对。英文名是候选常量后缀，不改游戏显示名；同一处理函数不代表相同技能效果。

**用户确认的命名优先级（2026-10-21）**：优先采用已核实处理函数的业务名称，去掉TECH_等固定前缀并统一单词分隔；保留可直接检索的函数词干，不另译游戏显示名作为首选。data名称写入中文注释。共享处理函数时，追加已证的异常、元素或范围子类型；子类型尚未确定则保留编号及原因，不合并ID、不猜译名称。函数名与实际实现冲突时，以实现证据为准。

**标识符与Parameter重名规则（用户最新补充）**：宏名及枚举标识符只使用ASCII英文、数字和下划线，中文技能名仅保留在注释及说明。Parameter重复项仍使用 `Parameter_$name`，两项命名为 `Parameter_DoubleEdgedBlade`（诸刃）和 `Parameter_AllOrNothing`（乾坤一掷）；英文后缀是本项目选择的技能名称译名，不宣称来自服务器标识符或官方英文名，不据此推导额外效果。该规则替代此前的中文后缀候选。

| ID | data名称／类别 | tech处理函数 | 候选后缀 |
| --- | --- | --- | --- |
| 0 | 连击 | TECH_ContinuationAttack | ContinuationAttack |
| 1 | 诸刃 | TECH_Parameter，TR／DR／MR参数 | Parameter_DoubleEdgedBlade |
| 3 | 乾坤一掷 | TECH_Parameter，HR／DD参数 | Parameter_AllOrNothing |
| 4 | 气功弹 | TECH_SpiracleShot | SpiracleShot |
| 5 | 崩击 | TECH_GuardBreak／GuardBreakEx | GuardBreak |
| 6 | 战栗袭心 | TECH_Forcecut | Forcecut |
| 7 | 护卫 | TECH_BodyGuard | BodyGuard |
| 8 | 圣盾 | TECH_SpecialGuard | SpecialGuard |
| 9 | 阳炎 | TECH_Mikawashi | Mikawashi |
| 10 | 防御魔法攻击 | TECH_MagicGuard | MagicGuard |
| 11 | 反击 | TECH_CrossCounter | CrossCounter |
| 12 | 明镜止水 | TECH_Consentration | Consentration，保留来源函数拼写 |
| 19～22 | 陨石／冰冻／火焰／风刃魔法 | TECH_Attack_Magic | AttackMagic加已证元素子类型，不合并ID |
| 23～26、27～30 | 上述四系的强力／超强魔法 | TECH_Attack_Magic | AttackMagic加已证元素及范围子类型 |
| 31 | 吸血魔法 | TECH_Dorain | Dorain，保留来源函数拼写 |
| 32～37 | 中毒／昏睡／石化／酒醉／混乱／遗忘魔法 | TECH_StatusChange | StatusChange加六类异常子类型 |
| 38～43、44～49 | 上述六类强力／超强魔法 | TECH_StatusChange | StatusChange加已证异常及范围子类型 |
| 50～53 | 地／水／火／风祈祷 | TECH_Treat_Type | TreatType加四属性子类型 |
| 54 | 属性反转 | TECH_Reverse_Type | ReverseType |
| 55／56 | 攻击／魔法反弹 | TECH_Reflection_Physics／Magic | ReflectionPhysics／ReflectionMagic |
| 57／58 | 攻击／魔法吸收 | TECH_Absorb_Physics／Magic | AbsorbPhysics／AbsorbMagic |
| 59／60 | 攻击／魔法无效 | TECH_Ineffective_Physics／Magic | IneffectivePhysics／IneffectiveMagic |
| 61～63 | 补血／强力补血／超强补血 | TECH_Heal_Magic | HealMagic加已证范围子类型 |
| 64～66 | 恢复／强力恢复／超强恢复 | TECH_LpRecovery | LpRecovery加已证范围子类型 |
| 67 | 洁净魔法 | TECH_StatusRecover | StatusRecover |
| 68 | 气绝回复 | TECH_Revive | Revive |
| 73／74 | 攻击／防御 | TECH_NormalAttack／NormalGuard | NormalAttack／NormalGuard |
| 75 | 毒性攻击 | TECH_StatusAttack | StatusAttackPoison |
| 76 | 昏睡攻击 | TECH_StatusAttack | StatusAttackSleep |
| 77 | 石化攻击 | TECH_StatusAttack | StatusAttackStone |
| 78 | 酒醉攻击 | TECH_StatusAttack | StatusAttackInebriety |
| 79 | 混乱攻击 | TECH_StatusAttack | StatusAttackConfusion |
| 80 | 遗忘攻击 | TECH_StatusAttack | StatusAttackForget |
| 94 | 混乱攻击 | TECH_PanicAttack | PanicAttack，见下一节 |
| 95 | 乱射 | TECH_RandomShot | RandomShot |
| 96 | 暗杀 | TECH_Assassin | Assassin |
| 97 | 大地之怒 | TECH_EarthQuake | EarthQuake |
| 98 | 跳舞 | TECH_Dance | Dance |
| 99 | 捷舞 | TECH_DefUp | DefUp |
| 100 | 霹雳舞 | TECH_DefDown | DefDown |
| 101 | 黏巴达舞 | TECH_AtkUp | AtkUp |
| 102 | skill表为梭舞，tech表为灵魂舞 | TECH_AtkDown | AtkDown |
| 103 | 啪啦啪啦舞 | TECH_AglUp | AglUp |
| 104 | 浩室舞 | TECH_AglDown | AglDown |

99～104还由服务器include/battle.h的BATTLE_PARAMETER_SKILL_*和客户端状态类型生产共同佐证。114在本data缺失，但服务器RCVUP宏与客户端独立技能114路径可佐证回复能力上升，来源等级与表内技能不同。

### StatusAttack异常类型依据

服务器battle_event.c的BATTLE_StatusAttackCheck（16411～16558行）按所属技能号75～80依次向BATTLE_checkStatusChange传入默认type=0～5；后者（16156行起）分别读取Poison、Sleep、Stone、Drunk、Confusion、Amnesia，并写入对应Mod状态。候选后缀中的Inebriety／Forget沿用客户端已有异常术语，对应服务器Drunk／Amnesia。

| 分派技能号 | 默认异常 | 服务器默认type | 候选后缀 |
| --- | --- | --- | --- |
| 75 | 中毒 | 0 | StatusAttackPoison |
| 76 | 昏睡 | 1 | StatusAttackSleep |
| 77 | 石化 | 2 | StatusAttackStone |
| 78 | 酒醉 | 3 | StatusAttackInebriety |
| 79 | 混乱 | 4 | StatusAttackConfusion |
| 80 | 遗忘 | 5 | StatusAttackForget |
| 259（所属技能1003，技术25900～25909） | 中毒 | 0 | StatusAttackPoisonStrike |

- GBK读取的tech.txt中，上述75～80及1003的TECH_StatusAttack记录均没有MD:覆盖；所属技能9909的技术7589显式MD:0，按技术号换算仍为75，复用StatusAttackPoison，不另造编号。
- 1003在服务器PUK2_SKILL条件下处理暴击／武器相关成功率后落入case75的中毒路径；因此259与75虽默认同为中毒，也保留独立编号。PoisonStrike取自“毒击”的英文命名，不把259当成另一种异常。未启用该条件时，不能将此默认分支视为已启用。
- 类型是默认业务语义，不是不可配置的限制：SK:及套装可改选所属技能，MD:及套装可覆盖异常；MD:255会随机选择0～5。这些语义不因常量命名改变。
- 此处服务器异常type为0～5，客户端普通减益类型为1～6，技能号又是独立编号域；三者不能直接互换或共用宏值。

## 4. 79与94的同名区别

两者在skill.txt均名为“混乱攻击”，但不能归入同一个枚举项，也不是“79格斗、94属性攻击”。

- **79：固定混乱异常攻击。** tech.txt的7900～7951系列使用TECH_StatusAttack，所查该技能记录没有MD:覆盖。服务器tech.c注册到BATTLE_COM_M_STATUSATTACK；battle_event.c的BATTLE_StatusAttackCheck在case79默认选择异常类型4，BATTLE_checkStatusChange的type4读取Confusion／更新混乱状态。SK:／MD:及套装覆盖仍属于原有可配置行为，不能据此新增固定限制。按用户最新命名优先级，候选名由ConfusionAttack调整为StatusAttackConfusion。
- **94：格斗系可学的随机异常攻击。** tech.txt的9400～9458系列使用TECH_PanicAttack；9409显示名“狮子奋迅”。服务器注册到BATTLE_COM_P_PANIC，命中后调用BATTLE_PanicCheck，并把返回的异常类型附加到战斗消息；客户端94另读该类型并选择减益图像。候选名PanicAttack，可注释“格斗混乱攻击／随机异常攻击”。
- **当前服务器的随机集合必须按可达代码解释。** BATTLE_PanicCheck的17439行使用RAND(0,4)，0直接返回；1／2／3／4分别对应睡眠／醉酒／混乱／遗忘。虽保留case5中毒、case6石化，但当前抽取范围不会到达，不能说当前实现随机六种异常。
- **职业证据**：skilllv.txt第38209～38215行给职业141～147配置技能94等级上限；jobs.txt确认141为见习格斗士、142为格斗士、147为拳圣。94同时也配置给士兵等职业，不能命名为“格斗专属”。79在这些格斗职业的表项上限为0，但有超级职等其他非零记录，不能断言仅宠物／怪物能用。
- 这里讨论的是异常状态附加，不是地、水、火、风属性攻击；“属性攻击”若用于指这类技能，容易与属性相克混淆。

## 5. 技术号与技能号不一致的实际样例

下表按用户确认的 `skillId = techId // 100` 决定命名编号，最后一列作为对应分派枚举的取值，不使用第一列的所属技能ID。该命名决定不表示已验证服务器实际发出的技术号；存在显示技术号重写的消费路径仍需按实际报文字段区分。

| data所属技能ID／名称 | TECH_ID样例 | 处理函数 | 采用的skillId（TECH_ID // 100） |
| --- | --- | --- | --- |
| 1001 戒骄戒躁 | 25700 | TECH_DelayAttack | 257 |
| 1002 一击必中 | 25800 | TECH_DelayAttack | 258 |
| 1003 毒击 | 25900 | TECH_StatusAttack（默认中毒） | 259，StatusAttackPoisonStrike |
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
| 9909 G咒术技能 | 7589 | TECH_StatusAttack，MD:0中毒 | 75，StatusAttackPoison |

例如DelayAttack对应257／258、Wattack对应260、Taunt对应105、MonWind对应400；不能分别使用data所属技能号1001／1002、1004、2008、257作为这些分派常量的值。共享处理函数仍按前述命名规则处理，不因取值规则而合并不同编号。

生产技能还存在500／501技术前缀对应不同所属技能的情况，不纳入本次战斗常量替换范围。同一换算编号若对应多种业务，保留各技术记录及原因，不以所属技能名称强行覆盖已有业务语义。

## 6. 未覆盖与后续实施边界

- 本data缺少此前扩展状态映射中的2006、2010、2012、2014、2015、2019、2027、2050～2052、2058～2067、2089～2095和3002～3016。现有CG_BATTLE_EXT_EFFECT_SKILL_*来源宏不能据此改为猜测的业务名称。
- 服务器宏名也需交叉核实：例如94的历史宏名含RANDOMDANCE，但data函数及当前服务器命中处理为PanicAttack；不直接复制历史命名作为业务结论。
- 后续若使用枚举，原始技能域与客户端分派域应明确区分，例如BattleSkillId与BattleSkillDispatchId。只为实际需要替换且证据充分的常量定义成员；原始skillId、Lua注册键、协议字段及跨域存储仍保留有符号整数，不限制自定义ID。
- 普通静态编号优先使用处理函数业务名称，再补已证子类型，中文注释保留data显示名、实际来源和编号域；技术号与所属技能号不一致时按用户确认的techId整数除以100取值。报文重写及同值多业务的消费位置仍需核对，未确认业务名的项保留数字并写明原因。与statusEffectType13DC的状态类型宏保持独立，不因本次技能命名规则改动已经落地的状态宏。
- 本轮仅建立分析依据，不自动创建枚举或修改业务。只做显式编码读取、源码／表格静态核对；未编译、运行客户端、计算摘要或新增测试。
