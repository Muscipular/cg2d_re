# 独立调色板全局内存生命周期 SPEC（PGM01）

2026-10-05，修正释放包装省略原共享指针重读和第二组取样时点的问题。
上位：[图形x64合同](cg2d_graphic_dx_cache_x64_spec.md)、[追踪体系](cg2d_traceability_acceptance_spec.md)。
固定快照：`cg2d-re-palette-global-memory-source-20261005`。

<a id="palette-global-memory-baseline"></a>
## 原事实和源码缺口

[原完整两根](cg2d_palette_global_memory_evidence.json)共126条/377B：522B30初始化94条/286B，522C50释放32条/91B；各两个直接入向。
522B30先队列/索引读取、发布W32数量，再两次GlobalAlloc(0x40/GPTR)→GlobalLock；
首8B索引表原清零，第二无指针记录步长0xB04；各索引发布对应记录引用及loaded低W8=0。
既有x64索引16B只拓宽首T*，loaded为U8及后三字节保留，记录仍0xB04，现有分配正文/字段合同不改。

522C50先GlobalHandle(records)→Unlock，随后**重读records**再GlobalHandle→Free；
再取index，在清records指针前保存该值；以保存index进行Handle→Unlock，随后**重读index**再Handle→Free，最后清index。
现ReleaseLockedGlobalMemory值形参固定两次GlobalHandle的入参，调用方还在清records后才取index，省略原取样位置。
不假设这些共享owner在API之间必不变化，按已证原读取/写入序表达。

<a id="palette-global-memory-decision"></a>
## 实施裁决

在既有StartupCommon.cpp/Startup.hpp拆出UnlockGlobalMemory及FreeGlobalMemory，分别只表达Handle→Unlock和Handle→Free。
移除仅两处调用的旧组合接口；不新建工具文件、类/句柄表/回调getter，不把Win32类型带入资源层。
GraphicRead释放顺序改为：

1. 用当前records解锁；重读当前records再释放。
2. 保存当前index，之后清records。
3. 用保存index解锁；重读当前index再释放，之后清index。

保留原八次SDK调用、结果弃置、无条件清指针及失败路径，不增NULL保护、重试、回滚或额外释放。
两个owner、原独立分配域、队列/DBF及其他缓存借用保持；不把SDKGlobalFree改成free/delete。
PGM01只改变释放中的取样/接线；两根登记原新基线，源级x64承载和SDK职责差异沿既定图形/边界裁决，不宣称实际ABI等价。

<a id="palette-global-memory-acceptance"></a>
## 限定静态验收

核三源/头精确片段、旧接口无活动引用、新接口仅两解锁/两释放调用，资源层的四个owner取样点/两清空点原序明确。
核原两完整根126条/377B、四直接入向、四申请/锁定及八释放API点；索引16B/记录0xB04和初始化其余正文保持。
补两语义/两源码处置、限定I/S证据及资源功能成员/合同、对应开放任务与唯一进度；生成/Check和新增引用/编码有效。
不构建、测试、运行或计算摘要；原格式明文、音频暂停、单线程及RPLAN.24保持，不加异常保护或称故障等价。
完整父/模式重建、全部上游/间接alias/缓存借用失效、SDK/ABI/真实生命周期及运行仍待，R1/R7/R8不关闭。

<a id="palette-global-memory-source-state"></a>
## 本组状态

组合释放接口已拆成两个SDK阶段，两个共享owner按原取样点重新读取，第二owner在第一指针清空前保存。
分配、存储/字段、原SDK域和失败清空顺序保持；本根源码及基线可追，完整图形寿命/SDK/运行未验收。
