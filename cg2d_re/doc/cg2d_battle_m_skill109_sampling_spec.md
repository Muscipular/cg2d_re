# 战斗M技能109取样SPEC（BNE01）

<a id="battle-m-skill109-sampling-baseline"></a>
## 原端基线与范围

本步只修复既有 `ProcessBattleMSkill109`。原主体 `42F035..42F3F2` 246条957B四页30call，真实尾 `436E03..436E17` 五条20B一页1call，两范围同属父427470，合251条977B五页31call。全部直接目标落本组或父默认437A29。

109主体内42F3DA已经Append55，42F3DF另读live caster/actor压参数，再跳436E03的Move；**不跳436DE2重新执行55**。五条Move后缀与[BMM01已有item主体内同区段](cg2d_battle_m_item_combo_sampling_evidence.json)逐字段一致，既有五条20B/1call非新增；本步新246条957B/30call分列。[BMO01实际低段表](cg2d_battle_m_skill4_sampling_evidence.json)的21 W32跳槽84B/118 W8索引118B定位case109→42F035，该202B只作已证上下文，不当本步原指令新增或全部技能验收。

[证据](cg2d_battle_m_skill109_sampling_evidence.json)保存全部实际字节、分页、边界/调用/跳转、三类flags分支和真实Move尾、改前源码/九补丁与旧812完整前态。完整父17298/SEH、其它技能及运行另验；网络原格式明文、音频暂停；显式UTF-8。

<a id="battle-m-skill109-sampling-decision"></a>
## 裁决与九组差异

依既有工程委托先SPEC后实施，单根九组九定义九处，不新增接口、owner、依赖、类型或保护。

| 项 | 改前差异 | 原端顺序与修复 |
|---|---|---|
| 1 | caster发布后又读全局位置 | GetPos返回位置先pub，actor沿同位置 |
| 2 | 初skill值先于payload | 固定caster的payload先取，再live skill |
| 3 | 初level值先于actor | live caster位置、actor、live level、payload |
| 4 | target255比较在pub之后/判空又读位置 | 返回target先比较再pub，同位置actor判空 |
| 5 | flags发布早于target采样，基础skill先于actor | 固定Take flags→live target→flags pub→actor→live skill→payload |
| 6 | target level值先于actor | live target、actor、live level、payload |
| 7 | caster/target比较和0动作另读target | 先live target再caster比较，0沿同target，7/9/10仍各次live caster |
| 8 | 106/57选择与caster actor先后隐含 | live caster、live target比较、沿同caster取actor，再对应动作 |
| 9 | EffectList live实参顺序隐含 | 另live W32 flags、live caster、live target、caster actor，末参数原0 |

第五项原42F111 Take后42F116读target，42F11C才pub flags，42F124取actor，42F12B读skill，42F131取payload。不得提前pub，也不得将该次target冻结给所有后续字段/动作。初caster和loop target的新局部用parsed前缀，保原成功分支casterPosition/targetPosition，避免新增遮蔽。

保全部正确生产/消费：入口两Take首弃/次GetPos，初caster空Finish直接返回；写skill/level/fpUsed，25/151。每target Take/GetPos，255退出，actor空Finish退出；正常再Take flags、两基础字段，caster/target不同才0/7/9/10，随后105。

105之后原42F1CB **只读一次内存W32 flags**；先80000，失败同一EAX再200000，满足后同一AL的bit2。现代固定flags三级选择保持，不把AL测试改称内存W8读取，也不在分支之间添加全局重读。
- 80000成功：caster与target同位置106否则57；另采样caster/target，20且target<10或21且target≥10则183，其余133；所有分支到EffectList。
- 无80000而有200000：bit2成立为target119/68→另live target写reactionImage1010=22329→target50→caster59；bit2不成立为target68→另live target写22329→target32→caster59。
- 两高位均无：caster68→另live caster写reactionImage1010=22331→caster59。
这些分支各动作/字段重新live取对应位置，常量生产和短路选择正确，不新增图号、金额或效果动作。

之后另读live flags-caster-target-actor调用EffectList(caster,target,flags,0)，74，循环；255/空Finish结束后55，再另live caster Move、packetSubState58加一、父默认。原初空直接返回，不消费该尾；原无RID、无额外caster终结EffectList(-1)。全部其它根、分派、字段与正确尾保持；已证地址native非拥有指针，S32/W32等非地址不拓宽。按RPLAN.24不新增补零/钳制/越界溢出、回绕或故障模型，不宣异常或编译后指令形态等价。

<a id="battle-m-skill109-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证明 |
|---|---|
| I | 单根九定义九处落码，整CPP从HEAD精确重放补丁，其它正文/属性和声明保持 |
| S | 两范围251条977B五页31call连续实际字节/全部直接出口，含真实Move后缀；case109实际202B入口和每处原EA |
| 消费 | 原取包/全部调用次数、三级固定W32/EAX/AL flags选择及各次live字段/动作，初空返回/循环Finish与55→Move增58无RID |
| 复用 | BMM既有五条20B/1call非新增，新246条957B/30call单列，不增重复55或额外末EffectList |
| 登记 | 仅旧812五引用/一symbol/本步对象、双域四局部I/S，七open进展/唯一进度同步 |
| 运行和整体 | 不构建/测试/运行/摘要；完整父SEH/所有消费者和接线-owner失效/SDK-ABI-PC53/时钟RNG画面与R1/R7/R8继续 |

E433→437仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60任务已闭10/完整验收0保持。不新F/X/adapted、分类或完整成员，不以本根/三级分支/真实尾或静态汇总代完整技能/父/整体验收。

<a id="battle-m-skill109-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill109`，旧812有界追加，快照 `cg2d-re-battle-m-skill109-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)和[看板](../plan/cg2d_measurement_dashboard.md)同步事实/剩余；不例行改总SPEC/索引或压历史。本步发现的九处正常差异当步修复，继续112等其它技能-全路由/完整父与全部消费/生命周期、运行门禁。
