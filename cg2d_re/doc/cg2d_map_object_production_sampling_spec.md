# 地图对象创建更新与文本缓冲复用 SPEC（MOP01）

2026-10-10。沿YGOS01的445350方向门禁局部证据，本步核完整445350–445A55：543条1797B/九页、28实际E8/十唯一callee、31跳/一RET，无switch。当前本体早已存在，不称首次创建地图链；恢复两坐标取样、四缓冲身份及两阶段引用共五组，原正常失败、文本门槛/字符字节、所有其它源码与类型保持。

<a id="map-object-production-baseline"></a>
## 取证及真实接线

原F:00045350尚无独立语义或适配处置。唯一入向50B894来自50B380；本步只取该父50B843–50B8AC附近真实指令窗，确认51B740图号映射→十三原参数→445350→4462B0查询，不重验完整接收父/协议。当前接线位于SceneState.cpp，真实声明在src/world/WorldUpdateDependencies.hpp，定义在YobiRuntime.cpp的cg::world::detail。

十直接callee为444FA0、446130、5183B0、441DB0、446210、401CC0、401CF0、52CED0、52D2F0、4461E0。已有2185/1298/1920/1248语义关联分别为图号判定、实体创建、类别数据角色及字段解析；不把1920数据关联或其余提供者定位当完整callee验收。当前提供者分别位于WorldObjectState.cpp、YobiRuntime.cpp、LegacyCryptInt.cpp、NetworkRuntime.cpp及Input.cpp；原明文协议与当前MSVC CRT边界保持，不重造未独立登记callee。

原完整指令、唯一入向/父局部窗、当前源差集及九个文本解析点见[证据](cg2d_map_object_production_sampling_evidence.json)。

<a id="map-object-production-decision"></a>
## 工程裁决与五组

依子AGENTS第8条工程委托，先写本合同再恢复原读取与缓冲身份：

1. 第一坐标临时Construct(mapX)后，先持样BYTE readCount、W32 encoded、W32 mask三值，再按该顺序发布coordinate0C，最后Destroy。
2. 第二坐标Construct(mapY)后，先持样W32 mask、W32 encoded、BYTE readCount，再按该顺序发布coordinate1C，最后Destroy。不复制virtual token/reserved、不清未写部分或用新临时替换同一对象；原两Construct/两Destroy/RNG及坐标参数位置保持。原DB的gameY/gameX名字不独立证明方向，不据此交换坐标。
3. 原四个256B未初始化文本槽分别命名text100/text200/text300/text400，对应原var_100/200/300/400；容量、栈承载/局部寿命保，不承诺x64物理栈地址或越界别名。
4. 记录阶段非type3第1/2/4段使用text300/200/400；type3第1段使用text400。
5. action阶段非type3第1/2/3/4段使用text400/200/300/100；type3第1段使用text100。每一解析、Unescape、两次strlen及复制均跟随同一槽，不按字段名另建一套缓冲或复用已经解析的记录文本。

旧源码按first/second/third/fourth语义命名，造成记录普通第1/4段和action type3第1段对应的跨阶段复用关系不同；此次恢复原局部对象身份及依赖关系，不猜未写字节或模拟失败残值。现字段解析器在无源时清输出首字节的真实合同保持；本步不增加清零或改原调用/返回弃用。

## 完整正常创建与记录更新

先FindObjectIndex(objectIndex)，负则TakeEmptyObjectIndex，仍负直接结束。仅新建时，type>3且当前地图模式0且5183B0非动画才选方向0；否则原传入方向。创建441DB0返回后先发布record.action，空则结束，不回滚已领取记录。

成功新建先used W16=2、重新借record.action.payload写index，再Sub446210类别低W16、objectIndex与value34=-1。已有记录直接进入共同写入，不重建action、不覆盖这些新建专用字段。

共同按imageNo W32→value40低W16→level S32发布；两临时坐标按上述各自三字段取样/发布。然后方向S32、flag1/2非零时只OR flags42低BYTE40/80，未设条件不清旧位。名称字节长度≤16才复制含NUL，超长保持旧内容；不放大容量或新增字符转换。

type非3记录独立解析第1/2/4段，门槛16/32/16，extraText≤16复制到textB8；不解析/补写记录第3段text97。type3只解析第1段并只清textB8首字节，其他文本尾及未写区保持。所有解析以ASCII'|'、ordinal、capacity255交既有提供者，随后Unescape，返回均弃用。

## flags与action阶段

Sub4461E0(type)后完整flags W32发布；flags&1000且level位100/40均置位才再次发布flags|400000。随后value118=1，再取record.action，空则结束。原AH/CH/CL测试是完整保存W32样本的寄存器位观察，字段不缩BYTE。

先当前action.imageNo，重新取record.action写field1D0=level，再重取action与record.flags114作完整action.flags OR。名称≤16时在第二次strlen前重新借record.action.nameC4目标，原字节/NUL及旧文本保持。之后独立重新解析action各段/各自借目标，按上表缓冲身份和原16/32/32/16及extra16门槛复制；type3只写titleE1，不额外清action.text144或复用记录阶段结果。最终fresh record.action写field1D8=完整value40 S32，不能改成记录阶段低W16。

原常量、字段宽度、原生非拥有action T*与表引用保持，不补默认值、钳制、拒绝、回滚/空门禁或原无保护。长度/原capacity255属于真实业务门槛，不以RPLAN.24删除。CRT直接使用已批准MSVC宿主，保现strlen/memmove表达式/字节和别名边界，不承诺原REP/机器私有ABI；不重建新复制函数。

<a id="map-object-production-acceptance"></a>
## 限定静态验收

核完整543条1797B/28E8/十callee/31跳一RET、唯一入向E8/十三push父窗口、九字段/九Unescape物理点与四原缓冲关系、五源组/一CPP和一头注释的整源重放；类型/表/其它源及四旧callee身份保持。十callee只当前提供者/原调用关系，不代完整机器ABI/全部业务输入及owner。

一新语义/F，MAP与YOBI各一新成员、TEXT加入本字段消费者一成员，均枚举false。六限定I/S分别登记MAP/YOBI/TEXT：MAP/YOBI I五组、TEXT I只三缓冲组，S均限定这一完整正常根；U09/U10/U11/T01/R01/R04/V04仍open。全部完整gates/184叶验收0/60任务关闭11与六closure和原范围政策保持。

原字段/资源/网络明文及GBK数据边界、WAV-only/SDL、MSVC CRT及批准数学边界保持；RPLAN.24不专项异常或模拟/填未写字节，完全不构建/测试/运行客户端或算法/检查产物/摘要，不引依赖，不重开/修改IDA。全部生产/唯一完整父/传递callee/正常失败与容量/alias/owner/SDK/机器ABI/声音画面/运行待，总SPEC§10不降。

<a id="map-object-production-source-state"></a>
## 当前状态

本步前Git提交说明“[#] 对齐图形偏移查询与挂件绘制合同”，完成提交说明“[#] 对齐地图对象生产并恢复历史源码引用”。统一cg2d_re分支，不打tag。五组/两关联注释已落实；完整原根/唯一十三参入向、九字段与九Unescape/四槽关系、整CPP/头及其它源/结构、全部旧语义/政策及引用静态核对通过。历史附件按补充合同恢复可读后，重新汇总/Check均退出0；初次149错误及派生失败保留。结果语义2187（2139F＋48X）、合同2249/3680=61.11%仅关联率、F1617/E1425、MAP138/Yobi67/TEXT29枚举false；派生仅六metrics/一F/三功能/七任务，184叶验收0/任务关闭11/六closure及全部完整gates保。十四本步文件，七外部改动与外部pelican暂存删除保留排除；全部生产/正常失败/容量/完整父/传递callee/ABI/alias/owner/SDK/声音画面与运行待，整体SPEC未完成。

<a id="map-object-production-history-reference"></a>
## 无tag历史附件定位修复

本步汇总及-Check各退出1，均报告149条已退休附件引用不存在，派生文件未写；随后派生计数校验亦失败，不能记通过。只读核六旧标签当前均不存在，HEAD仍为本步前提交，退休文件均不在工作树/HEAD；标签消失的作者与原因未证，不猜作源码改动。另有pelican_bicycle.svg删除在外部暂存区，须保留并排除本步提交。

六退休提交标题在当前可达历史中各唯一，核其直接前态的三十个既有白名单路径均存在：

| 旧证据定位名称（保历史） | 实际可读前态 | 路径数 |
|---|---|---|
| cg2d-re-asm-history-20261009 | [#] 按标准数学近似合同将音频ASM迁为CPP 的直接前态 | 3 |
| cg2d-re-msvc-crt-cpp-history-20261010 | [#] 将C++私有库对象收敛到MSVC标准库 的直接前态 | 13 |
| cg2d-re-msvc-crt-numeric-history-20261010 | [#] 将浮点格式与数值私有职责交由MSVC承接 的直接前态 | 7 |
| cg2d-re-msvc-crt-exit-history-20261010 | [#] 将错误信号与私有退出设施收敛到MSVC 的直接前态 | 2 |
| cg2d-re-msvc-crt-heap-history-20261010 | [#] 将通用堆策略与初始化收敛到MSVC 的直接前态 | 2 |
| cg2d-re-msvc-crt-multibyte-history-20261010 | [#] 将多字节与私有参数准备设施收敛到MSVC 的直接前态 | 3 |

依AGENTS第8条及同分支/不打tag要求，先写本补充再仅修改measure_cg2d.ps1六处历史读取表达式；按固定完整退休提交标题匹配（标题起点与尾空白/结束约束）定位前态，不创建tag/新Git引用，不恢复旧CRT/ASM文件，也不重写149旧附件或旧snapshot名称。三十路径白名单保持，旧CPP/numeric/signal/heap/multibyte当前source禁回退保持，ASM也明确同样禁当前source回退；其他缺路径仍失败。

这是证据可读性修复，不增加功能/原身份/软件证据或降低门禁；源及登记既定五组/六限定I-S保持。集中静态重放汇总器七处逻辑/一注释变化、六实际标题与三十路径、当前来源禁回退及白名单守恒通过，重新汇总/Check均退出0，派生守恒通过。此前两失败与派生失败保记录，不以过去通过代现在。
