# BMV01 战斗技能98入口准备取样

日期：2026-10-06。沿[BMU01技能93](cg2d_battle_m_skill93_sampling_spec.md)进入98，按既有工程委托拆分子SPEC并先记录后实施；本步单既有PrepareBattleMSkill98五组/五定义/五处，不代各模式主体或扩大业务。

<a id="battle-m-skill98-prepare-sampling-baseline"></a>
## 原端有界基线

原98入口准备42CF66..42D0D0完整86条362B两页11call，见[实际字节、分支与源码前态](cg2d_battle_m_skill98_prepare_sampling_evidence.json)。所有直接目标落本段或已证437A29父回存；末42D0CB Append64后正常落入42D0D0，**该地址开始live mode读取/分派，不纳入本步正文验收**。沿[BMO低段表](cg2d_battle_m_skill4_sampling_evidence.json)202B/有效低case0定位SKL98；这个表是SKL入口上下文，不是98各mode表。入口定位探针已见42D0D0后的比较链，不把现代switch当原跳表，也不以探针跨入mode6的内容宣主体取证完成。原553E58十二S32偏移48B沿[BMP已证来源](cg2d_battle_m_skill61_sampling_evidence.json)/既有常量保持，父17298统计不代全文。

## 三整数、字段和动作准备

Take并丢弃首mode整数→Take/pub value5A4A34→Take/GetPosition/pub同caster→同解析位置查actor，三Take次序保持；初始空actor Finish直接父回存，不调用模式主体。现代private bool helper仅将同门禁失败返回false，成功在全部原准备动作后返回true，不新增原函数身份/公开接口或假成功，原wrapper及其调用不改。

初始固定caster payload→live skill/pub103C；level字段另live caster→actor→live level→payload/pub1044。mode字段**另live caster→actor→live value5A4A34→payload/pub1048**，不用第三Take之前的初始解析mode或在actor之前读取mode，不跨两个字段复用actor。fpUsed1008另借当时caster/payload，固定S32 fpUsed保持。

Append25/55/39/40每次live caster；调用后live level比较12→live caster→低段原偏移553E58[level]+110056→actor/payload/pub1028，高段原111018保持，不新钳制。之后另live caster Append139/64，返回现代true供wrapper继续；各模式分派从原42D0D0另验。所有原正常失败/返回、字段宽度与业务范围/常数、取包和调用后live不变，准备段不消费目标、RID或推进58。

<a id="battle-m-skill98-prepare-sampling-decision"></a>
## 五组差异裁决

| 组 | 恢复 |
| --- | --- |
| caster | 同解析位置pub和actor查找，不pub后额外重读全局位置。 |
| skill | 固定caster payload先live skill/pub103C。 |
| level | live caster→actor→live level→payload/pub1044。 |
| mode | live caster→actor→live mode→payload/pub1048，与原初始Take/pub区别保持。 |
| 图号 | level比较12后caster先低段偏移读/加法，再actor/payload pub，原高段常数保持。 |

五处落码，其它源/头/布局/主体和正确核心保持；native非拥有T*仅局部借用，不新增原F/X/adapted/分类/完整成员、owner或公开接口。网络原格式仅明文/音频暂停持续；保正常S32和原业务范围，不新增越界/数学溢出/输入拒绝、异常补零/钳制/确定值模型。

## 98子SPEC推进划分

本步完成入口准备的局部实现/静态；剩余按既有现代根组织取证与开发，不以分组名称或路由存在代验收：

| 后续组 | 现代入口与边界 |
| --- | --- |
| 状态目标 | ProcessBattleMSkill98StatusTargets：现代0..5/9..11/18，原分派和完整消费单独核。 |
| mode6/7/8 | 三既有根及各共用尾，按真实原分支/读取差异分别记录。 |
| mode12/13/14 | 三既有根，原参数/状态/依赖与生命周期单独核，不预设新业务。 |
| mode15/16 | 既有DamageTargets参数6/3，两个原路径都核后才沿共用根实施。 |
| mode17/默认 | 原阶段、返回和尾及最终分派覆盖另核，现代default不自动视为原事实。 |

各组依据实际原端范围继续形成具名子SPEC；本步不建空实现/假体，不把98的整个42CF66..42EB32或现代0..18路由宣完整验收。

<a id="battle-m-skill98-prepare-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 86条362B两页11call连续实际字节与直接目标闭合，42D0D0明确正常mode分派边界；SKL202B入口/48B原偏移仅上下文。 |
| 实现 | HEAD整CPP单根五组五定义五处精确差异，三Take/同位置及字段-图号原序有限静态，wrapper/所有其它根和头保持。 |
| 消费 | 首丢弃/第二pub mode/第三caster、空Finish false或六动作后的true、每次live和三字段/正常图号保持，无目标/RID/58新消费。 |
| 台账 | 旧812仅有界追加，不新身份/adapted/分类/完整成员；双域四局部I/S/七任务open、全部完整门禁/其它指标保持。 |
| 限制 | 必要静态与获准measure/Check，不构建/测试/运行/摘要，不宣编译指令/SDK/SEH/ABI、所有98模式或异常等价。 |

<a id="battle-m-skill98-prepare-sampling-source-state"></a>
## 当前状态与后续

98入口准备五处实修，完整有界86条362B纳管；模式分派/各主体从42D0D0按上述真实依赖分组继续。所有其它技能/全路由、完整父与SEH、全部消费者和资源owner重进退出/借用失效、SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体仍待，不以有界准备段代完整98或整体。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E397→401仅双域四局部I/S。关联率不是完成率，本组14文件；固定快照：`cg2d-re-battle-m-skill98-prepare-sampling-20261006`。
