# BME01 战斗 M 冲突判定、事件调度与初始化

日期：2026-10-06；按既有工程委托裁决实施。沿[BMD01完整M分派](cg2d_battle_m_dispatch_sampling_spec.md)的实际43B290调用方及43B360事件回调，恢复两根十三源码片段，补两旧局部记录与初始化原身份，不新建协议/调度/owner框架。

<a id="battle-m-event-dispatch-sampling-baseline"></a>
## 原端完整基线

| 原入口及区间 | 当前符号 | 指令 / 字节 | 原入向 | 出向 |
| --- | --- | ---: | --- | ---: |
| 43B290..43B35A | CheckBattleMParticipants_43B290 | 79 / 202 | 普通call43B4BC，父43B360 | 3 |
| 43B360..43B60A | DispatchBattleEvents_43B360 | 203 / 682 | 4383C7取函数地址绑定，父4383B0；type1/data | 5 |
| 4383F0..438442 | InitializeBattleScene_4383F0 | 20 / 82 | 普通call43CC60，父43CBB0 | 2 |

三单chunk完整302条966B、五指令页、2普通调用/1真实绑定、去重3父及10出向见[原字节和引用](cg2d_battle_m_event_dispatch_sampling_evidence.json)。回调绑定不伪称直接call。旧语义814/815仅查找/相邻28条观察，本步追加完整根和当前源码状态，不覆盖历史或重复身份；初始化已有正确正文保持，只补缺失身份。

## 真实生产与消费合同

43B290从5A8A3C取值发布5AAA60，取头返回1立即返回1且不写输出；25头首命中才调439B30并按原0..21检查非零W8计数：previousSource==*source且index同源则允许，否则共享计数非零立即返回0，先前已写槽/输出/M分派副作用保持，失败前不做mask发布。成功/未知头路径再把本地非零槽OR入调用方mask后返回1。未知头原局部计数未初始化，本步不补零或加拒绝，未知存储/异常结果不宣等价。

43B360仅在(sceneType==10且sceneState按U32>5)或入口exitRequested==1时进入。activeEvents入口一次取样；无活动清pendingGroup，有活动且busy非0返回。退出门禁用同入口exitRequested，busy仍先发布1后消费门禁，exit时sceneState=7；不改成复读exit或active。等待结束时只在原activeEvents=0且4394A0(scene,1)非零：清wait/busy/group、推进入口取样后的回合数、sceneState=2；等待路径无其它调度。

正常调度按32个非拥有事件槽取首空位，原22B共享计数清零，然后逐非空事件固定payload并逐0..21复读A8/递增W8次数。每轮按原previousSource取样、localMask=0、43B290判冲突；非1直接返回。实际439070取头返回1则wait=1返回。已有事件且同source时合并5C/A8并发布预读游标；另仅CHT14/BST20可合并到前事件，并按原var_8(lastCreated)再置previousEvent。其余已知头由43B610建事件；成功才以当前slot形成bit再复读/OR活动mask、重找空槽、固定返回对象payload的5C=1，再发布游标/source/previousEvent。失败把lastCreated置null且保前事件/原已消费头和组界后计数；不加重试/保护/初值或回滚。未知头不创建事件，原groupBoundary末计数路径保持。

初始化4383F0先发布4383B0创建结果（失败null也发布）、清32事件T*及22角色T*表，然后原六W32标量归零，最后调用496570生产快照；当前正确正文保持，不在此加ClearYobi/释放。43CBB0原调用方不是本步完整验收，表非拥有寿命及清Yobi顺序仍按实际父/整体生命周期继续。

<a id="battle-m-event-dispatch-sampling-decision"></a>
## 十三片段的裁决

| 差异类 | 原端 / 实施 |
| --- | --- |
| 两头索引 / 事件slot / 位position | 原EDI/EBP/ECX均W32，现size_t存储扩宽；恢复S32与固定25/32/22容量的同型比较、14/20原常量含义。slot初始/主循环/工厂后循环分别覆盖，不拓宽非地址数据。 |
| 冲突成功mask | 43B2E7先取*mask，E9/EE构bit，再OR/pub；当前compound assignment先算bit，改先currentMask、再bit、最后pub。调度新事件相反为先构bit再读活动mask，原compound assignment正确保持，不抽统一工具抹平差异。 |
| 场景/退出取样 | 43B36F先取sceneType、379取exit，再按同sceneType决定state读取；显式sceneType先保存。43B3B8先保存exit==1条件，3BA busy=1，再消费同条件。 |
| 等待后的回合数 | 43B3FA先取原W32回合，3FF/405/40B三次清零，412递增/415发布；显式uint32 turn在三清零前保存，再按同turn正常+1发布。不引溢出/回绕模型。 |
| 共享参与计数零写 | 43B446/44C/452/458/45E五W32、464末W16；fill替换为五固定4B/末固定2B memcpy零值写，不做未对齐类型解引用，不清额外范围。 |
| 两事件合并字段 | 43B4F0/4F3和43B555/558先读5C与A8，再增/pub5C、OR/pub同已取A8；两个分支显式messages/eventMask先取，原S32/W32保持。 |
| CHT/BST合并var_8 | 43B566形成combinedMask、568取lastCreated、56C pubA8；在对应分支先combinedMask→mergedEvent取样→A8 pub，随后按同mergedEvent更新前事件，不加null限制。 |
| 具名过期注释 | WorldUpdateDependencies.hpp中“43B610创建仍待”已与BEV01实际工厂矛盾，改为“4383F0清表，43B610按当前eventSlot发布”；仅注释，不改声明/共享数组。 |

两根十三片段为源码明示取样、计算/发布次序和保宽恢复，不宣已证运行故障、编译后的指令形态或机器异常等价。正确初始化、所有其它CPP/旧头表/StrStr、BMR/BMS/BMD读器与4394A0/工厂保持；不新增父ABI桥、私有影子状态、RNG调用、释放/owner、保护/补零/钳制/回绕或异常模型。原格式明文和音频暂停持续。

<a id="battle-m-event-dispatch-sampling-acceptance"></a>
## 可度量验收

| 项 | 本步标准 |
| --- | --- |
| 原端覆盖 | 三完整302条966B/连续首尾/单chunk、五页与2普通call/1绑定3父/10出向所有实际字节/字段和取回值一致。 |
| 源码 | 整CPP从HEAD仅两根十三片段；原冲突/输出/合并/工厂失败、取样和各返回保持，正确初始化及其它CPP/公开头不改；共享声明头仅具名注释纠正。 |
| 宽度/接线 | 两S32头索引、slot/position及W8次数/S32消息数/W32mask与回合、非拥有32/22原生T*表保持；真实绑定/初始化/事件工厂消费可追。 |
| 台账 | 旧814/815历史追加完整根，新增初始化一F语义；三F adapted和双域成员共用、不重复语义/文档关联计数；七任务open。 |
| 验证 | 必要静态/获准文本计量/Check；不编译/测试/客户端或算法运行、不计算/比对摘要。 |

<a id="battle-m-event-dispatch-sampling-source-state"></a>
## 当前状态与后续

冲突判定/事件调度两根十三片段落码，正确初始化保持，实际43B360→43B290→439B30预扫描链和4383B0→43B360绑定/43B610创建消费纳管；不代全部3父/所有事件消息及资源消费、owner重进退出/浅别名/借用失效或SDK/ABI/PC53/时钟RNG/画面运行。仅双域四局部I/S，全部七任务与完整门禁和整体目标继续。

语义1348→1349/有效ID1343→1344、adapted451→454，YOBI50→53/BATTLE47→50同三F不双计、E333→337。关联1461→1462/3680=39.73%是关联覆盖率，不是完成率。LOOP31/R7区间67/3061/队列175/60任务及已闭10保持；本组十五文件，固定快照：`cg2d-re-battle-m-event-dispatch-sampling-20261006`。
