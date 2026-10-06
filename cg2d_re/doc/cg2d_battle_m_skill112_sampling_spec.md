# 战斗M技能112取样SPEC（BNF01）

<a id="battle-m-skill112-sampling-baseline"></a>
## 原端基线与范围

本步只修复既有 `ProcessBattleMSkill112`。原主体 `42F3F2..42F739` 为218条839B三页28call，255/循环空Finish真实退出到 `42998C..4299AD`：八条33B一页1call。两范围同属父427470，合226条872B四页29call；全部直接目标落本组或父默认437A29。

八条尾与[BMJ01已有状态消息共用尾](cg2d_battle_m_status_sampling_evidence.json)逐字段一致，既有八条33B/1call非新增，本步新218条839B/28call分列。[BMO01实际低段202B表](cg2d_battle_m_skill4_sampling_evidence.json)定位case112→42F3F2；21个W32跳槽84B/118个W8索引118B仅定位上下文，不作本步原指令新增或其它技能验收。

[证据](cg2d_battle_m_skill112_sampling_evidence.json)保存两个范围实际字节/分页、原宽度和选择、调用/跳转、位置切换路径、改前源码/九补丁及旧812完整前态。完整父17298/SEH、其它技能与运行另验；网络原格式明文、音频暂停，显式UTF-8。

<a id="battle-m-skill112-sampling-decision"></a>
## 工程裁决与九处差异

依既有工程委托先SPEC后实施，单根九组九定义九处，不新增接口/owner、依赖、类型或保护。

| 项 | 改前差异 | 原端顺序与修复 |
|---|---|---|
| 1 | caster发布后重读全局位置 | GetPos返回位置先pub，actor沿同位置 |
| 2 | 初skill先于payload读取 | 固定caster的payload先取，再live skill |
| 3 | 初level先于actor读取 | live caster位置、actor、live level、payload |
| 4 | target255比较在pub之后/判空又读位置 | 返回target先比较再pub，同位置actor判空 |
| 5 | 基础target skill先于actor读取 | live target、actor、live skill、payload |
| 6 | 基础target level先于actor读取 | live target、actor、live level、payload |
| 7 | caster位置与current比较/256-52 actor另读位置 | 89后先live caster再current比较，256/52沿同caster；51仍在52后重新live caster |
| 8 | target位置与current比较/256-52 actor另读位置 | 89后先live target再current比较，256/52沿同target；51仍在52后重新live target |
| 9 | 最后EffectList live实参顺序隐含 | 另live W32 flags、live caster、live target、caster actor，原末参数0 |

112原42F51C Take后42F521**先pub flags**，42F526才读target。该原顺序已经正确，保留；不能套用109的target先flags pub裁决。初caster/loop target新局部用parsed前缀。0动作之后42F572一次内存W32 flags，沿EAX/AH测试80000和2000/4000/8000；固定flags逐级选择保持，不将AH测试改称内存W8读取，不在各分支间重读全局flags。

正确生产/消费全部保留：入口两Take首弃/次GetPos，初caster空Finish直接返回；skill/level/fpUsed，再25/55/161/75/109/110，各动作live caster。每target Take/GetPos，255退出，空actor Finish退出；正常再Take flags并pub、两基础字段和target0，再flags选择：
- 无80000：target11。
- 有80000及2000：target121，caster111/37/89，caster位置与current比较；同位置256，非同52后另live caster51。
- 无2000但4000：target122；无2000/4000但8000：target123。
- 三低位均无：target111/37/89，target位置与current比较；同位置256，非同52后另live target51。

之后另live flags-caster-target-actor调用EffectList(caster,target,flags,0)，继续下个target；255/空Finish结束后live caster Move、packetSubState58加一、父默认。原末**无RID、无额外55/74、无caster终结EffectList(-1)**。初空直接返回不消费末尾；不把各次live全部冻结到解析位置或同一actor。

全部其它根/声明/分派和正确尾保持。已证地址native非拥有指针，非地址S32/W32等不拓宽；RPLAN.24不新补零/钳制/越界溢出、回绕或故障模型，不宣异常或编译后指令形态等价。

<a id="battle-m-skill112-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步可接受证明 |
|---|---|
| I | 单根九定义九处落码，整CPP从HEAD精确重放补丁，其它正文/属性与声明保持 |
| S | 两范围226条872B四页29call连续实际字节/全部出口，真实42998C尾、case112原202B表和每处原EA |
| 消费 | 原flags先pub/target后读、0后一次W32/EAX-AH逐级选择；两个位置比较/256-52固定位置和52后51重读、原全部取包/动作和失败/Move增58 |
| 复用 | BMJ既有尾八条33B/1call非新增，新218条839B/28call分列，不增RID或额外55/74/末EffectList |
| 登记 | 仅旧812五引用/一symbol/本步对象、双域四局部I/S，七open进展/唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH/所有消费者和接线-owner失效/SDK-ABI-PC53/时钟RNG画面及R1/R7/R8继续 |

E437→441仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60任务已闭10/完整验收0保持。不新F/X/adapted、分类或完整成员，不以本根/位选择/共享尾或静态汇总代完整父/功能/整体验收。

<a id="battle-m-skill112-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill112`，旧812有界追加，快照 `cg2d-re-battle-m-skill112-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)和[看板](../plan/cg2d_measurement_dashboard.md)同步事实与剩余，不例行改总SPEC/索引或压历史。本步发现的九处正常差异当步修复，继续114等其它技能-全路由/完整父、全部消费/生命周期和运行门禁。
