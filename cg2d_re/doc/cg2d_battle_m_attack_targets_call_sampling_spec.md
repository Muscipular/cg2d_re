# 战斗M公共攻击三父调用取样SPEC（BNK01）

<a id="battle-m-attack-targets-call-sampling-baseline"></a>
## 原端基线与范围

本步修复既有 `ProcessBattleMSkillAttackTargets` 包装及 `ProcessBattleMSkillHeader` 的三个调用点，四补丁/四处、两根；三组实际参数取样差异。公共九参424620本体已有[P01完整3089条源码基线](cg2d_battle_attack_targets_spec.md#attack-target-source-closure)，本步核心本体/公开声明保持，不重复核3089条，不把旧基线或本次桥接当运行验收。

[证据](cg2d_battle_m_attack_targets_call_sampling_evidence.json)四范围同属父427470，合80条304B四页九个静态call：
- 73零模式42B229..42B282：24条89B，包含424620/Move/RID与增58。
- 3100的43573F..43579B：25条92B，Take整数/424620/Move/RID及固定状态写。
- 普通公共攻击43695E..43699B：16条61B，424620之后沿原value角色，跳42B265共用Move/RID/增58。
- 普通模式前缀435B25..435B63：15条62B，Take mode后一次skill及72/2017/113/mode0条件；非零其它模式落435B63，仅作分流边界。

第一范围与[BNI已有case73区段](cg2d_battle_m_skill73_sampling_evidence.json)指令偏移281起24条逐字段一致，24条89B/3call非新增；其余56条215B/6call本步取证。原三个424620 call为42B253/43576F/436983，C++共用包装只有一个core表达式，不能以表达式数推原覆盖率。

3100 route由已保存的42AE37/42AE3D→42FBF4/42FBFA→43222F/432235→4340FE/434104/43410A→43573F决定；这些比较与跳转不写EDI。[BMN正常技能头](cg2d_battle_m_skill_entry_waiting_sampling_evidence.json)42AE2C给EDI置1，435793写该寄存器至58，现有固定1路径保持。路由/EDI来源只作状态写的局部上下文，不宣完整ABI寄存器或其它技能路由已验。

<a id="battle-m-attack-targets-call-sampling-decision"></a>
## 工程裁决与三组差异

依已有委托先SPEC后实施。原事实已区分三个父的采样，不新增接口体系、owner、依赖或类型，不更改公共九参原型；只扩既有私有包装的值参，并在三个调用点取样。

| 原call | 原取样与消费 | 改前差异及修复 |
|---|---|---|
| 42B253，技能73零模式 | 42B229 levelIndex→B22E level→B234 skill，然后九参调用 | 共用包装直接用三全局实参，求值次序隐含；caller明确三S32局部，再传包装 |
| 43576F，技能3100 | Take整数后435745 levelIndex→574A level→5750 skill，然后九参调用 | 同上；保原Take及固定1，case加局部作用域以免跨case初始化 |
| 436983，普通入口 | 435B32已读skill，条件分流后43695E levelIndex→6964 level，6981压原EAX skill | 包装再次live读取skill；caller传既有currentSkill快照，只在原时点另读index/level |

包装改为接收固定skill/level/index（仍S32），向真实424620传scene、这三值、固定fpUsed与四个独立S32*输出，顺序不改。四地址沿原caster5AAA64/value5ABA7C/target5AAA40/redirect5AAA70，不缓存成私有结构或复制四字段。实际x64地址由既有native指针承载，非地址仍原宽度。

core返回后另live value5ABA7C→该位置actor→Move，RID，然后原73/普通增58、3100写1。公共本体的返回/失败和四输出处理保持，不在包装增失败、空对象或255门禁，不把Move改为caster位置，也不新增终结EffectList。原mode0/72/113/2017分流、3100先Take及其余全部header分派保持。

P01的F:00024620完整源码及D01入口次级图号0一次保持，其他布局、取包、图数组、技能处理和consumer-owner没有新审批或模型。BNI当步未核该完整消费者只描述当时范围；本步借现有P01证明核心源码边界，另纳管三父参数桥，不据此宣完整73/其它父生命周期或运行通过。RPLAN.24不新补零/钳制/异常保护、回绕或故障模型，不宣编译后指令形态或异常等价。

<a id="battle-m-attack-targets-call-sampling-acceptance"></a>
## 可度量验收

| 局部阶段 | 本步证明 |
|---|---|
| I | 两根四补丁四处，从HEAD精确重放整个CPP；三调用点/包装显式固定参数，核心/公开头与其它正文保持 |
| W | 三个原九参call对照三个现有C++调用点和真实424620/四独立输出，value角色Move及两种58后继；仅静态接线，不代编译链接 |
| S | 四范围80条304B四页九call/全部直接出口、普通prefix分流/三差异原EA、原3100路由及EDI1来源上下文 |
| 复用 | BNI24条89B/3call非新增，新56条215B/6call分列；P01的3089条本体旧基线不计本步取证或重复验 |
| 登记 | 旧812五引用/一symbol/本步对象，P01原源码closed只加本桥引用；双域六局部I/W/S，七open不改完整状态 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH、其它技能/全路由、所有consumer-owner/SDK-ABI-PC53/时钟RNG画面及R1/R7/R8继续 |

E457→463仅YOBI/BATTLE六局部I/W/S；旧1350/有效ID1345、片段5/分类456、原成员55/52/26/32、关联1463/3680、60任务源码closed10/完整验收0保持。不新F/X/adapted/分类或完整成员，不重开P01，不以三父桥/静态接线或原core基线代所有callee/caller运行。F:00024620台账本体记录保持，经既有P01/共享战斗文档的本步追加追踪桥接修正，不将包装实现计为core重写。

<a id="battle-m-attack-targets-call-sampling-source-state"></a>
## 源码、进度与剩余

源码两根 `src/YobiRuntime.cpp#ProcessBattleMSkillAttackTargets` / `ProcessBattleMSkillHeader`，快照 `cg2d-re-battle-m-attack-targets-call-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)及[看板](../plan/cg2d_measurement_dashboard.md)同步。三组取样问题本步解决；继续其它技能真实消费者/全路由及完整父、生命周期与运行门禁。
