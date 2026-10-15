# 世界状态十七项活动构析与宿主登记 SPEC

> 2026-10-04；C02/C04局部实施，依[D02/D03与工程委托](cg2d_closure_decisions_spec.md)、[启动生命周期](cg2d_crt_startup_lifecycle_spec.md)。世界表、场景/坐标/移动/Ripple、玩家与主宠物各自保留唯一实际owner和现有模块位置。

<a id="world-lifecycle-baseline"></a>
## 1. 原表、登记与当前缺口

[只读原证据](cg2d_world_state_lifecycle_evidence.json)保存17原Init全部98条/直接入向与17槽；5183D0仅两条，尾入的5183F0四条登记属原5183E0，不扩大root chunks或新增F身份。另七聚合原身份136条去重保留，总234个实际指令地址，用于核数组参数、固定owner及合并归属；不是完整玩家EH/数组SEH、SDK或业务运行验收。

| 原槽→Init | 构造提供者 | 固定退出提供者 |
|---|---|---|
| 0x553014→0x43D760 | 0x43D770 / ConstructWorldObjects | 0x43D7A0 / DestroyWorldObjects |
| 0x553054→0x456FD0 | 0x456FE0 / ConstructSceneTransitionValue | 0x457000 / DestroySceneTransitionValue |
| 0x553064→0x467B90 | 0x467BA0 / ConstructLegacyPlayerCoordinates_467BA0 | 0x467BD0 / DestroyLegacyPlayerCoordinates_467BD0 |
| 0x553068→0x467BF0 | 0x467C00 / ConstructLegacyPlayerSnapshotX_467C00 | 0x467C20 / DestroyLegacyPlayerSnapshotX_467C20 |
| 0x55306C→0x467C30 | 0x467C40 / ConstructLegacyPlayerSnapshotY_467C40 | 0x467C60 / DestroyLegacyPlayerSnapshotY_467C60 |
| 0x553070→0x467C70 | 0x467C80 / ConstructLegacyWorldCoordinateState_467C80 | 0x467CA0 / DestroyLegacyWorldCoordinateState_467CA0 |
| 0x553074→0x467CB0 | 0x467CC0 / ConstructLegacyWorldCoordinateState_467CC0 | 0x467CE0 / DestroyLegacyWorldCoordinateState_467CE0 |
| 0x553078→0x467CF0 | 0x467D00 / ConstructLegacyWorldCoordinatePair_467D00 | 0x467D30 / DestroyLegacyWorldCoordinatePair_467D30 |
| 0x55307C→0x467D50 | 0x467D60 / ConstructLegacyWorldMoveTargets_467D60 | 0x467D90 / DestroyLegacyWorldMoveTargets_467D90 |
| 0x553080→0x467DB0 | 0x467DC0 / ConstructLegacyWorldMoveState_467DC0 | 0x467DE0 / DestroyLegacyWorldMoveState_467DE0 |
| 0x553084→0x467DF0 | 0x467E00 / ConstructLegacyWorldMoveState_467E00 | 0x467E20 / DestroyLegacyWorldMoveState_467E20 |
| 0x553088→0x467E30 | 0x467E40 / ConstructLegacyWorldMoveState_467E40 | 0x467E60 / DestroyLegacyWorldMoveState_467E60 |
| 0x55308C→0x467E70 | 0x467E80 / ConstructLegacyWorldMoveState_467E80 | 0x467EA0 / DestroyLegacyWorldMoveState_467EA0 |
| 0x553090→0x467EB0 | 0x467EC0 / ConstructLegacyWorldMoveState_467EC0 | 0x467EE0 / DestroyLegacyWorldMoveState_467EE0 |
| 0x553094→0x467EF0 | 0x467F00 / ConstructLegacyRippleGate_467F00 | 0x467F20 / DestroyLegacyRippleGate_467F20 |
| 0x5530FC→0x5183D0 | 0x5183E0 / ConstructLegacyPlayerRuntime_5183E0 | 0x518400 / ReleaseLegacyPlayerRuntimeAtExit_518400 |
| 0x553100→0x518520 | 0x518530 / ConstructPetRuntimeValues | 0x518560 / DestroyPetRuntimeValues |

每项正常构造完成后立即登记自己的固定无参退出，原表不消费S32结果；一次登记失败不短路后项，不回滚已完成构造或补析构。世界表1000项沿实际数组helper与typed成员回调，正序构造/逆序退出，原每项两个值同源；主宠物5项沿独立数组与六成员回调，不能与浏览宠物合并。玩家六值固定FDA700，场景73ECD0与13组坐标/移动/Ripple保留已存在实际存储，不重算业务布局/字段或RNG。

当前四自动适配（世界、场景值、玩家、主宠物）直接构造并以自身析构退出，四具名Init另沿旧退出表登记，尚不是同一活动路径。坐标/移动/Ripple四公开void包装另写了构造与std::atexit lambda；13具名Init又保留旧登记。把两套主体同时启用会重复构造/登记；只填全表不能解决。

<a id="world-lifecycle-decision"></a>
## 2. 本组选择与执行边界

1. 将17个既有Init的登记替换为实际std::atexit对应既有固定无参析构，仍先本项构造、立即登记并返回真实宿主S32。原数值53B406调用保留为基线，native职责依D02/D03迁移；不记为旧体等价或新源级原体调用。
2. LegacyMouse.cpp四自动构析适配保持类型/所在位置，各构造改调用本组对应Init并丢弃结果，删除自身析构动作，写平凡析构静态断言。不移动其他初始化，不新增第二调用者或自动退出补偿。
3. 现有InitializePlayerCoordinates、InitializeWorldCoordinateStates、InitializeWorldMoveStates、InitializeRippleGate保持接口及模块调用者，主体只依原对应槽序转调13个真实Init并逐次丢弃返回。取消这四包装中的重复构造与lambda登记，由同一具名provider承担构造/登记/退出，不增加RNG或发布步骤。
4. 世界/主宠物数组helper、typed成员回调、玩家六值构析及完整业务函数保持源码原文，存储和字段初值不改。原数组/成员失败前缀与SDK验收继续独立跟踪；本组不把原合并106条的存在当玩家EH源码/运行已闭。
5. 全应用原序接通时，必须同小步移除这八个模块适配的当前调用（四直接、四经公开包装），然后才从全应用序逐项调用17个Init。不能既保留模块调用又执行整表；当前位置只维持既有前置条件，不证明跨模块原序。
6. 不增once/懒初始化、预清/补值、回滚、保护、失败重试、默认成功、影子表、额外析构、RNG或重复调用保证；不扩大到鼠标、浏览宠物、选择器、标准流等其他owner，音频仍暂停。

真实宿主只承接本组私有退出表/分配与平台异常机制，业务可见对象、调用序和清理职责保留。代码移除的是并行自动析构/重复包装主体，不是原业务清理。

<a id="world-lifecycle-acceptance"></a>
## 3. 局部验收

| 门禁 | 可证明范围 |
|---|---|
| I | 17具名Init即时宿主登记原固定回调；存储/原值与数组、成员算法不变 |
| W | 八既有模块位置只调用同一provider；四公开包装按原各组序转调、不短路；四自动析构已移除 |
| S | 17槽/98原Init+4所属登记块、聚合归属可追踪；限定改动之外源码全文不变，原合并身份不拆分，宿主差异明确 |
| R | 禁构建/测试/运行；跨模块序/快速退出、登记失败与全成员/数组SEH/SDK和客户端行为未验 |

仅归档本组I/W/S，不关闭C02/C04、成员/数据闭包或完整功能门禁，不用局部原指令数作为总体覆盖率。

<a id="world-lifecycle-source-closure"></a>
## 4. 本组源码收口

WorldObjectState.cpp与SceneState.cpp的17个Init已改为完成既有构造后即时std::atexit原固定无参退出，传出实际宿主S32。LegacyMouse.cpp四原模块类型/位置保留，构造改调对应Init并丢弃结果，移除四自动析构动作，平凡析构写入静态断言，未编译验证。

四公开坐标/移动/Ripple包装保留接口和模块caller，只依各组原槽序转调13个Init并逐次丢弃结果；原重复构造和lambda登记主体已取消。模块四直接与四间接路径合计17项，同组后项不被前次登记结果短路。未来整表接通必须同小步移除八模块调用，不可双执行；本组保持现有位置，不外推跨模块顺序。

世界1000表、主宠物5数组、玩家六值、场景与坐标/Ripple实际存储/初值未改，原构析、值/RNG、数组helper、typed回调和成员源码原文保持。原玩家5183D0两条和5183E0所属四条登记块准确保留，不拆106条合并身份，完整玩家EH/数组SEH与SDK/运行仍待。本组只给构造完成后的登记和唯一活动路径证据，不称其余完整数据/失败清理已验。

17根记录、玩家登记块载体5183E0及53C8CD关联共19已有记录更新，语义仍1152条/1149有效ID、18pending/4分类引用。17入口adapted使已审51→68/3680，七聚合原身份仅作本组布局/归属证据，不以其原指令数或名称自动重分状态。

归档E-WORLD-STATE-LIFECYCLE-I/W/S-20261004三条限定局部证据，累计24条；实现固定至Git命名快照cg2d-re-world-state-lifecycle-source-20261004。C02/C04仍open，已关闭源码任务10项、叶验收0/182及完整功能门禁保持。全应用原序、其他owner、快速退出/完整成员EH/数组SEH/SDK和运行继续待，不构建/查产物/新增或执行测试/运行/文件摘要，音频继续暂停。

## 主地图入口基础状态的独立临时对象（BRY01，2026-10-07）

[BRY01 SPEC](cg2d_main_entry_state_carrier_sampling_spec.md#main-entry-carrier-source-state)恢复467F30末原var1C独立第三LegacyCryptInt局部，原16B平凡结构无自动C++构析；一次显式Construct／一次Destroy沿原第八阶段，不追加RNG消费。七组先读齐三字段／C821E8先两字段pub再count，原字段宽度、目的vft-padding及八原构销顺序保，其它状态／调用与返回保持。现三对象保真实身份，不以新增全局镜像或生命周期框架承接。

5199A0六清区的typed-x64存储、三现场S16恢复／两个S32取样发布、5-6-15状态数组和末direction=-1已有正文保持，仅正常基线纳管。根467F30真实EH第二chunk归属记录，不为原三跳另造函数、移植旧EH或称异常／SDK验收；全部owner寿命／alias／运行仍按原C类与U09／U11标准推进。两个相邻X7B仅原填充用途，不算生命周期闭合。

## 正常地图切换的两个数值临时对象（BSA01，2026-10-07）

[BSA01 SPEC](cg2d_normal_scene_controller_callee_order_spec.md#normal-scene-callee-source-state)恢复控制器状态3两个独立zero／one载体先mask／encoded／W8 count取齐再发布，原2次显式构造／2次销毁顺序及RNG不增；前一组已经暂存的9FB918→C821E8保持，目标vft／padding不写。六原空调用仍交既有NullSub_509080，不增加资源或业务寿命。

初始化519A80全文保持，HP／MaxHP各按原当时action复制，五petmap按持有编号首匹配现场重取action，非拥有引用和五W16清保持。完整原父及直接初始化根924条3782B，局部基线与源取样可追；不据此提升正常owner-alias、地图退出／重进、全部资源或运行门禁。原表与填充仅X用途，音频暂停保持。

## 玩家坐标比较的原读取副作用（BSB01，2026-10-07）

[BSB01 SPEC](cg2d_integer_left_compare_dispatch_spec.md#integer-left-compare-source-state)把4417E0的两个上界比较接具名401FA0：先保持原Add_401E10结果，再单次读取当前action的mapX1E0／mapY1F0，左S32比较和原AL条件保持。具名体保返回高24位，调用只取低W8；没有额外数值构造／销毁、计数或RNG，也不改同一action／载荷与回调其余正文。

同一入口六鼠标下界复用既有唯一LegacyMouseState，原薄读取没有附加副作用。4417E0完整原块仅两点上下文，AAS01既有骑乘取样／附加绘制范围保持，不以本步升级所有回调／资源／owner寿命、SDK或机器ABI／运行门禁；U11继续open，网络原格式明文和音频暂停保持。

## 玩家快照当前直接定义（LCI16，2026-10-15）

[LCI16](cg2d_cryptint_class_spec.md#cryptint-player-snapshot-direct)覆盖本文件两个快照表项的阶段提供者：C751C0／C751B0在中央原位置直接定义LegacyCryptInt，编译器承接默认构造及逆序析构；旧Init／构析叶与Startup仅保历史。玩家坐标之后先X后Y、后续世界状态之前的相对序及每槽一次RNG保持，三字段／value／赋值消费者继续使用各自同一对象。本步只静态核对，不提升原整表／异常／SDK和运行门禁。

## 其余坐标／场景标量直接定义（LCI17，2026-10-15）

[LCI17](cg2d_cryptint_class_spec.md#cryptint-other-direct-globals)将其余15个世界标量／12组启动改为中央原位置直接定义。场景值前后的资源初始化、玩家X→Y→快照X→Y→其余世界状态与波纹门禁顺序保持，各值一次RNG；三组双值退出仍逆序。玩家／宠物复合记录保持现有独立寿命；旧标量CRT／Startup和构析叶仅保历史。当前直接位置及语义／登记同步，不提升编译、整表／异常或运行门禁。

## 世界／玩家／主宠物直接定义（LCI18，2026-10-15）

[LCI18](cg2d_cryptint_class_spec.md#cryptint-composite-direct-globals)将世界千项表、玩家整体及主宠物五项表改为LegacyMouse.cpp原中央位置直接对象。世界表仍在防护值与变换缓存之间，鼠标→玩家→主宠物仍紧邻原启动序；元素／成员正序构造、退出双逆序和各组RNG保持。PlayerRuntimeSlots类型及字段断言移既有WorldUpdateDependencies.hpp，ClearRaw正文仍在SceneState.cpp；运行期清零不重构对象。原WorldObjects.hpp与纯Borrow／optional／启动包装退休，原身份／证据保历史，成员及摊位浏览的现存实现保留。仅静态，不提升异常、SDK或运行门禁。
