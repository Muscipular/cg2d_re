# BOR01 独立调色板初始化与索引发布 SPEC

2026-10-06：沿BOQ01 DBF生产与PGM01两池分配、BOM01消费者推进初始化缺口；按委托先记裁决再实施。保持已有SDK/私有容器批准边界。

<a id="independent-palette-initialization-sampling-baseline"></a>
## 原基线

522B30..522C4E / 既有语义1237 / F:00122B30，单chunk完整94条286B/两页、6call/7内跳/1ret；两个普通入向4577D8/父4576B0和466D15/父466B80。完整指令继承已归档[PGM01原证据](cg2d_palette_global_memory_evidence.json#/original_bodies/0)，本轮只核当前函数边界/入向与此前未闭的具体取样、hidden初值，不重新分析旧SDK释放根。

[本步证据](cg2d_independent_palette_initialization_sampling_evidence.json)保原全根分页、旧1237完整记录/原分类/定位、改前正文、两个补丁、hidden原58B7E8..58D348连续7008B及60个源常量段。1752项W32初值逐项一致，未改值；对原常量字节作静态初值对照，不执行CPP初始化或算法样例。

原522BC9先读count，522BDB才pub索引owner，清零用锁定返回的实际表；522C09先读count，522C0F才pub记录owner。逐项522C1E先记录基址、522C24索引基址，522C32pub实际record，522C35重取索引后522C3B清loaded，522C40 count样本先522C46 index++/比较。现源未显式表达这些阶段，本步恢复，不将正常同值误报成已观察到的颜色错误。

<a id="independent-palette-initialization-sampling-decision"></a>
## 本线程裁决与实现

沿既有批准宿主queue clear归起点，逐原hidden1752项S32样本插入，不排序/过滤/去重，再call522900。保存S32 queueCount并发布gIndependentPaletteCount，首GPTR+GlobalLock索引分配以该样本计量；lock后fresh clearCount先索引owner发布，以实际allocatedIndex清sizeof(nativeIndex)*clearCount。随后fresh recordCount计量0xB04原记录，GPTR+lock返回后fresh initialCount先records owner发布；index0，initialCount>0才进入逐项。每项先fresh记录基址T*再索引基址T*，用本项原record地址pub保存索引槽，随后fresh gIndependentPaletteIndex写loaded低W8=0；fresh nextCount先index+1，以该同样本S32比较决定继续，最终返原index。x64仅索引首T*拓宽8→16B，loaded仍U8/+8，records无指针0xB04与count S32保持；字节量按已批native分配边界，不新做溢出/失配保护。两owner不增null检查/释放旧池/回滚，GPTR/Lock适配与PGM01释放root不改。hidden表60段全1752W32原字节初值一致保持可写；本根不代私有容器内部/全部父、实际SDK/重建寿命/运行。

正文整体一个补丁，分别落实queueCount/首分配、clearCount/索引发布及取得表清零、recordCount/记录分配、initialCount/记录发布与首次判断、逐记录指针和尾数量取样。另一个补丁只更新hidden表来源注释，60段/初始化表达式/初值保持。原空/非正数量仍完成两次分配+锁定后返0，不前置拒绝；动态循环尾每次重取数量，不冻结全程长度。

PGM01原完整SDK/布局记录保，按当前已批GPTR/GlobalLock helper承接，不新增SDK方法。索引先Wpointer后loaded Byte的两个发布保，不能合整项赋零或持有跨项索引槽引用。源公开头、释放根及其它色表/DBF正文不改；未模拟异常/allocator回调或宣所有owner别名和失败时序等价。

<a id="independent-palette-initialization-sampling-acceptance"></a>
## 可度量局部验收

| 项目 | 本步条件 |
| --- | --- |
| 原与数据 | 完整94条286B/两页6call7内跳1ret、两入向；7008B原表/1752W32、60源段逐项一致 |
| 源码 | 整GraphicRead.cpp仅初始化正文1与来源注释1；原表、其它根、SDK adapter和头保持 |
| 历史 | 1237续记完整PGM01旧记录/原分类，source2/原直接定义1保持；不新增函数或新成员 |
| 计量 | 语义1404/1399及1517关联41.22%、分类522、RESOURCE41保持枚举open |
| 局部证据 | 本根RESOURCE独立I/S共2条static E649→651；runtime=false，原全门禁未过 |
| 完整验收 | 七open只引用/进展，60源码closed10/完整验收0、member_inventory_closed=false/gates空/global_accepted=false/accepted_leaf_features=0保持 |

仅必要静态全根/初值/完整源码重放/台账增量，执行measure/-Check；不构建/调试/查产物、不新增或执行测试、不运行客户端或算法样例，不计算hash。RPLAN.24不加越界/溢出/故障或保护策略；网络原格式明文、内存CryptInt保、音频暂停。

<a id="independent-palette-initialization-sampling-source-state"></a>
## 当前状态

本步落实独立调色板初始化的数量/取得表/record与loaded发布，保原1752项可写初值及旧释放SDK边界；命名快照 `cg2d-re-independent-palette-initialization-sampling-20261006`。继续真实上层初始化/重建、队列-DBF-两表owner与绘制借用消费；局部static不关闭完整寿命/资源画面运行或整体SPEC目标。
