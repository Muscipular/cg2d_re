# 战斗M技能117取样SPEC（BNH01）

<a id="battle-m-skill117-sampling-baseline"></a>
## 原端基线与范围

本步只修复既有 `ProcessBattleMSkill117`。原主体 `42F9BF..42FBF4` 为141条565B两页19call，真实尾 `429999..4299AD` 五条20B一页1call；两范围同属父427470，合146条585B三页20call。全部直接目标落本组或父默认437A29。

循环255/空Finish在主体42FBE1另live caster/actor并压Move参数，再跳429999，**不回42998C重新采样caster**。该五条后缀与[BMJ01已有状态消息共用尾](cg2d_battle_m_status_sampling_evidence.json)原八条尾的后五条逐字段一致，既有五条20B/1call非新增，本步新141条565B/19call分列。[BMO01实际低段202B表](cg2d_battle_m_skill4_sampling_evidence.json)定位case117→42F9BF；21个W32跳槽84B/118个W8索引118B只定位上下文，不当本步原指令新增或全低段路由验收。

[证据](cg2d_battle_m_skill117_sampling_evidence.json)保存两范围全部实际字节/分页、flags发布/两种结果路径和真实Move后缀、调用/跳转、改前源码/七补丁与旧812完整前态。完整父17298/SEH、其它技能与运行另验；网络原格式明文、音频暂停，显式UTF-8。

<a id="battle-m-skill117-sampling-decision"></a>
## 工程裁决与七处差异

依既有工程委托先SPEC后实施，单根七组七定义七处，不新增接口、owner、依赖、类型或保护。

| 项 | 改前差异 | 原端顺序与修复 |
|---|---|---|
| 1 | caster发布后重读全局位置 | GetPos返回位置先pub，actor沿同位置 |
| 2 | 初skill值早于payload | 固定caster的payload先取，再live skill |
| 3 | 初level值早于actor | live caster、actor、live level、payload |
| 4 | target255比较在pub之后/判空又读位置 | 返回target先比较再pub，同位置actor判空 |
| 5 | Take flags提前pub、EffectList实参读取隐含 | 固定返回W32 flags→live caster→live target→flags pub→caster actor，固定flags/0 |
| 6 | 基础target skill值早于actor | live target、actor、live skill、payload |
| 7 | 基础target level值早于actor | live target、actor、live level、payload |

第5项原42FADD Take后42FAE2先caster、42FAE8再target、42FAF0才pub flags，42FAF6取actor，42FAFF EffectList；不同于114 target先pub后caster。不统一套用相邻技能模板，不新增pub后flags重读或冻结两基础字段/后续动作到本次target。初caster/loop target局部用parsed前缀。

全部正确消费保留：入口两Take首弃/次GetPos，初caster空Finish直接返回；写skill/level/fpUsed，25/55/40/151/64各次live caster。每target Take/GetPos，255退出，actor空Finish退出；正常再Take flags与EffectList(caster,target,固定flags,0)，两基础字段，target0，调用后读取live内存W32 flags80000。成功target56，失败target68后另live target写reactionImage1010=原22331；两结果之后各次live caster73/74，再下一target。

循环255/空Finish末live caster Move、packetSubState58加一、父默认；初空直接返回不消费该尾。原末**无RID、无额外55、无caster终结EffectList(-1)**，不误用114的终结尾。失败图字段沿68调用后重新取位置，不冻结到判空或flags调用位置。

全部其它根/声明/分派与正确尾保持；已证地址native非拥有指针，非地址S32/W32等不拓宽。RPLAN.24不新补零/钳制/越界溢出、回绕或故障模型，不宣异常或编译后指令形态等价。

<a id="battle-m-skill117-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证明 |
|---|---|
| I | 单根七定义七处落码，整CPP从HEAD精确重放补丁，其它正文/属性与声明保持 |
| S | 两范围146条585B三页20call连续实际字节/全部出口，真实429999 Move后缀与case117实际202B定位表/七差异原EA |
| 消费 | 原caster-target先flags pub/固定参数、0后W32 live flags门禁/56或68-22331、73/74与全部取包/失败/Move增58 |
| 复用 | BMJ原尾后五条20B/1call非新增，新141条565B/19call分列，不增RID/额外55或末EffectList |
| 登记 | 旧812只五引用/一symbol/本步对象、双域四局部I/S，七open进展/唯一进度同步 |
| 运行和整体 | 不构建/测试/运行/摘要；完整父SEH/所有消费者与接线-owner失效/SDK-ABI-PC53/时钟RNG画面及R1/R7/R8继续 |

E445→449仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60任务已闭10/完整验收0保持。不新F/X/adapted、分类或完整成员，不以本根/两种结果/真实后缀或静态汇总代全低段路由/完整父/功能/整体验收。

<a id="battle-m-skill117-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill117`，旧812有界追加，快照 `cg2d-re-battle-m-skill117-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)和[看板](../plan/cg2d_measurement_dashboard.md)同步事实/剩余，不例行改总SPEC/索引或压历史。本步发现的七处正常差异当步修复，继续73/106族等其它技能主体及全路由/完整父、全部消费/生命周期与运行门禁。

## BNH01 路由元数据修正

六组已纳管证据（99族、107、108、109、112、114）的当前 `routing_context` 从技能4上下文复制时残留 `selected_case: 4`，与正确 `selected_cases`/`selected_target` 冲突。本步删除六份证据根上下文及旧812对应六对象的这一冗余字段；117同样仅用 `selected_cases: [117]`。逐个已保存202B表实际索引与目标作有限静态对照，全部其它字段/指令/字节保持，不重验六组功能、不增加证据门禁。`previous_semantic_record` 是当时HEAD完整前态，原样保留其历史取值。本步20文件精确提交。
