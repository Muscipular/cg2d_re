# BOO01 调色板初始化与读取取样 SPEC

2026-10-06：沿BON01查色/通道的真实初始化依赖补开发缺口；依现有工程委托，先记方案、本线程裁决实施。仅已批准共享资源和32位渲染，不扩大协议或音频范围。

<a id="palette-initialization-read-sampling-baseline"></a>
## 原基线与差异

| 原入口 / 已有语义 / 原ID | 当前符号 | 指令 / 字节 / 页 | call / 内跳 / ret | 普通入向 / 父 |
| --- | --- | --- | --- | --- |
| 450470 / 169 / F:00050470 | InitPalette32 | 387 / 1544 / 5 | 12 / 23 / 3 | 2 / 2 |
| 450A80 / 170 / F:00050A80 | ReadPalette | 77 / 234 / 1 | 7 / 4 / 2 | 1 / 1 |
| 合计 | 两已有原ID、单chunk、无switch | 464 / 1778 / 6 | 19 / 27 / 5 | 3引用、父按根分列 |

[完整证据](cg2d_palette_initialization_read_sampling_evidence.json)保原六页、连续字节/chunk、三普通入向、八段终止常量共100B、固定初始化128条写语句与128B局部初值、两个改前源码正文、现有绑定正文和两个完整旧映射。两根各6个format/FILE调用沿旧已接范围保，不重做CRT全体。原初始化的8/16/24区[4508AD,4508FA)/[450908,45098F)/[450994,4509E1)，共85条289B，按既定范围排除分列；共享判断与32路径保，不能把整个函数或8位索引资源排除。

唯一已证源码差异：4509EC先取R、4509F2再取G，整数(R<<8)+G后4509FF才取B；原源复合表达式没有显式表达该顺序。固定32项128B逐字节一致；ReadPalette头尾原450B39/450B3C与450B4F/450B52分别W32读取/发布，当前4B值赋一致。二者保持正确体，不改颜色值或以Byte分写替换DWORD行为。

<a id="palette-initialization-read-sampling-decision"></a>
## 本线程裁决与实现

### 初始化：修正1处取样

固定32项局部128Byte原R/G/B/flags初值保持；0..9/246..255固定色flags2，再10..15/240..245固定色flags5，目标每项B-G-R-flags写序。入口首判gPaletteInitialized，首次512B未清path/%s\%s/rb索引0，打开失败原字节Error Code13/uType10返0；16..239各三getc低U8先B再G再R/flags5，close后全1024Byte备份，备份在168号三Byte清黑之前。共享独立palette创建按已批准CPU native绑定承接：原CreatePalette输出槽非空判断，不把HRESULT猜为成功；现有实际对象分配/256色复制/owner发布保，失败原Error Code15/uType10返0。仅已批准32渲染，256色查色每项先R再G取样、(R<<8)+G，再B取样和pack00RRGGBB；完整W32pub，纯黑再W32=1，最终索引0W32清0。尾重新读取initialized，首次每项Blue→Green→Red各fresh通道指针写U8 i，随后initialized1/changing0/返1。共享色资源保，三个8/16/24区85条289B仅纳盘排除；不新增空指针/容量/EOF拒绝或业务限制，不外推所有父/SDK-owner/像素运行。

补丁仅将查色复合表达式拆成R样本、G样本、中间整数、B样本与最终整数；原完整W32颜色→纯黑W321→尾透明W320保持。固定色是不可变局部值，保持已正确初值和目标Byte发布顺序；不为局部不别名值新增框架。

原45083A先读renderer，450840..45084E清168色，45086B调用CreatePalette，45086E重新读renderer与+90输出槽、非空才继续。该机制按原[DX/CPU绑定决议](cg2d_resource_implementation.md)获批替换：现有CreateRendererPaletteBinding实际创建PaletteBinding、复制256色并交给renderer唯一owner，失败返回false后保原提示。保现有实现，不新增COM/HRESULT模拟；本次仅确认真实承接，不追认为所有构造、重入、销毁或分配失败时序运行通过。

### 读取：保留已正确正文

S32 index直接寻原PaletteFile130B条目path；256B未清局部path/%s\%s/rb，打开失败原字节Error/uType10返0。16..239共224色依次三getc低U8 B/G/R、flags5，不以EOF增失败。close之后从当前gPalette逐项W32载入再W32发布头0..15及尾240..255，两个独立顺序循环；当前PaletteEntry4B值赋正确，不能改Byte分写。成功返1，不修改168号、活动/loaded、initialized、查色表或renderer，不增加索引/output校验；原两返回及FILE六call保持，正常输出别名逐项消费，原地址x64 native承载。

首尾复制逐项读当前全局颜色后写output；不得改整块快照或三Byte复制，不假定正常别名输入无重叠。FILE保持三次读取后各Byte发布、flags5/关闭后复制。正常值宽S32/W32/U8保持，已证output/FILE/通道指针按x64原生地址承载。

<a id="palette-initialization-read-sampling-acceptance"></a>
## 可度量局部验收

| 项目 | 条件 / 本步计量 |
| --- | --- |
| 原覆盖 | 两完整464条1778B/六页、19call/27内跳/5ret、三入向；原85条289B低色深区分列 |
| 常量与初值 | 100B八段原终止常量；固定128条Byte初始化静态展开，当前32项128B一致，不运行算法 |
| 开发 | 整Palette.cpp仅Init1处取样补丁；Read与其它正文、公开头/绑定保持 |
| 旧映射 | 169/170续记、完整旧记录深拷贝，旧FILE属性与历史直接定义0/0保持，不伪造新函数 |
| 台账 | 语义1401/有效ID1396、1514文档关联41.14%不变；分类516→518，RESOURCE35→37/RENDER30→32枚举仍open |
| 局部I/S | 两根×两域×I/S共8条static，E629→637；根和provider runtime=false |
| 完整门禁 | 60任务源码closed10/完整验收0；七open只推进引用，member_inventory_closed=false/gates空/global_accepted=false/accepted_leaf_features=0保持 |

静态核对原页/chunk/调用分支、完整源码唯一补丁、128B初值/提示常量、两JSON完整增量与派生快照，只汇总文字、执行measure及-Check。不构建/调试/查产物、不新增或执行测试、不运行客户端/算法、不计算hash。RPLAN.24不设溢出/越界或故障策略；网络保原格式明文、内存CryptInt保、音频暂停。

<a id="palette-initialization-read-sampling-source-state"></a>
## 当前进度与后续开发

本步解决初始化查色取样差异并补两个既有根的可跟踪基线/分类/局部证据。固定初值、FILE链、备份先168置黑、真实独立绑定、中性三通道及Read逐DWORD头尾消费保持。命名快照 `cg2d-re-palette-initialization-read-sampling-20261006`。

全部调用父、实际初始化/重建/销毁owner与画面消费、正常别名及资源内容的运行验收继续按七项任务推进；本步局部静态不关闭它们。整体SPEC目标保持。
