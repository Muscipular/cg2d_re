# 分块内存比较的取样与共享返回 SPEC（MRC01）

2026-10-05，补齐54B160原W32复读取样的编译器约束及54B1AC的当前源码归属。
上位：[比较/字符串合同](cg2d_legacy_string_compare.md)、[边界分类](cg2d_function_boundary_classifications.md)、[追踪体系](cg2d_traceability_acceptance_spec.md)。
固定快照：`cg2d-re-memory-compare-sampling-20261005`。

<a id="memory-compare-sampling-baseline"></a>
## 原事实与表达缺口

[原完整两身份](cg2d_memory_compare_sampling_evidence.json)：
54B160的两chunk为54B160..54B1AC、54B1AD..54B20C，共77条/171B；
独立元数据54B1AC..54B1AD是C3/RET，一条/一字节，当前入向为54B166条件跳转和54B1AB顺序落入。
前者EAX为Count0，后者正常数量已减为0；不能为共享RET造独立返回0函数或套用NOP处置。
父入口当前直接xref为空，仅记录事实，不据此判不可达、排除或补音频caller。

原Count保持U32；未对齐走U8，双4B对齐走W32。
54B1B7先逐W32比较，失配后54B1BB/54B1BE再次读左/右W32；尾1..3B在54B1E6/54B1E8仍各读W32再按实际数量及00FF0000掩码判断。
现有CompareLegacyMemory_54B160已表达这些正常分支、精确-1/0/+1与Count0不读；
对齐判定却使用std::align/space=4构造舍入指针，调用合同并未证明短缓冲总有四字节存储。
原54B174/54B176只做地址OR后取低两位，不形成新指针；这里须按原比较表达承载。
六个W32读取表达式使用普通std::memcpy，没有显式防止复读合并或跨阶段重排的编译器约束。
本组解决这项源码表达缺口，不把未检查的生成代码记作已发生错误或已修复运行。

<a id="memory-compare-sampling-decision"></a>
## 实施裁决

在既有LegacyMemory.cpp匿名namespace增加只供本核心使用的ReadMemoryComparisonWord。
使用std::memcpy取得完整U32载荷，前后各放std::atomic_signal_fence(memory_order_seq_cst)；
六个W32读取点改为顺序调用该helper，首次比较、失配复读、尾读取三阶段各保留左后右。
只新增标准库atomic头，不新建工具文件、不引入外部依赖、原函数ID、全局状态或锁。
对齐分类改为比较边界内直接取两个uintptr_t地址的OR低两位；不保存整数地址、不截W32，也不构造对齐指针。
未对齐U8、原数量/掩码/返回和所有原正常控制流保持；不增加空指针/容量保护或有界尾读取。

本机xmemory:2732确认std::align实际形成char*+偏移，现移除该不必要的指针计算；其他核心的std::align不改。
本机MSVC 14.50.35717的atomic:268..272对非relaxed调用_Compiler_barrier；
xatomic.h:56映射到SDK内部_ReadWriteBarrier，公开代码使用标准接口，不直接调用弃用内建。
[标准接口文档](https://learn.microsoft.com/zh-cn/cpp/standard-library/atomic-functions?view=msvc-170#atomic_signal_fence)及
[编译器屏障说明](https://learn.microsoft.com/zh-cn/cpp/intrinsics/readwritebarrier?view=msvc-170)支持此SDK边界判断。
这里核本机标准头的编译器约束；不引入硬件fence、原子对象/跨线程同步或强转W32对象别名。
实际生成的读取宽度/次数/指令和ABI仍属于总SPEC运行前门禁，当前不检查产物。

父入口当前处置记adapted，共享RET记merged并绑定该核心的Count0/正常相等返回。
边界JSON只更新records/3的当前源码归属/证据，原ID/几何/字节/入向保持；
父语义沿同一记录更新，新增共享返回语义，不另造空函数或源码caller。

<a id="memory-compare-sampling-acceptance"></a>
## 限定静态验收

核原父77条/171B及共享RET一条/一字节、两chunk和两个入向，六阶段源读取关系与实际掩码。
核两源/头限定补丁、原地址低两位对齐判定、标准atomic头、helper内两signal_fence/一U32载荷复制、核心六次调用/阶段左右序，其他控制流与未对齐分支保持。
核本机标准头引用、原库存/分类与两身份，语义/处置/开放任务/唯一进度同步；派生汇总生成和Check、增量引用/UTF-8无BOM/LF有效。
不构建、测试、运行或计算摘要，不把源码/SDK头作为实际生成ABI或完整CRT/运行通过。

<a id="memory-compare-sampling-source-state"></a>
## 当前边界

本组落实原低两位对齐判断、比较W32取样表达及共享返回归属，已有比较算法保持。
原尾W32读及首三字节相等后按最后CF选-1/+1保持，异常不专项处理，不评越界故障等价。
全部直接/间接caller与alias、实际读取指令/ABI、完整CRT及运行未闭；音频继续暂停，原格式明文、单线程和RPLAN.24保持。
R1/R7/R8及整体SPEC继续推进。
