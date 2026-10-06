# BOP01 自动地图调色板取色生产 SPEC

2026-10-06：沿BOO01 ReadPalette的真实consumer推进取色生产链；依工程委托先记方案、本线程裁决实施。只补已证原取样/累计/组包和首色/候选计算阶段，现有正常数值结果一致，不误报值错误。

<a id="automap-palette-color-production-baseline"></a>
## 原基线

| 原入口 / 本步语义 / 原ID | 当前符号 | 指令 / 字节 / 页 | call / 内跳 / ret | 普通入向 / 父 |
| --- | --- | --- | --- | --- |
| 45A190 / 1401 / F:0005A190 | Sub_45A190 | 76 / 244 / 1 | 3 / 15 / 1 | 1 / 1 |
| 451D80 / 1402 / F:00051D80 | GetGraphicAutoMapColor_451D80 | 101 / 261 / 2 | 2 / 7 / 3 | 2 / 1 |
| 451E90 / 1403 / F:00051E90 | FindNearestPaletteColor_451E90 | 96 / 263 / 2 | 0 / 5 / 1 | 1 / 1 |
| 合计 | 三新机器映射，既有真实源码根 | 273 / 768 / 5 | 5 / 27 / 5 | 4引用、父分根列 |

[证据](cg2d_automap_palette_color_production_evidence.json)保五页原指令/连续字节/chunk、四普通入向、三个完整改前正文和三个补丁。所有业务常数来自原指令立即数/偏移，没有外置算法表，不运行样例。真实链45A130→45A190→450A80(BOO01)/451D80→45A900(BOL01)/451E90；本步三根纳管，不将调用父或已证邻根重验计入新完成数。

<a id="automap-palette-color-production-decision"></a>
## 裁决与源码阶段

### 自动地图颜色表生产：保留正确正文

入口先保存S32 gMaxGraphicCount，1024Byte局部Palette256清零；ReadPalette(0,palette)失败原返void并保持已有自动地图色。成功全500000Byte gAutoMapColors清0，入口count<=0结束；否则保存当前GraphicInfo native基址，固定count循环/每项32B步长。先S16 fileIndex再S32 mapNo，非0/1/6/8写该mapNo U8零；合格mapNo0写槽0；1000..19999调用当前record.seqNo的S32位值及同局部palette，低U8发布旧区mapNo；220000..265535同调用发布mapNo-200000。其它mapNo不发布。两有效调用之后才fresh gGraphicInfo，下一记录使用重取基址/原游标；不提前重取、整表快照、补guard或更改原业务范围。现有正确body保持；未重做LoadAutoMap父/auto.dat/完整map消费运行。

原局部palette确有rep stosd全1024B清零，当前初始化保留；禁止因上一根ReadPalette局部path未清而推断本根也应不清。成功后的500000B清色与两个业务map区保；不重建表格式/封包、缩小全表或增加负mapNo限制。

### 图形平均取色：两处阶段恢复

ReadGraphicDataByIndex(index,&gGraphicDataPtr,&width,&height,null)原接线保；失败W32 gGraphicDataBuffer=-1并返读入结果0，成功先采样height S32再发布index，height<=0返0。正height只保存一次decoded row地址与height，读取width S32；逐row/column消费U8索引，零索引跳RGB累计但仍fresh全局width再column+1比较。非零读取palette R/G/B，各W32累计，蓝Byte/旧blue和先算sum，count W32+1后blue局部发布；本步显式恢复该阶段，不宣既有值错误。行末沿最后width样本推进同row地址，不重取全局dataPtr/height。无非零像素返0；按U32 DIV先blue/count低U8→高Byte暂存、green/count低U8→中间Byte暂存、red/count低U8→移位/OR形成00BBGGRR，再call451E90(packed,palette,256)。W32累计/计数、原S32维度和native指针保，无新增overflow/尺寸/调色板guard；共享解码区实际owner及内容运行另验。

蓝贡献/旧累计/相加作为局部暂存，count增加后发布蓝sum；三平均值按原蓝/绿/红U32 DIV，蓝/绿组包的中间值先于红平均取样，最终00BBGGRR供最近色函数。当前正常值相同；不新增故障、回绕、透明过滤规则或变更像素取样范围。BOL01真实FILE/解码provider保持，只确认本根的实际接线与正常消费，不代整条owner运行。

### 最近颜色：区分首色与候选原计算

U32输入按低R/中G/高B拆U8，palette4B R0/G1/B2/flags3、flags不读。先取palette0 B/G，再输入R/G，再palette0 R及输入B；初始R/B/G差与平方的S32正常计算，R²+B²+G²转F64最近距离，selected0。候选仅S32 i16..<count-16，bound保存一次；每项先样本B/G/R，差先G/B/R，正常平方和按R²+G²+B²转F64，与closest严格<才更新距离及index，平局保先见。count<=32仍初读palette0后返0，不新增null/count拒绝或扩大候选范围。此前通用闭包正常数值相同，本步用首色/候选各自原顺序显式表达并去掉闭包，不误报值差异或宣x87/PC53/异常等价。原地址native、S32/W32/U8宽度保，不代所有palette别名与consumer运行。

原Byte差平方正常值范围有限，顺序调整不改变已实现正常结果。采用函数内显式局部样本/整数值，撤仅该函数的私有distance闭包；不新增工具文件/API，保严格<与候选边界。无异常模拟或编译器私有ABI等价主张。

### 汇总集合形态修复

派生全对象检查发现functionRows中的baseline_documents/feature_memberships直接接受Unique函数流水线：空项变null、单项变字符串，集合形态不稳定。原成员内容与计数未丢失，但客户端不能一致按数组消费。本步在两字段赋值点用@(...)显式捕获输出，空项[]、单项[完整字符串]、多项数组；不修改全局Unique函数，不扩大到其它字段，不改变成员、排序去重、统计或验收状态。重新生成看板/快照，逐原函数仅对这两字段作规范化并核完整对象差异，不重验未变业务。

<a id="automap-palette-color-production-acceptance"></a>
## 可度量局部验收

| 项目 | 本步通过条件 |
| --- | --- |
| 原证据 | 三完整273条768B/五页、5call/27内跳/5ret、四入向及单chunk，无switch |
| 源码 | GraphicRead.cpp两个局部补丁、Palette.cpp最近色正文一个补丁；ResourcePaths.cpp正确体、其余根及公开头保持 |
| 映射 | 1401/1402/1403三新语义，原机器映射缺失但各已有一历史直接定义，定义行不刷新 |
| 指标 | 语义1401→1404/有效ID1396→1399，关联1514→1517（41.14%→41.22%）；分类518→521 |
| 功能与局部证据 | RESOURCE37→40、RENDER32→34枚举open；3+2根成员各I/S共10条static，E637→647，runtime=false |
| 汇总形态 | 两functionRows集合稳定为数组，成员/排序/计数及状态不变；同批重新measure/-Check |
| 完整门禁 | 七open只进展/引用；60源码closed10/完整验收0、member_inventory_closed=false/gates空/global_accepted=false/accepted_leaf_features=0保持 |

仅定向文本静态核原全根、完整源码补丁重放、正常阶段/宽度/别名和台账完整增量，执行measure/-Check；不构建/调试/查产物、不新增/运行测试、不执行客户端/算法，不计算hash。网络原格式明文、内存CryptInt保、音频暂停；RPLAN.24不新增异常拒绝/钳制/故障模拟。

<a id="automap-palette-color-production-source-state"></a>
## 当前状态与后续开发

本步补齐自动地图取色生产的三机器映射和原计算阶段，保原色表清零/调用后重取GraphicInfo、共享解码区单次借用及宽度取样。命名快照 `cg2d-re-automap-palette-color-production-20261006`。auto.dat父、实际图形资源/地图消费与共享解码区owner的完整运行继续按收尾任务推进；局部静态不代整体SPEC验收。
