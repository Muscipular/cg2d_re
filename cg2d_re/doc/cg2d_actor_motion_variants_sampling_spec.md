# 普通与倍率移动取样 SPEC（BTW01）

日期：2026-10-07。承接BTV01真实移动消费链；按AGENTS.md第8条工程委托先记裁决再实施。[完整原指令、调用窗口与前态](cg2d_actor_motion_variants_sampling_evidence.json)。

<a id="actor-motion-variants-baseline"></a>
## 原端基线及差异

当前cg2d.exe完整442730为104条440B，4428F0为106条448B，共210条888B、四归档页两块、22call/14jump/2RET；各一入向、两父局部路径。两个入口同样先求目标X/Y并各落F32、步X零才读步Y、idle时W16标志非零才停动画并清0；moving持有步用于距离与平方和比较，到格pub目标X/Y后清27C/280，随后原动画(1,0)。但倍率和尾部临时取样不同。

| 入口/阶段 | 原端合同 | 差异与措施 |
| --- | --- | --- |
| 普通442730步进 | distance调用前持有stepY、stepX、worldY、worldX；平方和使用持有步；未到时fresh stepX+worldX先发布，再fresh stepY+worldY发布 | 当前正确，保持全部运动语句 |
| 倍率4428F0步进 | 9DAD78读倍率X×rawStepX保Double；读worldY/X后X落F32；再次读倍率Y×rawStepY落F32；distance后平方和及未到分支继续使用这两个持有的F32步，世界坐标在加法处fresh读 | 当前正确，保两次动态倍率读取及浮点落点，不猜初值、不当固定常数，不改成distance后重新读rate |
| 普通442730尾坐标 | Construct后readCount W8→encoded W32→mask W32三值全部取齐，再按同序pub、Destroy；完成X后才取worldY进行同样处理 | 当前两轴边读边pub，改为先三预取后同序发布 |
| 倍率4428F0尾坐标 | Construct后mask W32→encoded W32→readCount W8三值全部取齐，再按同序pub、Destroy；完成X后才取worldY | 当前两轴边读边pub，按此入口原不同顺序修复 |

普通X预取44287A/44287E/442882在首pub442886之前；Y预取4428BC/4428C0/4428C4在首pub4428C8之前。倍率X预取442A42/442A46/442A4A在首pub442A4E之前；Y预取442A84/442A88/442A8C在首pub442A90之前。两根各两构造/两销毁及RNG、X完整临时寿命后fresh worldY、原1/64与ftol表达式保持。

64/0/1/64复用BTS01/BTU01/BTV01已加载实际F32数据证据，不重复取证或猜动态倍率值。442730尾[4428E8,4428F0)实际8B逐地址90/NN_NOP、无入向/后真入口，只对齐用途；4428F0结束442AB0紧邻下一真函数，无尾区，不新增第二X身份。

两入口现void维持具名局部观察适配。普通唯一46A972后有循环索引/边界分支及RET，没有新EAX业务消费但可能透传残值；倍率唯一46A3CA后跨直接JMP到循环尾，随后下一call468200。不能据两局部窗口概括全部上游弃返或证明完整机器ABI。

<a id="actor-motion-variants-decision"></a>
## 具名裁决及实现

只改YobiRuntime两既有正文四片段。每轴Construct后按所属入口顺序用const W8/W32/W32全部取样，再按相同次序发布；普通readCount→encoded→mask，倍率mask→encoded→readCount。无新lambda/helper/声明/owner/布局，不修改通用工具。

其它运动、倍率动态读取、浮点落点/数值叶、ftol现承接、idle/到格/动画/W16、RNG和X/Y寿命全部保持。无新增故障、回绕模型、拒绝、补零或钳制。网络原格式明文、内部LegacyCryptInt、音频暂停、RPLAN.24持续。

<a id="actor-motion-variants-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 验收边界 |
| --- | --- | --- |
| 实际源码 | 一cpp，两既有正文，四片段 | 完整源仅四片段变化，每根两构造/两销毁、原其它语句保 |
| 原端 | 210条888B、四归档页、2局部路径、真实8B尾区 | 原始字段/字节/边界完整归档，不代父全CFG |
| 语义 | 新1668-1669，两真实F/adapted | 旧1668条、pending18/原ECX pending7及冻结定位保 |
| 功能登记 | LOOP80→82/MAP77→79，各两局部I/S，共4E各25真实来源 | 整叶门禁保持，不外推本步静态pass |
| 派生指标 | 语义1670/有效1663、文档1781（48.40%）、处置811/E928，R7已审378/余2683 | 汇总器生成，集中静态后写实值 |
| 任务 | U10/V03/R04更新current与引用，继续open | 11已闭任务与全叶/运行门禁保 |

完整两父与上游、全部正常生产消费/owner-alias寿命、数值、机器ABI、链接、画面、明文互通及运行R8仍未验。遵守不构建、不测试、不运行、不计算摘要，不降低标准。

<a id="actor-motion-variants-source-state"></a>
## 当前实现与证据状态

完整两根及两局部调用路径已取证，已确认四处入口各自不同的三预取差异；按本裁决实施并同步唯一进度、机器台账、对应任务与文档。集中静态/汇总实值另行追加。最终命名快照：cg2d-re-actor-motion-variants-sampling-20261007。

### 本步验收实值（2026-10-07）

四处预取/pub差异已实际修复，普通和倍率各自取样/数值/动画语句保持。

集中静态通过：完整YobiRuntime仅两正文四预取/pub片段变化，各自普通count-first/倍率mask-first顺序、各两构造/两销毁和所有其它语句保持；两根四归档段210条888B、两调用窗口、全部证据字段、完整机器/文档/任务模型及十三文件UTF8无BOM/LF、四局部I-S各25真实引用通过。纯文本汇总与-Check沿同一次执行均终0，完整派生快照模型比对通过。实际语义1670/有效1663、文档1781（48.40%）、处置811/E928、R7已审378/余2683、LOOP82/MAP79；原pending18/ECX pending7、差队列172/缺定位171/待归属43、closed11/完整叶0/globalfalse保。未编译、测试、运行或计算摘要；完整两父与上游、全部正常寿命/数值/机器ABI及运行R8继续。
