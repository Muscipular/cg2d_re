# BMW01 战斗技能98 mode6目标取样

日期：2026-10-06。沿[BMV01准备段](cg2d_battle_m_skill98_prepare_sampling_spec.md)继续mode6。按既有委托先SPEC后实施，单既有ProcessBattleMSkill98Mode6四组四定义四处；不扩业务或降验收。

<a id="battle-m-skill98-mode6-sampling-baseline"></a>
## 原端有界基线

| 范围 | 指令/字节 | call |
| --- | ---: | ---: |
| mode6主体42D142..42D285 | 76/323 | 8 |
| 空目标失败与效果终结参数尾42D68D..42D6AF | 10/34 | 1 |
| 正常共用消费尾42EB02..42EB32 | 12/48 | 3 |

三范围98条405B三页12call，见[连续实际字节、分支与源码前态](cg2d_battle_m_skill98_mode6_sampling_evidence.json)。全部直接目标落本组或已证437A29父回存，原42D280循环回42D151；没有把模式7/8或其它主体包含进范围。

入口42D0D0..42D142比较链另26条114B一页/0call，仅路由上下文：一次live mode读取，0..5/9..11/18比较转42E80D，mode6比较相等正常落42D142，不等转42D285。十个其它分支只定位，未验其主体；不是跳表，不代全部98模式覆盖。沿BMO已证202B低段表仅定位SKL98，准备段不重复计量；父17298统计不代全文。

<a id="battle-m-skill98-mode6-sampling-decision"></a>
## 四组差异与裁决

| 组 | 原序及恢复 |
| --- | --- |
| 目标 | Take/GetPosition所得S32 target先比较255，再pub同target；非哨兵沿同解析位置查actor/空门禁，不pub后再读全局槽。 |
| 效果 | 目标参数沿刚解析的targetPosition，之后live caster→actor，再4393A0(target,0,0)；不改读live target或跨调用固定caster。 |
| skill字段 | 效果调用后另live target→actor→live skill→payload/pub103C。 |
| level字段 | 另live target→actor→live level→payload/pub1044，不跨两字段复用actor。 |

之后另live target/payload value1018=288，五次Append80/83/68/81/56每次重新借live目标；Append68后reactionImage1010=22404，另live target/payload value101C=168，然后81/56、回下一Take。常数、正常字段宽度和业务范围保持，不误改成byte、不新增去重。

255哨兵直接走原42D696终结参数尾；初始目标actor为空走42D68D Finish后仍进入42D696。末按live caster→actor终结效果(-1,0,0)，原跳42EB02消费已准备参数；再live caster→actor Move→RID(scene)→固定scene payload58递增→437A29。现代CompleteBattleMSkill98Targets正确核心保持，不将失败break改return，不补caller/caster保护、假成功或新的owner逻辑。刚解析目标仅用于当次空门禁/首次效果，效果调用后全部字段和动作另借live目标。

仅四处落码，其它根、wrapper、准备段、共用尾helper、布局/声明和正确核心保持；非拥有native T*仅局部，不新增原F/X/adapted/分类/完整成员或公开接口，不外露SDL/Win32。保正常S32和原业务范围/常数，不增加越界/数学溢出/输入拒绝/异常补零/钳制/确定值模型。网络原格式仅明文/音频暂停保持。

<a id="battle-m-skill98-mode6-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 三范围98条405B三页12call连续字节/全部直接目标闭合；26条114B比较链/202B表只作入口上下文，不计完整主体。 |
| 实现 | HEAD整CPP单根四组四定义四处精确差异，比较pub/同位置空门禁/固定解析target效果与两字段actor先值有限静态，wrapper/准备/尾helper/其它根和头保持。 |
| 消费 | 单处静态Take循环，255或空Finish break均仍走原终结-Move-RID-增58；288/22404/168和80/83/68/81/56、调用后live借用保持。 |
| 台账 | 旧812仅有界追加，不新身份/adapted/分类/完整成员；双域四局部I/S/七open任务/全部完整门禁及其它指标保持。 |
| 限制 | 必要静态与获准measure/Check，不构建/测试/运行/摘要，不宣编译指令/SDK/SEH/ABI、完整98/其它mode或异常等价。 |

<a id="battle-m-skill98-mode6-sampling-source-state"></a>
## 当前状态与后续

mode6四处实修，完整有界主体与其两个实际尾98条405B纳管。mode分派入口比较链仅上下文；状态目标、mode7/8、12/13/14、15/16、17/默认继续按真实原端分组取证开发，不能以现代switch或本步共用尾正确代全部98/所有消费者与owner运行。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E401→405仅双域四局部I/S。其它技能/全路由-完整父-SEH/全部消费者-owner重进退出借用失效/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续；关联率不是完成率，本组14文件，快照`cg2d-re-battle-m-skill98-mode6-sampling-20261006`。
