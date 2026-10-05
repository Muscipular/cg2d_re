# BMJ01 战斗装备与状态消息取样

日期：2026-10-06；按既有工程委托先记录后实施，沿[BMI01转场/宠物消息](cg2d_battle_m_transition_sampling_spec.md)继续427470的六个装备/状态目标。四项差异、七处源码替换，加一条具名过期接线注释更正；全部其它正常/失败和消费序保持。

<a id="battle-m-status-sampling-baseline"></a>
## 原端有界基线

| case / 消息 | 原区间 | 指令 / 字节 | call |
| --- | --- | ---: | ---: |
| 6 / Equipment | 4299AD..429AC2 | 67 / 277 | 10 |
| 9 / StatusValue | 429AC2..429E77 | 222 / 949 | 24 |
| 10 / Debuff | 429E77..429FCD | 84 / 342 | 12 |
| 11 / Power | 429FCD..42A12D | 90 / 352 | 11 |
| 19 / Rebirth | 436FD8..437104 | 85 / 300 | 15 |
| 20 / QueuedValue | 437104..437203 | 69 / 255 | 12 |
| Debuff/Power共用推进尾 | 42998C..4299AD | 8 / 33 | 1 |
| Equipment/StatusValue共用就绪尾 | 437248..43727D | 14 / 53 | 3 |
| 空角色共用完成尾 | 42AAFE..42AB0C | 4 / 14 | 1 |

九个同属427470的有界区间643条2575B、十四指令页/89call，见[实际字节、分支与源码前态](cg2d_battle_m_status_sampling_evidence.json)。前六块617条2475B/84call，必须补三个共用尾才覆盖本组实际推进/就绪/失败结束。Power内429FD9就绪段复用BMI已证字节，43B610真实回调绑定/25头表/父13项回存及正确依赖沿既有合同，不重复取证或验其它目标。17298仍只是含两chunk的父统计。

## 六类消息的真实合同

入口固定payload及一次substate58 S32。除QueuedValue外仅substate1可ready0→live角色406260→Finish，其它非零无消息动作；QueuedValue任何非零substate都进入ready0判断，不错误收窄到1。Equipment/StatusValue经437248共用就绪尾，Debuff/Power/Rebirth/QueuedValue各已有等价原路径；helper返回后父仍回存，不把现代return当原父返回。

Equipment状态0取位置、先角色存在条件再caster pub，空Finish且不再取属性；图号映射后live角色载荷image1014、下一图号映射后live载荷baseImage13AC，各取样独立。随后live追加68、reaction22321/value1018=209分别重取载荷、追加80/66，MoveState后live58增量。

StatusValue先取并发布statusMode、technique，再取位置/存在条件/caster pub；正常才取resultFlags和值。castSkillLevel1044原先caster→角色，再读technique，再取该角色载荷/pub，不能先读全局technique。分派初次statusMode仍一次取样；模式0写1018=273，追加80/159/32，分别live写1030=value/1034=6，原resultFlags **W8**位20决定46/47；模式1追加131/160、1030=value/1034=5；模式2写1018=274，追加80/160、1030=value/1034=3。模式3/4写1018=273、追加80之后**live重读statusMode==4**才写image102C=104216/追加252，随后159、1030=value/1034=6，另一次W8位20决定47/46/38或36。未知模式仍MoveState→ParseRide4239D0→live58增量，不补默认动作或改成缓存第二次mode。

Debuff状态0先pubcaster并借初始角色载荷，再取state/type，随后重新借当前角色做空判断；不移动已有读取或加前置保护。相同debuffState13CC/type13D0不追加；state0追加142后固定原payload写13DC=-1/13D8=0，2/3追加142后固定原payload写13DC=type/13D8=state，再live角色追加141；其它state仅共用MoveState/58增量。固定旧payload与调用后live角色不得统一缓存。

Power先pubcaster/借角色/空Finish，固定该角色payload，第一整数<=0映射mode1否则3，再取type。相同13D8/13DC不追加；变化时追加144，写固定原payload的13DC/13D8，再live角色143，共用MoveState/58增量。原零mode目标的指令仍记录，正常映射只能1/3，现代源码按该正常合同承载，不把原未消费零mode路径或异常当新业务/范围排除，也不据此宣异常等价。

Rebirth先pubcaster再借角色、空Finish；正常以当前已借actor追加40，再取/pub resultFlags，取图号并映射，W32位80000决定是否再取/pub targetPosition，不命中保旧target。随后原读image局部、**先resultFlags再targetPosition**，然后caster/当前角色，AppendEffect439400的16B值语义参数保持{position,flags,0,image}；接着live角色163/64→MoveState→58增量。参数读取顺序恢复，不改字段布局/随机或AppendEffect内部。

QueuedValue状态0先Peek→位置映射/pubcaster→借preview；存在时busyFD8 W32非0或phase W32非0直接返回，不消费后续整数。否则再Take→位置/借actor，存在条件先caster发布；空Finish，正常Take value→live当前角色AppendQueuedTarget→live角色追加166→MoveState→58增量。166实际倒计时消费沿BMG合同，不在此直接扣时间/新增载体写入。

<a id="battle-m-status-sampling-decision"></a>
## 四项差异裁决

| 差异 | 恢复 |
| --- | --- |
| 三角色门禁 | Equipment/StatusValue/QueuedValue均先test actor再caster pub；保存actorPresent，发布后消费同条件，三个源码点。Rebirth/Power及Debuff各自原取样保持，不统一。 |
| 技能等级写入 | 429B24 caster→B2F actor→B36 technique→B3C actor.payload→B3F 1044 pub，显式caster/非拥有skillActor/skillLevel分阶段承载，不跨取包或动作缓存。 |
| 两flags byte读取 | 429BE3/429DE7原AL字节读/bit20门禁，源W32改unsigned char对象字节别名；两个时点独立live取样，不缓存整组结果。 |
| Rebirth效果参数 | 437091先flags、096再target，显式effectFlags后effectPosition，原16B字段序/0/image和之后live caster/角色读取保持。 |

一个既有根四逻辑七处，另将“仍待完整427470接入”注释更正为“已由427470调用并回存，完整战斗另验”；真实六case源码调用已在父，不把接线与全父验收混同。所有其它源、头、共享布局/正确核心保持，不增新原F/X、接口、owner、保护/新零写/异常回绕或故障模型；网络原格式明文与音频暂停继续。

<a id="battle-m-status-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 九区间643条2575B十四页连续实际字节/89call，六目标所有跳转落入本组区间或已证437A29回存；三个共用尾分列。 |
| 实现 | HEAD精确重放单根四逻辑七处及唯一接线注释，原三角色门禁/技能等级/两W8/flags→target取样、各正常/失败/返回与列表/RID/58增量有限静态。 |
| 真实消费 | 保模式3/4动作后的live mode读取、Debuff旧payload/当前角色区别、Power正常1/3和Queued任意非零substate/preview busy-phase，正确依赖保持。 |
| 台账 | 旧812只追加六装备/状态目标，不新身份/adapted/完整成员；仅双域四局部I/S，七任务open。 |
| 验证 | 必要静态/获准文本汇总和Check，不构建/测试/运行/摘要，不宣编译指令或异常等价。 |

<a id="battle-m-status-sampling-source-state"></a>
## 当前状态与后续

四项差异七处落码、具名接线注释纠正，六状态目标和三共用尾有界纳管。完整427470其它消息/技能目标、所有列表状态消费者及资源owner重进退出/借用失效、SDK/SEH/ABI/PC53/时钟RNG/画面继续实际开发验收，本步不称全17298父完成。

语义1350/有效ID1345、片段5/adapted456、YOBI55/BATTLE52/RENDER26/LOOP32及关联1463/3680=39.76%保持；E357→361仅双域四局部I/S。R7区间67/3061、队列175、60任务及已闭10和全部完整门禁保持，关联率不是完成率。本组14文件，固定快照：`cg2d-re-battle-m-status-sampling-20261006`。
