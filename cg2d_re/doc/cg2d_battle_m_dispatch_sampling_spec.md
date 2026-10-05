# BMD01 战斗 M 分派、参与计数与特效生产

日期：2026-10-06；按既有工程委托裁决实施。沿[BMS01预扫描](cg2d_battle_m_scan_sampling_spec.md)进入其真实父439B30，核全部消息/技能消费及439920/43BF30依赖，恢复父内三片段，不另造协议或调度框架。

<a id="battle-m-dispatch-sampling-baseline"></a>
## 原端基线

| 原入口及区间 | 当前符号 | 指令 / 字节 | 普通调用 / 父 | 出向 |
| --- | --- | ---: | ---: | ---: |
| 439B30..43AFF7 | ScanBattleMParticipants_439B30 | 1497 / 5319 | 1 / 1，父43B290 | 391 |
| 439920..439B22 | ScanBattleMAttackParticipants_439920 | 165 / 514 | 1 / 1，父439B30 | 30 |
| 43BF30..43BFE6 | TakeBattleMAuxGraphics_43BF30 | 63 / 182 | 39 / 5 | 14 |

三单chunk完整1725条6015B、41普通入向/去重6父、435出向与23指令页见[实际字节/引用](cg2d_battle_m_dispatch_sampling_evidence.json)。25消息头直接目标表加五个压缩技能表，共6逻辑表/11原字节区间/646B，407个输入槽可追；另有原S32范围/单值分支，不能只凭表槽漏掉257..260、405、406、2002/2005..2008、2021/2022、3001/3038、3100/3101、4002/4003。

原压缩表ncases是输入槽数，不是目标表项数；实际目标数20/8/3/5/5，W8索引数118/143/10/75/36，再映射W32目标，原低值0/262/2010/2027/3002。一次候选探查将输入槽数误作目标表长度，已拒绝并以真实两表字节/ind_lowcase/解码组重新取回，错误探查不写证据或台账；不改IDA或权限。

<a id="battle-m-dispatch-sampling-decision"></a>
## 三项差异与实施裁决

| 项 | 原端顺序 | 现有差异 / 实施 |
| --- | --- | --- |
| D1 参与计数清零 | flag=0后439B55/57/5A/5D/60五W32，再439B63末W16；合22B | 当前memset替换为五次固定4B、末次固定2B的memcpy值写，明确原宽度/顺序，不做未对齐类型解引用。原正常零写保持，不新增清理范围。 |
| D2 Attack同源判定 | 43ACB5取目标计数、B8递增、BA保存目标==source，BC发布计数，再分不同字段消费路径 | 当前在目标计数与读取/辅助消费后才判同源。先取/递增W8 targetCount，再保存Attack条件sourceTarget并发布同targetCount，Mode73短路不比较目标/source；后续flags&200只消费同一保存条件。 |
| D3 命名特效门禁 | 43A816先比较skill2002，43A820/826清两S32项，再82C消费保存条件 | 当前两项清零后才比较。先保存namedEffect再依次清两项，随后消费同条件。 |

三项恢复源码明示写入宽度和条件/发布次序，不宣已证运行故障、编译后机器指令形态或异常等价。只有父根三片段；两个正确依赖、现有私有布局分类helper及整个其它CPP/公开头、BMR/BMS读器、原CRT宿主承载保持，不为源码数量重写正确分支。

## 完整消息与技能消费合同

父入口source局部-1、先flag=0后22B清零；所有结束按原先flag后source发布，未知消息不消费、source=-1、flag仍0。SKL先五整数，tech<205000才按S32向零除100覆盖skill；私有GetBattlePreviewSkillLayout只是当前CPP对原父的分组，不造独立原F或X身份。已核25头与全部17布局、Mode98条件链及范围/单值分支；原字段合同沿[已有完整描述](ui_windows/shared_battle.md)落实：

- Attack/Mode73保mode0及技能72/2017/113的439920路径、mode1..4目标消费/辅助8整数和原尾RID；仅Attack同源且flags200额外读位置。Damage/ValuesRide/PairRide保RID；ValuesOnly/PairOnly保无RID，skill68和Mode98 mode11各原额外整数保持。
- SourceValue、NamedTargets、OptionalPair的target!=-1、DirectValues/DirectRecords不丢前缀、Team的0..9/10..19、RideRecords同源不重复计目标而读另一个参与位、EffectRecords同源S32对与名称表生产保持。Mode98全部0..18及未知mode合同、12/13首新source=FF时无RID、14原值source及FF结束保持。
- CLR/SUP循环最终source=FF；CMB首位只计数而最终source=FF；GMN首位只计数/gate0仍source=-1，最后22项增计数；POS先求宠物位再依次计数，MON先source计数再求宠物/计数并保gate字段。CHT原0..21门禁、字符串256、其它原字符串29和所有头的flag0/1、DSK三整数后全计数、END全计数保持。
- RID零模式19整数/2字符串，保与439770试读20整数的差异；非零模式只在flags80000时取字符串29/八整数。不能共用同一字段数量抹平原格式。
- 439920首映射位FF立即返FF且不计数；非FF先两参与者计数、flags/丢弃，skill81多一整数。260/80000、200附带位、94单整数、72/2017+flags1的附加攻击及800000辅助消费和各FF结束保持；后续对记录把currentSkill=-1、仍返回首位，不输出循环末位。
- 43BF30 mode1只消费预览八整数，不访问传入null输出；mode0按对象64读/51B740映射→主两输出→两组firstImage/secondImage/value，读取/发布序保持；其它mode无消费/输出。不新增null/位置/容量或图形保护。

## 共享特效数据与边界

父EffectRecords按原8B步长定位5A8A40[target]的两个S32：先两零写，仅skill2002再读两整数分别写、字符串29及value。5ABCE0按36B步长/5ABFB0边界是20条name32+S32；首空名复制含NUL再写value、首同名只写value、20条全无匹配不写。现有标准库字符串表达式承载原内联字节比较/复制，原父无相应call，不伪造StrCmp/StrCpy原出向或替换成另一原身份。共享World数组和既有消费符号保持，不新增影子副本/容量、owner、清理或初始化。

<a id="battle-m-dispatch-sampling-acceptance"></a>
## 可度量验收

| 项 | 本步标准 |
| --- | --- |
| 原端 | 1725条6015B/连续首尾单chunk、41普通call6父/435出向、23指令页与6逻辑表646B的实际取回值/分母一致。 |
| 源码 | 整CPP仅父三片段；D1五W32/末W16、D2同源条件先pub、D3命名条件先清两S32，两个正确依赖/全部消息与布局合同保持。 |
| 分类/表 | 五W8索引/W32目标表407槽与当前分组逐项一致；所有表外S32范围/单值分支可追。分类helper不新增虚构身份。 |
| 数据 | 原参与W8次数、S32输出/游标/特效对、20*36B名称表与字段数量、失败/终止/发布序保持；不扩协议、补零/钳制/回绕或异常保护。 |
| 追踪/验证 | 三新F/adapted和双域同成员、七任务保持open；仅必要静态/获准文本汇总与Check，不编译/测试/运行/摘要。 |

<a id="battle-m-dispatch-sampling-source-state"></a>
## 当前状态与后续

父三片段修复，全部消息/技能分派和两个正确读取依赖完整原基线纳管；M预览→参与计数/共享特效生产→现有后续消费可追，不代43B290及所有其它6父全文/全部消费资源/owner重进退出/浅别名/借用失效、SDK/ABI/PC53/时钟RNG/画面运行。仅双域四局部I/S，七任务/所有完整门禁和整体目标继续。

语义1345→1348/有效ID1340→1343、adapted448→451，YOBI47→50/BATTLE44→47同三F不双计、E329→333。关联1458→1461/3680=39.70%是关联覆盖率，不是完成率；LOOP31/R7区间67/3061/队列175/60任务及已闭10保持。原格式明文、音频暂停及RPLAN.24持续；固定快照：`cg2d-re-battle-m-dispatch-sampling-20261006`。
