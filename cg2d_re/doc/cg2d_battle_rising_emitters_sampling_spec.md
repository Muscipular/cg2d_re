# BNY01 上升粒子的两个上游发射回调

<a id="battle-rising-emitters-sampling-baseline"></a>
## 原端与实际差异

[完整证据](cg2d_battle_rising_emitters_sampling_evidence.json)：F:0001A310为41A310..41A528单chunk135条536B两页五call/11内部出口；F:0001A7D0为41A7D0..41A9D9单chunk123条521B两页两call/九内部出口，共258条1057B四页七call/20普通分支/两ret，无switch。两个原库存ID原无独立基线/语义-分类或战斗成员，历史定义存在、不在175定位队列。

随机发射器当前先取Y偏移再读emitter X/计算，原41A4AF取X偏移→41A4B3读X→41A4B6求S32和→41A4BC取Y偏移→F32 X发布。本步恢复这个唯一源码差异；索引发射器正常本体正确，保持。两原回调均由父41CD00在41D991/41DA59写callback槽，引用是地址绑定、不是普通call；父全文不据绑定列表计通过。

两根原各两16项S32局部栈表，共64项256B；当前复用三constexpr表48项192B，两个X表数值相同，RisingY的33/-33与EmitterY对应44/-44不同，不能合并这两个Y表。证据列全部项/栈槽、实际赋值与寄存器来源。共享的[BNX01上升更新](cg2d_battle_rising_emitter_fade_sampling_spec.md)不重验，原分配/设备/RNG与MarkDeath责任沿已有依赖。

<a id="battle-rising-emitters-sampling-decision"></a>
## 裁决与生产消费

先SPEC后修随机发射器一块：显式保存S32 particleX，再取Y偏移、转换并发布F32 X；不把业务和拓宽或变成F64求和。其余两个根/所有caller和原声明保持，只已证地址采用native非拥有T*。字段为原S32 phase/nameX/field58/坐标及偏移，F32位置/加速度，flags W32、混合参数/priority W8、field270 W16。

| 路径 | 完整正常合同 |
|---|---|
| 随机早退/阶段 | 初固定outer parent，deathFlag严格1只标死emitter并返回；否则先phase=old+1，按old<50生产，新phase==40才outer parent flagsOR2 |
| 随机创建 | 位置RNG(0,15)先分配，失败仍消费；成功绑定上升callback，设备真100000、假另RNG(0,1)再读flags选10000/8000。flags分支发布/CC/同值OR40000再pub、100/image110004/两W8值4 |
| 随机坐标/借用 | X偏移→live emitterX/S32和→Y偏移→F32 X发布→live emitterY/F32 Y发布→另live emitterY→0.2→particle父=当前emitter→particle phase=liveY+yOffset-100→清当前emitter field270，不是清particle |
| 随机完成 | 所有非早退路径最终读取emitter field58>=50，fresh outer parent非空/其payload1024非零才写2，随后标死emitter；生成/分配失败路径也到此段 |
| 索引阶段/失败 | phase先old+1；old>0且live nameX<32才phase0/分配；失败也nameX++，无RNG或设备查询，不增nameX下界保护 |
| 索引创建/坐标 | 成功flags先取/callback/CC/仅OR40000/pub/100/image110011/两W8值4；三次独立live nameX按原S32%16取X表、EmitterY表、endY表，两个live emitterY；0.1/particle父=emitter/phase=endY+liveY-100，清emitter W16 |
| 索引完成 | live field58>=32，fresh outer parent非空/1024非零才写2，然后MarkDeath；否则返回。没有随机型初outer parent deathFlag门禁 |

particle→emitter→outer actor为两级非拥有链，不能把particle直接接outer actor；粒子低于阈值给emitter field58加1，发射器到50/32才发布outer 1024状态。MarkDeath只标死，不立即free/摘链，不补owner/释放或缓存outer贯穿完成段。

不改为调用NewBattleRisingEffect_420D50：它的flags、加速度/偏移取样与field270写入对象不同，会改变RNG/字段合同；本步无这样的重构。保原old/new phase门禁、32/50阈值、signed%16及实际业务零/失败/完成1024非零条件，不新增判空、容量/范围保护、补零/钳制/回绕或异常故障模型。两个Y表仅常量基线，不运行算法样例。

新增语义1367/F:0001A310、1368/F:0001A7D0与adapted分类，FUNC-BATTLE成员69→71、完整枚举仍open；索引正确源保持不计开发新增。随机取样问题本步解决，全部上游父/owner/资源/ABI-SDK-PC53-RNG画面与运行另验。

<a id="battle-rising-emitters-sampling-acceptance"></a>
## 可度量验收

| 项 | 本步限定证明 |
|---|---|
| I | 两完整源/整CPP从HEAD仅随机一个补丁精确对照；父层次、阶段/失败/完成1024、清emitter W16与RNG-坐标顺序、三live signed%16静态 |
| S | 两完整258条1057B四页七call/20内部出口-两ret，原四局部数组64S32项256B/对应三源表192B逐项证据，两个父callback绑定类型分列 |
| 登记 | 两独立原ID/分类/成员71，各函数I/S共四E，七open及唯一进度同步，所有旧语义/其它域与完整门禁保持 |
| 度量 | 语义1367→1369/有效ID1362→1364、分类473→475、关联1480→1482/3680=40.27%、E529→533；历史定义已有，175队列保持 |
| 完整门禁 | 不构建/测试/运行/摘要，全部父/consumer-owner/资源重进退出、SDK-ABI-PC53-RNG画面及R1/R7/R8另验，整体未完成 |

YOBI55/RENDER26/LOOP32、片段5/R7区间67/3061、60源码closed10/完整验收0/P01与七open/全部完整门禁保持。正常代码问题已修，不把暂停执行所缺运行证据当代码完成或降低验收标准。

<a id="battle-rising-emitters-sampling-source-state"></a>
## 当前收尾

随机发射器X取样已修，两上游回调补独立基线。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)与[看板](../plan/cg2d_measurement_dashboard.md)同步局部状态，固定快照 `cg2d-re-battle-rising-emitters-sampling-20261006`。网络原格式明文/音频暂停及执行限制保持，沿真实依赖继续。
