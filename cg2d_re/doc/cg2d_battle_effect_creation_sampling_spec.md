# BNV01 位置与随机特效创建取样

<a id="battle-effect-creation-sampling-baseline"></a>
## 原端与实际差异

F:00019FC0（419FC0..41A0CA）单chunk84条266B两页五call、F:0001E680（41E680..41E712）单chunk41条146B一页四call，两完整原体共125条412B三页九call/四ret，无switch。[证据](cg2d_battle_effect_creation_sampling_evidence.json)保全部实际字节、分支/调用、两完整改前根与三个补丁。仅补这两项缺口，不重验BCS01已过角色创建或其它特效。

随机特效第二flags发布重新读取字段，原在41A00C用首次寄存器取样OR40000，再写image、再发布flags；位置特效第二flags发布也重新取字段，原两次发布用同一个EAX。位置X取样晚于image/direction、Y取样晚于X发布，原X先FLD、image/direction后截断，Y先FLD再发布X。这是取样/发布合同差异，不声称已观察画面错误或线程并发行为。

两ID原无独立语义/分类与战斗成员，历史直接定义存在，不在175定位队列。随机特效80直接call/28父、位置特效13直接call/9父全部地址列证据；数量不代全部父接线/借用生命周期完成。回调绑定419FA0/41E640、分配404AE0、设备44B610和图缓存522570/原S16宽字段保持，__ftol按[既有host裁决](cg2d_compiler_helper_spec.md#compiler-helper-decision)，不恢复x86帮助体或重验这些已证依赖。

<a id="battle-effect-creation-sampling-decision"></a>
## 本线程裁决与实施

沿既有工程委托，先SPEC后修改三块/两个根，无新API、全局状态或依赖。仅已证parent/返回地址采用native非拥有T*；image/direction/屏幕坐标仍S32，offset仍F32、flags W32、displayPriority/rebirthValue1/2 W8和field270 W16。

| 根 | 保留的完整正常合同 | 本步修正 |
|---|---|---|
| 随机特效 | New(2,0)失败返回原空；callback→priority CC→rate100→设备查询→flags分支；image→两W8值4→Load父graphicIndex→第一次live图宽S16/2的X-RNG→第二次live宽/4和父x→X发布→父graphicOffsetY/3的Y-RNG→另live offset/4、父y→direction→Y→W16零→返回 | 设备返回后flags只取一次；分支flags发布，再同值OR40000先计算、image写后发布配置flags |
| 位置特效 | New(2,0)失败返回原空；callback→priority CC→rate100→设备查询；parent offset40写→offset44写；direction0和W16零保持，无RNG | 同一取样两W32 flags发布；X取样先image/direction，截断后取Y、再发布X、再截断发布Y |

随机无设备分支使用10000、位置无设备分支8000，原AH操作仅修改寄存器，最终写flags仍W32。随机每次graphicIndex/宽度和graphicOffsetY各取两次，不能跨Load或RNG缓存成一次。原正常S32除2/3/4向零与两次RNG顺序保持，不加parent判空、范围/容量保护、补零、钳制、溢出回绕或故障模型；只保原业务失败/零字段。位置正常F32→S32转换沿既有编译器承接，不新增异常转换帮助器。

不变caller/回调/资源提供者及所有其它源码。新增语义1353/F:00019FC0、1354/F:0001E680，按获批native地址承载登记adapted；仅FUNC-BATTLE成员55→57，完整枚举不关闭，不将两个生产函数外推为YOBI/RENDER全功能验收。

<a id="battle-effect-creation-sampling-acceptance"></a>
## 可度量验收

| 项 | 限定验收依据 |
|---|---|
| 实现 | 三补丁/两个完整根，整CPP按HEAD加三个指定补丁全文对照；flags读取/两发布及X/Y采样序列、RNG/Load/失败返回和字段类型静态核对 |
| 原端 | 125条412B三页全字节/九call/六分支/四ret、两callback绑定和入向93处分别纳档，无switch；不代全部父全文 |
| 追踪 | 两独立身份/分类、成员57、各函数I/S共四E，七open任务和唯一进度同步，所有旧记录保持 |
| 度量 | 语义1353→1355/有效ID1348→1350、分类459→461、关联1466→1468/3680=39.89%、E501→505；队列175保持，不虚减 |
| 完整门禁 | 不构建/测试/运行/摘要；全部consumer-owner/资源重进退出、SDK/ABI/PC53/实际RNG与画面及R1/R7/R8另验，整体未完成 |

YOBI55/RENDER26/LOOP32、片段5/R7区间67/3061、60源码closed10/完整验收0/P01与七open、全部完整门禁保持。三处取样问题在本步解决，不将已修问题再挂为待办；尚需完整运行的内容按当前执行限制保留实际未验状态，不降低标准。

<a id="battle-effect-creation-sampling-source-state"></a>
## 源码与收尾

源码两个创建函数恢复原取样/发布，独立基线纳入映射/登记簿。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)和[度量看板](../plan/cg2d_measurement_dashboard.md)同步局部结果。固定快照 `cg2d-re-battle-effect-creation-sampling-20261006`。网络继续原格式明文，音频暂停；本步不扩展相邻特效或全面核对。
