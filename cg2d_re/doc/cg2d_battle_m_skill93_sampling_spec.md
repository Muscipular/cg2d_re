# BMU01 战斗技能93角色重建取样

日期：2026-10-06。沿[BMT01三技能及目标循环](cg2d_battle_m_skill12_82_97_sampling_spec.md)串行推进技能93；98多模式另拆具名子SPEC，未以本步验收。按既有工程委托先SPEC后实施，单既有ProcessBattleMSkill93五组/五定义/五处，不扩业务或降验收。

<a id="battle-m-skill93-sampling-baseline"></a>
## 原端有界基线

| 范围 | 指令/字节 | call |
| --- | ---: | ---: |
| 技能93主体42CB5E..42CF66 | 229/1032 | 37 |
| 正常结束尾432E99..432ECE | 12/53 | 2 |

两范围241条1085B四页39call，完整保存实际字节/分支，见[证据和源码前态](cg2d_battle_m_skill93_sampling_evidence.json)。主体42CF61跳432E99，尾最终437A29，全部直接目标落本组或已证父回存；不把原父437A29的定位探针纳入本步统计或当完整回存新证据。沿[BMO实际低段表](cg2d_battle_m_skill4_sampling_evidence.json)202B/有效低case0定位93，仅入口上下文，不代其它主体/高段路由；父17298统计不代全文。

## 入口与五处差异

Take并丢弃mode→Take/GetPosition/pub同S32 caster→同解析位置actor，空caster Finish直接回存。初始固定caster payload先live skill/pub103C；level字段另live caster→actor→live level→payload/pub1044，fpUsed1008保原读取/固定S32值。Append25/55/68→当前角色reactionImage1010=22338→Append67，每次调用后重新借当前caster。

目标循环Take/GetPosition→先比较255→pub同target→同解析位置查旧actor/MarkDeath，哨兵退出。MarkDeath后**live target→NewBattleActor438290→再live target/pub返回created**，不跨工厂固定旧槽或沿初始target发布，也不增加空actor/工厂失败判断。MarkDeath既有标记死亡行为保持，不改成立即释放；工厂返回含null的原pub/后续访问不补假体或默认。

工厂返回created及其actorPayload固定借用，后续只改原字段，不在各Take之后重借created或payload。nameC4原29容量TakeString→Take image/Sub51B740/pub imageNo→Take/pub field1D0→四次Take/Construct401CC0/Copy438270/Destroy401CF0分别readiness170、maxHp180、barValue1B0、barMaximum1C0→Take/pub payload.flagsFDC。四临时数值载体构造-复制-销毁/原16B内存值语义及丢弃复制返回保持，属于既有内存数值表示，不扩网络加解密；正确核心不改。

stateFE0原W32=0，再fields288四W16槽按3/2/1/0置0；固定actorPayload.flagsFDC逐次原W32读取高位01000000/02000000/04000000/08000000分别槽0/1/2/3置1。castPlayIndexFC4=0/castListIndexFC8=-1、列表首0/次-1及各原正常零保持，不误改这些flags为byte。created固定对象的offset40/44与x/y各次live target→pointIndex→point，不能跨坐标缓存；scaleX/scaleY原F32零，payload.battlePosition18读live target，UpdateBattleAlbumFlag4238F0(created)后再live caster→target→actor用于4393A0(0,0)。此参数时序固定，后当前target payload.value1018=204→Append80/54，各次live目标，继续循环，不新增去重/owner管理。

退出时末效果-1/0/0终结当前caster、另live caster Move、固定scene payload58递增/父回存，**没有RID消费**；初始空caster直接Finish返回，不走此尾。原字符串/十处静态整数取包、四内存载体/各高位测试/W16槽序、调用后live/列表和正常失败全部保持。五处为同解析caster、payload先skill、level字段、target比较pub和同位置MarkDeath、Album调用后caster-target-actor效果参数。

<a id="battle-m-skill93-sampling-decision"></a>
## 五组差异裁决

| 组 | 恢复 |
| --- | --- |
| 初始caster | 同解析位置pub和actor查找，不pub后再读全局位置。 |
| skill字段 | 先固定caster payload，再live skill/pub103C。 |
| level字段 | live caster→actor→live level→payload/pub1044。 |
| 目标 | 255条件先pub、同target查旧actor/MarkDeath；后续工厂两次live target保留。 |
| 效果 | Album调用之后caster-target-actor取样；固定created/payload和后续live target借用区别保持。 |

五处落码，其它主体/源码/头/布局和正确核心保持；native非拥有T*只局部借用，不新增原F/X/adapted/分类/完整成员、公开接口/owner或SDL/Win32对象外露。网络原格式仅明文/音频暂停，不设新越界/数学溢出/输入拒绝/异常补零/钳制或确定值模型。

<a id="battle-m-skill93-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 两范围241条1085B四页39call连续字节/全部直接目标闭合，202B入口映射case93仅上下文。 |
| 实现 | HEAD整CPP单根五组五定义五处精确差异，条件pub/字段/同解析位置/Album后参数序有限静态，其它正文/头保持。 |
| 消费 | 十处TakeInt/一次29容量TakeString、四构造-复制-销毁与固定created/payload、逐次W32高位/W16槽序/坐标live、工厂前后两次target和原末终结-Move增58无RID保持。 |
| 台账 | 旧812仅有界追加，不新身份/adapted/分类/完整成员；双域四局部I/S/七任务open、全部完整门禁及其它指标保持。 |
| 限制 | 必要静态与获准measure/Check，不构建/测试/运行/摘要，不宣编译指令/SDK/SEH/ABI、工厂owner运行或异常等价。 |

<a id="battle-m-skill93-sampling-source-state"></a>
## 当前状态与后续

技能93五处实修，完整正常有界主体和真实结束尾241条1085B纳管。工厂/MarkDeath/四内存载体原消费及正常失败保持，不代所有actor owner/资源寿命运行。98现代入口列出的0..18模式按具名子SPEC逐项取证、开发和验收；其它各技能/全路由、完整父与SEH、全部列表消费者和资源owner重进退出/借用失效、SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体仍待，不以有界完整主体代完整函数或整体。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E393→397仅双域四局部I/S。关联率不是完成率，本组14文件；固定快照：`cg2d-re-battle-m-skill93-sampling-20261006`。
