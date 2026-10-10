# 倒计时界面消费 SPEC（TUC01）

2026-10-10。继续在 cg2d_re 分支完成整体 SPEC，不打 tag。依子 AGENTS 第8条工程委托，先记本 SPEC，再修正已证源码差异；本步覆盖两个状态栏完整原根与两套转生按钮，保持原业务门槛和既有替换边界。

<a id="timer-ui-consumption-baseline"></a>
## 原根、提供者与真实接线

| 原根 | 完整范围（尾不含） | 指令/字节/分页 | E8/跳转/RET |
|---|---|---|---|
| 470AC0，窗口1状态栏 | 470AC0–47116E | 463/1710/8 | 21/40/2 |
| 4718F0，窗口2状态栏 | 4718F0–471CC7 | 298/983/5 | 17/6/1 |
| 497C80，战斗转生按钮 | 497C80–497E24 | 119/420/2 | 5/14/3 |
| 47B210，窗口10转生按钮 | 47B210–47B3CF | 137/447/3 | 9/16/3 |

共1017条3560B/18页、52个实际E8/14个目标、76跳9RET。七个数字绘制物理点按mode分支执行四次，不能按静态调用数增加绘制。两状态栏分别只有470A9F/470AB0实际E8入向；IDA将尾部单列，不据此重划目录。现Sub_470A80及Sub_470AB0实接/返回1保持；这里只核对应局部调用窗，不宣布完整开窗/父路由通过。

两个按钮的IDA数据引用落在整张元素表起点；实际机器字节证明56B710第7项（0起）+1C为497C80、561B78第6项+1C为47B210。现WindowPrototypes.cpp分别接窗口26/10对应同项，原型32B与运行元素36B分别映射；只证明两项，不重新验收整表/原型/owner。仅地址用原生指针，字段非地址宽度保持。

timerRemaining FDA70C为S32、timerMode FDA710为完整S32；沿PTP01/GTP01同源生产、扣减及既有reader。DrawStatusBarTimer/FormatStatusBarVitals、BorrowBattleRebirthSlot106C和字段reader为已有本机helper，不捏造独立原F。52个原调用的实际目标、现提供者和已有身份分别记录；未独立纳管的callee仅列真实源码，不能代称其完整根或生命周期闭合。

<a id="timer-ui-consumption-decision"></a>
## 正常合同与裁决

两个状态栏原16B图形描述符均先完整清零，现descriptor {}保持；256B格式缓冲原未预清，现buffer保持。既有HP/FP场景10、位置<20、初始action非空分支、最大后当前独立采样、S32/%4d/%4d及24B/28B目标保持。窗口1的FP复制先strlen+1再持有timeWindow；窗口2直接strcpy，不增加窗口读取。原CRT按已批MSVC替换、54F638时钟F64常量及正常S64截断边界保持；不恢复旧CRT/x87算法。窗口1时间图形和加点/伤势/邮件状态及闪烁生产完整源保持，窗口2不增加提示生产；原VOID裁决保持，机器残值ABI仍待。

共享时长helper只被两个状态栏以firstElement=8/2调用，先一次S32 remaining→hours→小时十位/个位→minutes→分钟十位/个位，恢复小时位先于minutes形成。三元素visible分别fresh窗口清零；一次完整S32 mode读取，非0图号基数3B6A0、0为3B664。描述符+8颜色FFFFFFFF、+13值4保持；窗口1原mode后先取首窗口，再写描述符，窗口2写描述符后才取首窗口。以既有firstElement区分这两个取样时点，不增公共参数/API、门禁或默认指针；后面三个数字仍各fresh窗口。

四数字元素映射0/0/1/2：第一位element.y→element.x→window.y；第二/三位window.y→element.y→element.x；第四位element.y→window.y→element.x。各自齐取这三项后计算y+3，再取window.x并算第一位x-6/其余x+1；恢复三类坐标取样序，不将各次窗口或元素字段合并缓存。四次DrawGraphic仍layer DF/参数0/同描述符、弃返回，不省前导0、不加冒号/格式/限制。

497C80保持初始元素/图形借用，5ABC40字节、玩家值08、remaining>0门槛；失败visible0/图号3B9CE/返回0，成功visible1后窗口26状态和选择模式8决定图号。初始action和payload分别原有判空，BorrowBattleRebirthSlot106C现正常空分支保留；hover位1按原一次持有供文本与尾图号。click位2按CloseSubwindow→选择槽8=1/模式8→FlushEntries→fresh位置/action/payload槽106C决定R|0或R|1→原socket发送弃返→fresh命令action标死→清指针→动作状态1→SE57/320/240→consumed1。末图号3B9CD/hover3B9CF/重新读取选择槽优先3B9CE，不能因发送结果回滚。

47B210恢复初始window→elements→单U8动作标志→原held元素图形的读取序，改用既有ReadPlayerActionFlagsByte，不扩宽为W16。随后原flag80/440B50/值08/remaining/canRebirth业务短路及失败fresh窗口visible0保持。持有转生BYTE，再一次持有hover位1；锁定BYTE初读控制click只播放SE55/320/240且不消费。未锁click先持有转生状态选择opcode62/61，Y→X crypt读取→freshsocket发送弃返，然后读occupancy[7]旧BYTE→发布转生0/1→原低BYTE减/加→SE51/320/240→consumed1。图号3B57A→hover3B57C→state位8优先3B57B→fresh锁定BYTE再次优先3B57B；既有GBK提示字节/WAV-only声音及网络命令/字段保持。

仅八逻辑组：小时位计算一组、首窗口取样一组、三类坐标三组、单字节flag取样一组、两回调hover持有各一组；三个既有body及关联注释最小调整，两状态栏父body、格式/helper/表/类型/其它源保持。无新函数/全局/工具/API/布局/owner。RPLAN.24不新增空值/下标/容量/输入限制、保护、默认值、补零、钳制、回滚、异常或溢出模拟；原业务零初始化/失败/门槛保留。

<a id="timer-ui-consumption-acceptance"></a>
## 限定静态验收

核四完整连续原根/边界/实际E8及跳转返回、两父局部窗、两原型实际项字节/现接线及现提供者。整WindowRuntime.cpp/WindowRuntimeState.hpp精确重放八逻辑组与指定关联注释；四父/共享格式、字段与载荷helper及所有其它源保持。原1017指令提供完整对照范围，限定源码正常合同S，不保证优化后机器读取宽度、独立加载/指令、并发或ABI/数值等价。

旧999/1000保历史子记录追加当前完整对照，两按钮新增两个语义；四根各补F处置。UIRUNTIME四成员/BATTLE仅497C80一成员、两限定S零I；U08/U09/U10/U11/G01/R01/R04/V03/V04九任务保持open。其它语义/证据/政策/完整gates、功能枚举、六closure与总SPEC§10保持，不将关联率称完成率。

只做必要文本静态、measure_cg2d.ps1及-Check，不构建/测试/运行客户端或算法样例、不检查产物/计算摘要/引依赖。完整父/窗口路由/原型/全部callee、字段生产消费/alias-owner、渲染/网络/音频/实际交互、SDK/数值/机器ABI及全生命周期待；WAV-only、MSVC直接CRT和原格式明文的已批边界不变。

<a id="timer-ui-consumption-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐倒计时回复生产并恢复缩放发布顺序”；本步拟提交说明“[#] 补齐倒计时界面消费并恢复字段取样”。原完整取证、源码八组、核心静态/登记、文本汇总/Check及派生范围核对通过。语义2220=2172F+48X，文档关联2282/3680=62.01%仅关联率，F处置1654/证据1456，UIRUNTIME71/BATTLE425仍未验收。派生只改六metrics/四F/两功能/九任务，184叶验收0、60任务关闭11、六closure开放/全部完整gates及其它字段守恒。整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_timer_ui_consumption_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐倒计时界面消费并恢复字段取样」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x470AC0 | 0x47116E | 463/1710 |
| 0x4718F0 | 0x471CC7 | 298/983 |
| 0x497C80 | 0x497E24 | 119/420 |
| 0x47B210 | 0x47B3CF | 137/447 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
