# 鼠标坐标临时量取样与发布 SPEC（BUU01）

2026-10-08，沿 BUT01 已修窗口输入的真实八个提供者继续开发。上位：[旧鼠标状态](cg2d_legacy_mouse.md)、[六个事件](cg2d_legacy_mouse_events.md)、[窗口修复](cg2d_window_input_gate_restore_spec.md)、[追踪验收](cg2d_traceability_acceptance_spec.md)。
固定快照：`cg2d-re-mouse-coordinate-publication-20261008`。

<a id="mouse-coordinate-publication-baseline"></a>
## 完整原基线与源码差异

[原指令、入向和八尾用途](cg2d_mouse_coordinate_publication_evidence.json)保存八个完整原根307条1279B/八页/八连续chunk，34个直接call、四个GetTickCount间接call、两jump、八RET。十四个直接入向全部来自窗口回调：位置一处，六按键各两处，滚轮一处；不等于完整窗口已验收。

| 原入口/库存ID | 半开边界 | 指令/字节 | 新端符号/原新结论 |
|---|---|---:|---|
| 506890 / F:00106890 | 506890..5068FF | 27/111 | ResetLegacyMousePosition_506890；配对发布需修 |
| 506900 / F:00106900 | 506900..5069DD | 57/221 | UpdateLegacyMouseLeftDown_506900；配对发布需修 |
| 5069E0 / F:001069E0 | 5069E0..506A8B | 40/171 | UpdateLegacyMouseLeftUp_5069E0；配对发布需修 |
| 506A90 / F:00106A90 | 506A90..506B44 | 40/180 | UpdateLegacyMouseLeftDoubleClick_506A90；配对发布需修 |
| 506B50 / F:00106B50 | 506B50..506C32 | 58/226 | UpdateLegacyMouseRightDown_506B50；配对发布需修 |
| 506C40 / F:00106C40 | 506C40..506CEB | 40/171 | UpdateLegacyMouseRightUp_506C40；配对发布需修 |
| 506CF0 / F:00106CF0 | 506CF0..506DA4 | 40/180 | UpdateLegacyMouseRightDoubleClick_506CF0；配对发布需修 |
| 506DB0 / F:00106DB0 | 506DB0..506DC3 | 5/19 | AddLegacyMouseWheelDelta_506DB0；正确保持 |

前七根均复用同一16B栈临时量，X和Y各独立调用401CC0构造、按分支读取三字段、按原序发布目标payload、调用401CF0析构；先完成X整个阶段，才开始Y。十四构造/十四析构，沿构造各一次现有RNG，不读取坐标明文或额外消耗RNG。当前两次AssignFromInt虽保持构造次数，却用通用CopyPayload合并不同读取/发布顺序。

| 分支 | X源读取/目标发布次序 | Y源读取/目标发布次序 | 目标 |
|---|---|---|---|
| 位置 | encoded→mask→readCount | readCount→encoded→mask | mouseX、mouseY |
| 左按下 | mask→encoded→readCount | mask→encoded→readCount | buttonCoordinates[0/1] |
| 左抬起 | encoded→mask→readCount | readCount→encoded→mask | [2/3] |
| 左双击 | encoded→mask→readCount | readCount→encoded→mask | [0/1] |
| 右按下 | mask→encoded→readCount | mask→encoded→readCount | [4/5] |
| 右抬起 | encoded→mask→readCount | readCount→encoded→mask | [6/7] |
| 右双击 | encoded→mask→readCount | readCount→encoded→mask | [4/5] |

每次必须先按表读齐全部三个源字段，再按表写目标；不能读写交错、统一顺序、整体结构复制或写目标token/reserved。原reset与up/double的Y先读W8，按下两轴先读mask W32。
六事件旗位读取/发布、目标槽、时钟与返回已经正确：左按下edge/pulse/held先读齐再同序发布；右按下held/edge/pulse先读齐，再pulse/held/edge发布。up/double先读pulse/edge、发布pulse，再读held、发布edge/held；不预读held。

普通down坐标后独立tick写对应downTick/elapsed0，再Track(x,y,0/1)，fresh S32阈值比较，小于返Track原值；否则读当前socket、Report(socket,1)、再Reset并返其值，无额外连接门禁。up不取tick，只清对应两个时钟槽并返0；double独立tick、清elapsed并返原W32样本。滚轮读D68DC0的W32尾槽、加delta、写回，返回入口delta；现有代码正确，不为纳管修改或登记新增I。
原八尾1+3+5+12+14+5+12+13=65B均实际NN_NOP/90，无入向、后接真函数，只归对齐用途，不制造函数或排除。

<a id="mouse-coordinate-publication-decision"></a>
## 委托裁决与实施边界

依用户工程委托先固定本SPEC，采用一cpp七个配对片段的修复：

1. 每根声明一个既有LegacyCryptInt局部，同一对象两次构造与析构；保留正常Ctor→三字段取样→三字段发布→Dtor→下一轴顺序。
2. encoded/mask采用U32，readCount采用U8，先读齐再写；目标token与reserved不动。沿既有401CC0/401CF0提供者，不新建helper、函数、owner、第二份状态或RNG。
3. 六旗位、down追踪及真实Report/Reset调用、tick数量/阶段与返回正文保持；滚轮完整正确正文保。所有初始化/退出/注册、头文件、窗口其余消息及本组外提供者保持。
4. 八原新映射1723..1730按现有固定布局/消息定宽与宿主时钟边界登记adapted；不把既有实现登记成八个新增实现，七根实际发布修复才入限定I。位置void只承接本次唯一原caller弃返回，不声称原EAX固定值或全部间接ABI已验。
5. 旧鼠标文档“统一AssignFromInt保持读取顺序”由本步具体矩阵覆盖。窗口入口现在已有真实调用，本步确认十四局部调用，不外推整个窗口或所有输入焦点。旧回绕用词仅保原W32宽度事实，不按此建立异常策略、模拟或等价门禁。

内部LegacyCryptInt沿原状态布局保，不是网络加解密。网络保持原格式明文/两加解密叶排除，音频暂停不扩，RPLAN.24越界/数学异常不专项处理、不猜语义或降低正常验收标准。

<a id="mouse-coordinate-publication-acceptance"></a>
## 可度量的限定验收

集中核八根连续307条1279B/八页、十四Ctor/十四Dtor、四间接tick和两追踪尾链、十四窗口引用及八65B用途。
逐分支对照三字段读取/发布，静态核同一局部对象、X析构后才构造Y、目标token/reserved不写、无CopyPayload/AssignFromInt残留；用原全文构造仅七片段差异模型，核一cpp其他全部内容/滚轮及头文件正确保留。

登记八新原新基线/adapted和八R7对齐用途；LOOP136→144/INPUT30→38，各限定I/S两证据、各25来源，I只七根发布修复、S含滚轮正确保留。预期语义1731/有效1724、文档1842（50.05%）、F处置872/E1026、R7已审437/余2624。完整功能门禁仍开放，pending18/ECX pending7、差队列及原closed11不变；T02/R04/V04只current和引用，保持open。

文本汇总及Check生成派生，完整差异模型和真实来源/编码核对；不构建、测试、运行算法样例/客户端、检查产物或计算摘要。
这些静态证据不证明全部原父/callee、原生ABI、焦点/IME、资源/alias寿命、业务失败、画面、文件、明文互通或R8完成；完整适用验收仍需继续。

<a id="mouse-coordinate-publication-source-state"></a>
## 本步状态

方案已依工程委托确定七配对发布修复和一正确滚轮保持。执行与实际静态结果同步唯一进度，整体目标继续active。

### BUU01 实际限定静态结果

八完整原根307条1279B/八页、十四Ctor/十四Dtor、四间接tick/两追踪尾链、十四引用与八65B用途，七配对原读写矩阵/同一临时量X先Y后/目标token保、整个cpp仅七片段差异、完整机器及派生差异模型、十三文件编码/四锚点和四限定I-S各25来源已通过。文本汇总与Check均退出0。
实际语义1731/有效1724、文档1842（50.05%）、F872/E1026、R7已审437/余2624、LOOP144/INPUT38；其他域/分母/差队列/pending18及ECX pending7/closed11与全门禁保。源码一cpp七配对实修，滚轮正确保持不计I；未编译、链接、运行或证明完整窗口/输入验收，整体目标继续active。

## BUU01旗位通过断言失效与纠正（BUW01，2026-10-08）

[本步数值原证](cg2d_cursor_flag_sampling_restore_spec.md#cursor-flag-sampling-baseline)与[纠正裁决](cg2d_cursor_flag_sampling_restore_spec.md#cursor-flag-sampling-decision)明确撤销本文“六旗位原采样/发布正确保”的断言及其四个过宽E-MOUSE-COORDINATE-PUBLICATION限定I/S通过。原D68D98 held/D68D9C edge/D68DA0 pulse直接操作数，与IDA名称直觉不同；旧snapshot六源旗位读写阶段有误。本次已修六块；登记簿四旧result改fail并保存previous_result=pass/错误范围/新更正引用，不删除原记录/tag或把旧snapshot修成通过。

原七配对坐标的十四Ctor/十四Dtor、各分支三字段先读齐后发布矩阵仍有独立完整原指令证据，继续保留；其它正常位运算、tick/追踪/返回正文和标准owner布局不动。八旧语义/F都追加本更正，旧组合flags断言置false，新数值阶段恢复事实另记。本文先前实际静态通过是当时模型结论，旗位部分现明确失效，不能作为当前有效验收。

当前BUW01另立六数值旗位和光标取样修复证据；完整门禁/原功能分母和运行限制保持，不以纠正或源码存在宣称所有别名/ABI/输入运行验收。

### BUW01 当前修复限定静态结果

四完整原根193条708B/六页、六数值旗位36条214B编码/地址双证、原11标量88B/四28B用途、两cpp仅十三片段/正确配对与tick-tracker尾链保、光标原计时-pulse-请求-Y-X-API/公共尾阶段、完整机器/派生差异模型、十五文件编码/四锚点及七新限定证据各25来源均通过。文本汇总与Check均退出0。
四旧BUU01过宽I/S已明确fail，previous pass/独立正确坐标事实和更正引用保留，当前修复七新限定证据pass；这是旧snapshot判断失效的历史记录，不是当前代码故障遗留，未删除失败或用新修复改写旧通过。
实际语义1745/有效1738、文档1856（50.43%）、F886/E1039、R7已审448/余2613、LOOP158/INPUT51/RENDER59/STARTUP118；其他域/原分母/差队列/pending18与ECX pending7/closed11和完整门禁保持。未编译、链接、运行或证明完整原父/别名/ABI/显示验收，整体目标继续active。
