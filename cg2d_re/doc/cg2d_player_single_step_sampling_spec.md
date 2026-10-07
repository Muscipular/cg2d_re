# 玩家单步临时坐标取样 SPEC（BUC01）

日期：2026-10-08。承接BUB01真实469FC0→46B1B0生产消费链，按AGENTS.md第8条先记后实施。[完整原指令、局部窗口、实际常量和源前态](cg2d_player_single_step_sampling_evidence.json)。

<a id="player-single-step-baseline"></a>
## 原端基线与差异

当前cg2d.exe完整[46B1B0,46B36C)共119条444B/两页一块、13直接call/7jump/1RET，唯一入向469FC0@46A122。原父完整98条456B复用[已固定BUB01](cg2d_player_move_control_sampling_evidence.json)，本步局部调用后ADD ESP,0Ch→ECX指向玩家Y→GetVal，没有消费单步返回；不代完整上游机器ABI。

| 取样点 | 原端完整预取与发布序 | 差异及修复 |
| --- | --- | --- |
| X，46B21E-46B239 | mask W32→encoded W32→count W8，首pub前已全部读取，再同序写快照X | 当前边读边pub；改为三const预取再同序pub |
| Y，46B252-46B26E | encoded W32→mask W32→count W8，首pub前已全部读取，再同序写快照Y | 同样三const预取后pub，不套X的mask-first顺序 |

X构造/预取/pub/销毁完整结束后才构造Y。保持两Ctor/两Destroy与RNG消费，不改内部LegacyCryptInt算法或生命周期；该内部存储与网络明文要求不同。

原初player指针读取一次，非空才读取payload；rate speed×实际F32 0.01先落F32，随后payload/child空门禁。previousY用持有player，previousX在Read之后重新读player，child在两读取后从持有payload重新读给442110，ST0返回被丢弃。当前该段顺序正确，保持代码与原NULL短路，不新添指针保护或另造owner。

原快照Multiply X64→世界X W32→差X落F32，然后Multiply Y64→世界Y W32→差Y保更高精度。原方向计算/平方根/除法/步长与方向同步由现有MapEffectMath承接，所有现有数值表达式保持。本步取得54D4E4/54D420/54DD44三个4B实际已加载F32：0.01/0/4；它们证明常量位值，不证明完整数值/x87精度/舍入或运行等价。

原末调用519EE0后只收栈/RET，EAX沿该callee；现void按唯一局部未消费窗口具名适配。不新增固定返回，不宣称上游全部弃返或机器ABI已验。尾4B按实际insn_t.itype逐项NN_NOP/90、无入向/后46B370，只用途，不新F或范围排除。

<a id="player-single-step-decision"></a>
## 具名裁决及实施

只修src/YobiRuntime.cpp中MovePlayerOneStep_46B1B0两片段，各先const预取三值，再按原序pub/Destroy。原正确player/payload/child取样段、所有其它语句与全体其它源保持。无新lambda/helper/声明/owner/布局、无额外字段/RNG访问；既有下游按原合同保。

新增1679/adapted及一个真实F；网络原格式仅明文、内部LegacyCryptInt、音频暂停、RPLAN.24继续。只给本步适配和证据，完整功能标准保。

<a id="player-single-step-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 边界 |
| --- | --- | --- |
| 源码 | 一cpp、一个既有正文两片段 | 完整源精确差异，X/Y不同序先三预取后pub，其余语句不变 |
| 原端 | 119条444B/两页、唯一局部窗口、父证据复用、12B实际常量/4B用途 | 原指令/宽度/调用归档，不代完整数值或机器ABI |
| 身份 | 新1679/adapted | 旧1679条/冻结定位/pending18与ECX pending7保持 |
| 功能 | LOOP91→92/MAP88→89，各两局部I/S，共4E各25来源 | 完整叶/运行门禁不伪闭 |
| 派生 | 语义1680/有效1673、合同1791（48.67%）、处置821/E952、R7已审389/余2672 | 汇总器生成，集中静态后记实值 |
| 任务 | U10/V03/R04仅current/引用，仍open | closed11、差队列与完整门禁保持 |

完整正常owner-alias寿命/生产消费/callee/上游数值/x87舍入/机器ABI/链接/画面/明文互通及R8未验；不构建、不测试、不运行、不计算摘要，不降低验收标准。

<a id="player-single-step-source-state"></a>
## 当前实现状态

原单根119条及唯一入向窗口/常量/尾区已完整取得，两处快照差异确认。先记本SPEC，再实施与同步唯一进度/台账/任务；静态和汇总后另记实值。命名快照：cg2d-re-player-single-step-sampling-20261008。

BUC01集中静态已通过：完整Yobi仅一既有正文两片段，X/Y各原三预取/pub、正确player/payload/child段、全部数值源码及两Ctor/Destroy保持；两原页119条444B/元数据、唯一调用窗口/父证据复用、12B实际F32/4B用途、证据/台账/文档与三任务模型一致。汇总及 -Check 已通过，完整派生快照实值为语义1680/有效1673、合同1791（48.67%）、处置821/E952、R7已审389/余2672、LOOP92/MAP89。全部旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11及完整门禁保持；只局部静态，不代完整数值/x87舍入/正常寿命/机器ABI/编译/链接/运行。命名快照：cg2d-re-player-single-step-sampling-20261008。
