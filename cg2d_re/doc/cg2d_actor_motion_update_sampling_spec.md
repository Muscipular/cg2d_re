# 运动位置临时字段取样 SPEC（BTV01）

日期：2026-10-07。承接BTU01真实运动链，按AGENTS.md第8条工程委托先记后实施；[原指令、调用窗口、常数及完整前态](cg2d_actor_motion_update_sampling_evidence.json)。

<a id="actor-motion-update-baseline"></a>
## 原端基线与差异

当前cg2d.exe完整442460为24条74B，4424B0为168条637B，共192条711B、四归档页两块、18call/17jump/2RET、10入向八父局部路径。归档页不等于MCP请求次数；调用后最多12条局部路径不证明全部父CFG或机器ABI。

| 入口/阶段 | 原端可核对合同 | 当前差异与处置 |
| --- | --- | --- |
| 442460首点移除 | NULL返回；S16计数>0才减并写回W16；每轮重读S16界限，下点X读写后下点Y读写 | 正文正确，保持原样，补真实基线和追踪身份 |
| 4424B0目标/首点 | 首X乘64落F32，首Y保Double；worldX一次，X相同才取worldY；S16路径>0；旧坐标X/Y、路径Y/X/speed依次取样 | 当前正确，保留原顺序、速率FSTP F32及首点移除调用 |
| 4424B0子对象 | flags完整W32取样与0x200000测试，payload.action2C单次取样/NULL后同child，child.speed与原rate传入，结果pop | 当前正确，不窄化flags，不改指针或速度结果消费 |
| 4424B0运动/动画 | 两目标均F32；步X零才取步Y；idle时W16标志非零才停止动画、清0；moving先持有stepY/stepX再worldY/worldX调用距离、计算持有步平方和；未到时fresh步X/worldX先pub、fresh步Y/worldY后pub；到格pub目标X/Y后清两步；随后动画(1,0) | 当前正确，禁止将持有步值改为距离调用后的新读取 |
| 4424B0尾部坐标 | X world×实际F32 1/64→ftol→低W32→Construct；readCount W8、encoded W32、mask W32全部取齐，再同序pub、Destroy；完成X后才读取worldY并按同序处理Y | 两处现为边读边pub，修为先三预取后同序发布 |

尾X预取4426BF/4426C3/4426C7在首pub4426CB之前；Y预取442701/442705/442709在首pub44270D之前。两轴各一次构造/销毁，RNG消费、X完整临时寿命先于fresh Y求值保持。54DD48实际4B为0000803c，逐字节loaded、完整flags值有效、F32=0.015625；64及0沿既有有据记录复用，不猜未加载数据。6B与3B尾区逐地址90/NN_NOP、无入向、后真入口，仅对齐用途。

两入口原EAX/AX随路径变化，不伪造统一业务返回。442460四入向中内层随后flags MOV EAX覆盖，三外层先MOV AX新order后MOVSX EAX；4424B0六入向包含x87比较/FNSTSW AX、直接跳转后的同类局部观察以及新MOV EAX覆盖。保留现void的具名局部观察适配，完整八父、全部机器ABI仍未验。

<a id="actor-motion-update-decision"></a>
## 具名裁决及实施

仅修改YobiRuntime的Sub_4424B0一既有正文两片段。每轴构造后按原顺序定义const uint8 readCount、const uint32 encoded、const uint32 mask，取齐三项后按count→encoded→mask发布，随后既有Destroy。不同入口的encoded-first/mask-first不套到此入口，不修改通用CopyPayload或AssignFromInt。

Sub_442460正文、运动与动画所有其它语句保持；无新lambda、helper、声明、owner或布局。原数值叶与ftol承接表达式保持，不新增回绕模型、故障/边界保护或拒绝。仅拓已证地址，原W8/W16/W32合同保持；内部LegacyCryptInt仍需要，网络继续原格式、仅明文，音频暂停。

<a id="actor-motion-update-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 验收边界 |
| --- | --- | --- |
| 源码 | 一cpp、一既有正文、两个片段；442460完整正文不改 | 精确全文差异、取样/pub顺序、两构造/两销毁及其它源保持 |
| 原端证据 | 192条711B、四归档页、10调用窗口、1个新常数4B、2个真实尾区9B | 原始字段/字节/边界/元数据完整存档，局部窗口不代父全CFG |
| 语义身份 | 1666-1667，两真实F/adapted | 旧1666条、pending18及ECX未知pending7保持 |
| 功能登记 | LOOP78→80、MAP75→77；各两局部I/S，共4E，各25真实来源 | 完整叶门禁不变，不能由局部pass宣称整叶通过 |
| 派生指标 | 语义1668/有效1661，文档1779（48.34%），处置809/E924，R7已审377/余2684 | 由纯文本汇总器生成，集中静态后记录实值 |
| 任务 | U10/V03/R04只更新current与证据引用，仍open | 11已闭任务保持，不伪闭正常寿命或运行R8 |

完整父、callee/owner-alias正常寿命、数值、机器ABI、链接、画面、明文互通及运行R8仍未验，后续继续收口。遵守不构建、不测试、不运行、不计算摘要，不降低标准；局部静态不代表整体完成。

<a id="actor-motion-update-source-state"></a>
## 当前实现与证据状态

已获得完整两根及必要局部路径，确认两处取样差异；按本裁决实施先三预取后发布。源码、机器台账、任务及唯一进度同步，集中静态与汇总的实际结果另行追加。最终命名快照：cg2d-re-actor-motion-update-sampling-20261007。

### 本步验收实值（2026-10-07）

两轴三预取后发布已实际落实，正确首点移除正文保持。

集中静态通过：完整YobiRuntime仅一正文两预取/pub片段变化，正确442460/两构造两销毁/原运动与其它源保持；完整原根四归档段及10调用窗口、全部证据字段、机器台账/五文档/三任务、十三文件UTF8无BOM/LF与四局部I-S各25真实引用通过。纯文本汇总与-Check沿同一次执行均终0，完整派生快照模型比对通过。实际语义1668/有效1661、文档1779（48.34%）、处置809/E924、R7已审377/余2684、LOOP80/MAP77；原pending18/pending7、差队列172/缺定位171/待归属43、closed11/完整叶0/globalfalse保持。未编译、测试、运行或计算摘要，完整父/正常寿命/数值/机器ABI与运行R8继续。
