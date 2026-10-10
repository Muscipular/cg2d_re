# 聊天窗口输入创建、按钮与滑块 SPEC（CWC01）

2026-10-10。沿CWP01核窗口35实际初始化与按钮/滑块链，依子AGENTS第8条先记原事实和裁决。统一cg2d_re分支、不打tag；保当前GBK、原格式明文、MSVC直接CRT和WAV-only，不恢复构建/测试/运行。

<a id="chat-control-baseline"></a>
## 原根和真实原型

| 原根 | 完整范围（尾不含） | 指令/字节 | E8/跳/RET |
|---|---|---|---|
| 4A6DD0，聊天输入创建/初始化 | 4A6DD0–4A6E98 | 51/200 | 5/3/2 |
| 4A6FC0，按钮操作 | 4A6FC0–4A755C | 400/1436 | 21/51/10 |
| 4A8140，滚动滑块位置 | 4A8140–4A81C5 | 41/133 | 3/2/2 |

共492条1769B/九页、29实际E8/14目标、56跳14RET。只有滑块四实际入向（本步按钮内部3/CWP01几何外部1），初始化与按钮是原型数据绑定，不能把IDA整结构/数组起点的单个数据xref当唯一回调字段。

原57C130第35指针指向54EAB0；其48B原型有20元素/570CB0、初始化4A6DD0、绘制4A82C0和释放4A4210。20项元素每项32B，callback在+1C，3..7/10..14/19共11项实指4A6FC0，现有原生原型保持。12槽分派数据4A755C依index3..14对应原完整分支，8/9/11/12进default；已属CAC18旧X:000A755C-000A7590，仅复用/核原字节，不新增X处置或把余部当新函数。19虽绑定同回调，仍按原外层范围返回0；不新增按钮行为。

<a id="chat-control-decision"></a>
## 输入创建及按钮正常合同

4A6DD0参数恰-1时入口窗口先y再x→57E970/mode3创建弃返→新窗口/元素0 data发布焦点→同窗口重新取元素0 data写真实626AA0 context→SetContext→返1；其它参数按现获批设备边界恰1决定先元素18、再新窗口元素17可见1/0→CWP01几何→新窗口先y再x→同原型/mode2创建弃返→返1。两个原返1和未检查创建结果保持，不补保护/失败回滚或固定新行为；现有WindowInputData原生context指针承载不变，正确正文保持。

4A6FC0先C9A96C W32门禁，零返0；各case原低位及400/800按钮位、提示/图号/SE51或55/320/240和consumed结果保持。字数/行数四请求叶已在BVP01完整纳管，复用旧1784..1787，不重取/改正文；同一state可顺序触发两操作/两SE，不能合并互斥。范围1..5环、字号0..2环、颜色跳4及0..9环、音效后重新读颜色再写626840、范围提示寿命180、上下滚动原count/0边界及后滑块保持。原门槛/环/滚动上限归位是业务事实，不能删或扩为新钳制。

三处窗口取样修正：case3原先当前window→hover判位→elements→分支内data，现条件先于窗口/元素读取，恢复持有window/判位/elements后分支；case6原window→hover判位→elements/data持有，再提示/模式图号，恢复该序；case13原window先于hover，再经该窗口写元素11图号，恢复先持有window和hover取样。三处仅在原初始图号/提示范围持有，不跨按钮请求/音效复用，后续越界提示分支仍新取当前窗口。

另case13原先scroll W32→count W32→scroll+1/比较/发布，现先加一再读count，恢复先持有旧scroll、再取count、再算新scroll；不将count提前到按钮门槛外或跨音效/滑块缓存。两逻辑组共四处，条件值与发布/返回序保持；不改case14、其它按钮正文、当前GBK提示字节或引入默认分支。

## 滑块数据与浮点链

4A8140先FindWindow35，原null返；持有该窗口，先scroll→elements→count→4742C0弃返→新count→该窗口新elements。count==0按元素14 y-14写元素8 y返；非零重新取scroll的S32转浮点→元素10 height-14 S32→重新取count做浮点除法→乘span→原ftol低S32→元素14 y-position-14写元素8 y。现有double与int64后低S32承载保持，不改整数先乘后除或新造私有ST0算法；不据此证明全FPU精度/控制字/状态、机器ABI或运行像素等价。正确正文保持，原null/count分支不增减。

两逻辑组四处/三定义和三真实声明六关联注释。初始化声明在WindowPrototypeDependencies.hpp，按钮/滑块声明在WindowRuntimeState.hpp；真实签名/原型绑定/布局/owner保持，不新增函数/API/类型/字段或分配释放。RPLAN.24不新增异常/越界/溢出模拟、保护/默认值/补零/钳制/容量限制/回滚；原门槛/环/边界归位/返回保。

<a id="chat-control-acceptance"></a>
## 限定静态验收

核三完整原根/实际E8/56跳14RET、四普通入向、窗口35/20项原型11真实callback字段和旧12槽表，与当前不改原型对应；整WindowRuntime.cpp/两真实内部声明头精确重放两组四处/六注释，两个正确body和所有其它source/旧原型与范围处置保持。

旧1969全部CAC18来源/表/分类子记录保历史追加，两个新语义/三个F；INPUT/UIRUNTIME各三成员、NETWORK仅按钮作为CTS01范围/字号/颜色生产，三限定S零I/九任务open。全枚举/完整gates/六closure/总SPEC§10不变。

只必要文本静态和measure_cg2d.ps1/-Check，不构建/测试/运行客户端或算法/产物/摘要/依赖。完整窗口父/创建释放/绘制、焦点/输入/元素/载荷/请求/日志数量/scroll/hover/范围字号颜色/音效与CTS01网络全部生产消费和alias-owner寿命、CRT/SDK/FPU数值/机器ABI/优化后读取与真实操作/输入文字声音画面及全生命周期待；关联率不是完成率。

<a id="chat-control-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐聊天几何生产并恢复设置字节取样”；拟提交说明“[#] 补齐聊天窗口控制并恢复按钮现场取样”。三完整原根/窗口35与11真实callback/旧12槽表及两组四处修正、登记、静态汇总/Check和派生范围核对通过。语义2247=2199F+48X，函数处置1686/证据1482；文档关联2309/3680=62.74%仅关联率，INPUT69/UIRUNTIME101/NETWORK88且枚举未闭。184叶验收0、60任务关闭11/开放49、六closure开放及所有完整门禁保持。未构建/测试/运行，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_chat_window_control_pipeline_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐聊天窗口控制并恢复按钮现场取样」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x4A6DD0 | 0x4A6E98 | 51/200 |
| 0x4A6FC0 | 0x4A755C | 400/1436 |
| 0x4A8140 | 0x4A81C5 | 41/133 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
