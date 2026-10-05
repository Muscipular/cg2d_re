# BAR01 战斗退场清槽与装备/状态恢复取样 SPEC

<a id="battle-attachment-recovery-baseline"></a>
## 原基线与实际差异

沿[BAP01附件生产/发布](cg2d_battle_attachment_producer_spec.md#battle-attachment-producer-source-state)推进退出与恢复；[完整状态恢复、父元数据/八片段及源差异](cg2d_battle_attachment_recovery_evidence.json)分列保存，不把局部窗当完整父验收。

422370完整[422370,4225D0)177条608B、单chunk；7普通call来自414050三次、4154C0两次、415940两次。固定入口payload/FDC快照决定所有选择，首10清位用入口快照，其余原live FDC读改写、六debuff/附着/反转/六status与成对共用清位掩码保持；不能把8000/20000/80000失败分支分别改成清自身位。原422451按1000选3FA66666/3F800000，422460先判400，再422463/422466发布两轴scale，之后按先前判定创建或当前parent.payload附着槽标死/重新取载荷清槽；源400判定在scale发布之后。

三父仅本组退出/恢复窗：
| 父 | 原完整元数据（未据此核完整正文） | 七槽清理窗 | 装备恢复窗 |
|---|---|---|---|
| 414050 | [414050,4150EB)，1136条4251B | [414399,41444E)52/181B；[41445C,41450B)49/175B；[414C75,414D24)49/175B | [414FA1,41504A)51/169B |
| 4154C0 | [4154C0,415939)，277条1145B | [415555,415620)49/203B | [41578A,415863)51/217B |
| 415940 | [415940,415FF2)，389条1714B | [415C01,415CCF)50/206B | [415E6F,415F4E)50/223B |

八窗401条1549B，加完整状态根共578条2157B；父元数据1802条7110B另外列，不当本次全部已核。三父入向各为404DB0一个call；与状态根7call合10普通入向/4已知父，不混原窗内所有callee次数或helper函数数量。

五清理窗确为同一七槽次序105C→1054→1068→1058→13B4→13B8→13BC：各取一次供判空/MarkDeath，调用后清同槽，再取下一槽；固定该窗payload，不预取全部指针。第一次空值基准EDI在4143AC/4143B6分路置0；414C6A已将ESI清0、415543已将EDX清0，另两前置窗16条66B单列证明比较基准，不猜寄存器初值。源ClearBattleRideAttachments判空后又取槽作参数，增加读取。

三装备恢复窗都取当前对象/载荷/gear一次供判空和MarkDeath；调用后重新读当前对象/载荷清gear，之后取carrier.gearImage0C，正值才NewBattleGear(current,image)，创建后再读当前对象/载荷pub返回值；再重新读对象/载荷/aux0，消费后重新读以清槽，再取aux1同样处理。源RestoreBattleRideGear已保这些global slot/payload跨调用复读，但各附件判空后又读取参数，不能以入口parent/payload一次缓存整个恢复路径。

<a id="battle-attachment-recovery-decision"></a>
## 裁决与实施边界

依AGENTS.md第8条委托采用，只src/YobiRuntime.cpp三个既有根十一片段：
- 422370在选scale后先保存hasAttachedEffect=(入口flags&400)!=0，再两轴pub，按同判定进入原创建/标死分支；原所有flags/特殊共用清位掩码/子调用和当前载荷复读保持，不新guard或改失败。
- ClearBattleRideAttachments七个原生非拥有YobiAction*分别在各原阶段取一次并供原判空/MarkDeath，原清槽后才取下一槽。保持七槽顺序/固定payload；不清rider13C8、effect1060或types13C0/13C4，不立即free，不预取数组或循环合并。
- RestoreBattleRideGear仅gear、aux0、aux1各取一次供判空/MarkDeath；原调用后*slot/payload重新读取和清槽、carrier图号读取时点、正值创建/发布后以及两个aux之间的所有对象/载荷复读保持，不复用已经过调用的parent/payload或新增guard。返回失败原仍pubnullptr，不回滚或新fallback。
- 两helper仅CPP适配共用实现，无独立原函数身份，不造F/X库存或新接口；对应本次五清理/三恢复窗，当前三父调用点/其它正文一字不改，不因helper名字覆盖所有退场或全部owner寿命。

正常原指针仅按native T*/T**拓宽，非地址字段宽度/入口FDC及F32 bitcast值保持。MarkDeath仍只标死/按原槽写清，实际回收沿Yobi链；原父/载荷/创建失败合同和音频调用保持，不加Read/Ctor/Destroy/RNG、分配释放或处理数学溢出/越界、保护/回绕/故障模拟。网络原格式明文及音频暂停继续。

<a id="battle-attachment-recovery-acceptance"></a>
## 可度量验收

1. 一完整177条608B及三父实际元数据/八窗401条1549B分列，10普通入向/4父可追；578条2157B主体观察范围与16条66B比较前置窗另列；父八窗以外1401条5561B含这两前置窗，三父其余1385条5495B未核。原窗边界均从实际instruction head取，不从指令中间截窗。
2. 全CPP从HEAD只能精确重放三根十一片段；scale选择→400判定→两轴pub、七槽各单次快照及逐槽消费/清理、装备三指针取样与所有原global/payload复读成立；三父本体/五与三实际helper调用/四已有入口声明、其他源/头/汇编保持。
3. 一完整状态根/三父本组片段身份纳管，语义1320→1324/有效ID1315→1319、代码片段5保持；四adapted422→426。YOBI21→25/BATTLE18→22同四F不双计，E293→297仅双域局部I/S共用同十一片段/限定原证据，LOOP31/R7区间67/队列和全部完整门禁保持。
4. U08/U09/G01/G02/R01/R04/V04以及R1/R7/R8/整体open；完整三父/所有动作与参数、数据资源生产和全部owner重进退出/浅别名/借用失效、SDK/ABI/PC53/时钟RNG/画面运行仍需相应证据，不借完整元数据或部分范围冒充已验收。
5. 只必要集中静态/获准文本汇总/-Check/引用/UTF-8无BOM/LF及精确提交，不构建/测试/算法样例/客户端/摘要、不全面复查或例行改总SPEC/索引及压历史。

<a id="battle-attachment-recovery-source-state"></a>
## 源状态与剩余

RestoreBattleStateEffects_422370已有公开声明；两个CPP helper继续被414050/4154C0/415940的五/三实际表达式消费。父本体未改，仅helper依赖修正，原入/出阶段顺序与四入口声明不变，不增加对外节点布局或独立“清理成功”入口。

全部三个完整父、动作/实参/数据资源与owner寿命、SDK/ABI/PC53/时钟RNG/实际画面未闭；局部源形式不证明本机逐指令/原异常重叠或并发等价。固定快照`cg2d-re-battle-attachment-recovery-20261006`。
