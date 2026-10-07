# 鼠标追踪原生链接与失败时序 SPEC（BUV01）

2026-10-08，沿BUU01普通鼠标down的Track→Report→Reset真实尾链推进。
上位：[追踪器](cg2d_legacy_mouse_tracker.md)、[固定池字节桥](cg2d_login_negative_selection_byte_owner_spec.md)、[网络原格式明文](cg2d_network_compatibility_spec.md)、[工程裁决](cg2d_closure_decisions_spec.md)。
固定快照：`cg2d-re-mouse-tracker-native-links-20261008`。

<a id="mouse-tracker-native-links-baseline"></a>
## 原事实与缺口

[完整证据](cg2d_mouse_tracker_native_links_evidence.json)保存十根262条799B/十一页、14directcall/34jump/19RET、25引用来自13父，仅引用身份不作所有父复核。

| 原入口/ID | 半开边界 | 指令/字节 | 当前符号/本步 |
|---|---|---:|---|
| 506580 / F:00106580 | 506580..5066C1 | 91/321 | ResetLegacyMouseTracker；原生池、失败发布与取样修复 |
| 5066D0 / F:001066D0 | 5066D0..5066E9 | 3/25 | ResetLegacyMouseTrackerDefaults；正确保 |
| 50E7B0 / F:0010E7B0 | 50E7B0..50E7C8 | 5/24 | ConfigureLegacyMouseTracker；正确保 |
| 5066F0 / F:001066F0 | 5066F0..506720 | 20/48 | InsertAfter；改原生借用链接 |
| 506720 / F:00106720 | 506720..506757 | 20/55 | Detach；改原生借用链接 |
| 506760 / F:00106760 | 506760..506780 | 13/32 | TakeFreeHead；去handle拒绝 |
| 506780 / F:00106780 | 506780..5067A0 | 13/32 | EvictActiveTail；去handle拒绝 |
| 5067A0 / F:001067A0 | 5067A0..5067E2 | 20/66 | AllocateAndActivate；原生候选 |
| 5067F0 / F:001067F0 | 5067F0..50686C | 55/124 | TrackLegacyMouseDown；原生遍历/普通S32加一 |
| 512CA0 / F:00112CA0 | 512CA0..512CE8 | 22/72 | Report；原PGXE明文/取样正确保 |

节点前四项x/y/tag/count均S32，+10/+14是实地址：原Reset按pool+index×24写相邻节点/四哨兵地址，Insert/Detach读取并直接解引用这些字段。现Node32把它们变成W32 handle、额外nativePoolNodeCount校验及Resolve失败早退；这不符合当前已证地址优先原生T*裁决。count的bit_cast回绕helper也与RPLAN.24冲突。

原Insert保anchor/inserted/anchor.next真实nullptr失败；写next.prev→anchor.next→inserted.prev→inserted.next，返1。原Detach保node/next/prev真实nullptr失败；写next.prev→prev.next→node.prev=0→node.next=0，返1。Take/Evict仅比较对应哨兵，其他候选包括null仍调用Detach后返回保存候选；不得额外解析拒绝。Allocate空闲优先、null才淘汰、仍null返0；写x/y/tag/count=1、插活动头、返1。

Track保存activeHead.next，null返0；保存next为null则进入分配并无条件返1。按x→y→tag短路比较；活动首项tag==0抑制增量，其余匹配项普通S32加一；摘链→插头→fresh count返回，继续遍历时清抑制标记并重读新项next。不能增加范围/损坏链接检测或回绕策略。

Reset保存旧pool，非固定且非空才free；随后单次capacity取样用于100/10/5000门禁和申请长度。原5065C6在检查返回前发布分配结果，null也先发布再回固定池/容量100；现实现缺少null发布。成功分支fresh容量用于清理，回退清理容量保存100，清理目标是保存的返回pool。阈值<5才写50，此门禁来自原业务，必须保留。
之后先保存初始容量/当前pool，按原活动头尾→空闲头prev→空闲尾next/prev→空闲头next发布。首轮prev用保存pool，后轮fresh pool；每次prev写后fresh容量判断末项，next写使用fresh pool，增index前保存循环容量，续行再取pool，最终返回index，不能在尾部另读容量替代。

原PGXE字串5B和默认阈值50/容量100/固定池地址12B均已加载。Report先Build原命令，再保存bufferLength，然后Format整数、fresh buffer追加、fresh buffer发送；当前无缓存data()访问器逐次读取同源，正确保，不加密/改格式或计新增I。七尾65B实际NN_NOP/90、无入向、后真函数仅对齐用途。

<a id="mouse-tracker-native-links-decision"></a>
## 已委托裁决与源码边界

1. 将既有节点类型改名LegacyMouseTrackerNode，仅next/prev改非拥有Node*；四S32、唯一容量/阈值和gTracker owner保持，四哨兵和100个固定节点仍属于既有同一存储。不增owner/外部依赖/SDK类型。
2. 物理节点按本机指针宽度，动态申请/清理按sizeof(Node)×保存或fresh原容量，正常失败/释放顺序保。去W32 handle身份常量、NodeHandleAt、ResolveNode、nativePoolNodeCount及计数回绕helper，保原真实nullptr/哨兵失败与链写序。
3. Restore Reset的分配结果先发布、失败回退、保存清理指针/容量和循环pool/容量采样，返最终index。Track改普通++count，不加数学异常处理或模拟。
4. 既有ReadLegacyMouseTrackerFixedByte接口不变，始终按原24B步长、next16/prev20偏移读固定池视图；前16B从现场S32取，链接按实际T*对应固定节点索引或四哨兵投影原W32身份字节。投影只读/不可解引用，不截native地址、不写回或保存链接镜像；固定池在动态pool切换后仍观察自己的旧内容。LegacyUiRecord十二偏移/全部生产消费保持，不用当前动态pool反算。
5. 源一cpp十一片段、一个hpp一类型布局片段：七原根和已有BYTE桥改变、三个虚构helper退休；默认配置/网络配置/Report三正确根保持，无新增函数或helper。既有公开签名、所有鼠标调用、网络接线和其他owner不改。
6. 旧追踪/事件/BRU关于保W32 handle、24B物理节点、解析保护和计数回绕的当前承载说明，由本步原生链接及原24B只读视图覆盖；历史取证/字节合同保。仅已证地址拓宽，不据此改变其他非地址字段。

本裁决依用户未决工程委托先记SPEC后实施。RPLAN.24异常不专项处理、不故障/回绕模拟或记等价通过；原malloc失败、null/哨兵返回、配置门禁和正常链行为仍验。网络原格式明文/两叶排除、内部LegacyCryptInt、音频暂停和执行限制保。

<a id="mouse-tracker-native-links-acceptance"></a>
## 可度量限定验收

静态核十完整原根/原字串和默认值17B/七65B用途；节点四S32与两非拥有T*、原空值/哨兵分支、写序/LRU/返回及Reset失败和采样；BYTE桥原24B视图/十二原消费偏移和五身份、无额外存储/截断；整个cpp/hpp仅十二批准片段。
七原根及BYTE桥实修，默认/配置/Report正文完整保；新记录不是新实现。十adapted原新基线1731..1740/七R7用途，LOOP144→154/INPUT38→47各限定I/S，NETWORK61→63和LOGIN13保持仅各限定S，共六证据/各25来源。预期语义1741/有效1734、文档1852（50.33%）、F882/E1032、R7已审444余2617。
T02/R04/V03/V04只current/引用，open/closed11、pending18/ECX pending7、差队列和完整门禁不变。文本汇总/Check生成派生、完整差异模型及编码引用核对，不手改生成物；不编译、测试、运行客户端/算法样例或摘要。
完整原父/callee、所有alias寿命、完整登录/窗口/配置与业务失败、机器ABI/链接/画面/文件/明文互通和R8仍开放，不以节点适配或原格式字串证明全部运行。

<a id="mouse-tracker-native-links-source-state"></a>
## 本步状态

原生地址/正常失败时序/固定池字节视图已具名裁决；实施与实际静态结果同步唯一进度，整体目标继续active。

### BUV01 实际限定静态结果

十完整原根262条799B/十一页、14directcall/34jump/19RET及25引用13父身份、原17B/七65B用途、两源码仅十二批准片段、原T*字段/原nullptr-哨兵/链写序及失败发布和pool-capacity采样/index返回、固定24B只读视图移交、完整机器及派生差异模型、十六文件UTF8/LF/四锚点及六限定证据各25来源已通过。文本汇总与Check均退出0。
实际语义1741/有效1734、文档1852（50.33%）、F882/E1032、R7已审444/余2617、LOOP154/INPUT47/NETWORK63/LOGIN13；原分母/其他域/差队列/pending18与ECX pending7/closed11和完整门禁保。七原根和BYTE桥实修/三旧helper退休，默认/网络配置/Report三正确正文保持不计新增I；未编译、链接、运行或证明完整窗口/登录/配置/明文互通验收，整体目标继续active。
