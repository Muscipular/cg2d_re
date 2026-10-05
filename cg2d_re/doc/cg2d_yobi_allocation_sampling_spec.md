# YAS01 Yobi 分配与插链取样 SPEC

<a id="yobi-allocation-sampling-baseline"></a>
## 原端基线与缺口

沿[YSS01哨兵初终](cg2d_yobi_sentinel_sampling_spec.md#yobi-sentinel-sampling-source-state)、[YLC01摘链回收](cg2d_yobi_cleanup_sampling_spec.md#yobi-cleanup-sampling-source-state)的同一链生产端推进。原404AE0全体[404AE0,404C42)，118条354B，一chunk；113个type17调用、86个已知父函数及1个未归属调用点，见[原指令/引用及源码差异](cg2d_yobi_allocation_sampling_evidence.json)。完整保存不证明全部父实参、载荷类型和寿命/运行已验。

调用点4FAAA5为已证type17 `call 404AE0`，指令头/5B字节与周边[4FAAA0,4FAAB5)十条21B代码已保存；当前数据库get_func(4FAAA5)为空，前一函数末界4FAA95、后一函数起址4FACE0，不将空归属计为父函数，也不据相邻入口猜父身份或修改数据库。113调用点全保留，父计数仅统计非空已知86项。

原事实和当前差异：
1. action用cAlloc(1,0x330)，空值路径依次两条原MessageBoxA消息后返回空。U32 payloadSize为0跳过载荷分配；非零调用cAlloc(1,payloadSize)，原404B44比较返回EDI、404B46发布至action.payload，再据该结果分流；成功REP STOS用EDI快照按完整U32字节数再次清零。源现发布后复读action.payload判断，再复读供memset。
2. 默认值在404B96–404BFF按原序写callback0/U8 priority/S32 graphic=-1/cache=-2/frameRate100/F32 scaleX/Y=1，随后明确写16B描述符：偏移0/2/4/6各W16零、8处W32 FFFFFFFF、12/13各W8零、14处W16零；然后batchType低W8零、color2 S32 -1、rebirthValue1/2低W8零、walkSpeed100。源现描述符仅4个W8 FF写，其余依calloc零值，未承接原显式字段写入/宽度。
3. 404C05先读共享start，404C0A读end，404C10读取start.next，404C13读取end.next作stop。源现start及其next先读，再读end及其next，未固定原两指针先取样。
4. 找到插入点后404C2D保存point.prev，404C2F/C32分别发布action.next/prev，404C34再次读point.prev，再404C36发布当次前驱next，最后404C39发布point.prev=action。源现贯穿使用第一次前驱指针，省略两次新节点链接发布后的复读。

按完整U8 priority做无符号<=扫描，同优先级FIFO；若初point==stop或扫描到stop，返回新action但不插链。完整W8包括FF，不据早期“有效业务priority<FF”的描述增拒绝/钳制/改空返回/自动free；本合同只明确原分支，不扩展业务输入或设置新owner。

<a id="yobi-allocation-sampling-decision"></a>
## 工程裁决与实施边界

依AGENTS.md第8条委托采用，仅四具名源码片段：
- 载荷分配返回保存void* payload，以其值先算missingPayload，再发布到action.payload；沿既有失败分支先free实际action、原一条消息/空返回，成功用同快照memset。保原零尺寸跳过、U32数量和再次清零，不能省略已证清零或新增分配保护/失败回退。
- 描述符以现有16B bytes值类型承载，不添公开类型/接口；局部U16零和U32 FFFFFFFF分别通过memcpy发布0/2/4/6四W16、8处W32，12/13各直接W8零，再14处W16零。现行Win64小端、原字节/宽度与各发布时点明确；不整体memset/聚合一次覆盖这些字段，不改变其它默认值顺序或添加未知字段。
- 插链入口显式start→end→start.next→end.next；stop保持一次缓存，扫描使用当前point的U8优先级与next。不改比较、排序算法/首个更高优先级前插入或两未插链返回分支。
- 首次previous供action.prev发布；action.next/prev写后取currentPrevious=point.prev，向此时前驱写next=action，再写point.prev=action。保持四次链接发布顺序，不把前驱快照贯穿，也不提前发布到全局头/尾。

沿已有批准应用std::calloc(1,sizeof原生YobiAction))/std::free域，只拓宽已证指针；priority仍U8、payloadSize仍U32、非地址标量不扩宽。主失败两消息、载荷失败消息/释放、本体返回原生指针与所有其它源码/头/汇编、YSS01/YLC01保持。内存数值载体、网络原格式明文和音频暂停不随此泛化变更；异常不新增拒绝/补零/钳制/回绕或故障模拟/称等价，原显式正常零写不作为新异常保护。

<a id="yobi-allocation-sampling-acceptance"></a>
## 可度量验收

1. 完整118条354B与113type17call/86已知父+1未归属调用点、库存/chunk可追；只本组分配/默认字段/插链局部，不能代全部父实参/载荷类型和寿命/SDK运行。
2. 全文只精确重放四片段；载荷一次返回快照→原比较/发布/同快照清零、两失败路径消息与释放保持；描述符五W16/一W32/两W8各按原序显式发布，默认标量/回调及U8/U32保持。
3. start/end先读、两next后读；首次prev用于action.prev，两新节点链接写后复读point.prev再发布前驱next、最后point.prev；unsigned<=、同值FIFO、缓存stop及两未插链返回不变。对未知输入/异常不新增保护或模拟。
4. 既有404AE0语义只追加本组来源/证据/边界，所有旧字段保留，新增一adapted/MAP与LOOP同ID，四局部I/S共用同一源修正/原证据，不增语义条数/有效ID或计113调用点全部通过。
5. 六open任务及全部完整门禁/功能/R1/R7/R8/整体不升级；仅获准文本汇总/-Check、相关引用/UTF-8无BOM/LF/精确提交，不构建/测试/运行或计算摘要。

<a id="yobi-allocation-sampling-source-state"></a>
## 源状态与剩余

真实源码NewYobiAction_404AE0及既有include/cg/YobiRuntime.hpp声明；单一原生action/payload和共享哨兵链继续，各已有实体/附件/战斗/UI实际生产调用保持。descriptor bytes仍16B原值类型，memcpy仅表达已证W16/W32发布，不另立owner或平台业务接口。

本组静态结果归档唯一进度与登记簿。113调用点所有实参/载荷类型及全部callee、原生分配域实际调用与资源/对象生产、链别名/未插链对象和owner重进退出/借用失效、真实ABI/SDK/PC53/时钟RNG/完整画面运行未闭；完整体保存和局部源实修不代这些验收。原格式明文/音频暂停与现行不执行限制保持。固定快照`cg2d-re-yobi-allocation-sampling-20261005`。
