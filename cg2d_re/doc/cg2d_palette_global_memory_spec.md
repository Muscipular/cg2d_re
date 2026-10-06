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

## BOR01 初始化数量与取得表发布补充（2026-10-06）

[BOR01 SPEC](cg2d_independent_palette_initialization_sampling_spec.md#independent-palette-initialization-sampling-decision)/[本步证据](cg2d_independent_palette_initialization_sampling_evidence.json)。1237/522B30完整94条286B/两页6call-7内跳/1ret，两个普通入向；原指令继承PGM01完整归档，本轮核边界/入向及缺口取样。hidden58B7E8..58D348全7008B/1752W32与60源段初值逐项一致，保持可写表。初始化落实queueCount/首分配、clearCount先索引pub/实际取得表清零、recordCount与lock后initialCount先记录pub、逐项records→index→recordpub→fresh index loaded0、尾fresh count先index++比较。正文1/来源注释1，正常值和旧SDK适配/释放根保。

本步更新原PGM01“初始化正文保持”的历史状态：当前只按BOR01恢复初始化取样，GPTR/GlobalLock接口及PGM01释放根完全保持，不把局部发布取样外推为所有SDK/owner寿命已验。

1237完整旧PGM01记录/分类保留续记，既有source2/历史定义1不变，语义1404/有效ID1399及1517关联41.22%、分类522/RESOURCE41保持；两局部static I/S E649→651。没有新增原函数或成员，七open仅进展/引用，60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及全门禁保持。

仅正常局部static，native index16B仅首T*拓宽、loaded仍U8、记录0xB04无指针/count S32保，字节量在已批native API边界。保两GPTR/GlobalLock后才判初始数量，不新做空指针/失配/溢出保护、旧池回收/回滚。全部父/真实SDK/queue-DBF两表owner-正常别名/画面运行继续推进；未构建/测试/运行/hash，网络原格式明文、音频暂停。
