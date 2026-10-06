# 战斗成对效果/距离状态取样SPEC（BQA02）

日期：2026-10-07。按工程委托先记裁决再实施，上承战斗收尾、字段/别名与追踪验收体系。

<a id="battle-effect-pair-distance-sampling-baseline"></a>
## 原端完整基线与缺口

| 原入口 | 当前源符号 | 条数/字节/页 | call/jump/ret |
| --- | --- | --- | --- |
| 0x00413B40 | UpdateBattleAction221_413B40 | 248/837/4 | 23/21/1 |
| 0x00413E90 | UpdateBattleAction222_413E90 | 98/332/2 | 11/7/2 |

两根346条1169B/六页、34call/21入口/28普通内部jump/三ret，无switch；两唯一404DB0入向六条实参-弃返邻窗。

221四实际TEST BYTE消费在413CCA、413DA2、413E15、413E4A：恢复数显后bit2/同hitIndex扣减命中bit2/扣减数显后bit2/扣减共同尾bit20。源W32表达式恢复unsigned byte对象表示，共享字段仍W32，所有写者和初始/后续W32 image/damage缓存保持；寄存器AL/AH后用不能误当内存W8。

**明确保留原接收者**：扣target.readiness比较<0后的413D5E/62/66先取临时mask/encoded/readCount，413D6A/74/7A写原EBP即source action.readiness，原确实清源HP，不改为target；恢复数显后图22454接target，扣减数显后413E24 push EBP接action，不改目标。现有代码/齐读阶段/原顺序已正确，不按直觉改业务。

222四原F32常数54D438/54D424/54D428/54D48C字节00003443/0000b443/000034c3/0000003f，对应180/360/-180/0.5；原先FCOM比较再FST发布，第二阈值还用未落位中间，现double中间保。实际x87/生成机器未验，数学/异常不专项模拟。

<a id="battle-effect-pair-distance-sampling-decision"></a>
## 实施裁决与完整正常合同

只221四低W8读取片段，各独立原阶段显式取当前flags；413DA2沿已缓存hitIndex，另三阶段fresh currentIndex不合并。其它原宽度、所有HP/图标接收者、正常业务上限、目标/源别名和状态保持；222正确本体不改。旧正常位掩码值一致，读宽度差异不能宣称运行数值错。

1. 221：固定payload，imageNo初始-1，fresh current.position先保存attackerPosition→439470推进。fresh currentIndex捕targetPosition和initialFlags W32后共享59E980目标T*/其payload一次保存，initialFlags bit4图110000；否则400时技术8/targetPosition阈12图110098或111048并源RandomEffect110014；其它skillLevel>=9图110014。target+10=共享attacker，fresh current.flags W32写target FE4→ApplyFlags(attackerPosition,action)。fresh index/flags W32先8000决定共同跳尾，4000恢复分支先10000000图22459，否则给target.readiness加amount，GreaterCrypt(target,maxHp)才普通CopyPayload上限（原encoded/mask/readCount交替，token/reserved不复制）；随后fresh flags W32→targetY→amount→targetX数显type5/style2/flags，再fresh current.flags低W8 bit2给target图22454，Attribute(action,target)。非4000且2000清时扣减：10000000用fresh共享当前目标图22459；否则target.readiness扣amount/<0 Construct0，原先齐读mask/encoded W32/readCount W8后按原序写action.readiness（确为源对象，不是target）→Destroy，现正确体保。fresh hitIndex.position3非0才同cached index.flags低W8 bit2生成target110008，imageNo非负才target图、共享attacker/target Rebirth；fresh flags W32→Y→amount→X数显type6/style2/flags，再fresh current.flags低W8 bit2给action图22454（不是target），Attribute(共享attacker,target)。扣减分支共同fresh current.flags低W8 bit20才源FDC W32|2/gate1/alive1。恢复或8000/2000跳尾不执行此激活。共同source439340→439470弃返，所有参数/返回与source/target别名、真实生产消费/业务上限和未决寿命范围保。

2. 222：fixed payload，fresh current.position/共享59E980目标供Turn(action,target,0,10.0F)，调用后另fresh current.position/共享目标供有符号Distance<20。未近则共同Move→PlayFrame(action,0)返回；近时fresh skill恰266或2022才另fresh目标payload.FF0清0，再fresh current共享目标439340→source439470，捕opponent原+10指针后source.phase22→Turn(action,opponent,0,0.0F)。原angle F32载入+真实F32 180，中间先比较180后F32发布，>180减360，否则<=-180加360，中间不重读已落位角度；speed F32载入×真实F32 0.5后F32发布。随后Move→PlayFrame0返回。其它skill近时先MarkDeath(action)→声216(x)（原第三Y未消费、音频暂停）→Move→PlayFrame0返回。保两个目标重取、参数/返回、指针先phase后发布和未落位比较/正常S32业务，不改正确体。

两完整原根346条1169B/六页、34call/21入口/28普通内部jump/三ret、两唯一404DB0入向六邻窗与四F32原常数。只221四实际低W8消费，初次/随后image/damage flags W32与寄存器AL/AH消费、源对象HP零临时齐读/清源、图标的源/目标接收者和普通CopyPayload上限全部保持；222整个正确体与未落位浮点中间/实际F32存储、两个fresh目标/MarkDeath/声参数/状态及返回保持。正常旧掩码值相同，不称数值/并发或机器故障。只当前完整正常源接线静态，不外推递归父/全部子依赖/其它生产与owner寿命、SDK/生成机器ABI/实际x87/画面运行；音频暂停，RPLAN.24不专项异常/保护/回绕/故障等价保。

<a id="battle-effect-pair-distance-sampling-acceptance"></a>
## 限定验收

- 两完整原根、实际连续字节边界、34call/21入口/28内部jump/三ret、两父六邻窗和四F32常数、四W8消费可追。
- 整YobiRuntime只有221四片段，222及其它全源/普通CryptInt helper和头保持；原清源HP三齐读与发布、两图标接收者、W32缓存/非拥有目标/业务上限和未落位比较/状态/返回/调用序保持。
- 新增1472..1473两原已有定义正常语义/equivalent：语义1472→1474/有效1467→1469、关联1585→1587/3680（43.12%）、BATTLE149→151枚举open、分类597→599、E752→754仅两局部I/S，每项十独立实来源。R7 260/3061/余2801、差队列172/待归属43及其它完整门禁不变。
- U08/R01/R04/V04只当前/引用仍open，原60任务关闭条件保，完整战斗/生命周期/运行及R8未关闭。

<a id="battle-effect-pair-distance-sampling-source-state"></a>
## 当前交付

221四原低W8消费恢复，221/222两个完整成对效果/距离状态基线纳管；快照 `cg2d-re-battle-effect-pair-distance-sampling-20261007`。未编译/调试/测试/运行/摘要，音频暂停、原格式明文及原验收标准保持。
