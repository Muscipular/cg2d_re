# 选择器逐帧分发、元素绑定与初始化 SPEC（SFD01）

2026-10-10。沿SLC01悬停上行及真实逐帧依赖推进，依子AGENTS第8条工程委托裁决。同一cg2d_re分支、不打tag。

<a id="selector-frame-baseline"></a>
## 原根与静态数据

4C6790完整168条486B/三页，4C6EC0完整173条749B/三页，4C7A90完整21条63B/一页，合362条1298B/七页、20实际E8十目标、20跳十RET。五入CALL仅核字节/父及后邻；两图表各16B：[98133,98133,98135,98134]与[98137,98137,98139,98138]，四case跳表16B目标4C67F7/4C685B/4C6879/4C696A，符合现switch1..4。

4C6790已有CAC18关联旧1988，保历史追加完整根及处置，不重复F；新增6EC0/7A90两F。旧2285/6B10仅追加局部返回证据，SLC01正文与历史保持。

<a id="selector-frame-decision"></a>
## 实施合同与取舍

SFD01三完整原根362条1298B/七页，20实际E8/十目标、20跳十RET；五入CALL/后邻与四case跳表16B、两图表32B有据，未验全外父。逐帧保入口page1..2/S8正模式预处理与新page四分派、原step递减与-182业务钳制、宠物范围仅低AL/usedW16和切页/悬停/预览/两按钮/文字顺序。descriptor0原首次取地址后两Draw共借；首按钮windowY/windowX/elementY/elementX，次按钮elementY/windowY/elementX/windowX，均先算Y后X，第二组只在首Draw后取样。三组三处/六注释，两整源其它全文、绑定与初始化正确body、三类型/表值及字段宽度保持。绑定原模式低BYTE写value64；非零装配槽19不写，先callback0后slot18，逐槽fresh窗口借用；零模式二十槽/四回调保持，无owner迁移。首次init0时先mode地址/previous=-1、切页后initialized1，尾分发保。既有6B10首CALL残留EAX经6790/7A90返回，两窗口488990/499990均立即MOV EAX,1/RET覆盖；只闭此已知上行不消费结论，三void正常调用副作用保持，不称全机器ABI等价。旧1988关联升级完整根追加但CAC18历史保，旧2285/处置追加局部返回证据，新增两F/三adapted处置、三限定S零I/十任务open；全部旧政策/48有界代码/完整枚举gates/六closure/总SPEC§10保。全部callee/回调与生产消费/alias-owner寿命、SDK/生成取样/机器ABI/实际选择器像素及运行仍待，无新API/helper/结构/字段/owner/保护/默认值/补零/容量上限/新钳制/回滚或异常模型；WAV-only/明文网络/宿主MSVC保持。

绑定非零槽0..7对应窗口3/20/16/17/18/19/4/5，随后槽9..14=10..15、槽8=6、15..17=7..9，借window21后先callback0再slot18；callback0/1/2/3=499C30/4B0F10/49AC40/4B0F10。零槽0..7=4/17/18/19/20/21/5/6，9..14=11..16、8=7、15..17=8..10、18=23、19=22，四回调488FB0/489F00/489760/48A6C0。模式分别借57E42C/57DA28；逐项fresh窗口保持。

页1先按S8正mode取6990，x>-182时6AC0(x-step)，调用后取新step/元素指针，step减1再读x，<=-182原业务钳制；否则按mode转3/2。页2正mode切3否则悬停；页3先6990及宠物判定，失效清mode0、mode0切2，否则悬停/预览/两按钮/6D90；页4切0、default无业务。首绘X偏移85、次绘118，两Y偏移180，原图号索引不新增校验。初始化BYTE modeFF切0，其它切1，切页调用后置initialized1。三定义与三真实声明六注释；两窗口已有return1、绑定和初始化正确body保持。

<a id="selector-frame-acceptance"></a>
## 限定静态验收

核三连续完整原根、E8目标/五入CALL、三表48B、两按钮持址及两套字段取序/先Y后X；逐帧分派与原业务钳制、绑定各槽/回调发布和初始化/两窗口MOV1-RET。两整源按精确差异回放，三类型/两窗口正文和原字段宽度保持。仅两新F/三处置及旧2285追加、三限定S零I/十任务open；全部完整验收及全生产消费/所有callee/alias-owner寿命和机器ABI/像素/运行保持待。

只做必要静态文本和measure_cg2d.ps1/-Check，不构建/测试/运行客户端或算法/检查产物/计算摘要/引依赖。

<a id="selector-frame-source-state"></a>
## 当前状态

前提交“[#] 补齐选择器局部坐标与悬停取样”；拟提交“[#] 补齐选择器逐帧分发与元素绑定”。方案已记录并实施，源码和登记的必要静态核对通过；静态汇总/-Check终态均退出0，派生范围核对通过。

语义2289=2241F关联+48有界代码记录，函数处置1734/证据1526，INPUT111/UIRUNTIME148/RENDER90；文档关联2351/3680=63.89%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持。登记、静态汇总/-Check终态均退出0；派生仅六metrics/四F/三功能/十任务变化，完整门禁保持。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_selector_frame_binding_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐选择器逐帧分发与元素绑定」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x4C6790 | 0x4C6976 | 168/486 |
| 0x4C6EC0 | 0x4C71AD | 173/749 |
| 0x4C7A90 | 0x4C7ACF | 21/63 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
