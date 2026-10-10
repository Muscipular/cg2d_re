# 玩家倒计时回复生产 SPEC（PTP01）

2026-10-10。沿GTP01同源倒计时补MTg实际入口50C7A0；依子AGENTS第8条先记本SPEC，再恢复第一次时长缩放在mode发布前的计算时点。当前同源字段/负时长分支、SDK/返回/门槛和消费源保持，不引入新业务或异常模型。

<a id="player-timer-reply-baseline"></a>
## 完整原根与实接

F:0010C7A0 / Sub_50C7A0原范围50C7A0–50C7E4（尾不含），完整18条68B/一页、两条件跳一RET、零E8；50C7CE通过IAT54D1F8调用GetTickCount一次。唯一E8入向513710，父512E40，随后平衡栈/XOR EAX；只局部参数/弃返窗，不重验完整dispatcher。现MTg的TryDispatchReply<Sub_50C7A0,I,I>对应seconds/mode与既有原生socket；socket原未读，当前宽度保，不改命令/字段/解码或明文。

原gate9A6E0C的55条直接数据引用仅证明53读/两写，不代表53完整消费者：45EBB9/父45EBB0写1、51C59B/父51C430写同源清零值。真实现Source分别SceneState.cpp的Sub_45EBB0和WorldUpdate.cpp::Update，沿已完整BTD01/BTB01旧1606/185；不重复全根或把退出/更新的全部状态称闭合。本步保持两个writer/初始化和所有其它gate消费。

原timerRemaining FDA70C为S32、timerMode FDA710完整S32、timerTick FDA714为U32；现同源字段、GTP已补2215/458F40扣减、无独立原F的两个reader和ConsumeBattlePlayerTimer当前提供者保持。最后清626C14为完整W32零，不猜新的输入或owner用途。完整原码、SDK/唯一父、门槛writer与现同源helper见[证据](cg2d_player_timer_reply_producer_spec.md#evidence-archive-20261011)。

<a id="player-timer-reply-decision"></a>
## 正常合同与本线程裁决

入口只读gate一次，非0立即返回该完整U32位型，无模式/时长/tick/626C14发布或SDK。不能改为bool/固定1/0，不能提前清场或合入network返回。通过才按S32 seconds比较，负时长按原XOR EAX置0；这是原业务分支，保持且不扩为新保护/上限。

原50C7B3先以LEA完成duration×5，随后读/发布mode（50C7BA）；其后50C7C0/50C7C3再×5/×5、50C7C6再×8，50C7C9发布毫秒remaining。当前先发布mode后才求duration×1000，计算时点不符；**只增加S32 firstScale=duration*5在mode发布前，剩余表达为firstScale*5*5*8。**保三个×5和×8的正常表达顺序/最终1000单位、字段宽度和发布序，不引U64、位回绕/溢出模拟、钳制或输入容量限制。

随后唯一一次GetTickCount→发布timerTick14→626C14=0→返回同一tick完整U32，不重读tick槽、增加第二采样或按mode0跳过写入。原负秒仍发布mode/零remaining/tick/输入零；原mode的全部S32值保，不变bool/掩码或新增有效类别。现socket不参与业务，已批原生socket宽度保持。

只改既有body一逻辑组及一定义/一声明关联注释，无新函数/API/布局/owner。ReadPlayerTimerRemaining0C/Mode10及ConsumeBattlePlayerTimer属既有本机提供者，不捏造原函数身份；消费helper/同源tick推进、所有其他源码/门槛writer保持，不宣本步重新验证全部战斗、状态栏、转生按钮/网络生命周期。RPLAN.24不新增保护、默认值、补零、钳制、回滚或异常模型；保原负时长归零业务。只核源码表达序，不保证优化后机器指令/独立加载、ABI/数值或并发等价。

<a id="player-timer-reply-acceptance"></a>
## 限定静态验收

核18条连续原码/边界、两跳一RET/单间接tick/唯一E8及父短窗；完整gate早退位型、S32负秒分支、首×5→mode→余×5×5×8→remaining→tick→清626C14→tick返回。只读55数据引用/两已纳管writer来源及三个无原F helper，核同源字段类型/现MTg绑定与ReadTickCount32实际Win32桥，整SceneState.cpp/WorldUpdateDependencies.hpp精确重放一组/两注释；全部其他body/类型/表/原gate和消费源保持。

新增一个语义/F，MAP/Yobi/BATTLE/NETWORK各一个依赖成员及四限定S零I；U08/U09/U10/G01/R01/R04/V03/V04八任务保持open。原功能枚举/完整gates/六closure、其它登记及总SPEC§10守恒，关联数量非整体验收或完成率。

只做必要文本静态/measure_cg2d.ps1/-Check，不构建/测试/运行客户端或算法/产物/摘要/依赖。完整唯一父/MTg全部收发与正常失败/53门槛读者/同源字段全部生产消费、玩家与输入alias-owner、SDK/数值/机器ABI/战斗/状态栏/转生按钮/画面及全生命周期待；WAV-only、原格式明文/MSVC直接CRT及現文本字节/普通整数边界保。

<a id="player-timer-reply-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐游戏校时与倒计时并恢复持有取样”；本步拟提交说明“[#] 补齐倒计时回复生产并恢复缩放发布顺序”。统一cg2d_re分支、不打tag。先记SPEC，原完整取证/源码一组、核心静态/登记、文本汇总/Check及派生范围核对通过。语义2218=2170F+48X，文档关联2280/3680=61.96%仅关联率，F处置1650/证据1454，MAP170/Yobi100/BATTLE424/NETWORK76仍未验收；派生仅六metrics/一F/四功能/八任务变化，184叶验收0、60任务关闭11、六closure开放/全部完整gates及其它字段守恒，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_player_timer_reply_producer_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐倒计时回复生产并恢复缩放发布顺序」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`generated_machine_reads_verified`=false；`runtime_verified`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
