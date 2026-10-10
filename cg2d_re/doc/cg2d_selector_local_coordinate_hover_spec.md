# 选择器局部坐标、宠物判定与悬停 SPEC（SLC01）

2026-10-10。沿ISP01三待闭依赖补局部坐标/宠物范围及真实悬停消费，依子AGENTS第8条裁决。同一cg2d_re分支、不打tag。

<a id="selector-local-baseline"></a>
## 完整原根与静态表

五根完整243条771B/八页、九E8五目标、26跳12RET，26实际CALL及四后邻有据；不代全外父。4C63C0/4C6400各19条57B，51C410九条19B，4C6B10为194条634B，4C71B0两条4B。原界尾分别4C63F9/4C6439/51C423/4C6D8A/4C71B4，不重造已登记的ISP坐标/列表根。

四S32表共112B，与现const数组逐项相同：玩家X=[100,36,36,165,165,36,165,165]，Y=[30,130,80,80,130,30,30,180]；宠物X=[36,165,36,165,36,165]，Y=[80,80,130,130,30,30]。仅读已打开IDA静态字节，不运行坐标样例。

八宠物判定消费者七TEST AL、一AND EAX,FF，均只消费低W8；原高24位保留输入索引的一部分，不据新bool记整体EAX等价。悬停父的4C6790两调用中，首点后pop/pop/RET向上传递原残留EAX；其当前void和上行语义分开记录，完整机器ABI/消费者待。

<a id="selector-local-decision"></a>
## 范围、索引与悬停合同

两local先负slot返回-1，不读mode；S8 mode0只slot<8读玩家表，正mode只slot<6读宠物表，其它-1。坐标表有效范围不成为动态列表数量上限。51C410只判断S32索引0..4；4C71B0只单读同一state selectedPet70，原S32返回，不加校验。四正确叶正文和四表保持。

4C6B10的value3ED非零且hoverIndex非负才查询X/Y，否则只清计数。每轴子调用后取入口元素指针和fresh当前window，原window坐标先element坐标，算local+(window+element)-24。初绘框后才读S8 mode；玩家直接借item，正模式保本state selectedPet非负门槛，再调用第一选择器CF3B10索引getter借宠物item；不追加索引<5或改owner。

used W16非零才读占用BYTE。占用时再次绘框，原flags低BYTE bit80控制22739标记，再绘图号；未占用时先以5*W32计数比较256，原饱和255保持，否则读取计数低BYTE*5取低BYTE发布descriptor[2][11]。两flags只读BYTE，W16存储不改；计数W32存储不改，不冻结两次读取。三descriptor和图号/243139绘制顺序保持。

末尾新hover样本与previous比较：相同先借count，清hover=-1，再count+1；不同先count0、previous=hover、hover=-1。负mode/负本state宠物号早返不新增清槽；当前void仅承载既有副作用，不把上行EAX与整个父关闭。

四组五处：两轴取序各一组、两flags BYTE一组、alpha BYTE一组；五定义/五真实声明十注释。四整源其余全文、四结构/表和字段宽度保持；无新API/helper/布局/字段/owner/保护/默认值/补零/容量限制/新钳制/回滚或异常模型。

<a id="selector-local-acceptance"></a>
## 限定静态验收

核连续243条771B、九实际E8五目标/26跳12RET、26调用点/四后邻，八低W8消费者与首悬停父返回上行事实；四表112B、四叶正常分支/返回、父两坐标先window后element、三BYTE源及原饱和/计数尾序，四整源精确差异和四类型保持。

五新F/五adapted，INPUT/UIRUNTIME各五成员、RENDER一成员，三限定S零I/十任务open。完整所有父/callee及4C6790上行结果、全生产消费与alias-owner/退出寿命、SDK/机器ABI/生成BYTE访问/实际悬停和像素/运行待；48有界代码、完整枚举/gates/六closure/总SPEC§10保持。

只做必要静态文本和measure_cg2d.ps1/-Check，不构建、测试、运行客户端/算法、检查产物、计算摘要或引依赖；网络/文本、MSVC CRT和WAV-only保持。

<a id="selector-local-source-state"></a>
## 当前状态

前提交“[#] 补齐选物回调与列表坐标取样”；拟提交“[#] 补齐选择器局部坐标与悬停取样”。源码对齐、登记、静态汇总/-Check终态退出0及派生范围核对通过。语义2287=2239F关联+48有界代码、处置1731/证据1523，INPUT108/UIRUNTIME145/RENDER89；文档关联2349/3680=63.83%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_selector_local_coordinate_hover_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐选择器局部坐标与悬停取样」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x4C63C0 | 0x4C63F9 | 19/57 |
| 0x4C6400 | 0x4C6439 | 19/57 |
| 0x51C410 | 0x51C423 | 9/19 |
| 0x4C6B10 | 0x4C6D8A | 194/634 |
| 0x4C71B0 | 0x4C71B4 | 2/4 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
