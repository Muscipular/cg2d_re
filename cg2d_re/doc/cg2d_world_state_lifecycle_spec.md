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
