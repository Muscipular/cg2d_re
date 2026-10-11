# BMSA01 LYIm／M／SKL／ITM 逐项静态验收

日期：2026-10-11。依据用户“逐项验收”，本线程恢复这条战斗封包链的静态核查。沿总 SPEC §10；不构建、不测试、不运行客户端或算法样例，不检查编译产物、不计算摘要，不修改 IDA 数据库，不创建源码、指令或原字节 JSON 副本。

当前已推进至下文 BMSA02：第一批分派/调用对照保持，第二批已修退场扫描的消息种类索引宽度。前文“没有源码改动”等结论仅指第一批；完整技能效果、资源、寿命与运行仍未验完。

<a id="battle-m-skill-item-acceptance-scope"></a>
## 范围与判定

逐项核对分成：接收及 M 出队、SKL 编号分派、角色动作分派、ITM 三模式的实际数据消费、等待及完成回收。分派一致只证明目标对应；函数有正文只证明实现存在。下表不把这些结果升级为所有技能效果、字段读取时序、正常失败、资源/别名寿命、SDK/ABI、画面音频或运行通过。

本轮明确完成的静态项：

- SKL：193 个显式编号与原端分派逐项对应，缺接/多接为 0。原五张压缩表的 W8 索引、W32 目标实际读取后，与 IDA 的 case 分组交叉核对；另按原指令纳入表外范围/单值分支。
- 角色：0..256 的 257 个 W32 表槽与当前首个动作调用逐项对应，错接为 0；200 个不同调用目标都有非空源码定义。原表实际 1028B，不把共用目标扩成新原函数。
- ITM：模式 0/6/7 的目标、值、效果及动作消费如下逐项追到；既有 BMM01 解析合同继续有效。未知模式、未知子状态和空目标的原分支保留。
- 没有发现需要立即改动的分派或所查道具下游代码。本轮没有源码改动，没有新增实现/函数身份、关闭任务或完整功能门禁。

<a id="battle-m-skill-item-acceptance-baseline"></a>
## 原端分派依据

原入口为 427470，主体分派从 42AE37 开始；本轮不声称完整覆盖该父的两 chunk、SEH 或全部正文。

| switch 指令 | W32 目标表 / 槽数 | W8 索引表 / 槽数 | 输入低值 | 实际字节 |
|---|---|---|---:|---:|
| 0x42AE60 | 0x437D54 / 21 | 0x437DA8 / 118 | 0 | 202 |
| 0x42FC32 | 0x437E20 / 9 | 0x437E44 / 144 | 262 | 180 |
| 0x431CB3 | 0x437ED4 / 6 | 0x437EEC / 21 | 2002 | 45 |
| 0x432276 | 0x437F04 / 4 | 0x437F14 / 40 | 2028 | 56 |
| 0x434127 | 0x438188 / 4 | 0x438198 / 20 | 3019 | 36 |

五表合 519B。不能把索引槽数当目标槽数。表外依据：42AE37..42AE49 的 257..260；42FBF4..42FC12 的 2027 与 406；43222F..432276 的 2068..2079 和 3002..3016；43282A..432867 的 2089..2095、2096..2101 与 3001；4340FE..434130 的 3100；43579B..4357BA 的 3101、4002、4003。3037 原入口 434130 与同族 43412E 的入参差异继续由源码参数 1/0 承载。

角色分派：404DD5，W32 表 40573C，257 槽/1028B；逐槽实际目标与 case 解码一致。再逐个读取目标处第一条 call/jmp，并与当前角色 switch 的首个调用地址对应。

<a id="battle-m-skill-item-acceptance-groups"></a>
## SKL 执行组

下表是现有源码分组。带两条处理器的入口仍按原 mode 条件选择，不因组号合并分支、读取或完成方式。

| 组号 | 当前处理器 / 源码定义行 | 本轮结论 |
|---|---|---|
| G01 | [ProcessBattleMSkill4](../src/YobiRuntime.cpp#ProcessBattleMSkill4)：16491 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G02 | [ProcessBattleMSkill61Family](../src/YobiRuntime.cpp#ProcessBattleMSkill61Family)：16610 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G03 | [ProcessBattleMSkill50Family](../src/YobiRuntime.cpp#ProcessBattleMSkill50Family)：16821 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G04 | [ProcessBattleMSkill12](../src/YobiRuntime.cpp#ProcessBattleMSkill12)：16866 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G05 | [ProcessBattleMSkill82](../src/YobiRuntime.cpp#ProcessBattleMSkill82)：17002 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G06 | [ProcessBattleMSkill97](../src/YobiRuntime.cpp#ProcessBattleMSkill97)：17064 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G07 | [ProcessBattleMSkill93](../src/YobiRuntime.cpp#ProcessBattleMSkill93)：17115 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G08 | [ProcessBattleMSkill98](../src/YobiRuntime.cpp#ProcessBattleMSkill98)：18030 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G09 | [ProcessBattleMSkill107](../src/YobiRuntime.cpp#ProcessBattleMSkill107)：18081 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G10 | [ProcessBattleMSkill108](../src/YobiRuntime.cpp#ProcessBattleMSkill108)：18129 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G11 | [ProcessBattleMSkill109](../src/YobiRuntime.cpp#ProcessBattleMSkill109)：18227 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G12 | [ProcessBattleMSkill112](../src/YobiRuntime.cpp#ProcessBattleMSkill112)：18327 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G13 | [ProcessBattleMSkill114](../src/YobiRuntime.cpp#ProcessBattleMSkill114)：18416 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G14 | [ProcessBattleMSkill117](../src/YobiRuntime.cpp#ProcessBattleMSkill117)：18496 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G15 | [ProcessBattleMSkill106Family](../src/YobiRuntime.cpp#ProcessBattleMSkill106Family)：19442 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G16 | [ProcessBattleMSkill99Family](../src/YobiRuntime.cpp#ProcessBattleMSkill99Family)：19778 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G17 | [ProcessBattleMSkill263](../src/YobiRuntime.cpp#ProcessBattleMSkill263)：18566 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G18 | [ProcessBattleMSkill400](../src/YobiRuntime.cpp#ProcessBattleMSkill400)：18603 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G19 | [ProcessBattleMSkill401](../src/YobiRuntime.cpp#ProcessBattleMSkill401)：18711 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G20 | [ProcessBattleMSkill402](../src/YobiRuntime.cpp#ProcessBattleMSkill402)：18793 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G21 | [ProcessBattleMSkill405](../src/YobiRuntime.cpp#ProcessBattleMSkill405)：18854 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G22 | [ProcessBattleMSkill404Family](../src/YobiRuntime.cpp#ProcessBattleMSkill404Family)：18976 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G23 | [ProcessBattleMSkill267](../src/YobiRuntime.cpp#ProcessBattleMSkill267)：20697 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G24 | [ProcessBattleMSkill2005Family](../src/YobiRuntime.cpp#ProcessBattleMSkill2005Family)：20982 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G25 | [ProcessBattleMSkill2028](../src/YobiRuntime.cpp#ProcessBattleMSkill2028)：19596 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G26 | [ProcessBattleMSkill3101](../src/YobiRuntime.cpp#ProcessBattleMSkill3101)：21472 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G27 | [ProcessBattleMSkill32Family](../src/YobiRuntime.cpp#ProcessBattleMSkill32Family)：20002 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G28 | [ProcessBattleMSkill19Family](../src/YobiRuntime.cpp#ProcessBattleMSkill19Family)：20301 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G29 | [ProcessBattleMSkill19Family](../src/YobiRuntime.cpp#ProcessBattleMSkill19Family)：20301 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G30 | [ProcessBattleMSkillAttackTargets](../src/YobiRuntime.cpp#ProcessBattleMSkillAttackTargets)：21619；[ProcessBattleMSkill73NonzeroMode](../src/YobiRuntime.cpp#ProcessBattleMSkill73NonzeroMode)：21492 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G31 | [ProcessBattleMSkillAttackTargets](../src/YobiRuntime.cpp#ProcessBattleMSkillAttackTargets)：21619 | 分派对应；各主体完整效果仍按既有子合同继续验收 |
| G32 | [ProcessBattleMSkillAttackTargets](../src/YobiRuntime.cpp#ProcessBattleMSkillAttackTargets)：21619；[ProcessBattleMSkillAttackNonzeroMode](../src/YobiRuntime.cpp#ProcessBattleMSkillAttackNonzeroMode)：21108 | 分派对应；各主体完整效果仍按既有子合同继续验收 |

<a id="battle-m-skill-item-acceptance-skill-rows"></a>
## SKL 逐编号结果

以下每一行的本轮结论均为“分派对应”。主体/子模式的既有取样合同沿 [SKL 入口与等待](cg2d_battle_m_skill_entry_waiting_sampling_spec.md)、[战斗共享合同](ui_windows/shared_battle.md)及各技能子 SPEC；没有给任何编号新增完整业务或运行通过。

| 技能编号 | 原入口块 | 当前执行组 |
|---:|---|---|
| 0 | 0x435B25 | G32 |
| 1 | 0x435B25 | G32 |
| 3 | 0x435B25 | G32 |
| 4 | 0x42B282 | G01 |
| 5 | 0x435B25 | G32 |
| 6 | 0x435B25 | G32 |
| 12 | 0x42C053 | G04 |
| 19 | 0x43412E | G28 |
| 20 | 0x43412E | G28 |
| 21 | 0x43412E | G28 |
| 22 | 0x43412E | G28 |
| 23 | 0x43412E | G28 |
| 24 | 0x43412E | G28 |
| 25 | 0x43412E | G28 |
| 26 | 0x43412E | G28 |
| 27 | 0x43412E | G28 |
| 28 | 0x43412E | G28 |
| 29 | 0x43412E | G28 |
| 30 | 0x43412E | G28 |
| 31 | 0x43412E | G28 |
| 32 | 0x432ECE | G27 |
| 33 | 0x432ECE | G27 |
| 34 | 0x432ECE | G27 |
| 35 | 0x432ECE | G27 |
| 36 | 0x432ECE | G27 |
| 37 | 0x432ECE | G27 |
| 38 | 0x432ECE | G27 |
| 39 | 0x432ECE | G27 |
| 40 | 0x432ECE | G27 |
| 41 | 0x432ECE | G27 |
| 42 | 0x432ECE | G27 |
| 43 | 0x432ECE | G27 |
| 44 | 0x432ECE | G27 |
| 45 | 0x432ECE | G27 |
| 46 | 0x432ECE | G27 |
| 47 | 0x432ECE | G27 |
| 48 | 0x432ECE | G27 |
| 49 | 0x432ECE | G27 |
| 50 | 0x42BEAC | G03 |
| 51 | 0x42BEAC | G03 |
| 52 | 0x42BEAC | G03 |
| 53 | 0x42BEAC | G03 |
| 54 | 0x432ECE | G27 |
| 55 | 0x432ECE | G27 |
| 56 | 0x432ECE | G27 |
| 57 | 0x432ECE | G27 |
| 58 | 0x432ECE | G27 |
| 59 | 0x432ECE | G27 |
| 60 | 0x432ECE | G27 |
| 61 | 0x42B600 | G02 |
| 62 | 0x42B600 | G02 |
| 63 | 0x42B600 | G02 |
| 64 | 0x432ECE | G27 |
| 65 | 0x432ECE | G27 |
| 66 | 0x432ECE | G27 |
| 67 | 0x432ECE | G27 |
| 68 | 0x432ECE | G27 |
| 72 | 0x435B25 | G32 |
| 73 | 0x42AE67 | G30 |
| 75 | 0x435B25 | G32 |
| 76 | 0x435B25 | G32 |
| 77 | 0x435B25 | G32 |
| 78 | 0x435B25 | G32 |
| 79 | 0x435B25 | G32 |
| 80 | 0x435B25 | G32 |
| 81 | 0x435B25 | G32 |
| 82 | 0x42C241 | G05 |
| 83 | 0x42B600 | G02 |
| 84 | 0x435B25 | G32 |
| 85 | 0x435B25 | G32 |
| 86 | 0x432ECE | G27 |
| 87 | 0x435B25 | G32 |
| 88 | 0x42BEAC | G03 |
| 93 | 0x42CB5E | G07 |
| 94 | 0x435B25 | G32 |
| 95 | 0x435B25 | G32 |
| 96 | 0x435B25 | G32 |
| 97 | 0x42C736 | G06 |
| 98 | 0x42CF66 | G08 |
| 99 | 0x432867 | G16 |
| 100 | 0x432867 | G16 |
| 101 | 0x432867 | G16 |
| 102 | 0x432867 | G16 |
| 103 | 0x432867 | G16 |
| 104 | 0x432867 | G16 |
| 105 | 0x435B25 | G32 |
| 106 | 0x431CBA | G15 |
| 107 | 0x42EB32 | G09 |
| 108 | 0x42EC95 | G10 |
| 109 | 0x42F035 | G11 |
| 110 | 0x435B25 | G32 |
| 111 | 0x431CBA | G15 |
| 112 | 0x42F3F2 | G12 |
| 113 | 0x435B25 | G32 |
| 114 | 0x42F739 | G13 |
| 117 | 0x42F9BF | G14 |
| 257 | 0x435B2A | G32 |
| 258 | 0x435B2A | G32 |
| 259 | 0x435B2A | G32 |
| 260 | 0x435B2A | G32 |
| 262 | 0x435B25 | G32 |
| 263 | 0x42FC39 | G17 |
| 266 | 0x435B25 | G32 |
| 267 | 0x434EDF | G23 |
| 400 | 0x42FDCF | G18 |
| 401 | 0x430162 | G19 |
| 402 | 0x430463 | G20 |
| 404 | 0x43097A | G22 |
| 405 | 0x43062E | G21 |
| 406 | 0x43097A | G22 |
| 2002 | 0x431CBA | G15 |
| 2005 | 0x4357BA | G24 |
| 2006 | 0x432ECE | G27 |
| 2007 | 0x435B25 | G32 |
| 2008 | 0x435B25 | G32 |
| 2010 | 0x432ECE | G27 |
| 2011 | 0x432ECE | G27 |
| 2012 | 0x432ECE | G27 |
| 2013 | 0x432ECE | G27 |
| 2014 | 0x432ECE | G27 |
| 2015 | 0x432ECE | G27 |
| 2016 | 0x435B25 | G32 |
| 2017 | 0x435B25 | G32 |
| 2019 | 0x432867 | G16 |
| 2021 | 0x435B25 | G32 |
| 2022 | 0x435B25 | G32 |
| 2027 | 0x432ECE | G27 |
| 2028 | 0x43227D | G25 |
| 2050 | 0x432ECE | G27 |
| 2051 | 0x432ECE | G27 |
| 2052 | 0x432ECE | G27 |
| 2055 | 0x43412E | G28 |
| 2056 | 0x43412E | G28 |
| 2057 | 0x43412E | G28 |
| 2058 | 0x432ECE | G27 |
| 2059 | 0x432ECE | G27 |
| 2060 | 0x432ECE | G27 |
| 2061 | 0x432ECE | G27 |
| 2062 | 0x432ECE | G27 |
| 2063 | 0x432ECE | G27 |
| 2064 | 0x432ECE | G27 |
| 2065 | 0x432ECE | G27 |
| 2066 | 0x432ECE | G27 |
| 2067 | 0x432ECE | G27 |
| 2068 | 0x43412E | G28 |
| 2069 | 0x43412E | G28 |
| 2070 | 0x43412E | G28 |
| 2071 | 0x43412E | G28 |
| 2072 | 0x43412E | G28 |
| 2073 | 0x43412E | G28 |
| 2074 | 0x43412E | G28 |
| 2075 | 0x43412E | G28 |
| 2076 | 0x43412E | G28 |
| 2077 | 0x43412E | G28 |
| 2078 | 0x43412E | G28 |
| 2079 | 0x43412E | G28 |
| 2089 | 0x432ECE | G27 |
| 2090 | 0x432ECE | G27 |
| 2091 | 0x432ECE | G27 |
| 2092 | 0x432ECE | G27 |
| 2093 | 0x432ECE | G27 |
| 2094 | 0x432ECE | G27 |
| 2095 | 0x432ECE | G27 |
| 2096 | 0x435B2A | G32 |
| 2097 | 0x435B2A | G32 |
| 2098 | 0x435B2A | G32 |
| 2099 | 0x435B2A | G32 |
| 2100 | 0x435B2A | G32 |
| 2101 | 0x435B2A | G32 |
| 3001 | 0x432867 | G16 |
| 3002 | 0x432ECE | G27 |
| 3003 | 0x432ECE | G27 |
| 3004 | 0x432ECE | G27 |
| 3005 | 0x432ECE | G27 |
| 3006 | 0x432ECE | G27 |
| 3007 | 0x432ECE | G27 |
| 3008 | 0x432ECE | G27 |
| 3009 | 0x432ECE | G27 |
| 3010 | 0x432ECE | G27 |
| 3011 | 0x432ECE | G27 |
| 3012 | 0x432ECE | G27 |
| 3013 | 0x432ECE | G27 |
| 3014 | 0x432ECE | G27 |
| 3015 | 0x432ECE | G27 |
| 3016 | 0x432ECE | G27 |
| 3019 | 0x435B25 | G32 |
| 3029 | 0x434EDF | G23 |
| 3037 | 0x434130 | G29 |
| 3038 | 0x435B25 | G32 |
| 3100 | 0x43573F | G31 |
| 3101 | 0x43699B | G26 |
| 4002 | 0x435B2A | G32 |
| 4003 | 0x4357BA | G24 |

## 技能 98 的子模式

19 个正常 mode 按现有入口逐个盘点；本轮新取 42D0D0..42D13C 的直接比较链确认状态类分派，其余原块沿对应已存子合同，不声称本轮重验全部主体。

| mode | 原主体入口 | 当前消费者 |
|---|---|---|
| 0、1、2、3、4、5、9、10、11、18 | 42E80D | ProcessBattleMSkill98StatusTargets |
| 6 | 42D142 | ProcessBattleMSkill98Mode6 |
| 7 | 42D28E | ProcessBattleMSkill98Mode7 |
| 8 | 42D6BC | ProcessBattleMSkill98Mode8 |
| 12 | 42D7DA | ProcessBattleMSkill98Mode12 |
| 13 | 42DD1B | ProcessBattleMSkill98Mode13 |
| 14 | 42E01E | ProcessBattleMSkill98Mode14 |
| 15 | 42E1ED | ProcessBattleMSkill98DamageTargets(numberType=6) |
| 16 | 42E444 | ProcessBattleMSkill98DamageTargets(numberType=3) |
| 17 | 42E6A7 | ProcessBattleMSkill98Mode17 |
| 其它 | 原共用消费尾 | 仅施法者 Move、RID、58 增量；不补默认目标或动作 |

state2 的 37 编号沿 BMN01，当前列表仍为 19..31、61..63、83、400..402、2028、2055..2057、2068..2079、3037。最后参与者的推进对象与施法者门禁对象不同；不把所有 SKL 等待态统一为一条 Finish 路径。

<a id="battle-m-skill-item-acceptance-actor-rows"></a>
## 动作消费逐槽结果

以下合并显示相同消费者的槽号，实际比较仍逐槽进行。每行结论仅为“原调用目标与当前调用目标对应，消费者有非空定义”。各消费者自身的完整数值、分支、资源及寿命验收不由本表替代。

| 动作槽 | 原调用目标 | 当前消费者 / 定义行 |
|---|---|---|
| 0 | 0x406650 | [UpdateBattleAction0_406650](../src/YobiRuntime.cpp#UpdateBattleAction0_406650)：24051 |
| 1 | 0x406720 | [UpdateBattleAction1_406720](../src/YobiRuntime.cpp#UpdateBattleAction1_406720)：24089 |
| 2 | 0x406760 | [UpdateBattleAction2_406760](../src/YobiRuntime.cpp#UpdateBattleAction2_406760)：24104 |
| 3 | 0x406C30 | [UpdateBattleAction3_406C30](../src/YobiRuntime.cpp#UpdateBattleAction3_406C30)：24127 |
| 4 | 0x406D70 | [UpdateBattleAction4_406D70](../src/YobiRuntime.cpp#UpdateBattleAction4_406D70)：24146 |
| 5 | 0x40CFA0 | [UpdateBattleAction5_40CFA0](../src/YobiRuntime.cpp#UpdateBattleAction5_40CFA0)：24165 |
| 6 | 0x406CD0 | [UpdateBattleAction6_406CD0](../src/YobiRuntime.cpp#UpdateBattleAction6_406CD0)：24183 |
| 7 | 0x409410 | [UpdateBattleAction7_409410](../src/YobiRuntime.cpp#UpdateBattleAction7_409410)：24204 |
| 8 | 0x4094A0 | [UpdateBattleAction8_4094A0](../src/YobiRuntime.cpp#UpdateBattleAction8_4094A0)：24226 |
| 9 | 0x409510 | [UpdateBattleAction9_409510](../src/YobiRuntime.cpp#UpdateBattleAction9_409510)：24243 |
| 10 | 0x409610 | [UpdateBattleAction10_409610](../src/YobiRuntime.cpp#UpdateBattleAction10_409610)：24275 |
| 11 | 0x4067F0 | [UpdateBattleAction11_4067F0](../src/YobiRuntime.cpp#UpdateBattleAction11_4067F0)：24290 |
| 12 | 0x406930 | [UpdateBattleAction12_406930](../src/YobiRuntime.cpp#UpdateBattleAction12_406930)：24346 |
| 13 | 0x406A80 | [UpdateBattleAction13_406A80](../src/YobiRuntime.cpp#UpdateBattleAction13_406A80)：24389 |
| 14 | 0x409680 | [UpdateBattleAction14_409680](../src/YobiRuntime.cpp#UpdateBattleAction14_409680)：24431 |
| 15 | 0x406E10 | [UpdateBattleAction15_406E10](../src/YobiRuntime.cpp#UpdateBattleAction15_406E10)：25017 |
| 16 | 0x4072E0 | [UpdateBattleAction16_4072E0](../src/YobiRuntime.cpp#UpdateBattleAction16_4072E0)：25239 |
| 17 | 0x4093B0 | [UpdateBattleAction17_4093B0](../src/YobiRuntime.cpp#UpdateBattleAction17_4093B0)：25643 |
| 18 | 0x408170 | [UpdateBattleAction18_408170](../src/YobiRuntime.cpp#UpdateBattleAction18_408170)：25100 |
| 19 | 0x4082F0 | [UpdateBattleAction19_4082F0](../src/YobiRuntime.cpp#UpdateBattleAction19_4082F0)：25141 |
| 20 | 0x408410 | [UpdateBattleAction20_408410](../src/YobiRuntime.cpp#UpdateBattleAction20_408410)：25653 |
| 21 | 0x408750 | [UpdateBattleAction21_408750](../src/YobiRuntime.cpp#UpdateBattleAction21_408750)：25755 |
| 22 | 0x4087C0 | [UpdateBattleAction22_4087C0](../src/YobiRuntime.cpp#UpdateBattleAction22_4087C0)：25772 |
| 23 | 0x4090F0 | [UpdateBattleAction23_4090F0](../src/YobiRuntime.cpp#UpdateBattleAction23_4090F0)：26025 |
| 24 | 0x409380 | [UpdateBattleAction24_409380](../src/YobiRuntime.cpp#UpdateBattleAction24_409380)：26106 |
| 25 | 0x406BA0 | [UpdateBattleAction25_406BA0](../src/YobiRuntime.cpp#UpdateBattleAction25_406BA0)：26118 |
| 26 | 0x40A9E0 | [UpdateBattleAction26_40A9E0](../src/YobiRuntime.cpp#UpdateBattleAction26_40A9E0)：26149 |
| 27 | 0x40AA70 | [UpdateBattleAction27_40AA70](../src/YobiRuntime.cpp#UpdateBattleAction27_40AA70)：26168 |
| 28 | 0x40ACF0 | [UpdateBattleAction28_40ACF0](../src/YobiRuntime.cpp#UpdateBattleAction28_40ACF0)：26253 |
| 29 | 0x40AE50 | [UpdateBattleAction29_40AE50](../src/YobiRuntime.cpp#UpdateBattleAction29_40AE50)：26304 |
| 30 | 0x40AF30 | [UpdateBattleAction30_40AF30](../src/YobiRuntime.cpp#UpdateBattleAction30_40AF30)：26334 |
| 31 | 0x40B020 | [UpdateBattleAction31_40B020](../src/YobiRuntime.cpp#UpdateBattleAction31_40B020)：26364 |
| 32 | 0x40B410 | [UpdateBattleAction32_40B410](../src/YobiRuntime.cpp#UpdateBattleAction32_40B410)：26402 |
| 33 | 0x40B450 | [UpdateBattleAction33_40B450](../src/YobiRuntime.cpp#UpdateBattleAction33_40B450)：26412 |
| 34 | 0x40B5E0 | [UpdateBattleAction34_40B5E0](../src/YobiRuntime.cpp#UpdateBattleAction34_40B5E0)：26422 |
| 35 | 0x40B930 | [UpdateBattleAction35_40B930](../src/YobiRuntime.cpp#UpdateBattleAction35_40B930)：26513 |
| 36 | 0x40BAD0 | [UpdateBattleAction36_40BAD0](../src/YobiRuntime.cpp#UpdateBattleAction36_40BAD0)：26570 |
| 37 | 0x40BB20 | [UpdateBattleAction37_40BB20](../src/YobiRuntime.cpp#UpdateBattleAction37_40BB20)：26585 |
| 38 | 0x40BB70 | [UpdateBattleAction38_40BB70](../src/YobiRuntime.cpp#UpdateBattleAction38_40BB70)：26600 |
| 39 | 0x40BBC0 | [UpdateBattleAction39_40BBC0](../src/YobiRuntime.cpp#UpdateBattleAction39_40BBC0)：26615 |
| 40 | 0x40BD40 | [UpdateBattleAction40_40BD40](../src/YobiRuntime.cpp#UpdateBattleAction40_40BD40)：26666 |
| 41 | 0x40BD60 | [UpdateBattleAction41_40BD60](../src/YobiRuntime.cpp#UpdateBattleAction41_40BD60)：26697 |
| 42 | 0x40BDB0 | [UpdateBattleAction42_40BDB0](../src/YobiRuntime.cpp#UpdateBattleAction42_40BDB0)：26710 |
| 43 | 0x40BE30 | [UpdateBattleAction43_40BE30](../src/YobiRuntime.cpp#UpdateBattleAction43_40BE30)：26721 |
| 44 | 0x40BDF0 | [UpdateBattleAction44_40BDF0](../src/YobiRuntime.cpp#UpdateBattleAction44_40BDF0)：26733 |
| 45 | 0x40B130 | [UpdateBattleAction45_40B130](../src/YobiRuntime.cpp#UpdateBattleAction45_40B130)：26743 |
| 46 | 0x40B200 | [UpdateBattleAction46_40B200](../src/YobiRuntime.cpp#UpdateBattleAction46_40B200)：26760 |
| 47 | 0x40B3F0 | [UpdateBattleAction47_40B3F0](../src/YobiRuntime.cpp#UpdateBattleAction47_40B3F0)：26837 |
| 48 | 0x40B490 | [UpdateBattleAction48_40B490](../src/YobiRuntime.cpp#UpdateBattleAction48_40B490)：26845 |
| 49 | 0x40B5B0 | [UpdateBattleAction49_40B5B0](../src/YobiRuntime.cpp#UpdateBattleAction49_40B5B0)：26886 |
| 50 | 0x40B3C0 | [UpdateBattleAction50_40B3C0](../src/YobiRuntime.cpp#UpdateBattleAction50_40B3C0)：26896 |
| 51 | 0x40BE70 | [UpdateBattleAction51_40BE70](../src/YobiRuntime.cpp#UpdateBattleAction51_40BE70)：26905 |
| 52 | 0x40BE90 | [UpdateBattleAction52_40BE90](../src/YobiRuntime.cpp#UpdateBattleAction52_40BE90)：26913 |
| 53 | 0x40BF20 | [UpdateBattleAction53_40BF20](../src/YobiRuntime.cpp#UpdateBattleAction53_40BF20)：26935 |
| 54 | 0x40BFE0 | [UpdateBattleAction54_40BFE0](../src/YobiRuntime.cpp#UpdateBattleAction54_40BFE0)：26965 |
| 55 | 0x40C0C0 | [UpdateBattleAction55_40C0C0](../src/YobiRuntime.cpp#UpdateBattleAction55_40C0C0)：26997 |
| 56 | 0x40C160 | [UpdateBattleAction56_40C160](../src/YobiRuntime.cpp#UpdateBattleAction56_40C160)：27005 |
| 57 | 0x40C1A0 | [UpdateBattleAction57_40C1A0](../src/YobiRuntime.cpp#UpdateBattleAction57_40C1A0)：27017 |
| 58 | 0x40C1E0 | [UpdateBattleAction58_40C1E0](../src/YobiRuntime.cpp#UpdateBattleAction58_40C1E0)：27028 |
| 59 | 0x40C240 | [UpdateBattleAction59_40C240](../src/YobiRuntime.cpp#UpdateBattleAction59_40C240)：27039 |
| 60 | 0x40C220 | [UpdateBattleAction60_40C220](../src/YobiRuntime.cpp#UpdateBattleAction60_40C220)：27050 |
| 61 | 0x40C280 | [UpdateBattleAction61_40C280](../src/YobiRuntime.cpp#UpdateBattleAction61_40C280)：27057 |
| 62 | 0x40C2B0 | [UpdateBattleAction62_40C2B0](../src/YobiRuntime.cpp#UpdateBattleAction62_40C2B0)：27066 |
| 63 | 0x40C2E0 | [UpdateBattleAction63_40C2E0](../src/YobiRuntime.cpp#UpdateBattleAction63_40C2E0)：27075 |
| 64 | 0x40CBB0 | [UpdateBattleAction64_40CBB0](../src/YobiRuntime.cpp#UpdateBattleAction64_40CBB0)：27106 |
| 65 | 0x40C380 | [UpdateBattleAction65_40C380](../src/YobiRuntime.cpp#UpdateBattleAction65_40C380)：27113 |
| 66 | 0x40C480 | [UpdateBattleAction66_40C480](../src/YobiRuntime.cpp#UpdateBattleAction66_40C480)：27162 |
| 67 | 0x4106B0 | [UpdateBattleAction67_4106B0](../src/YobiRuntime.cpp#UpdateBattleAction67_4106B0)：27174 |
| 68 | 0x40C0E0 | [UpdateBattleAction68_40C0E0](../src/YobiRuntime.cpp#UpdateBattleAction68_40C0E0)：27198 |
| 69 | 0x40C120 | [UpdateBattleAction69_40C120](../src/YobiRuntime.cpp#UpdateBattleAction69_40C120)：27209 |
| 70 | 0x4070E0 | [UpdateBattleAction70_4070E0](../src/YobiRuntime.cpp#UpdateBattleAction70_4070E0)：25179 |
| 71 | 0x40C4D0 | [UpdateBattleAction71_40C4D0](../src/YobiRuntime.cpp#UpdateBattleAction71_40C4D0)：27220 |
| 72 | 0x40CAA0 | [UpdateBattleAction72_40CAA0](../src/YobiRuntime.cpp#UpdateBattleAction72_40CAA0)：27237 |
| 73 | 0x40CAC0 | [AdvanceBattleLinkedCast_40CAC0](../src/YobiRuntime.cpp#AdvanceBattleLinkedCast_40CAC0)：33513 |
| 74 | 0x40CAF0 | [AdvanceBattleEffectAndCast_40CAF0](../src/YobiRuntime.cpp#AdvanceBattleEffectAndCast_40CAF0)：33522 |
| 75 | 0x40CB10 | [UpdateBattleAction75_40CB10](../src/YobiRuntime.cpp#UpdateBattleAction75_40CB10)：27246 |
| 76 | 0x40CB30 | [UpdateBattleAction76_40CB30](../src/YobiRuntime.cpp#UpdateBattleAction76_40CB30)：27254 |
| 77 | 0x40CB50 | [UpdateBattleAction77_40CB50](../src/YobiRuntime.cpp#UpdateBattleAction77_40CB50)：27262 |
| 78 | 0x40CB80 | [UpdateBattleAction78_40CB80](../src/YobiRuntime.cpp#UpdateBattleAction78_40CB80)：27270 |
| 79 | 0x40CBD0 | [UpdateBattleAction79_40CBD0](../src/YobiRuntime.cpp#UpdateBattleAction79_40CBD0)：27278 |
| 80 | 0x40CC80 | [UpdateBattleAction80_40CC80](../src/YobiRuntime.cpp#UpdateBattleAction80_40CC80)：27307 |
| 81 | 0x40CCB0 | [UpdateBattleAction81_40CCB0](../src/YobiRuntime.cpp#UpdateBattleAction81_40CCB0)：27317 |
| 82 | 0x40CCE0 | [UpdateBattleAction82_40CCE0](../src/YobiRuntime.cpp#UpdateBattleAction82_40CCE0)：27327 |
| 83 | 0x40CD10 | [UpdateBattleAction83_40CD10](../src/YobiRuntime.cpp#UpdateBattleAction83_40CD10)：27337 |
| 84 | 0x40CD70 | [UpdateBattleAction84_40CD70](../src/YobiRuntime.cpp#UpdateBattleAction84_40CD70)：27353 |
| 85 | 0x40CDE0 | [UpdateBattleAction85_40CDE0](../src/YobiRuntime.cpp#UpdateBattleAction85_40CDE0)：27373 |
| 86 | 0x40C520 | [UpdateBattleAction86_40C520](../src/YobiRuntime.cpp#UpdateBattleAction86_40C520)：27414 |
| 87 | 0x40C780 | [UpdateBattleAction87_40C780](../src/YobiRuntime.cpp#UpdateBattleAction87_40C780)：27487 |
| 88 | 0x40C8D0 | [UpdateBattleAction88_40C8D0](../src/YobiRuntime.cpp#UpdateBattleAction88_40C8D0)：27527 |
| 89 | 0x40C8F0 | [UpdateBattleAction89_40C8F0](../src/YobiRuntime.cpp#UpdateBattleAction89_40C8F0)：27535 |
| 90 | 0x40C910 | [UpdateBattleAction90_40C910](../src/YobiRuntime.cpp#UpdateBattleAction90_40C910)：27542 |
| 91 | 0x40CF30 | [UpdateBattleAction91_40CF30](../src/YobiRuntime.cpp#UpdateBattleAction91_40CF30)：27591 |
| 92 | 0x40CF50 | [UpdateBattleAction92_40CF50](../src/YobiRuntime.cpp#UpdateBattleAction92_40CF50)：27599 |
| 93 | 0x40CF70 | [UpdateBattleAction93_40CF70](../src/YobiRuntime.cpp#UpdateBattleAction93_40CF70)：27607 |
| 94 | 0x40D010 | [UpdateBattleAction94_40D010](../src/YobiRuntime.cpp#UpdateBattleAction94_40D010)：27618 |
| 95 | 0x40D0B0 | [UpdateBattleAction95_40D0B0](../src/YobiRuntime.cpp#UpdateBattleAction95_40D0B0)：27638 |
| 96 | 0x40D170 | [UpdateBattleAction96_40D170](../src/YobiRuntime.cpp#UpdateBattleAction96_40D170)：27668 |
| 97 | 0x40D1A0 | [UpdateBattleAction97_40D1A0](../src/YobiRuntime.cpp#UpdateBattleAction97_40D1A0)：27677 |
| 98 | 0x40D1D0 | [UpdateBattleAction98_40D1D0](../src/YobiRuntime.cpp#UpdateBattleAction98_40D1D0)：27686 |
| 99 | 0x40D390 | [UpdateBattleAction99_40D390](../src/YobiRuntime.cpp#UpdateBattleAction99_40D390)：27737 |
| 100 | 0x40D3C0 | [UpdateBattleAction100_40D3C0](../src/YobiRuntime.cpp#UpdateBattleAction100_40D3C0)：27747 |
| 101 | 0x40D580 | [UpdateBattleAction101_40D580](../src/YobiRuntime.cpp#UpdateBattleAction101_40D580)：27800 |
| 102 | 0x40D6C0 | [UpdateBattleAction102_40D6C0](../src/YobiRuntime.cpp#UpdateBattleAction102_40D6C0)：27839 |
| 103 | 0x40DBE0 | [UpdateBattleAction103_40DBE0](../src/YobiRuntime.cpp#UpdateBattleAction103_40DBE0)：30288 |
| 104 | 0x40DC90 | [UpdateBattleAction104_40DC90](../src/YobiRuntime.cpp#UpdateBattleAction104_40DC90)：27986 |
| 105 | 0x40DE40 | [UpdateBattleAction105_40DE40](../src/YobiRuntime.cpp#UpdateBattleAction105_40DE40)：28034 |
| 106 | 0x40DF50 | [UpdateBattleAction106_40DF50](../src/YobiRuntime.cpp#UpdateBattleAction106_40DF50)：28076 |
| 107 | 0x40DF70 | [UpdateBattleAction107_40DF70](../src/YobiRuntime.cpp#UpdateBattleAction107_40DF70)：28083 |
| 108 | 0x40DF80 | [UpdateBattleAction108_40DF80](../src/YobiRuntime.cpp#UpdateBattleAction108_40DF80)：28089 |
| 109 | 0x40E4B0 | [UpdateBattleAction109_40E4B0](../src/YobiRuntime.cpp#UpdateBattleAction109_40E4B0)：30313 |
| 110 | 0x40E560 | [UpdateBattleAction110_40E560](../src/YobiRuntime.cpp#UpdateBattleAction110_40E560)：28238 |
| 111 | 0x40E970 | [UpdateBattleAction111_40E970](../src/YobiRuntime.cpp#UpdateBattleAction111_40E970)：28369 |
| 112 | 0x40EB00 | [UpdateBattleAction112_40EB00](../src/YobiRuntime.cpp#UpdateBattleAction112_40EB00)：30339 |
| 113 | 0x40EFE0 | [UpdateBattleAction113_40EFE0](../src/YobiRuntime.cpp#UpdateBattleAction113_40EFE0)：28420 |
| 114 | 0x40F0A0 | [UpdateBattleAction114_40F0A0](../src/YobiRuntime.cpp#UpdateBattleAction114_40F0A0)：28453 |
| 115 | 0x40FB80 | [UpdateBattleAction115_40FB80](../src/YobiRuntime.cpp#UpdateBattleAction115_40FB80)：28637 |
| 116、117、118、119、120、121、122、123、124、125、126、130、131、132、133、134、135、136、137、146、149、150、151、152、153、157、158、161、217、250、252 | 0x40FC10 | [UpdateBattleFactoryAction_40FC10](../src/YobiRuntime.cpp#UpdateBattleFactoryAction_40FC10)：28652 |
| 127、128、129 | 0x40FF90 | [UpdateBattleActions127To129_40FF90](../src/YobiRuntime.cpp#UpdateBattleActions127To129_40FF90)：28702 |
| 138 | 0x40FF40 | [UpdateBattleAction138_40FF40](../src/YobiRuntime.cpp#UpdateBattleAction138_40FF40)：28719 |
| 139 | 0x410000 | [UpdateBattleAction139_410000](../src/YobiRuntime.cpp#UpdateBattleAction139_410000)：28732 |
| 140 | 0x410730 | [UpdateBattleAction140_410730](../src/YobiRuntime.cpp#UpdateBattleAction140_410730)：28944 |
| 141 | 0x410E90 | [UpdateBattleAction141_410E90](../src/YobiRuntime.cpp#UpdateBattleAction141_410E90)：28958 |
| 142 | 0x410F70 | [UpdateBattleAction142_410F70](../src/YobiRuntime.cpp#UpdateBattleAction142_410F70)：28997 |
| 143 | 0x410FC0 | [UpdateBattleAction143_410FC0](../src/YobiRuntime.cpp#UpdateBattleAction143_410FC0)：29008 |
| 144 | 0x411240 | [UpdateBattleAction144_411240](../src/YobiRuntime.cpp#UpdateBattleAction144_411240)：29066 |
| 145 | 0x411290 | [UpdateBattleAction145_411290](../src/YobiRuntime.cpp#UpdateBattleAction145_411290)：29078 |
| 147 | 0x411320 | [UpdateBattleAction147_411320](../src/YobiRuntime.cpp#UpdateBattleAction147_411320)：29096 |
| 148 | 0x40FEF0 | [UpdateBattleAction148_40FEF0](../src/YobiRuntime.cpp#UpdateBattleAction148_40FEF0)：29112 |
| 154 | 0x40F790 | [UpdateBattleAction154_40F790](../src/YobiRuntime.cpp#UpdateBattleAction154_40F790)：29124 |
| 155 | 0x40F800 | [UpdateBattleAction155_40F800](../src/YobiRuntime.cpp#UpdateBattleAction155_40F800)：29139 |
| 156 | 0x40FB30 | [UpdateBattleAction156_40FB30](../src/YobiRuntime.cpp#UpdateBattleAction156_40FB30)：29247 |
| 159 | 0x4107C0 | [UpdateBattleAction159_4107C0](../src/YobiRuntime.cpp#UpdateBattleAction159_4107C0)：29378 |
| 160 | 0x410B00 | [UpdateBattleAction160_410B00](../src/YobiRuntime.cpp#UpdateBattleAction160_410B00)：29384 |
| 162 | 0x410E30 | [UpdateBattleAction162_410E30](../src/YobiRuntime.cpp#UpdateBattleAction162_410E30)：29390 |
| 163 | 0x4113A0 | [UpdateBattleAction163_4113A0](../src/YobiRuntime.cpp#UpdateBattleAction163_4113A0)：29572 |
| 164 | 0x411A50 | [UpdateBattleAction164_411A50](../src/YobiRuntime.cpp#UpdateBattleAction164_411A50)：29779 |
| 165 | 0x411D30 | [UpdateBattleAction165_411D30](../src/YobiRuntime.cpp#UpdateBattleAction165_411D30)：29851 |
| 166 | 0x411CB0 | [UpdateBattleAction166_411CB0](../src/YobiRuntime.cpp#UpdateBattleAction166_411CB0)：29937 |
| 167 | 0x411FA0 | [UpdateBattleAction167_411FA0](../src/YobiRuntime.cpp#UpdateBattleAction167_411FA0)：29953 |
| 168 | 0x411FC0 | [UpdateBattleAction168_411FC0](../src/YobiRuntime.cpp#UpdateBattleAction168_411FC0)：29959 |
| 169 | 0x411FE0 | [UpdateBattleAction169_411FE0](../src/YobiRuntime.cpp#UpdateBattleAction169_411FE0)：29965 |
| 170 | 0x412000 | [UpdateBattleAction170_412000](../src/YobiRuntime.cpp#UpdateBattleAction170_412000)：30008 |
| 171 | 0x412020 | [UpdateBattleAction171_412020](../src/YobiRuntime.cpp#UpdateBattleAction171_412020)：30014 |
| 172、173 | 0x4120F0 | [UpdateBattleActions172_173_4120F0](../src/YobiRuntime.cpp#UpdateBattleActions172_173_4120F0)：30037 |
| 174 | 0x4121D0 | [UpdateBattleAction174_4121D0](../src/YobiRuntime.cpp#UpdateBattleAction174_4121D0)：30061 |
| 175 | 0x412210 | [UpdateBattleAction175_412210](../src/YobiRuntime.cpp#UpdateBattleAction175_412210)：30072 |
| 176 | 0x412260 | [UpdateBattleAction176_412260](../src/YobiRuntime.cpp#UpdateBattleAction176_412260)：30083 |
| 177 | 0x412290 | [UpdateBattleAction177_412290](../src/YobiRuntime.cpp#UpdateBattleAction177_412290)：30094 |
| 178 | 0x4122C0 | [UpdateBattleAction178_4122C0](../src/YobiRuntime.cpp#UpdateBattleAction178_4122C0)：30102 |
| 179 | 0x4122E0 | [UpdateBattleAction179_4122E0](../src/YobiRuntime.cpp#UpdateBattleAction179_4122E0)：30108 |
| 180、181、182、183、184、185、186、187、188、189、190、191、192、193、194、195、196、197、198、199、200、201、202、203 | 0x412310 | [UpdateBattleActions180_203_412310](../src/YobiRuntime.cpp#UpdateBattleActions180_203_412310)：30116 |
| 204 | 0x4124E0 | [UpdateBattleAction204_4124E0](../src/YobiRuntime.cpp#UpdateBattleAction204_4124E0)：30157 |
| 205 | 0x4127B0 | [UpdateBattleAction205_4127B0](../src/YobiRuntime.cpp#UpdateBattleAction205_4127B0)：30258 |
| 206 | 0x4127F0 | [UpdateBattleAction206_4127F0](../src/YobiRuntime.cpp#UpdateBattleAction206_4127F0)：30268 |
| 207 | 0x412830 | [UpdateBattleAction207_412830](../src/YobiRuntime.cpp#UpdateBattleAction207_412830)：30277 |
| 208 | 0x412870 | [SetBattleAction208Target_412870](../src/YobiRuntime.cpp#SetBattleAction208Target_412870)：31835 |
| 209 | 0x412900 | [UpdateBattleAction209_412900](../src/YobiRuntime.cpp#UpdateBattleAction209_412900)：31853 |
| 210 | 0x412F70 | [UpdateBattleAction210_412F70](../src/YobiRuntime.cpp#UpdateBattleAction210_412F70)：32070 |
| 211 | 0x413070 | [ApplyBattleSceneEffects_413070](../src/YobiRuntime.cpp#ApplyBattleSceneEffects_413070)：32092 |
| 212 | 0x413110 | [UpdateBattleAction212_413110](../src/YobiRuntime.cpp#UpdateBattleAction212_413110)：32133 |
| 213 | 0x4131B0 | [UpdateBattleAction213_4131B0](../src/YobiRuntime.cpp#UpdateBattleAction213_4131B0)：32158 |
| 214 | 0x4132B0 | [UpdateBattleAction214_4132B0](../src/YobiRuntime.cpp#UpdateBattleAction214_4132B0)：32203 |
| 215 | 0x4133D0 | [UpdateBattleAction215_4133D0](../src/YobiRuntime.cpp#UpdateBattleAction215_4133D0)：32243 |
| 216 | 0x413400 | [UpdateBattleAction216_413400](../src/YobiRuntime.cpp#UpdateBattleAction216_413400)：32252 |
| 218 | 0x413550 | [UpdateBattleAction218_413550](../src/YobiRuntime.cpp#UpdateBattleAction218_413550)：32307 |
| 219 | 0x413750 | [UpdateBattleAction219_413750](../src/YobiRuntime.cpp#UpdateBattleAction219_413750)：32438 |
| 220 | 0x413980 | [UpdateBattleAction220_413980](../src/YobiRuntime.cpp#UpdateBattleAction220_413980)：32462 |
| 221 | 0x413B40 | [UpdateBattleAction221_413B40](../src/YobiRuntime.cpp#UpdateBattleAction221_413B40)：32470 |
| 222 | 0x413E90 | [UpdateBattleAction222_413E90](../src/YobiRuntime.cpp#UpdateBattleAction222_413E90)：32576 |
| 223 | 0x413FE0 | [UpdateBattleAction223_413FE0](../src/YobiRuntime.cpp#UpdateBattleAction223_413FE0)：32613 |
| 224 | 0x414050 | [UpdateBattleAction224_414050](../src/YobiRuntime.cpp#UpdateBattleAction224_414050)：30606 |
| 225 | 0x4150F0 | [UpdateBattleAction225_4150F0](../src/YobiRuntime.cpp#UpdateBattleAction225_4150F0)：31219 |
| 226 | 0x4154C0 | [UpdateBattleAction226_4154C0](../src/YobiRuntime.cpp#UpdateBattleAction226_4154C0)：30978 |
| 227 | 0x415940 | [UpdateBattleAction227_415940](../src/YobiRuntime.cpp#UpdateBattleAction227_415940)：31070 |
| 228 | 0x416000 | [UpdateBattleAction228_416000](../src/YobiRuntime.cpp#UpdateBattleAction228_416000)：31337 |
| 229 | 0x416020 | [UpdateBattleAction229_416020](../src/YobiRuntime.cpp#UpdateBattleAction229_416020)：31345 |
| 230 | 0x4160A0 | [UpdateBattleAction230_4160A0](../src/YobiRuntime.cpp#UpdateBattleAction230_4160A0)：31360 |
| 231 | 0x416150 | [UpdateBattleAction231_416150](../src/YobiRuntime.cpp#UpdateBattleAction231_416150)：31381 |
| 232 | 0x416220 | [UpdateBattleAction232_416220](../src/YobiRuntime.cpp#UpdateBattleAction232_416220)：31411 |
| 233 | 0x416860 | [UpdateBattleAction233_416860](../src/YobiRuntime.cpp#UpdateBattleAction233_416860)：31587 |
| 234 | 0x416AB0 | [UpdateBattleAction234_416AB0](../src/YobiRuntime.cpp#UpdateBattleAction234_416AB0)：31665 |
| 235 | 0x416DB0 | [ApplyBattleCastEffects_416DB0](../src/YobiRuntime.cpp#ApplyBattleCastEffects_416DB0)：31828 |
| 236 | 0x406630 | [UpdateBattleAction236_406630](../src/YobiRuntime.cpp#UpdateBattleAction236_406630)：29404 |
| 237 | 0x416DD0 | [UpdateBattleAction237_416DD0](../src/YobiRuntime.cpp#UpdateBattleAction237_416DD0)：32634 |
| 238、239 | 0x417410 | [UpdateBattleActions238_239_417410](../src/YobiRuntime.cpp#UpdateBattleActions238_239_417410)：32820 |
| 240 | 0x417AC0 | [UpdateBattleAction240_417AC0](../src/YobiRuntime.cpp#UpdateBattleAction240_417AC0)：33018 |
| 241 | 0x417B80 | [UpdateBattleAction241_417B80](../src/YobiRuntime.cpp#UpdateBattleAction241_417B80)：30471 |
| 242 | 0x417CC0 | [UpdateBattleAction242_417CC0](../src/YobiRuntime.cpp#UpdateBattleAction242_417CC0)：30511 |
| 243 | 0x417D80 | [UpdateBattleAction243_417D80](../src/YobiRuntime.cpp#UpdateBattleAction243_417D80)：33045 |
| 244 | 0x418550 | [UpdateBattleAction244_418550](../src/YobiRuntime.cpp#UpdateBattleAction244_418550)：33244 |
| 245 | 0x418580 | [UpdateBattleAction245_418580](../src/YobiRuntime.cpp#UpdateBattleAction245_418580)：33253 |
| 246 | 0x4185C0 | [UpdateBattleAction246_4185C0](../src/YobiRuntime.cpp#UpdateBattleAction246_4185C0)：33264 |
| 247 | 0x4185F0 | [UpdateBattleAction247_4185F0](../src/YobiRuntime.cpp#UpdateBattleAction247_4185F0)：33273 |
| 248 | 0x418620 | [UpdateBattleAction248_418620](../src/YobiRuntime.cpp#UpdateBattleAction248_418620)：33282 |
| 249 | 0x4071C0 | [UpdateBattleAction249_4071C0](../src/YobiRuntime.cpp#UpdateBattleAction249_4071C0)：25202 |
| 251 | 0x40B180 | [UpdateBattleAction251_40B180](../src/YobiRuntime.cpp#UpdateBattleAction251_40B180)：33302 |
| 253 | 0x40FE50 | [UpdateBattleAction253_40FE50](../src/YobiRuntime.cpp#UpdateBattleAction253_40FE50)：33325 |
| 254 | 0x40FEB0 | [UpdateBattleAction254_40FEB0](../src/YobiRuntime.cpp#UpdateBattleAction254_40FEB0)：33337 |
| 255 | 0x4186B0 | [UpdateBattleAction255_4186B0](../src/YobiRuntime.cpp#UpdateBattleAction255_4186B0)：33349 |
| 256 | 0x418B50 | [UpdateBattleAction256_418B50](../src/YobiRuntime.cpp#UpdateBattleAction256_418B50)：33507 |

<a id="battle-m-skill-item-acceptance-item"></a>
## ITM 三模式的实际后续

| 模式 | 解析/生产 | 下游消费 | 本轮静态结论 |
|---|---|---|---|
| 0 | caster 25/40/138/71；每目标按 kind==7 选 150，否则 149，再 160；写 castSkillLevel1044=0、value1030=当前消息、value1034=5 | 71 遍历 effects1C，Move 各目标并保哨兵后的额外 MoveEffect；149/150 经 40FC10 选效果 type16/type18→41CD00；完成回调 41A160 写父 value1024=2，162 清字段并推进；160 消费 value1030/value1034，数显及资源更新后进入 37 | 解析到数值/特效/等待的实际接线已核。kind7 与其它值、255、空目标及 Finish 后公共尾沿 BMM01，不改提前 return |
| 6 | caster 25/40/138/73/0；写目标 value1030；双方效果记录；目标 0/154/155/156/73/74；值==1 与其它值的 57/58 重复和图号不同 | 154 创建 type24，绑定 41A270 缩放回调，等其 phase1；155 创建 type22，绑定 41A1C0，按 value1030 选择收尾缩放值和各分支声音；完成时将 type24 的 phase 置2；回调缩至 phase3 后，156 标死/清 effect1060/推进 | value1030 在此用于缩放过程的结果分支。两个效果指针、随机/音效调用、phase1/2/3及标死顺序已追到；不把本模式误记成通用数值飘字 |
| 7 | 写 value1018=438，追加 80/205/38/37；无目标/数值解析 | 80 读取 value1018 播放 WAV 音效并推进；205 设 characterAction=16，PlayFrame 严格返回1才清帧并推进；38/37 分别 120/60 次计数等待后推进 | 音效、动画及等待接线已核；不新增该模式没有的目标或数值效果 |
| 非 0/6/7，或非 0/1 子状态 | 原解析/回存分支 | 不新增默认效果或 Finish | 按 BMM01 保留 |

本轮只读重取 40F790、40F800、40FB30、4127B0、41A160、41A1C0、41A270、406630、40CC80 完整反编译，并核缩放比较的局部指令；相关当前源码按字段/分支/调用逐段对照。40FC10 的 30 个类型对应及 410B00 数值路径复用已有完整 [工厂合同](cg2d_battle_factory_batch_exit_sampling_spec.md)和 [资源变化合同](cg2d_battle_resource_copy_spec.md)；41CD00 仅沿已存 [完整工厂合同](cg2d_battle_effect_factory_sampling_spec.md)新核 type16/18/22/24 及 link 公共尾，不能外推其它 type 全部重新通过。

<a id="battle-m-skill-item-acceptance-lifecycle"></a>
## 共有调度、结束和回收

- LYIm 的 M 经四槽队列进入战斗场景；4383B0 绑定 43B360，43B610 绑定 427470；每帧 404C50 调用对象 callback。沿 [事件调度](cg2d_battle_m_event_dispatch_sampling_spec.md)与 [执行门禁](cg2d_battle_event_execution_gates_spec.md)。
- AppendCast 写角色 castListCA4 和 busyFD8；Move 用当前播放下标发布 phase，Advance 增下标，列表结束转 236。236 清 busyFD8、phase=0，再执行待机；原 406630 与当前源码对应。
- ITM state1 仅在 CheckBattleTurnReady(scene,0)==1 时 406260(caster)→Finish。CheckReady 按参与位图检查 busy 和归位条件；SKL 的七种结束及 state2 沿 BMN01，不统一套 ITM 条件。
- Finish 剩余消息>1时继续扫描同事件；最后一条清活动位、注销事件槽、重置参与动作列表并 MarkDeath。后续 404C50 摘链，再 404D10 释放 payload/action。沿 [结束与场景合同](cg2d_battle_event_finish_scene_lifecycle_sampling_spec.md)和 [Yobi 清理合同](cg2d_yobi_cleanup_sampling_spec.md)。
- RenderChara 把活对象 graphicIndex、位置、descriptor 写入既有图形队列；完整像素/动画帧/裁剪/画面及声音结果仍需相应验收，不据入队函数存在关闭这些门禁。

<a id="battle-m-skill-item-acceptance-source-state"></a>
## 尚未关闭的逐项验收

| 剩余项 | 实际状态 |
|---|---|
| 每个 SKL 主体、每个 mode 的全部字段/结果分支/调用时序 | 既有子 SPEC 有界取样可复用；本轮新增的是逐编号路由和逐动作槽目标对照，没有逐项完成所有传递消费者 |
| 全部技能/道具效果的正常创建失败、动画/图形资源、帧事件和音频依赖 | 尚未完整验收；消费者有正文和本表接线对应不代替这些合同 |
| 重进战斗、正常结束、退场、掉线时的全部 owner/别名/借用失效 | 仅所述常规事件/效果标死及回收路径可追，完整寿命未闭 |
| SDK/生成字段访问/机器 ABI、完整父两 chunk/SEH | 未关闭；本轮只读源码和限定原端证据 |
| 当前版本真实收发、同输入/资源/初值、时钟与 RNG、状态/画面/声音回放 | 未执行；此前不构建、不测试、不运行的约束仍有效 |
| 整体 SPEC、完整战斗/网络功能门禁 | 保持开放，不以本表或有限静态通过代替总 SPEC §10 |

此表是本次“逐项验收”的第一批可执行静态结果及剩余列表，不能标作 193 个技能、三个道具模式的完整验收完成。继续按当前网络/战斗优先级处理剩余实际合同；原事实不足的项沿用户“阻塞先跳过”，保留具体未知，不猜语义或擅加保护。

<a id="battle-m-skill-item-acceptance-exit-counter"></a>
## BMSA02：退场扫描的索引宽度与失败传播

2026-10-11，继续第一批验收。退场扫描43BAA0由场景7/9在UpdateAll之后调用，仅消费残余CHT/BST，丢弃其它头；它不负责标死SKL/ITM对象。后续YobiClearUp只标死，实际摘链/释放仍由UpdateAll负责，不能用“调用了清理函数”认定所有借用已失效。

### 已确认差异与实施方案

原43BAC7清EDI、43BAE5递增EDI、43BAF0/43BAF5比较14/20；EDI是32位非地址消息种类计数。当前Sub_43BAA0将kind声明为std::size_t，在x64承载中扩为64位，与本线非地址字段不扩宽的要求不符。恢复std::int32_t kind，并将25项固定数组的size比较转换到同一S32类型；原首命中、未匹配/非14和20继续扫描、字段/字符串读取、调用次序和返回保持。没有新增guard、容量限制、默认值、重试或业务变化，也不声称观察到运行故障或编译指令差异。

此前[BEMS01](cg2d_battle_exit_message_scan_spec.md)的字段、取样、CHT/BST调用及正常计时证据保留；旧“正文正确保持”的结论未覆盖本轮发现的本地索引宽度差异，以本节修正后的源码状态为准，不追认旧记录为完整通过。

### 特效创建失败和父对象借用

原41E3C4的13个W32目标与41E3F8的48个W8选择值已实际读取，全部type0..47映射到既有工厂分支；其13个分组不能当作13种效果或所有消费者已验。原分组：普通35种→41CD2A；13→41D776；17→41E144；19→41E1B9；25/33→41DA04；30→41D831；31→41D891；32→41D8EE；34→41DADB；35→41DC27；36→41D961；43→41E290；47→41D6EB。实际表共100B。

- 通用NewYobiAction的action分配失败返回空；payload分配失败先free已分配action再返回空，尚未接入链表。普通特效创建分支的空返回不执行link公共尾，不会把父value1024写成1。
- 工厂40FC10弃用创建返回值并进入162；创建失败时没有生成完成回调的value1024=2。162只在该字段为2时清字段并推进，不能凭“创建函数返回了”认定事件已经完成。CheckReady已有超过1200计数后重新排11/40的恢复路径，保原行为，不新增失败兜底或回滚。
- 154的type24空返回后仍读取effect1060->phase；type34/43在判空前写deathFlag。原相应访问顺序保持；RPLAN.24对此类空地址异常不专项模拟，不登记故障等价通过或以新增判空掩盖原事实。
- link==1的公共尾写特效的非拥有battleAttachmentParent10，再写父payload.value1024=1；完成回调41A160在帧结果严格1时沿父回写2，再MarkDeath自己。41A1C0只读父deathFlag并按原播放/投影，不能当作完成回写2的回调。正常父借用的完整创建/删除顺序、浅复制及所有分支仍未全部证明。
- 场景进入先YobiClearUp，再InitializeBattleScene清事件/角色表；场景7先退场标志→UpdateAll→残余CHT/BST扫描→清追加链→清角色表，转场成功后才YobiClearUp；场景9不调用清追加链，后续YobiClearUp顺序沿原父。这些是静态调用/标死/回收职责，完整退场、掉线和全部owner/alias仍保持待验。

本步只修Sub_43BAA0一个局部索引及同型比较；所查其它创建、回调、回收和场景正文保持。逐项结果继续为有限静态，所有资源/失败/寿命和运行的剩余门禁不因本节关闭。
