# 原格式明文传送子 SPEC

> D04；2026-10-04，以用户连续两条要求共同为准：“网络需要兼容原有格式，不要自创和修改”“不做加解密，只做明文传送”。
> **原帧结构、正文组装与协议处理保留，只旁路加解密传明文。** 不恢复401960/401AB0；此前本轮曾写的自定义明文候选及恢复加密解释均撤回，未实施。
> 上位：[收尾决议](cg2d_closure_decisions_spec.md)、[网络接线SPEC](cg2d_network_exclusion_wiring_spec.md)；原证据：[网络合同](cg2d_network_runtime.md)。P09/P10源码与局部静态对照已落位，见[帧实施子SPEC](cg2d_network_plaintext_frame_spec.md)；完整网络与互通未验。

> 2026-10-09当前实施以[本机服务端明文合同](#local-server-plaintext-contract)为准。旧“仅去算法、保留密文帧外壳和命令变换”的实现不能与当前服务端互通，下列旧W16合同仅保留为原端历史基线。

> 2026-10-16：按用户选定的N01–N03，当前网络增量按 [NHM01](cg2d_network_hook_business_migration_spec.md) 实施。原命令／字段／LF及不做加解密的合同保持；N01收发回调、N02压缩接收与N03登录前导的分步状态及运行边界见该SPEC。

> 2026-10-16源码清理：[NKF01](cg2d_network_plaintext_frame_spec.md)已删除旧W16收发帧及短状态；本次按用户要求执行[NKC01](cg2d_network_plaintext_frame_spec.md#retired-command-transform-removal)，一并删除无当前调用的4038A0／403A70命令混淆函数。下文旧正文保留约定按历史时点理解，当前LF收发、原命令／字段、Base62和文本转义保持。

<a id="local-server-plaintext-contract"></a>

## 2026-10-09：本机连接卡住的协议修复决议

用户选择IP:2（127.0.0.1:9030）。本机端口已监听，客户端systemlog记录CONNECT OK；服务进程来自 `D:/Source/cgmsv/cgmsv_server/cmake-build-release-mingw/cgmsv.exe`。实际服务端现有格式由net.c的GetOneLine_fix（628–655）、nrproto_util.c的nrproto_CreateHeader/nrproto_Send（619–622、598–613）及nrproto_GetMessageInfo（148–157）证明：直接原命令、原字段和LF，不含W16头、随机前缀、滚动状态尾或命令混淆。include/version.h定义VERSION_TW，include/nrproto_util.h在该版本不启用nrproto__ENCRYPT。

原客户端仍发W16头/随机前缀/变换命令，且按W16长度等待原命令开头的服务端回复，双方无法按相同格式派发。按用户“兼容原有格式、只传明文”和既有工程委托，本线程裁决：客户端实际收发直接兼容上述已经存在的服务端格式；撤回2026-10-04把密文传输外壳解释为明文业务必保项的实施决定。不改服务端、不恢复加解密、不加入格式协商/自动检测或另一套协议。

实施边界：既有NetworkRuntime.cpp增加真实明文入队/取行提供者，InitializeNetworkRuntime绑定它，NetworkLoop以LF取完整行后交既有派发；命令生产与消费直接使用JFVf/whV等原命令。JFVf四字段顺序、base62整数、旧转义/空格/GBK正文、回调、发送队列及部分发送保持。半行不消费、多行逐行消费；重新初始化清实际收发数量、轮询相位与计时状态，远端recv=0转现有断线路径，防止连接界面继续等待。旧403C50/403E50和4038A0/403A70正文保留作原端对照，退出当前明文客户端收发链，不能把保留源码称为当前接线或新增范围排除。

验收只核当前新增差异、真实接线与必要Debug编译；客户端/算法样例与试包仍不自行运行。实际登录成功、半包/多包及完整生命周期必须另有运行证据，不能以服务启动、CONNECT OK或编译通过代替。

<a id="local-server-plaintext-source-state"></a>

### CAB05 当前落地结果

源码仅修改NetworkRuntime.cpp：两个私有明文提供者、命令组装与派发、初始化接线/队列数量重置及recv=0断线处理。JFVf四字段、108个原回复命令/参数及回调、原整数/转义工具保持；四个旧帧/命令变换函数正文保留，当前明文链不再调用。服务端五份相关源码与提交cecfb008b1a8fb5116345c7fcea91047bce247ae一致；仅取证，未改动或重启服务端。

复用Windows/MSVC/x64 Debug预设增量构建，三库及真实客户端链接返回0，0错误、3条既有C4244警告。日志为build/windows-msvc-x64/build-plaintext-login-20261009.log，EXE为7980032B；已更新H:/Games/cgmsv_21.2a/CrossGate7.1.0/cg2d_re_client.exe，旧版备份带.before-plaintext-login-20261009后缀。未自行启动客户端、试包、执行测试或计算摘要；用户重新运行IP:2的结果尚待确认，FUNC-NETWORK/FUNC-LOGIN、V03及整体SPEC完整门禁不关闭。

证据：[CAB05记录](cg2d_local_plaintext_login_evidence.json)；源码快照：cg2d-re-local-plaintext-login-20261009。旧D04证据按原历史范围保留，不改写为本次运行通过。

<a id="window-report-plaintext-wiring"></a>

## NWR01：明文发送接回窗口报告状态清零（2026-10-12）

开发入口来自全量函数表4589A0／4589B0／4589C0的明确接线缺项：三操作只在当前无调用的旧403E50帧正文内联，活动QueuePlaintextProtocolBytes未消费报告状态。窗口枚举仍生产gWindowReportValue，MainLoop按原计时条件发送PGXE后将gWindowReportDeadline写0xFFFFFFFF；初始零存储不代后续清零。

原父限定指令位置：403E99..403EB1先判D9F2D0严格1并清零，跳过报告分支；403EB3..403ED9依次调用4589A0读取S32报告值并判正、510140返回严格1、4589C0读取完整W32截止值并按S32判负，随后4589B0清同一报告槽。403E93容量失败在这些状态动作之前。原六处73ECF4引用为MainLoop读取、枚举读写和两报告叶读写；本次只取这组引用及上述限定指令，不重复导出父正文。

依据既有明文合同及工程委托，实施决定为：在QueuePlaintextProtocolBytes轮询和当前队列容量门禁通过后、复制正文之前，保留D9F2D0严格1时清零的优先分支，其后按原三条件采样并清gWindowReportValue。当前明文接收无D9F2D0置1生产者，不制造置1或模拟旧CR首标记；保留已有状态优先级不增加协议字段。截止值判负包括其完整W32负位型，不改成仅等于0xFFFFFFFF。

仍直接发送原命令、原字段和LF，不发送13／FC／随机首字节、W16外壳或滚动状态尾，不为退休的随机首字节恢复RNG调用。原PGXE参数、MainLoop发送及写截止值时点、窗口枚举、唯一共享owner、队列长度／部分发送和失败返回保持；门禁失败时不清报告或首标记状态。三叶在当前发送路径内联，不新增重复包装或影子状态。

本步验收范围仅为这段缺失副作用的实际接线和静态条件／顺序；不声明403E50整根等价、全收发和全部正常寿命通过，不关闭NETWORK／LOOP／U10／V03或整体门禁，不构建／测试／运行／查产物／摘要或引依赖。

<a id="window-report-plaintext-source-state"></a>

### NWR01 源码落地

QueuePlaintextProtocolBytes已在两门禁之后、原正文复制和长度发布之前接回首标记优先及报告三条件／清零；所有发送字节、原PGXE和MainLoop保持。当前原六条报告叶指令共23B、父限定条件及唯一owner均可定位，旧403C50／403E50正文只作历史对照，注释已明确当前无调用。

全量表三叶的提供者改为活动发送内联操作，两旧帧备注及受影响行号同步；语义记录仅更新这三叶的当前来源和边界。登记两条限定I／S及三项内联合并关系，NETWORK补三实际成员；全部任务、六类closure、功能成员枚举及完整门禁保持。源码限定补丁逆向恢复提交前全文、条件／时点／失败出口和UTF-8检查通过；没有编译或运行结果，不称真实报告清零或互通运行已验。

## 1. 唯一允许的传输差异

| 边界 | 新端做法 | 继续保留 |
|---|---|---|
| 发送原401960位置 | 直接传递原非加密组包已生产的完整明文字节，不调用加密、不生成加密专属填充/变换 | 原命令变换、字段/状态/窗口报告及正文顺序；不能只用token字符串代替完整组包 |
| 原包头 | 沿原W16读写方式和长度计数约定；包长记录这次实际传输的明文正文长度，不含原2字节头 | 原头结构/字段宽度/字节序、发送区与长度owner，不增加版本/标志/校验字段 |
| 接收原401AB0位置 | 原整包可用后直接取该明文正文作为原“解密后输出”对应输入 | 原容量/返回/派发、CR/LF、NUL、队列消费和共享状态，不调用解密或伪造解密成功码 |

包头计数随实际传输字节变化是旁路算法所需的长度对应，不把原长度字段重新解释为含头总长；发送/接收采用同一原约定。没有证据的字段算法仍按原指令补证，不能自行定义默认值。

401960/401AB0继续[范围排除](cg2d_network_cipher_scope.json)。父入口403E50/403C50保留真实非加密职责；不写空加密器、假解密器或固定成功实现来保留旧调用。

## 2. 不允许顺带修改

- 不改为CR/LF分包，不加自动探测、协商、静默回退或服务端新协议。
- 不改原字节编码、转义、空格、命令变换4038A0/403A70、状态字段、正常错误消费或业务含义；算法专属加密填充与业务正文必须按证据区分。
- 不新增N>=1、0x3FFE等包长接受规则，不擅增/减现有缓冲容量、钳制/截断、丢包或重同步策略；原正常容量失败照旧，异常域仍按RPLAN.24。
- 不为适配密文对端恢复加解密，不修改服务端；实际互通留待具备相同明文模式的对端验证，不能把“原结构兼容”称为可直接连接密文模式服务器。

## 3. 收发顺序

发送：原命令/字段/状态生产→原窗口报告采样/清零→得到原加密输入位置的完整明文及长度→按原帧头方式登记实际传输长度→沿原发送队列/socket/部分发送/返回合同。发送区真实容量/长度和错误分支必须有证据，不凭分配总大小猜布局。

接收：原门禁和带外JPCGEXCEPTION01处理→原W16包长与“包长+2”就绪判断→直接取得明文及长度→原目的容量检查→复制→原首次末CR改LF/置D9F2D0→补NUL→508230消费包长+2→原状态/队列尾更新→原成功返回。

原解密失败的Decrypt Error分支因算法不再执行而不再产生，不伪称该分支等价；目的容量、队列、socket等非加密失败不随之删除。小于等于16B与大于16B的状态更新保持原分支；大包原593FB8→593FB0→接收尾NUL→593FB4→593FBC顺序不合并。状态从与原解密结果对应的明文字节按原算法生产，不补常量。

两父现已按[完整帧合同](cg2d_network_plaintext_frame_spec.md)取证并实现非加密职责；上述不表示全部网络功能或运行门禁已通过。具体正常读取、字段、返回值和带外标记的判断方式沿原证据，不新增保护门槛。

## 4. 单一板块实施与验收

| 小步 | 产出 / 关闭条件 |
|---|---|
| N1 原边界核对 | 对403E50/403C50标明加密专属处理、真实明文生产、包头读写/计数、状态消费者及唯一owner；复用既有原证据，缺事实继续补 |
| N2 发送父 | 真实明文帧生产、发送长度、窗口报告和返回/失败接全，不调用401960 |
| N3 接收父 | 原整包就绪、目的容量、CRLF/NUL、队列消费和状态接全，不调用401AB0 |
| N4 成对接线 | InitializeNetworkRuntime_508140绑定真实发送父；NetworkLoop_507DF0调用真实取包，返回0才分发；不用空回调，清无用加密私有依赖 |
| N5 验收 | 当前核源码/原格式/长度与状态时序；用户允许运行后做明文收发、半包/连续包、登录和V03，当前未互通 |

发送报告三入口与signed门禁见[接线SPEC](cg2d_network_exclusion_wiring_spec.md#三个发送侧状态入口的显式存储关联2026-10-02)。P09/P10源码工作项已关闭；两父154/146条、真实回调/Loop接线及局部I/W/S证据见[源码收口](cg2d_network_plaintext_frame_spec.md#frame-source-closure)。仍保留18项pending入口身份及完整互通/功能验收残余；不是算法恢复或网络完成。


## RPA01 两父原跳转表数据依据（2026-10-05）

[403A54/403C38两表](cg2d_unassigned_range_classification_spec.md#range-classification-switch)各四原W32目标，共32B、八目标，实际父分别4038A0/403A70；原cmp EDX,3、unsigned ja与EDX×4窗口有证据。两父已有NetworkRuntime源码/签名/接线不改，只追加原数据/边界区间依据，不迁原x86表/地址或恢复加解密。

本步64个X区间属于R7原字节用途分类，无网络业务开发或运行证据；原格式明文/两帧叶排除及47个网络成员保持。两原表局部依据不代完整父/所有alias/owner、SDK/ABI或完整互通验收。

## BPD01 原数值字段与明文边界（2026-10-06）

[BPD01具体SPEC](cg2d_delimited_numeric_normal_sampling_spec.md#delimited-numeric-normal-decision)/[原证据](cg2d_delimited_numeric_normal_sampling_evidence.json)。三个真实数值解析根完整104条286B/三页4绝对call14内跳6ret，26普通点16不同父分根1/13/12点和1/6/9父，只纳盘。单52D050正文恢复正常S32 value/sign与一次字节样本、下一字节先取再游标推进；52CFF0状态/52D0D0栈field两正确体保持。字符表、先乘62、任意位置负号、非法0/空0与field空-1/status空2不写输出均原样保。

这三根是原数字字段解析，base62字符表/原minus位置职责保持；正常算式和字节取样恢复不产生新传送格式、帧或字段定义，不恢复两加解密叶。128B栈字段/cap127、低字节分隔符/ordinal、状态和output写入规则均原样保；不引入UTF8/Big5猜测或严格数字校验。

三新1419–1421对应已有原根；语义1419→1422/有效ID1414→1417、关联1532→1535（41.63%→41.71%）、分类540→543、NETWORK47→50枚举open，六独立正常局部static I/S E689→695。三冻定位各1不刷新；RENDER52/YOBI56/RESOURCE41/AUDIO32暂停、pending18、队列175/需核46/暂停25、R7区间67/3061及其它门禁保；U10/R01/R04/V03四open只进展引用，60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保持。

只NetworkRuntime.cpp单Base62正文，两个正确包装/四DFI工具、其它实现/头/API/共享缓冲及帧-108handler/provider/owner保持，不新增helper或输出默认值，不复用5176B0前缀负号。撤专项U32回绕，正常S32算式/原业务字符-容量/返回保持，无额外拒绝/保护/钳制或异常模型。完整26点之父/字段-108回复-请求与状态/正常alias-缓冲及输出owner/关闭退出/SDK-机器ABI和实际互通未验；未构建/测试/运行/hash，原格式只明文、两加解密叶排除/内存CryptInt保、音频暂停，整体SPEC继续。

## BPE01 原协议整数正常算术（2026-10-06）

[BPE01 SPEC](cg2d_protocol_integer_normal_arithmetic_spec.md#protocol-integer-normal-decision)/[原证据](cg2d_protocol_integer_normal_arithmetic_evidence.json)：只恢复5176B0正常S32累计/首位sign/最终乘符号，保原base62字节表、首位负号、非法0和正确517190包装。解析不更改原帧/组包/协议字段/文本转义，不恢复两加解密叶。不能借用52D050任意位置负号算法。

两原根57条132B及158普通点独立纳盘；六上批具名证据拆回独立文件引用不增原验收，四新正常局部I/S不代真实互通。语义1422条/1417原ID、合同关联1535/3680（41.71%）、分类543、NETWORK50枚举未闭均保持；E695→699只增加两个既有根各I/S四条正常局部证据。60任务源码closed10/完整验收0、叶验收0/global_accepted=false及完整门禁保持。

## 地图窗口请求的上游参数基线（BUN01，2026-10-08）

[地图请求基线](cg2d_map_window_search_baseline_spec.md#map-window-search-baseline)完整核46AF30：搜索成功后每轮按原endY/endX/startY/startX S16、floor/mapId W32、native socket取样，交当前510710的socket/mapId/floor/startX/startY/endX/endY；调用后fresh窗口数，尾等待标志/方向/坐标/tick原序保。与窗口生产/原索引表共388条1302B/八页，正确源码、协议与共享网络缓冲保持，代码0改动。

510710当前已有UUN六整数构造/发送源码，本步只证明上游原调用点/字段序和现接线；该叶完整原端基线仍缺，不能称其原体/完整格式互通或网络运行已验，留为下一项取证缺口。不改消息名/字段/顺序/帧或编码，不引入加解密，网络原格式明文合同及401960/401AB0排除保持。FUNC-NETWORK成员/原任务状态与全网络门禁不因本上游记录变更。

BUN01必要静态已通过：四既有正确根及SceneState/相关CPP/声明/owner/网络源全文保持，源码0改动、仅新增两域S/新增I为0。四原根388条1302B/八页、五局部后窗/四尾26B用途与完整证据/机器及文档模型一致。汇总及 -Check 通过，派生实值：语义1708/有效1701、合同1819（49.43%）、处置849/E994、R7已审415/余2646、LOOP120/MAP117。旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11及完整门禁保；FUNC-NETWORK原57成员/既有门禁保持，510710原体不由本步外推。未编译、测试或运行，完整正常寿命/数值/x87/全父/callee/ABI/像素/明文互通及R8继续。命名快照：cg2d-re-map-window-search-baseline-20261008。

## UUN完整构造与整数token基线补齐（BUO01，2026-10-08）

[新基线](cg2d_map_request_original_tables_spec.md#map-request-tables-baseline)补齐上一节510710原体缺口：77条247B/两页，原命令数据55554E00为UUN；六S32字段各先capacity W32，再format token，后fresh buffer追加，末fresh buffer/socket交既有明文SendProtocolBuffer并沿返回。整数token517120全19条64B、两capacity/两destination独立取样，format→copy→原2000空格append→fresh buffer返正确全保；160入向只身份，不全面复核68父。

地图核验469290完整345B与27条CRC体、两768B原表一起归档；只CRC表恢复原512常量，协议/NetworkRuntime正文全部保持。46AF30/469290两真实请求参数和原字串/分隔/编码/帧/发送/返回保，未新建、更名或修改格式，也不恢复加解密。完整互通/全部callee/owner-alias及R8仍未验；本次新增两网络成员57→59，仅本域S，不新增网络I成果或伪闭门禁。

BUO01必要静态已通过：完整Scene只两数据初始化改原768B/512常量，类型/符号/owner/断言和四正确正文及NetworkRuntime/全部其它源保持；每个源常量与原实际字节对应，未执行生成/CRC或摘要，不称旧值错误。四根229条724B/六页、原表/字串774B、三28B用途及两个后窗/完整引用身份与证据和台账/文档模型一致。单引用检查曾因顶层单元素数组展开不匹配，已保留数组形状并重做该项通过。汇总及 -Check 通过，派生实值：语义1712/有效1705、合同1823（49.54%）、处置853/E999、R7已审418/余2643、LOOP124/MAP119/NETWORK59。旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11与完整门禁保。未编译、测试或运行，原全部写别名/160全父/正常寿命/数值/机器ABI/像素/明文互通及R8继续。命名快照：cg2d-re-map-request-original-tables-20261008。

## jmb地图回包消费基线（BUP01，2026-10-08）

[原局部接线](cg2d_map_reply_reset_sampling_spec.md#map-reply-reset-decision)核58B5D4已加载6A6D6200原命令jmb；512F31..513041的92条覆盖原比较、九次517190整数解析、socket加九字段、51302C回调及明确xor EAX0返回。现TryDispatchReply模板已绑定world::detail::Sub_509090及九I，原512E40已有1216，仅本块承接，不重复5779条总根。

回包509090完整35条108B，正文/BYTE宽度/原gate与floor及bit0消费正确保持。仅SceneState复位4691D0两组改先读齐再pub，NetworkRuntime全文和原格式/命令/字段/解析/帧及明文传送保持，不恢复加解密。NETWORK成员59→60，只新增509090与本域S，无网络I或完整门禁闭合。六reset后窗不代所有EAX消费或完整父/生命周期/互通及R8。

### BUP01 本步实际结果

本步必要静态通过：源码全文仅两取样片段、原两完整根/jmb局部接线/六后窗/两尾、两机器台账及五文档/四任务完整模型均核定；25来源/四锚点和UTF-8无BOM/LF通过。文本汇总与-Check通过，完整派生快照保持其余对象：语义1714/有效1707、文档1825（49.59%）、处置855/E1004、R7已审420/余2641，LOOP126/MAP121/NETWORK60。两I三S均仅本步静态范围，四任务仍open，完整叶验收0、整体未验收；未编译、测试或运行，不以静态替代完整互通、寿命及R8。提交固定命名快照cg2d-re-map-reply-reset-sampling-20261008，整体目标active。

## aBS地图增量回包消费与取样修复（BUQ01，2026-10-08）

[原接线](cg2d_map_incremental_reply_sampling_spec.md#map-incremental-reply-wiring)核58B5D0已加载61425300为aBS；512E40的[513041,513137)85条局部含六次517190，第七文本5171B0反转义后capacity W32→workspace基址→slot读取/517690复制，原八实参交513122回调，51312A xor EAX0返回。现TryDispatchReply六I一T及默认mask=0正确全保，既有父1216不重复整根复核，不改格式/字段/容量/编码/帧或明文传送。

509100完整197条694B/三页；一cpp四片段修入口W32持样、BYTE/mapId采样，并撤与现RPLAN.24冲突的旧整帧外-1/额外WORD策略。正常三平面解析/终止轮WORD写、地图文件消费及复位、状态与返回保，未新增协议或加解密。NETWORK60→61，新增本根I/S只计实际开发/静态，完整互通和寿命门禁仍开放；提供者1248/1420/728/1713等保，未代其实际文件写入或运行验收。

### BUQ01 本步实际结果

本步必要静态通过：完整原根197条/三页、aBS局部85条/字串/七段布局/10B用途，源码全文仅四片段、两机器台账及五文档/四任务完整模型均核定；四锚点/25来源和UTF-8无BOM/LF通过。文本汇总与-Check通过，完整派生快照其余对象保持：语义1715/有效1708、文档1826（49.62%）、处置856/E1010、R7已审421/余2640，LOOP127/MAP122/NETWORK61。三I三S均仅此根实际开发/静态，四任务open、完整叶验收0、整体未验收。旧异常-1/额外WORD已撤，既有帧零初始化适配保留并披露，不记故障等价PASS。未编译、测试或运行，不代完整寿命/文件运行/互通及R8。命名快照cg2d-re-map-incremental-reply-sampling-20261008，整体目标active。

## 鼠标追踪配置与PGXE上报基线（BUV01，2026-10-08）

[新原生链接基线](cg2d_mouse_tracker_native_links_spec.md#mouse-tracker-native-links-baseline)与[裁决](cg2d_mouse_tracker_native_links_spec.md#mouse-tracker-native-links-decision)含50E7B0五条24B配置、512CA0完整22条72B PGXE Report，两根正确正文保，仅新增本域限定S，不称新网络I。配置忽略首参，原容量→阈值→Reset调用保持；下游只已证地址拓宽为原生借用链接/修复原malloc失败发布和取样，不改网络两S32字段或协议分派。

原PGXE5B已载且源码同原文字，Report先Build→保存W32 bufferLength→Format整数→fresh同源buffer追加→fresh同源buffer发送并返回；ProtocolOutputAccess::data不缓存指针。取证十根组262条799B/十一页、25引用13父只身份与七65B用途，不证明其全部父或所有帧消费。NETWORK61→63、六总局部证据中本域仅S，V03仍open；格式兼容/仅明文、两加解密叶排除及完整互通运行门禁保持。

### BUV01 实际限定静态结果

十完整原根262条799B/十一页、14directcall/34jump/19RET及25引用13父身份、原17B/七65B用途、两源码仅十二批准片段、原T*字段/原nullptr-哨兵/链写序及失败发布和pool-capacity采样/index返回、固定24B只读视图移交、完整机器及派生差异模型、十六文件UTF8/LF/四锚点及六限定证据各25来源已通过。文本汇总与Check均退出0。
实际语义1741/有效1734、文档1852（50.33%）、F882/E1032、R7已审444/余2617、LOOP154/INPUT47/NETWORK63/LOGIN13；原分母/其他域/差队列/pending18与ECX pending7/closed11和完整门禁保。七原根和BYTE桥实修/三旧helper退休，默认/网络配置/Report三正确正文保持不计新增I；未编译、链接、运行或证明完整窗口/登录/配置/明文互通验收，整体目标继续active。

## BVI01aSEl原十三字段及正常失败合同（2026-10-08）

[原基线](cg2d_creation_request_tick_sampling_spec.md#creation-request-baseline)、[取样裁决](cg2d_creation_request_tick_sampling_spec.md#creation-request-decision)及[原证](cg2d_creation_request_tick_sampling_evidence.json)：创建启动/轮询/aSEl三完整根210条703B/五页，两个实际父局部28条119B含重叠/两X17B用途；启动与13字段组包正确保持，仅一cpp一个elapsed片段恢复先计时调用、后fresh读取共享起始W32再相减。命令aSEl、12整数/一文本按原field1..13，13次先取容量/格式后重读输出再append，末517480原返回直透；现剩余11整数循环不缓存共享容量/输出，正常发送失败仍由外层继续请求状态/tick，不新帧、字段或加解密。原完成W16回复持样/清状态、严格600000阈值/15B格式及未清状态、发送失败后的请求1/tick序保，七源中其余六源/声明及BRW回复/DZ接线全文保持。153/154已有原新更新、新1763正确组包；CHARACTER26/NETWORK64、CHARACTER两I-S/NETWORK一S各28来源，U10/V03/R04仍open；完整请求往返/其它父-callee/owner-alias寿命/机器ABI链接运行未验，原格式明文/完整门禁保。

### BVI01 当前创建请求计时取样与原格式链限定静态结果

集中静态通过：三完整原根210条703B/五页、30直接/两计时间接/两跳/五RET，两个实际父局部28条119B含重叠、两X17B逐BYTE NN_NOP用途；七整源只一cpp一个elapsed片段，先计时调用再fresh读取共享起始W32后相减，原完成回复/严格600000阈值/15B原格式点和正常失败后状态/tick序保持。启动/正确aSEl十三字段的独立容量/格式后输出重读/结果直透、BRW创建回复与DZ接线及全部其它源/声明保持。完整机器与派生仅具名差集；十五文件UTF8无BOM/LF、四锚点/三限定证据各28真实来源通过。

文本汇总及Check均退出0；实际语义1764/有效1757、合同1874（50.92%）、F907/E1082、R7已审467余2594、CHARACTER26/NETWORK64。原分类120/mixed3/完整入口排除113、BVH原新与pending7/18、closed11/差队列及完整闭包保持。只有154计时读点产生新I，153启动/1763组包保持正确且无新I；原格式明文不改，不声称实际超时运行错误或异常回绕等价。完整请求往返、其他父-callee/正常owner-alias寿命、机器ABI链接/运行与R8未验，整体目标继续。

## BVK01emRV/Godf原字段与发送回调（2026-10-08）

[BVK01原基线](cg2d_login_deletion_send_chain_baseline_spec.md#send-chain-baseline)、[实施决定](cg2d_login_deletion_send_chain_baseline_spec.md#send-chain-decision)和[完整原证](cg2d_login_deletion_send_chain_baseline_evidence.json)：登录启动/emRV/Godf/发送包装四完整根134条426B/四页、16直接八目标/3间接/4跳4RET；四正文正确，源码改动0。

emRV严格三S32(+4C/+50/+18)、Godf严格一S32注册号，各容量先于token/格式后fresh输出，末发送结果直透；原命令、字段数、空格、转义及外帧不改。发送包装当前已接D04明文回调，旧文“仍经排除加密”是早期历史，不作当前边界；两算法叶仅原排除，本链正常职责纳管。

151/752已有原新更新、1764/1765补缺失组包映射，四F处置/三域仅S各21来源、无新I；预期语义1766/有效1759、文档1876（50.98%）、F913/E1088、LOGIN15/CHARACTER29/NETWORK67，R7已审467余2594保持。原格式明文/合法NULL_FUNC缺省、排除和执行限制保持；全83发送父/其它callee、正常owner-alias寿命、完整登录/删除往返、机器ABI链接/运行与R8未验，完整门禁不解除。

## BWU01分派头比较按原字节合同承载（2026-10-08）

[实施决定](cg2d_dispatch_header_byte_spec.md#dispatch-header-byte-decision)、[原块/433B原标记与源差集](cg2d_dispatch_header_byte_evidence.json)。一个共享模板片段把宿主strcmp承接改为原两对U8/同样本NUL/加2循环，匹配才进入原解码与真实callback。

全部108原命令、参数类型/读取掩码/命令次序、接收头生产、原组包/帧与明文旁路保持，不增字段、字符转换、输入限制、保护或默认成功。两网络加解密叶既定排除继续；局部字节读取有据不代实际明文互通、完整owner/正常寿命及机器门禁。

<a id="local-login-diagnostics-decision"></a>
## CAB26 本机连接的最小阶段诊断（2026-10-09）

当前批处理原字节语法已只读确认启动cg2d_re_client.exe，IP:2为127.0.0.1:9030；当前源码未发现可据此更改原协议的差异。既有日志只有连接层结果，不能区分登录包未入队、已发送但无回复、whV回调未消费或远端关闭。本步沿用户修复连接卡住和工程委托，增加最小阶段日志以便下一次实际运行定位真实阻碍，不能将诊断开发称为已经修好互通。

只在现LoginState/NetworkRuntime使用已有Sub_44B5C0/原格式器记录：登录入队返回、登录阶段实际发送字节数、whV结果或因请求状态忽略，以及远端正常关闭。只记录固定标签和S32数值，不记录账号、密码、MAC、URL或封包/回复正文；不用新日志框架/owner/协议、协商或输入保护。既有状态/返回、发送队列/部分发送、LF/原字段、108派发和正常原调用相对顺序保持；额外日志FILE I/O/时延为明确诊断差异，不宣与原调用列表逐条相同，写失败按已有日志返回被忽略，不影响请求决策。

必要当前Debug沿既有BWZ01客户端连接授权，客户端/试包/算法运行、测试和摘要仍不自行执行；当前运行许可和新版IP2反馈仍待。本步不改批处理的musicoff或其他参数，不恢复newest/加解密；明确日志本身不提供成功、半包/断线/失败/正常寿命或整体验收证据。

<a id="local-login-diagnostics-source-state"></a>
## CAB26 当前诊断实现（2026-10-09）

LoginState已在原step重读/status1后记录JFVf入队返回；NetworkRuntime记录whV结果或原状态忽略、recv0原断线字段发布后标签，以及阶段71..80正发送/原队列消费后的字节数。只三64B数值缓冲和两固定标签，未记录账号/密码/MAC/封包或回复正文；发送字节数是该阶段全部队列，不推定全是JFVf。移除新增诊断并恢复原忽略返回表达式后，两整CPP与HEAD全文相同，批处理确为新版/IP2且未改参数；详见[证据](cg2d_local_login_diagnostics_evidence.json)。额外FILE I/O/时延为已决诊断差异，不宣原调用列表逐条相同。

当前Debug三库/客户端0错误、5既有警告（2原Winsock弃用/3原回调宽度），MAP实际入口/回调/日志存在，新诊断无警告，旧窗口/宽度问题未因本步关闭。8022528B新版更新游戏目录、旧同长版备份before-local-login-diagnostics-20261009，只核复制/长度，不比摘要/内容。未运行客户端/试包/算法、测试或工具，互通问题未称修好；原明文/LF及WAV/GBK/宋体/ASCII光标保持，运行许可和新版IP2反馈、完整网络/生命周期/机器ABI与整体门禁继续。三诊断格式不计原1023调用/241字面量；快照cg2d-re-local-login-diagnostics-20261009。

<a id="dispatch-callback-width-decision"></a>
## CAB27 派发回调的原参数宽度（2026-10-09）

CAB26当前编译的三项C4244已按原函数与实际512E40调用窗定位，沿工程委托在现有合同内修复：50EEB0和50E6C0首参均来自原fd/socket且全体不读，x64仅将这两处声明/定义拓宽为uintptr_t；前者第二个未读业务参数保W32，后者后三个S32及52CDE0返回透传保持。不得在共享TryDispatchReply截断socket或扩宽非地址字段。

50E770的原调用者517190解析后push完整EAX，原体只MOV AL、写gPukVer再返回；第二入口参数据此承接S32，在消费点显式转uint8_t，再写原W8状态并返回该字节。保原低字节消费，不扩宽状态、不改解析、加拒绝/钳制/异常模拟或通用转换工具。原高EAX24位及全机器返回不代验，已证调用者在回调后xor EAX,EAX，当前派发也忽略该返回。

完整原三根51条135B及三个实际调用邻窗有据；Kka/ftaN/PfM命令、全部字段/解码/掩码、派发次序和明文LF保持。只六个现有源码/头文件的声明、定义与一个低字节消费点，不改记录表正文、空间音效或登录诊断；无新依赖、协议或业务。必要Debug沿BWZ01既有连接授权；运行/试包/测试/摘要限制保持，本步不将宽度修复称为IP2互通已修好，也不外推各callee、正常寿命、机器ABI或全功能验收。

<a id="dispatch-callback-width-source-state"></a>
## CAB27 回调宽度实际实现（2026-10-09）

六个现有源码/头文件已落实两未读socket首参uintptr_t，以及Puk回包完整S32入口→明确低U8消费→原W8状态/字节返回。六整文件与HEAD加明确替换全文一致；三原派发Kka/ftaN/PfM表达式、记录表/空间音频正文、CAB26登录诊断、所有其他网络逻辑保持。三个完整原函数51条135B及各唯一实际入向/调用邻窗，见[证据](cg2d_dispatch_callback_width_evidence.json)。原高EAX24位与全机器返回未代验，现已证派发忽略返回。

当前BWZ01必要Debug三库/客户端0错误，NetworkRuntime及本步三处回调C4244为0；头文件传播触发更多旧源重编，其余实际97警告仍待：LegacyUiRecord11、LoginState2、YobiRuntime41、WindowRuntime43。没有抑制、替换旧Winsock API或因编译通过关闭这些缺口；不称全项目无警告。8022528B新版部署游戏目录、旧同长版备份before-dispatch-callback-width-20261009，仅复制/长度。未运行客户端/试包/测试/工具/算法或摘要，IP2问题不称已修好；原明文/LF及WAV/GBK/宋体/ASCII光标保持。完整网络/音频/寿命/ABI、成员枚举与整体门禁仍待；快照cg2d-re-dispatch-callback-width-20261009。

<a id="login-receive-diagnostics-decision"></a>
## CAB29 登录接收与派发诊断及字段名称（2026-10-09）

本轮聚焦IP2收包→完整LF行→108派发→whV→原请求状态消费，当前源码没有支持改原协议/字段顺序的收包故障证据。现有CAB26诊断能记录入队/发送和whV/断线，却不能说明正接收是否入接收队列、完整行派发的原返回；沿修复连接问题和工程委托补两个最小阶段事件，不将诊断存在称为互通已修好。

NetworkLoop正recv后保持先receiveReset=0、原AppendReceiveQueue，再在71..80登录阶段记录接收字节数和原入队返回；返回仍不参与决策。完整LF行取得后仅保存原NetworkDispatch返回，按原顺序清全部缓冲/表后再记录该返回，日志资格在派发前按同一阶段捕获，避免回调改变阶段后漏记。两个64B局部、现53B304/Sub44B5C0，不记录账号/密码/MAC、头/包/回复正文，不新增框架/owner、超时、重试、保护或协议。

原派发105个早返匹配返回0，尾iKe/kiG/TgX即使命中仍沿原-1；未知也返回-1。因此新日志只叫RESULT，不把-1改为未知/失败门禁，不影响回调、状态或返回。额外FILE I/O/时延和局部观察为具名诊断差异；原相对调用顺序、LF/队列/半行/多行/字段及全部原分支保持，不能宣新增日志后与原调用列表逐条相同。

当前本机服务端只读源码nrproto_serv.c:1672..1677按四文本原序，第二绑定passwd、第三绑定cdkey；callfromcli.c:427..438实际第二保存passwd交AuthQue.password，第三保存cdkey交AuthQue.cdkey。现客户端已按原DA3A1C、DA38DC副本、DA3A5C、适配器顺序发送，只是Sub512800形参/临时量把第二叫account、第三叫password，LoginState副本也误叫accountCopy。本步仅将这些名称纠正为password/account/passwordCopy，类型/值来源、每轮先capacity→转义→fresh输出追加/末发送及字节次序完全保持；不交换字段或更改源缓冲。服务端源码只读，不修改、重启或据此证明运行二进制身份；既有历史文字引用由本节明确更正。

必要当前Debug沿BWZ01客户端连接授权。运行/试包/测试/算法/工具和摘要限制保持；新版IP2反馈/运行许可仍待，全部原父/callee/正常寿命/机器ABI、真实互通和整体门禁不因本步关闭。两新格式属于诊断，不加入原1023调用/241字面量库存或新原成员分母。

<a id="login-receive-diagnostics-source-state"></a>
## CAB29 当前接收诊断与名称纠正（2026-10-09）

NetworkLoop正recv后持有原入队返回，按71..80阶段记录字节/返回；完整LF行只持有原派发返回，保原全部清缓冲/表顺序，随后按派发前捕获的阶段资格记录RESULT。两个64B最大51/34B含NUL，只有S32数值/固定标签，不记录身份/头/包/回复正文，不参与请求决策。-1兼含三个尾命令的原匹配路径，未作为未知/失败门禁。额外FILE I/O/时延具名披露，不宣原调用列表逐条等价。

JFVf四文本按原DA3A1C、DA38DC副本、DA3A5C、adapter原序，类型/值来源和全部容量→转义→fresh输出追加/末发送保持；只把第二/第三参数/临时量名称改password/account，LoginState副本名改passwordCopy。整NetworkRuntime与HEAD加alpha重命名/两诊断精确替换一致，整LoginState只四处变量更名，头文件只注释/形参更名；108派发/回调正文、原状态和失败/队列/明文LF保持。服务端只有源码只读消费证据，未改/重启/证明运行二进制身份，见[证据](cg2d_login_receive_diagnostics_evidence.json)。

当前BWZ01必要Debug三库/客户端0错误，新诊断/NetworkRuntime和上步LegacyUiRecord无警告；头文件传播其它重编源实际87警告，包含既有GetVersionExA新重编一项，其余86保持，不抑制/改指定API或宣全工程无警告。MAP实际登录/接收/派发/whV入口存在，8022528B新版部署游戏目录，旧同长版before-login-receive-diagnostics-20261009备份，仅复制/长度。未运行客户端/试包/测试/算法/工具、重生成WAV或摘要，不称IP2已修好；运行许可/反馈、完整网络/正常寿命/ABI和整体门禁继续。两新诊断格式不计原格式/原身份数量；快照cg2d-re-login-receive-diagnostics-20261009。

<a id="local-service-observation-20261009"></a>
## CAB32 IP2实际服务来源与运行待项（2026-10-09）

本步在2026-10-09 09:16（Asia/Shanghai）前只读取得本机监听/进程及游戏日志，未建立测试连接、发送封包或启动客户端。9030监听owner不是H盘发行目录的程序，而是下列源码目录构建程序；这是实际进程路径证据，不能外推其二进制与当前源码逐项一致。

| 观察项 | 实际值与范围 |
|---|---|
| 监听 | 0.0.0.0:9030，Listen，PID 14904；不代认证/数据库/业务健康验收 |
| 服务程序 | D:/Source/cgmsv/cgmsv_server/cmake-build-release-mingw/cgmsv.exe，参数-d 1；进程创建2026-10-09 00:15:57.128556+08:00 |
| 当前客户端 | 查询cg2d_re_client.exe没有返回运行中进程；没有可继续读取的新客户端会话 |
| 游戏systemlog.txt | 227B，最后写入2026-10-09 00:16:16.4210159+08:00；指定ASCII诊断仅CONNECT OK，没有CAB26/CAB29的LOGIN诊断。其他字节未猜编码/输出，未记录身份或封包正文 |
| 当前服务源码 | include/version.h定义VERSION_TW/CGMSV_CONFIG，nrproto_util.h的TW分支不启用nrproto__ENCRYPT；nrproto_util.c按该分支直接拆分/发送原明文。build.ninja仅对应当前源码编译规则，不能证明已运行二进制的宏/内容 |

现有JFVf四字段、whV整数/文本回复及原LF合同保持；当前未取得支持更改协议、字段、认证或超时的新增故障证据，服务端文件不修改/重启。本次将此前未知的实际监听owner与程序路径收口，运行二进制内部身份、当前客户端登录阶段和完整往返仍未证明。

V01首次补明确open记录，V03/全部功能门禁保持open；只登记限定来源证据，不新增原身份、I/W/S/R通过、编译、部署或功能完成数。CAB15已提出的新客户端运行许可仍未得到答复，按AGENTS第12条不擅自启动/试包；服务监听不能替代登录验收，也不能据旧日志宣新版仍卡在哪一阶段。整体SPEC目标继续，后续以获准的新客户端诊断/实际往返推进，不重复无变化日志或为等待制造源码修复。

<a id="local-connection-priority-confirmed-20261009"></a>
## CAC05 本次连接故障优先项确认（2026-10-09）

用户在本轮澄清中明确“此前故障已好，继续整体缺口”。本次不再把旧连接中现象当作当前未修复故障，不重复修改明文/LF、认证字段或超时。CAC04既有选人入场反馈保持；本次未提供新版本/环境/运行轨迹，不新增完整网络或登录R门禁通过。全部适用互通、业务/失败/生命周期与总SPEC验收继续保留；本线程不运行客户端/试包或测试。

<a id="neighbor-fim-plaintext-current-20261010"></a>
## 2026-10-10 NFI01：当前fIM整数分隔补正

沿[已确认本机明文格式](#local-server-plaintext-contract)落实[邻格/fIM局部合同](cg2d_neighbor_fim_request_plaintext_spec.md#neighbor-fim-decision)：现server nrproto_splitString接受0x17下个token起点却只终止空格/LF/CR，nrproto_a62toi遇该非Base62字节返回0；nrproto_serv的RECV_PI=fIM按原x/y/dir读取三字段。因此客户端510B80取消唯一17写入，保普通空格。原time/seed/rand消费/选择扫描、Base62/字段、命令和LF、日志/回调/共享buffer与队列/部分发送/原返回保持，服务端无修改。

这是按CAB05现格式修复遗留非兼容分隔，不宣原17机器码等价，不恢复W16/命令变换/加解密、不新增协议/协商/字段/容量或异常保护。源码静态判据成立，未构建/测试/算法运行/试包/客户端或服务运行/摘要；实际fIM互通及完整网络/登录/所有owner与总SPEC§10继续待验。

## 2026-11-05 商店60条的收发容量例外

用户明确要求收包64 KiB、回复容量1024字节，覆盖本SPEC第2节“不得擅增缓冲容量”在本次商店扩容所需的接收与结算文本范围，不改命令、字段、编码、分隔或服务端。平台接收队列、主线程原始接收队列、单行分发缓冲与压缩帧解码上限统一为65536字节；主线程派发区保留原起点，接收区和尾槽顺移避免重叠。平台发送队列仍为16384字节，协议工作区维持现有65536字节，解码正文累积区维持现有容量。

普通商店所有模式及两种晶石商店的结算文本调用统一使用1024字节GBK转义容量；容量含结尾NUL，转义后正文须小于1024字节。单行分发缓冲同样含NUL，含LF的单行有效字节须小于65536；超限继续走原容量失败路径，不截断或分片重发。服务端实际回复长度、压缩帧及结算互通未运行验证，不能把静态容量调整记为完整收发验收。
