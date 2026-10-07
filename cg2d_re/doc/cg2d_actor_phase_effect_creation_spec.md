# 骑乘效果对象创建指针 SPEC（BTX01）

日期：2026-10-07。承接BTW01运动/动画消费链，按AGENTS.md第8条工程委托先落裁决后实施。[完整指令、表项、调用窗口与前态](cg2d_actor_phase_effect_creation_evidence.json)。

<a id="actor-phase-effect-baseline"></a>
## 原端基线与确认差异

当前cg2d.exe完整43E490为879条2841B，43EFC0为97条403B，共976条3244B、十五归档页两块、54call/129jump/14RET、21入向六父局部窗口。整根归档不表示54个callee/全部父/数值/像素或正常寿命已验，局部窗口不代全CFG。

| 阶段/入口 | 原端可对照合同 | 当前差异及处置 |
| --- | --- | --- |
| 骑乘效果创建 | 43E6BE创建；43E6C8发布action08；43E6CB用同EAX对象写callback=0；随后再次读槽写imageNo等字段 | 共享helper首写前重读action08，改为创建返回先持有、发布后同指针首写 |
| 下坐骑效果创建 | 43EBF2创建；43EBFC发布；43EBFF仍同EAX对象写callback=0，后续字段fresh槽读 | 同一共享片段修复两实际路径，分配次数保 |
| 43E490状态与返回 | phase0返回0；1/2调用运动与待方向处理/各阶段逻辑，3旋转；默认清phase并返回0；十三RET中十二处1、末共享0路径 | 原入口正文保持，数值/渲染/附件与音频旧调用保；不据局部修复外推全callee |
| 43EFC0动作映射 | 保存payload；初次同ride写输入动作；imageNo一次取样；四区段十一项表，7/范围外先主动作0再fresh ride置0；其余四图号区段/后置100900/100901选择11；无ride写输入 | 当前正确，正文不改；图号区段及原后置冗余比较也保留 |
| 43EFC0重播 | restart非零依序主last=-1、fresh44/34/38逐项NULL后同指针写last=-1 | 当前正确，正文与NULL边界保持 |

每次效果创建只有callback首写使用创建返回持有指针；后续imageNo、characterAction、W8 displayPriority=10、W32 flags=0x34仍逐次fresh槽取样。原创建后的首写不判空，不增失败门槛。播放、两附件描述符借用/取样、完成后的MarkDeath/槽清空保持；无新owner或布局。

43EFC0原EAX可为图号、动作或附件地址，现void具名局部观察保；20入向包含移动后下一ftol、动作回复尾RET可能透传、51A060尾RET，不称全上游纯弃返。43E490真实0/1结果由441848后测试消费。完整六父与上游/机器ABI尚未验。

两个真实X共68B：43EFA9..43EFC0含3B原LEA ECX,[ECX+0]对齐、4项W32跳表16B、4B真实90/NN_NOP；43F153..43F180含1B NN_NOP和11项W32表44B。15表项逐W32 loaded/完整flags/目标归属核实，分别来自43E4F0/43F02C各一个真实表引用。3B不是NN_NOP，不逐字节伪解码，不将有引用表区称无入向或整区NOP；仅字节用途，不生造F或范围排除。

<a id="actor-phase-effect-decision"></a>
## 具名裁决及实施

只修改已有DrawActorPhaseEffect一正文一片段：先const原生非拥有created=NewYobiAction(2,0)，发布payload.action08=created，再created.callback=nullptr；其余语句保持。无新函数、lambda、声明、共享owner、布局或业务。两原入口正文、阶段1/2调用以及所有其它源保持。

基线/追踪纳入两真实F：状态入口有真实0/1业务返回，动作入口保具名void局部观察适配；不以正确正文纳管制造代码变动。网络原格式明文、内部LegacyCryptInt、音频暂停与RPLAN.24保持，不添加回绕、故障、钳制或保护。

<a id="actor-phase-effect-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 验收边界 |
| --- | --- | --- |
| 代码 | 一cpp、一既有helper正文、一片段；两原入口正文不改 | 整文件精确差异；创建→pub→同created首写；后续fresh字段与分配/清理调用保 |
| 原端 | 976条3244B/十五归档页、21局部窗口、15实际表项及68B用途 | 原字段/字节/边界/入向完整归档；不代所有父/callee通过 |
| 语义 | 新1670-1671，两真实F/adapted | 旧1670条、pending18/ECX pending7与冻结定位保 |
| 功能 | LOOP82→84/MAP79→81，各两局部I/S，共4E各25来源 | 完整叶及运行门禁保，不伪闭 |
| 派生 | 语义1672/有效1665、文档1783（48.45%）、处置813/E932、R7已审380/余2681 | 汇总器生成，集中静态后记录实值 |
| 任务 | U10/V03/R04仅current/引用更新，继续open | closed11与全部原对象、完整门禁保 |

全部正常生产消费/owner-alias寿命、54callee、完整六父与上游、数值、机器ABI、链接、画面、明文互通和运行R8未验。继续遵守不构建、不测试、不运行、不计算摘要，不降低标准。

<a id="actor-phase-effect-source-state"></a>
## 当前实现与证据状态

完整两根及必要表/调用窗口已取证，确认共享helper首写指针差异；先记本SPEC再实施。纯文本汇总/集中静态结果另行追加。最终命名快照：cg2d-re-actor-phase-effect-creation-20261007。

### 本步验收实值（2026-10-07）

共享创建片段已实际修复，覆盖骑乘与下坐骑两路径；正确动作映射和两原入口正文保持。

集中静态通过：完整YobiRuntime仅既有DrawActorPhaseEffect一片段变化，创建→发布→同created清callback覆盖两实际路径；两原入口及所有其它语句、后续fresh字段/原分配播放清理与音频旧调用保持。两根十五归档段976条3244B、21局部窗口、两混合区15表项/实际3B LEA与5B NN_NOP、全部证据字段及完整机器/文档/任务模型、十三文件UTF8无BOM/LF和四局部I-S各25真实引用通过。纯文本汇总与-Check沿同一次执行均终0，完整派生快照模型比对通过。实际语义1672/有效1665、文档1783（48.45%）、处置813/E932、R7已审380/余2681、LOOP84/MAP81；原pending18/ECX pending7、差队列172/缺定位171/待归属43、closed11/完整叶0/globalfalse保。未编译、测试、运行或计算摘要；完整六父与上游/54callee/正常owner-alias寿命/数值/机器ABI及运行R8继续。
