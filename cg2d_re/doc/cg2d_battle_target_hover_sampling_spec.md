# BQV01 战斗目标选择、高亮与两数据叶SPEC

2026-10-07。沿[BQU01完整主分发](cg2d_battle_command_state_dispatch_sampling_spec.md#battle-command-state-dispatch-sampling-source-state)，补43C120目标选择→43C0A0高亮，以及Reset9/活宠索引依赖。依AGENTS.md第8条工程委托先记裁决再实施，[四完整原根、源前后态与依赖桥](cg2d_battle_target_hover_sampling_evidence.json)为依据。

<a id="battle-target-hover-sampling-baseline"></a>
## 完整原基线与差异

| 原根 | 当前符号/实际文件 | 指令/B/页 | CALL/jump/RET | 台账 |
| --- | --- | --- | --- | --- |
| 43C120 | GetSelectedBattleEntry_43C120 / YobiRuntime.cpp | 226/690/3 | 15/30/9 | 1560新正常F |
| 43C0A0 | DrawBattleEntryHover_43C0A0 / YobiRuntime.cpp | 39/114/1 | 1/2/1 | 1561新正常F |
| 43C490 | Sub_43C490 / ui/WindowRuntime.cpp | 9/22/1 | 0/1/1 | 1562新正常F，正确体保持 |
| 43C4B0 | Sub_43C4B0 / world/SceneState.cpp | 13/46/1 | 0/3/1 | 1563新正常F，正确体保持 |

合计287条872B/六页、16直接CALL/两callee、36普通jump/十二RET，无switch。46实际代码入向/16不同父仅登记：目标两入向来自已过498520，高亮十五来自完整目标根，reset一入向来自498520，活宠28入向15父。不能据入向推其它父/完整生命周期已验。

| 已证源码差异 | 原点 | 裁决 |
| --- | --- | --- |
| 两flags消费与共用桥 | 43C1E4/43C0AF TEST BYTE action+24,8；当前消费和桥都从W32 flags截取 | 两consumer改调用既有GetYobiActionFlagsLowByte，该桥改unsigned byte视图真正取低W8；flags仍共享W32 |
| DX表取样阶段 | 43C0B5图号W32→43C0B8借表→43C0BF hoverImage→Y/offsetX/offsetY/X | 原生非拥有DxGraphicEntry表T*只借一次，保动作字段顺序 |
| 同表维度消费 | 43C0E6 MOVSX WORD表+6高→43C0EA WORD+4宽 | 既有render文件添加三小桥，height/width从同held表与图号取S16，不fresh查表、不加载 |

两consumer体四片段、共用BYTE桥一体一处；render既有文件追加三个承载桥和opaque typed声明，无新原函数身份、句柄表或独立工具文件。普通旧值一致，不称并发/数值/故障修复；尺寸/坐标正常计算和原返回序保。两正确数据叶不改源码。

<a id="battle-target-hover-sampling-decision"></a>
## 目标编号、绘制与承载裁决

1. 原入口held targetFlags W32先于hover W32；构造四排20个原W32位置值：14/12/10/11/13，19/17/15/16/18，9/7/5/6/8，4/2/0/1/3。现constexpr表与完整序列一致，仅局部无写值承接，不新建R7表或改变对照分母。
2. 前20槽依序借action T*，非空、cacheIndex W32等于held hover、flags实际低W8 bit8才命中。优先40→80→100→200（原W32 targetFlags的DL/DH寄存器消费），不能改成多目标合并、窄化flags或按业务重新排序；无这些位继续扫描，其后无命中-1。
3. bit40直接返回原position。bit80四排/五列完整查表定位，不提前break改变完整查找结构；配对排0/1、2/3，先纵向同列，再有右邻才画右邻，再有左邻才画左邻，原call序保；返回position+20。原用于初值的栈槽没有初始化，但合法0..19全覆盖并在使用前写row/column，现初始position仅不可消费的暂值，不设未初始化/异常模型或填充门禁。
4. bit100 position<10画0..9除自身，返回40；否则画10..19除自身，返回41。bit200画0..19除自身，返回42。所有高亮返回不用于替代target，失败门禁/调用与编号保持；两真实49895D/498CD4调用继续交给已过主根按原H/S/I/W格式明文发送，本文不重复核其完整发送。
5. 高亮只在slot action非空且BYTE flags8存在才继续，不新增index保护/补空。图号W32→借DX表T*→hoverImage W32→action Y/offsetX/offsetY/X W32→同held表高度S16后宽度S16；原x+offsetX-2/y+offsetY-2、x+offsetX+width+2/y+offsetY+height+2矩形四lowW16、层206/held hoverImage/0传原既有DrawGraphicHover，void弃原callee返回，不改图层/扩大矩形/截W32地址。
6. render::DxGraphicEntry仅前置声明给调用期非拥有指针，实际定义留既有render内部。BorrowGraphicDxCacheEntries只返回当前真实全局表；两个FromEntries桥从该表/图号读高度/宽度W16再有符号扩展。只拓宽原地址，原x64已批DX条目布局/两W16宽高/既有owner及shared table保持；不导出SDL/COM对象、不增Load、范围检查或新取样。旧ReadGraphicDxWidth/Height/HeightFromField正文和所有调用不改，不用新接口外推全模块验收。
7. 共用GetYobiActionFlagsLowByte只改这一真实BYTE取样实现，现公开参数/返回U8与原共享W32字段保；其已有调用共用修复，不额外改各父正文或声称全父流程通过。本点保flags其它字节，不新OR/AND写。
8. Reset43C490 count S32<=0直接返回，>0按借values T*逐项写W32 -2，已过主根原数组/9项调用对应；void保真实不消费返回。活宠43C4B0五槽依序，used原W16严格1且flags6AA原W16严格2才返回首index，没命中-1；不把任意非0视占用、不加状态bit4、不扩扫描数。现PetRuntimeRecord两字段/原5110跨度已正确，不改上游生产。
9. 不按猜测处理其它位或新增目标；全targetFlags/技能/宠物生产、全部父、owner/窗口寿命与运行另验。缺失源定位首先用快照实际路径，43C4B0在world/SceneState.cpp，不能假设在UI；本批查到不存在的GraphicRuntime.hpp后改读真实GraphicDxCache接口，不生成假文件。

## 四相关R7区间与机器台账

四未审X [43C112,43C120)14B、[43C3D2,43C3E0)14B、[43C4A6,43C4B0)10B、[43C4DE,43C4E0)2B共40B。各全字节90、只读decode逐项NOP，数据库各一align10h项，逐字节无任何代码/数据入向，且前一完整根真实RET结束、后一为已知函数入口。mapped只填充用途，不造新F或功能排除；R7 265→269/3061余2792。

1560..1563四新正常语义、四equivalent/四BATTLE成员：语义1560→1564/有效1553→1557、文档关联1671→1675/3680（45.52%）、分类690→694/BATTLE242→246枚举open/E794→796两局部I/S。无F/有效区间各7、差队列172/缺定位171/43待归属及全部原门禁保持；U08/R01/R03/R04/V04五任务只本批当前/引用，60源码closed10/完整验收0/叶0/global=false。

<a id="battle-target-hover-sampling-acceptance"></a>
## 可度量验收

- 四完整单chunk/连续原字节、287/872B/六页、16directcall两callee/36jump十二RET、46实际代码入向/16父，目标15高亮call与两个主根调用逐点可追。
- 两体四片段+共用BYTE桥一处与三承载桥/opaque声明前后冻结，整三个实现/声明文件只有授权差；正确UI reset/世界活宠体和旧DX接口不改。两实际BYTE原点、held表/hover/动作字段/height-width阶段、原W32/W16存储/坐标算式及四低W16绘制接口保持。
- 四排20项值和原顺序/bit优先、纵邻→右邻→左邻、两十位/全体排除自身及所有返回码/无目标、W16严格活宠/Reset9正常分支静态对应；不用样例算法/运行或异常模型证明。
- 四X 40B完整字节/连续NOP decode/逐byte无入向与前RET/后函数边界用途；四新F/处置/246open成员、两局部I/S/五任务/派生看板与完整快照及编码/引用/diff，measure/-Check只文本汇总。

<a id="battle-target-hover-sampling-source-state"></a>
## 本步交付

先落SPEC与完整证据，实施两体四片段、共用真实BYTE桥与三DX承载桥，再同步台账/五任务/唯一进度/派生看板，精确提交14文件并固定快照`cg2d-re-battle-target-hover-sampling-20261007`。目前限四根正常源码静态、调用期DX承载与四填充用途；全生产/其它父/owner寿命/SDK/机器ABI/画面运行/R8继续推进。音频暂停、不构建测试运行摘要/RPLAN.24、原协议兼容明文和适用完整验收标准保持，整体目标不按局部完成缩小。
