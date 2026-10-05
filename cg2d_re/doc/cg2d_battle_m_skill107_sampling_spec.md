# 战斗M技能107取样SPEC（BNC01）

<a id="battle-m-skill107-sampling-baseline"></a>
## 原端基线与范围

本步限定既有 `ProcessBattleMSkill107`；原 `42EB32..42EC95` 属 `UpdateBattleEvent_427470`，完整正常有界主体93条、355B、两页、15call；六个直接跳转均落本主体或父默认437A29。取证包含循环、两种失败和末终结尾，无既有重叠尾加入分母，不据完整父17298条统计宣父已验。

原case107经[BMO01实际低段入口表](cg2d_battle_m_skill4_sampling_evidence.json)的21个W32跳槽84B/118个W8索引118B定位42EB32。该202B是已证上下文，保get_lowcase()=0/默认437A29，非正文新增原指令，也不验全部其它技能。

[原证据](cg2d_battle_m_skill107_sampling_evidence.json)保存实际分页字节、边界、调用/跳转、改前源码、五处补丁与旧812完整前态。网络原格式明文、音频暂停；源码/文档显式UTF-8。

<a id="battle-m-skill107-sampling-decision"></a>
## 裁决与具体差异

依用户既有工程委托先SPEC后落码，单根五组五定义五处修复；不新增接口、依赖、owner或保护。

| 项 | 改前差异 | 原端顺序与修复 |
|---|---|---|
| 1 | caster发布后从全局重读位置 | GetPos返回位置先发布，actor沿该位置 |
| 2 | 初skill右值早于载荷读取 | 固定caster的payload先取，再读取live skill，写103C |
| 3 | level右值早于角色读取 | live caster位置、actor、live level、payload、1044 |
| 4 | target255比较位于发布之后，判空又读全局位置 | 返回位置先比较再发布，actor判空沿同位置 |
| 5 | Take flags先pub，effect实参/全局重读顺序隐含 | 固定W32返回flags，live caster位置先flags pub，再live target、actor；EffectList沿固定flags/0 |

原42EC28 Take之后42EC2D先读caster，42EC35才发布flags，42EC3B读target，42EC40取caster actor，再42EC49 EffectList。不能把flags发布提前，或在无原依据处重读发布后的全局flags。

所有正确消费保留：入口两Take首值弃用、次值GetPos/caster；初caster空Finish后直接返回，**不执行末尾**。正常写skill/level/fpUsed，再25/151/55/104，各次调用沿live caster；目标循环Take/GetPos，255退出，actor空Finish后退出，正常再Take flags和EffectList(caster,target,flags,0)。循环结束后live caster效果终结(-1/0/0)、Move、packetSubState58加一、回父默认；空target路径仍消费该尾。原末**无RID**，不附加其它动作/字段或失败条件。

按RPLAN.24不新补零、钳制、越界/溢出、回绕或故障模型，不宣异常等价。已证地址用native非拥有指针；S32/W32及既有字段宽度保持，不把工程显式取样表述当编译后指令形态证明。其它全部根、正确尾、声明及分派保持。

<a id="battle-m-skill107-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证明 |
|---|---|
| I | 单根五定义五处落码，整CPP从HEAD精确重放补丁，全部其它正文/属性/声明保持 |
| S | 93条355B两页15call连续实际字节、全部直接出口；case107实际202B表上下文和每处原EA有来源 |
| 消费保持 | 原调用次数、25/151/55/104、两种空失败/255与取包、末终结Move增58且无RID |
| 登记 | 只旧812追加五引用/一symbol/本步对象；双域四局部I/S，七open任务进展与唯一进度同步 |
| 运行与整体 | 本步不构建/测试/运行/计算摘要；完整父SEH/全部消费者和接线、owner重进退出借用失效、SDK-ABI、PC53、时钟RNG/画面及R1/R7/R8另验 |

E425→429仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60任务已闭10/完整验收0保持。不新增F/X/adapted、分类或完整成员，不因子SPEC存在或文本汇总退出0改变完整门禁。

<a id="battle-m-skill107-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill107`，原新关系追加既有812，快照 `cg2d-re-battle-m-skill107-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)和[看板](../plan/cg2d_measurement_dashboard.md)承载本步事实与剩余，不改总SPEC/索引或压历史。

本步发现的五处正常差异当步修复。后续推进108等其它技能/全路由及完整父、所有消费者/生命周期和运行门禁；不将未完成范围改为已验收。
