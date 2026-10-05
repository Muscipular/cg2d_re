# BMQ01 战斗技能50/51/52/53/88共用主体取样

日期：2026-10-06。沿[BMP01技能61共用主体](cg2d_battle_m_skill61_sampling_spec.md)串行推进下一主体；按既有工程委托先SPEC后实施，单既有ProcessBattleMSkill50Family五组/五定义/五处，不扩技能业务或降验收。

<a id="battle-m-skill50-sampling-baseline"></a>
## 原端有界基线

42AE60实际低段表cases50/51/52/53/88→42BEAC；沿[BMO已证有效低case0/202B表](cg2d_battle_m_skill4_sampling_evidence.json)定位，不以raw lowcase共享字段作有效值，也不代全路由。主体42BEAC..42C053完整103条423B两页13call，所有直接目标落本块或已证437A29父回存，见[字节、分支、表及源码前态](cg2d_battle_m_skill50_sampling_evidence.json)。沿BMP已证553E58十二S32偏移48B/既有常量，不改表；父17298统计不代全文验收。

先Take并丢弃mode，再Take/GetPosition保存同S32 position、借actor，**比较actor空先pub casterPosition**，同条件决定Finish直接回存；原只有空角色门禁，不新增255判断/范围拒绝或默认。非空再Take resultFlags/pub，然后每字段重新借当前caster：caster→actor→live skill/level→payload/pub103C/1044，fpUsed1008保原当前角色/固定S32 fpUsed。Append25/55/40/139逐次live caster，调用后live W32 flags比较255；不等255时再读caster一次，同位置作为4393A0 actor与target，flags沿已取值/amount原0。

不论flags是否255，另live caster追加效果-1/0/0终结，再Append64。后live level比较12，再live caster，低段才读553E58[level]+110056，再actor/payload/pub1028；高段原111018保持，不增钳制。末再live caster Move，固定scene payload58递增/父回存，**本主体没有RID消费**；不套用技能4/61尾。三个Take顺序/flags255业务哨兵、每次调用后live角色/字段、各原常数/正常零、原不同空出口/返回和帧子状态保持。

<a id="battle-m-skill50-sampling-decision"></a>
## 五组差异裁决

| 组 | 恢复 |
| --- | --- |
| 空角色条件 | actor查找→空比较→pub casterPosition→同条件，不把比较推到pub后。 |
| skill字段 | live caster→actor→live skill→payload/pub103C，右值不越过actor读取。 |
| level字段 | live caster→actor→live level→payload/pub1044，另借当前角色。 |
| 自身效果 | flags255门禁后一次caster用于actor与target，不额外重读位置或合并后续终结。 |
| 图号 | level比较12→live caster→低段偏移取值→actor/payload/pub；原高段常数保持。 |

五处落码，其它源码/主体/Header/Waiting/五整数、头/布局与正确核心保持；不新增原F/X/adapted/分类/完整成员或owner/公开接口，native非拥有T*只局部借用。网络原格式明文/音频暂停持续，不设新越界/数学溢出/输入拒绝/异常补零/钳制或确定值模型。

<a id="battle-m-skill50-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 103条423B两页13call连续实际字节/全部直接目标闭合；202B低段表逐索引跳槽匹配五case，仅上下文；48B偏移沿已证生产者。 |
| 实现 | HEAD整CPP单根五组五定义五处精确差异，空比较pub/两字段/自身效果同位置/图号取样有限静态；其它正文/头保持。 |
| 消费 | 三Take、原空actor Finish直接回存、flags255门禁/效果终结/64/图号/Move/++58与所有调用后live保持，不新增RID。 |
| 台账 | 旧812仅有界追加，不新身份/adapted/分类/完整成员；双域四局部I/S/七任务open及全部完整门禁和其它指标保持。 |
| 限制 | 必要静态与获准measure/Check，不构建/测试/运行/摘要，不宣编译指令/SDK/SEH/ABI或异常等价。 |

<a id="battle-m-skill50-sampling-source-state"></a>
## 当前状态与后续

五技能共用正常主体五处实修，完整有界103条423B纳管。入口202B仅五case定位，原48B偏移保持；其它各技能/全路由、全部列表消费者和资源owner重进退出/借用失效、完整父与SEH、SDK/ABI/PC53/时钟RNG/画面及整体R1/R7/R8继续，不以有界完整主体代完整函数或整体。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E385→389仅双域四局部I/S。关联率不是完成率，本组14文件；固定快照：`cg2d-re-battle-m-skill50-sampling-20261006`。
