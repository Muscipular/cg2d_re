# 窗口释放取样与终止标志 SPEC

编号 BVR01；2026-10-08。承接BVQ01分类关闭的真实FreeWindow依赖，修单根六个既有阶段，补释放/全关闭/标记三根基线。

<a id="window-release-source-baseline"></a>
## 原基线与实际缺口

[完整原证](cg2d_window_release_source_sampling_evidence.json)保存三完整根184条565B/四页、14直接调用/六目标、一个release间接调用、28跳/三返回；11普通CALL与四尾跳分列。现有三正文和类型/声明均已存在，独立原新/基线/处置/成员缺失。

| 原身份 / 新语义 | 完整根 / 指令字节 | 本步 |
|---|---|---|
| F:00071270 / 1797 | 471270–47141A / 140条426B | 六释放阶段实修 |
| F:00072190 / 1798 | 472190–4721FE / 35条110B | 正确71槽全关闭与MIS62保持 |
| F:00072220 / 1799 | 472220–47223D / 9条29B | 正确存在窗口marker50保持 |

471270先release()，其后fresh id定位state、回存x/y、两次W32 flags修正、清state.window、焦点/拖动清理及七类元素释放，最后释放元素数组/实例。原flags确实W32读写，AND DL只寄存器处理；现源两W32操作正确，不能误改BYTE写。新增写接口是处理Opaque类型可见性的必要承载，不能以UI层直接访问不完整类型收口。

实际差异：
- 4712C8先取focusId，4712CE取window.id；现比较表达式没有显式固定两读取先后。
- 类型1/5原先取全局focusedData，再取当前element.data；现比较表达式未固定顺序。
- 类型2原data读一次，经非空判断后取同data.action，471334直接W32 deathFlag=1。现再次取data且调用通用MarkDeath，后者包含action为空则跳过的额外判断。
- 类型3原text指针一次取样后同时判断和free；现data.text重复读取。
- 类型4/6/7原data一次取样后判断/free；现重复取值。
- 尾部原elements一次取样后判断/free；现重复取值。

<a id="window-release-source-decision"></a>
## 实施决定与保持项

1. 只WindowRuntime.cpp::FreeWindow_471270六片段：保存focusId→fresh window.id再比较；类型1/5保存全局focusedData→elementData再比较；类型2持同一次data样本并通过既有Yobi边界MarkDeathUnchecked承接原直接deathFlag=1；类型3持textToFree；类型4/6/7持data；尾部持elementsToFree。后四处用同样本判断/free或标志写，恢复原阶段，不跨回调缓存后续fresh槽。
2. 为case1/5、case2、case4/6/7局部变量增加各自已有switch分支作用域，不能跨case初始化。YobiAction只在YobiRuntime.cpp中定义，UI层不得直接访问字段；在既有YobiRuntime.cpp/公共头新增一个void MarkDeathUnchecked(YobiAction*)边界，正文无空判断只写deathFlag=1，不对应独立原函数/身份。单一非拥有T*接口不暴露布局或SDL对象；不增owner、字段/另一个窗口状态，不重构通用MarkDeath。
3. 原类型2无action空判断，本根经唯一新边界恢复原直接标志写，避免通用helper额外跳过；YobiRuntime::MarkDeath及其他已证调用正文保持，新边界不造原地址，不宣机器内联/ABI等价。仅本原点的正常写和判断有据，不专项模拟空指针/异常或宣故障等价；原data非空门禁保，类型3原无data空保护不新增。
4. release回调后字段复读、state/flags顺序、焦点取消、每轮fresh元素/数量、输入取消后fresh载荷、标志写后fresh槽free、文本free后fresh text清null/数据free与原7类default保持。最后仍fresh元素数组读→可选free→置null→释放保存实例；不新增窗口保护或故障处理。
5. 全关闭与marker两根全文保持。全关闭按71槽CloseWindow并清槽，七W32 occupancy清0；仅原BYTE标志非0才Y→X→socket原MIS62，忽略业务发送成功条件，发送后清标志并透传S32结果，flag0返0。marker仅71个state中window非空者置marker50=1，其余字段/空槽旧marker不改。
6. 新1797..1799三adapted F，公共UI46→49，两限定I/S；I仅单根六阶段实修，两正确主体不记新I。已有CloseWindow/BVQ原分类关闭、BVP原MIS、同源数字与typed载荷承载复用，不以此重计或验收全部父/callee/域。
7. U04/U11/V03/R04仅当前进展/来源增加，仍open。原网络格式明文、音频暂停/RPLAN.24、所有其它成员/完整门禁保；只必要静态/文本汇总/-Check，不构建、调试、测试、运行、产物或摘要。恢复源读取/写入行为不外推机器ABI、真实资源流及完整寿命已验。

<a id="window-release-source-acceptance"></a>
## 度量与验收

| 项目 | 本步要求 |
|---|---|
| 原证 | 三完整184条565B/四页、14direct六目标/一release间接、28跳3RET；11CALL/四尾跳与六阶段原点 |
| 源实修 | 一个既有根六片段、正确switch作用域；两个CPP/一个公共头，唯一native边界helper承接类型2原直接标志写，无新增保护 |
| 保持实现 | flags两W32合法读写、回调/后续fresh槽与释放顺序、全关闭/marker及通用MarkDeath正确全文保持；载荷/窗口接口头不动 |
| 追踪 | 三原新/F、公共UI49两个限定I/S；两正确主体无新增I |
| 汇总预期 | 语义1800/有效1793、基线1910（51.90%）、F948/E1102 |
| 完整门禁 | pending18/closed11/全叶0/globalfalse、R7已审467余2594保；全部release目标/父/callee、资源生产/正常owner寿命、机器ABI/运行与R8仍待 |

<a id="window-release-source-state"></a>
## 收口顺序

先SPEC与完整原证/源前态，再单根六阶段实修和真实三原新登记；局部合同、四任务及唯一进度同步，集中静态/生成汇总/-Check后记录实际值，精确提交命名快照cg2d-re-window-release-source-sampling-20261008。整体SPEC目标与原适用标准保持。

### BVR01本步实际静态结果

集中静态通过：三完整184条565B/四页、14direct六目标/一个release间接、28跳3RET；11CALL与四尾跳分列，原七类释放表逐项对应，71 marker与七W32 occupancy范围有据。原flags实际两W32读写，AND DL不是BYTE存储；现正确字段操作保。

FreeWindow单根六阶段精确实修：focusId→window.id、focusedData→elementData原序保存，类型2同data样本且经唯一Yobi边界承接原直接deathFlag=1，类型3固定text、类型4/6/7固定data，最后固定fresh元素数组。两个CPP/一个Yobi公共头、唯一必要native helper/API；UI不直接访问不完整YobiAction。局部变量各自case作用域，回调后fresh字段/槽、文本free后的fresh清null、元素数量复读和最终实例释放保。通用MarkDeath的额外空判断只在本原调用点不再借用，通用正文及其它调用/载荷/窗口接口保；为opaque YobiAction新增唯一void MarkDeathUnchecked(T*)在既有runtime/公共头内只写flag，不公开布局、不造原函数地址/身份；不加action保护、异常模拟或故障等价验收。全关闭与marker两正确主体全文保持、不计新增I。

完整机器和派生差集通过，文本汇总/-Check均退出0。新增1797..1799三adapted F、公共UI49，两限定I/S各24来源，I仅单根六阶段；实际语义1800/有效1793、基线1910（51.90%）、F948/E1102。四任务仍open，R7已审467余2594、pending18/closed11/全叶0/globalfalse、原差队列与CRT43保持。原格式明文/MIS62与实际结果、音频暂停/RPLAN.24及执行限制保；所有15入向/父/release目标/callee、资源生产与正常owner-alias寿命、机器ABI/完整画面运行与R8待验，整体SPEC目标继续推进。
