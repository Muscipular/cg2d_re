# 选择器切页、显示与回调发布 SPEC（SPG01）

2026-10-10。沿SFD01切页生产和显示依赖，依子AGENTS第8条委托裁决。同一cg2d_re分支、不打tag。

<a id="selector-page-baseline"></a>
## 完整原根

4C6440=12条33B，4C6470=52条141B，4C6500=30条75B，4C6550=177条551B/三页，4C6AC0=28条73B，4C7340=24条64B；合323条937B/八页、19E8九目标、35跳十一RET。24入CALL只核点/父及六后邻，未代完整外父。五case原表20B目标4C65C9/4C65F5/4C6697/4C66F4/4C6744，对应page0..4；六X表复用SLC01已核静态证，不重取。

6550已有旧1987/CAC18取表关联，保全部历史追加完整根/六处置；其余五新F。正确显示、隐藏、移动、模式切换五正文保持，不为登记改业务。

<a id="selector-page-decision"></a>
## 决策与实施边界

SPG01六完整原根323条937B/八页，19实际E8九目标、35跳十一RET，24入CALL/六后邻与五case跳表20B有据，只核点不代全外父。6440保page2槽6及value64=0槽19独立显示；6470保page3槽7/8、15/16、原BYTE3ED及宠物AL范围后单读itemSlots，动态起始到<6原循环、17及模式0槽19，不新增容量/负索引保护；6500原序隐藏6/7/8/15/16、9..14/17和模式0槽19，主元素不动。6550保page1/S8正mode预处理、低AL/usedW16判定及7340，pageW32发布→6500→新page五分派；page0/4退页、page1原图98146/98147与每轮fresh宠物/动态count及step21，page2清mode后双路绑定、移动-182/visible/图，page3移动/文本/显示及BYTE3ED==1回调。两组五处仅CC40D4读低BYTE和四次callback发布先借目标元素后读同state回调；原S32/W32储宽、指针owner及五分派/图号/取序保持。6AC0保主x、四按钮x-15与六原表+x-24，不新增钳制；7340只写参数低BYTE，先旧page，页2不切，页3新mode0切2否则3，其它不切，bool限原AL固定1、高24位未记等价。五正确body及两整源其它全文/三类型/原六坐标表保，六定义/六声明十二注释，旧1987/CAC18完整根追加/五新F六adapted、三限定S零I/十任务open。6440/6500原残留EAX在父独立MOV覆盖，6470/6AC0/6550有残留转发，当前void副作用适配保且已知逐帧上行由两窗口MOV1覆盖；7340只AL置1，不以混合结果宣称全ABI不消费或机器等价。全部外父/callee/回调与动态slot/宠物/窗口/模式全生产消费/alias-owner寿命、SDK/生成BYTE/机器ABI/真实页切换像素与运行待。无新API/helper/结构/字段/owner/保护/默认/补零/新容量上限/钳制/回滚/异常模型；WAV-only/明文网络/MSVC/48有界代码/完整枚举gates/六closure/总SPEC§10保持。

页0先NULL回调/隐藏主元素→移动16→mode=-1；页1按S8 mode0显示玩家/98146、正mode显示宠物/98147，AL合法后原动态count循环不加cap，每轮写visible后重取selected/count；最后NULL回调/visible1/step21。页2清mode→显示玩家，装配W32非零或CC40D4低BYTE0时回调2，否则先元素0回调1、再元素1回调3，移动-182后visible1/图98146。页3移动-182→69B0→6470，原BYTE3ED==1回调2，否则NULL，visible1/98147；页4先value60=0→移动16→NULL/隐藏→mode=-1。default无业务。保持各次fresh元素，四动态回调读前先借其目标，保持原写序；不新增通用setter/helper或缓存所有元素。

<a id="selector-page-acceptance"></a>
## 限定静态验收

核六完整连续原根及19实际E8九目标、24入CALL/20B表，BYTE源和四次目标/回调顺序、原五页分派/动态count/visible图号与mode逻辑。两整源精确差异回放、五正文/三结构和原表保持；旧1987历史追加、五新F/六处置，三限定S零I/十任务open。全部正常owner/生产消费与回调及外父/callee、原高EAX/完整机器ABI/生成BYTE访问、真实切页像素与运行待，不提升完整枚举/gates/六closure/184叶验收或总SPEC§10。

只做必要静态文本与measure_cg2d.ps1/-Check，不构建/测试/运行/产物检查/摘要/外依赖。

<a id="selector-page-source-state"></a>
## 当前状态

前提交“[#] 补齐选择器逐帧分发与元素绑定”；拟提交“[#] 补齐选择器切页与显示合同”。方案已记录并实施，源码及登记必要静态核对通过；静态汇总/-Check终态均退出0，派生范围核对通过。

语义2294=2246F关联+48有界代码记录，函数处置1740/证据1529，INPUT117/UIRUNTIME154/RENDER92；文档关联2356/3680=64.02%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持。登记、静态汇总/-Check终态均退出0；派生仅六metrics/六F/三功能/十任务变化，完整门禁保持。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_selector_page_visibility_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐选择器切页与显示合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x4C6440 | 0x4C6461 | 12/33 |
| 0x4C6470 | 0x4C64FD | 52/141 |
| 0x4C6500 | 0x4C654B | 30/75 |
| 0x4C6550 | 0x4C6777 | 177/551 |
| 0x4C6AC0 | 0x4C6B09 | 28/73 |
| 0x4C7340 | 0x4C7380 | 24/64 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
