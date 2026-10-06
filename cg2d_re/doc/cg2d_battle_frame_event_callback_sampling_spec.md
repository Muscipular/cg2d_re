# BOD01 帧事件消费与动画消退回调

<a id="battle-frame-event-callback-sampling-baseline"></a>
## 原端与实际差异

[完整证据](cg2d_battle_frame_event_callback_sampling_evidence.json)覆盖41B440..41BAFB独立单chunk，471条1723B六页38call/56普通内部分支/一ret，无switch。两callback地址引用41DEDA/41DF6D均来自已过[BNZ01共同创建](cg2d_battle_effect_factory_sampling_spec.md)，通过初始化助手传callback；不当普通call或全部consumer/owner已验。

F:0001B440此前无独立语义/分类/战斗成员，历史定义已有、不在175定位队列。当前四处语言级差异：入口payload先phase；负HP临时encoded晚于copyTarget查表；治疗数字style5的Y-80晚应在targetX取样之后；default数字style6同样需显式恢复Y/数值/X后相减。

本根初target查表在initialFlags之前，治疗style5是flags→position3→targetY→targetX→S32 Y-80，均有完整原指令依据；不同于[BOC01两命中回调](cg2d_battle_impact_callbacks_sampling_spec.md)，不照搬其相反取样序。本步四补丁解决所有上述差异，其它全部源码保持。

<a id="battle-frame-event-callback-sampling-decision"></a>
## 本线程裁决与完整合同

按既有用户授权先SPEC后实施：保存initialPhase，负HP保存fresh copyIndex/临时encoded后目标查表并按W32 encoded/W32 mask/W8 readCount发布；两个数字分支各显式取targetY/targetX后正常S32 Y-80，其中治疗金额仍先于targetY。通用CopyPayload/固定内存CryptInt/接口不改，token/reserved保持，原HP上下限是业务规则。

| 阶段 | 完整正常合同 |
|---|---|
| 入口/消费门禁 | 原无action/payload/target新判空。phase0取live当前目标，目标phase非零立即返回，延迟/动画/坐标尾全跳过；否则先取旧frameEvent、phase1，旧事件>=10000才消费。消费前没有PlayFrame或MarkDeath自身 |
| 图号/后续门禁 | 固定初游标/target，随后flags/targetPayload；4选110000、800选110099、400选110098、否则才RNG0..2+110001。406260后live源18写固定target opponent，fresh当前flags写固定targetPayload FE4。必须再读action.frameEvent：fresh<20000且源1038不严格1才进入实际HP分支，不能重用入口事件或改成1038非零都绕过 |
| 实际HP/状态 | 先406390(live源18,action)再fresh resolved；flags8写固定target phase31并直接去清事件，不写本分支FD0。否则70000清且position3非零才可选flags2的110008、主图随机效果、重生效果、fresh金额先当前对象SubtractAssign、fresh对象Less(0)/原零临时和三字段复制/析构；fresh flags20直接写固定targetPayload FDC OR2、全局gate1/alive1，无额外40门禁。随后fresh事件-10000传既有音效接口(320)，音频暂停 |
| 五分类/FD0 | category一次取样，10000→40000→20000→20000000→default优先。前三种Advance固定target后场景phase严格8才payload00++；20000 Advance后fresh10000000只当前对象22459，否则AddAssign/原上限CopyPayload、style5数字、fresh2的22454和属性效果。20000000只Advance/110474视觉效果，无scene计数；default可100000标死固定1054对象、旧FDC清20/地址清零，再Advance及图标或style6数字/22454/属性，无scene计数。实际HP分支非8的所有分类结束都按固定targetPayload FDC bit1写源FD0=14或28，不新增position3非零门禁 |
| 特殊视觉分支 | fresh事件>=20000或1038严格1不406390、不加减HP、不走五分类。fresh flags8仅目标phase31；否则70000清且position3非零才可选2的110008/主图随机效果/重生效果，之后固定target phase26、fresh事件和当前action X决定sound：事件>=20000减20000，否则减10000；音效后固定targetPayload FDC决定FD0。其它情况不写FD0。两消费分支均清W32 frameEvent |
| 延迟/动画 | 所有非入口早退路径old58-1先发布，只有old58<=0才正FD0减一，再fresh重读FD0严格0才Play(1)。严格返回1时图号110140..110151含端点先MarkDeath，仍设备查询/flags OR100000或8000/首次nameY++。之后无论Play结果，fresh oldFade>0再nameY++，oldFade>=20再MarkDeath；同一回调可增两次，阈值依据第二段的旧值，不改死亡后早退 |
| 坐标尾 | 所有非入口早退路径最终X截断→volatile屏幕X偏移→F32 Y取样→正常S32 X发布→Y截断和volatile Y偏移相加发布；原__ftol沿CPH01 host，标死不free/摘链 |

本步保真实事件10000/20000、1038严格1、70000/8/20/2与五分类、原RNG条件/图号范围/HP上下限、nameY/FD0业务0，不新增容量/溢出拒绝、保护、补零、回绕或故障模型；不因RPLAN.24异常不专项处理删业务门禁。action/target/payload/1054与真实void callback沿获准native非拥有地址，其余S32/U32/F32/W32/W8原宽。

旧网络格式明文保持；内存既有CryptInt保持布局/生命周期，不扩展网络加解密。原音效额外Y栈字沿既有52C450说明未被本体读取，现二参接口及暂停范围保；不宣音频验收。语言级取样修复不代编译器实际指令顺序、x87 PC53、原EAX或机器ABI运行证明。

新增语义1380绑定单原ID/adapted分类，FUNC-BATTLE成员82→83，完整枚举仍open；四源码问题本步解决，已过共同创建/初始化/命中父叶不全面复验，Moving/Homing真实运动回调继续开发。

<a id="battle-frame-event-callback-sampling-acceptance"></a>
## 可度量验收

| 项 | 限定依据 |
|---|---|
| I | 单根四补丁/整CPP从HEAD精确重放；原target/flags和治疗金额/Y顺序、fresh事件/1038两分支、HP三字段/上下限、五分类/scene计数/FD0不同门禁、old58/freshFD0/nameY两次增与标死后继续、偏移尾定向静态 |
| S | 完整471条1723B六页38call/56内部分支/一ret，两真实callback地址引用；实际取样/原W32/W8宽度与既有复制/固定布局/volatile偏移声明分列 |
| 追踪 | 单独ID/分类和成员83、局部I/S共两E；七open/唯一进度同步，全部旧记录和完整门禁保持 |
| 度量 | 语义1380→1381/有效ID1375→1376、分类486→487、关联1493→1494/3680=40.60%、E555→557；175定位队列保持 |
| 整体 | 不构建/测试/运行/摘要；全部父-callee-owner/资源-ABI-SDK-PC53-RNG画面及R1/R7/R8另验，整体未完成 |

YOBI55/RENDER26/LOOP32、片段5/R7区间67/3061、60源码closed10/完整验收0/P01与七open/所有完整门禁保持。仅本步正常源码及完整原根局部I/S，不以单根通过代战斗整体或全生命周期验收。

<a id="battle-frame-event-callback-sampling-source-state"></a>
## 当前收尾

帧事件回调四处取样已修，事件消费/HP与视觉分支/动画消退及尾坐标基线独立可追踪。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)/[看板](../plan/cg2d_measurement_dashboard.md)同步，固定快照 `cg2d-re-battle-frame-event-callback-sampling-20261006`。继续沿运动回调缺口开发。
