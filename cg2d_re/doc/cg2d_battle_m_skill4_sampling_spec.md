# BMO01 战斗技能4取样

日期：2026-10-06。沿[BMN01技能入口与等待态](cg2d_battle_m_skill_entry_waiting_sampling_spec.md)串行推进正常主体。按既有工程委托先记录后实施，单既有ProcessBattleMSkill4九组/九定义/九处，不扩技能业务或降低整体验收。

<a id="battle-m-skill4-sampling-baseline"></a>
## 原端有界基线

原42AE60低段稀疏表case4→42B282；实际21W32跳槽84B和118W8索引118B共202B，保存真实宽度、低case0、默认437A29和case映射，仅为入口定位上下文，不代表其它主体/高段分派验收。技能4主体42B282..42B600完整256条894B四页26call，全部直接跳转落本块或已证437A29父回存，见[字节、分支、入口表及源码前态](cg2d_battle_m_skill4_sampling_evidence.json)。父17298条统计不是全文验收。

正常初始零来自[BMG01已证入口](cg2d_battle_event_execution_gates_spec.md)：427496 EDI置0后a5/var724两图号及a2/imageNo/a3三二元素S32数组逐槽原W32置0。本根两图号/三数组保留一次初值，循环间沿原值跨目标复用，不新增每轮清理或默认。元数据保存八处零写和其生产者，不把原零当异常补零。

主体按mode Take→caster Take/GetPosition/pub同S32位置/查actor；初始空caster Finish后直接回存，不Move/RID或增58。初始payload取样→live skill/pub103C；下一字段每次live caster→actor→live level→payload/pub1044，再各次live caster/actor/payload pub fpUsed1008和modeFF4。四次Append依25、55、151、70，每次重新借当前caster。

目标循环Take/GetPosition→比较255→pub同位置→同解析位置查actor。255退出；空target Finish后退出循环，随后仍Move当前caster、RID及++58。非空Take flags/pub，再Take amount S32，后live flags800000才43BF30读辅图；无此位保既有图值/数组，不补空amount或默认。43BF30参数角色scene/类型0和三数组/两图号保持，音频暂停。

43BF30后439400参数原firstImage→live flags→live target→live caster→actor，现代先保存这些值再组成既有16B非拥有效果记录/调用。两个图索引原S32，在两元素firstImages中取第一个>0，否则索引0。439400之后43BC10原selectedImage→secondImage→firstImage→live flags→live target→live caster→actor，分别保存原序后调用；两次效果之间不跨调用合并或沿旧caster/flags/target。

Append目标0后live flags原W32入寄存器，AL/AH只局部测试，不能误改全局读取成byte。原108/40、amount及3、8000/4000/2000/20000000各门禁与优先级保持：40先Append34，再live target→player比较，同位置用于256或52，52后51重新live target；165/27先由flags bit2计算kind，再live target→actor调用。各有动作调用后live flags；11后100才76，76后再live flags，20才46。全部重复列表、数值、正常零和空/哨兵不同结束、Move/RID/58及父回存保持。

<a id="battle-m-skill4-sampling-decision"></a>
## 九组差异裁决

| 组 | 恢复 |
| --- | --- |
| 初始caster | 同解析位置用于pub和actor查找，不pub后再读全局位置。 |
| 初始skill | 先借已取caster payload，再读取live skill/pub103C。 |
| skillLevel | live caster→actor→live level→payload/pub1044，不让右值等级读越过actor。 |
| 目标 | 保存255比较后pub同target，再同解析位置查actor；原不同失败出口保持。 |
| 主效果参数 | firstImage→flags→target→caster→actor顺序固定；使用既有记录，不新owner。 |
| 两图索引 | imageIndex/循环index均S32，非地址不得随x64拓成size_t。 |
| 辅效果参数 | selected→second→first→flags→target→caster→actor各次读取固定。 |
| player门禁 | target先player比较，256/52同target，51继续live target。 |
| 165/27 | kind先target/actor，动作之后原live flags保持。 |

九组九处落码，所有其它主体、Header/Waiting/五整数、头/布局和正确核心保持；不新增原F/X/完整分类、native非拥有T*不外露SDL/Win32。网络原格式仅明文、音频暂停、不设新越界/数学溢出或异常确定值模型。

<a id="battle-m-skill4-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 256条894B四页26call连续实际字节与直接目标闭合；入口表202B逐W8索引/W32目标核，case4匹配本块；八原零写沿BMG生产者。 |
| 实现 | HEAD整CPP单根九组九定义九处精确差异；两S32索引、条件pub/参数时序和原消费有限静态，其余源码/声明保持。 |
| 消费 | 初始空caster直接Finish返回、目标空Finish后仍Move/RID/58，255退出、图值跨目标、三数组、不同门禁/优先级/调用后live、重复列表保持。 |
| 台账 | 旧812仅有界追加，不新身份/adapted/分类/完整成员；双域四局部I/S、七任务open、所有完整门禁及其它指标保持。 |
| 限制 | 必要静态与获准measure/Check，不构建/测试/运行/摘要，不宣编译指令/SDK/SEH/ABI或异常等价。 |

<a id="battle-m-skill4-sampling-source-state"></a>
## 当前状态与后续

技能4九组九处实修，完整正常主体256条894B纳管；入口表202B仅路由上下文。各其它技能主体、全部列表状态消费者和资源owner重进退出/借用失效、完整427470父与SEH、SDK/ABI/PC53/时钟RNG/画面和整体R1/R7/R8继续按真实缺口开发验收，不以有界完整主体代完整函数或整体完成。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E377→381仅双域四局部I/S。关联率不是完成率，本组14文件；固定快照：`cg2d-re-battle-m-skill4-sampling-20261006`。
