# 光标取样与鼠标旗位原地址纠正 SPEC（BUW01）

2026-10-08，沿BUV01原生追踪后回到鼠标状态的逐帧生产/绘制。上位：[旧光标](cg2d_legacy_cursor_update.md)、[六事件](cg2d_legacy_mouse_events.md)、[BUU01配对发布](cg2d_mouse_coordinate_publication_spec.md)、[验收体系](cg2d_traceability_acceptance_spec.md)。
固定快照：`cg2d-re-cursor-flag-sampling-restore-20261008`。

<a id="cursor-flag-sampling-baseline"></a>
## 原事实与已发现差异

[原完整页和数值地址](cg2d_cursor_flag_sampling_restore_evidence.json)固定四根193条708B/六页、六directcall/六indirectcall/27jump/四RET，18引用15父仅身份。原88B为11个光标图号/packed offsets标量记录，步长12；四尾28B实际NN_NOP/90无入向/后真函数。六鼠标事件只复取旗位相关36条214B窗口，不重新验整个旧根。

| 原入口/ID | 半开边界 | 指令/字节 | 当前源码 |
|---|---|---:|---|
| 506E00 / F:00106E00 | 506E00..507083 | 174/643 | UpdateCursor_506E00；取样/额外策略需修 |
| 506DE0 / F:00106DE0 | 506DE0..506DFB | 9/27 | RequestCursorState_506DE0；正确保 |
| 506DD0 / F:00106DD0 | 506DD0..506DDB | 2/11 | ResetLegacyMouseTail_506DD0；正确保 |
| 506870 / F:00106870 | 506870..50688B | 8/27 | ResetLegacyMouseState_506870；正确保 |

### BUU01旧旗位推断纠正

原数值地址证明held=D68D98、edge=D68D9C、pulse=D68DA0；IDA名称gMouseBtnDown2实际D68D98，gMouseBtnDown实际D68DA0。此前按名称把held/pulse读序反置，BUU01“旗位原序正确保”结论不成立，不能继续保pass。原三字段坐标发布矩阵仍有独立完整指令支持，未把该正确部分撤掉。

| 事件 | 原读/写序（R/W） |
|---|---|
| 左按下 | R edge→held→pulse，W edge→held→pulse |
| 右按下 | R pulse→edge→held，W held→pulse→edge |
| 左右up/double | R held→edge，W held，再R pulse，W edge→pulse |

最终位运算不变：left/right目标位仍1/2，up分别edge|4/8、held/pulse&~1/~2，double edge|11/22及held/pulse置1/2。问题是数值slot的取样/发布阶段，不改字段布局/位义、配对坐标/RNG/tick/追踪与返回。现六源块需纠正。

### 逐帧取样与额外策略

原入口先保存leftDownTick，再清edge W32。左非零独立tick后fresh左down做差并发布leftElapsed，否则保存旧leftElapsed；右非零再次独立tick、fresh右down做差，然后506E43重新保存leftElapsed，506E48才发布rightElapsed；右为零只保存旧rightElapsed。后左右pulse用这两个保存值，不能等右发布后再从全局重新取两侧。

pulse真实owner为D68DA0，不需改字段归属。elapsed<=500：先保存pulse，再清anchor/清本bit；anchor0：先保存pulse，再anchor=elapsed/置本bit。左已有anchor时先差值/100判断，再读pulse并清/置；右已有anchor时506EE5先保存pulse再判断100，随后可写anchor和同一保存值。当前通用helper把部分pulse读取移到anchor写后/判断后，需恢复两侧差异。

表驱动原image→Y+16/独立请求/有符号Y offset→X+16/独立请求/X offset→Draw，三次请求和Y后X/RNG保。当前状态0..10拒绝、canDraw和AddSigned16Wrapping是额外策略，按RPLAN.24撤；正常S32 offset直接加，图号/偏移bit_cast只保持位型不属于算术回绕，保留。系统分支只保原请求变化/API顺序/矩形条件/再取请求选句柄，无额外state拒绝。POINT/RECT零初始化属既有API失败局部适配，本步保持，不增加成功门禁。公共尾consumed→请求清0→gValue9A6F44→TailReset原序保。

请求9条force非零不读旧值/否则保存旧值非0返旧或写新、尾字段两条清0、鼠标D0零化后DB8=-1三正确根保。当前MainLoop/LoadingScreen和ClientInitialization已真实调用，旧文档“未接Loading/Init”仅历史，不作本次全父验收。

<a id="cursor-flag-sampling-decision"></a>
## 委托裁决与最小实施边界

1. 两cpp十三片段：六事件仅换正确数值字段取样/发布序，其他完整正文保；光标七片段恢复tick/pulse采样、撤state拒绝/canDraw与offset算术wrapper。两个既有helper退休，无新helper、owner、表、头文件或函数。
2. UpdatePulse保原阈值500/100和现位型/anchor；用现bit==2区分右侧“先读pulse再判断”阶段，来源是两个已证固定调用，不扩其他业务。两elapsed保存后依序传入，原0/1/2次tick、Y/X读取/RNG消费数保。
3. 原11图号/offset、状态0独立可写记录及1..10共享ResourcePaths owner保持，不复制表；原16B描述符/Draw参数、API局部适配及公共尾保。
4. 四新语义/adapted登记；八旧BUU01语义/F追加本更正，原旧旗位组合assert置false并记更正事实，当前修复验证另记。四个旧E-MOUSE-COORDINATE-PUBLICATION限定I/S改fail、保存旧pass标记/坐标正确部分、附此数值原证和新裁决；保原记录与tag，不将旧误判算当前有效通过。
5. 既有其他证据仅当本次事实直接推翻才改；不外推整片窗口/输入/网络或降低验收标准。网络原格式仅明文、内部LegacyCryptInt、音频暂停及RPLAN.24异常不专项/不模拟保持。

<a id="cursor-flag-sampling-acceptance"></a>
## 可度量的限定验收

集中静态核四完整原根、六数值旗位窗口36条214B、原11标量88B及四28B用途；直接按字节编码/操作数地址核六事件R/W矩阵，核保持14Ctor/14Dtor发布正文与全部后续调用不变。
核原tick/pulse阶段、三请求/Y后X/正常S32偏移/系统API与尾部，整个两cpp仅十三批准片段/零头文件及其他源码变更；三正确根完整保，四旧E确为fail/更正引用可读，八旧语义/F保稳定ID/owner/符号。

四新1741..1744/adapted及四R7用途，LOOP154→158/INPUT47→51/RENDER57→59各限定I/S，STARTUP117→118仅S，共七新证据各25来源。预期语义1745/有效1738、文档1856（50.43%）、F886/E1039/R7已审448余2613；旧四E由pass改fail不删分母，完整门禁/accepted0及差队列/pending18/ECX pending7/closed11保持。T02/R04/V04仅current/引用，仍open。
文本汇总与Check生成派生，完整差异模型与UTF8/锚点核对；不构建、测试、运行客户端/算法样例、检查产物或摘要。完整原父/callee、正常业务失败、所有owner/alias寿命、焦点/IME、显示像素、机器ABI/链接/运行与R8继续。

<a id="cursor-flag-sampling-source-state"></a>
## 本步状态

字段名称误判已明确撤销旧通过；按数值地址、正常取样和RPLAN.24具名裁决实施，实际静态结果同步唯一进度。整体目标继续active。

### BUW01 当前修复限定静态结果

四完整原根193条708B/六页、六数值旗位36条214B编码/地址双证、原11标量88B/四28B用途、两cpp仅十三片段/正确配对与tick-tracker尾链保、光标原计时-pulse-请求-Y-X-API/公共尾阶段、完整机器/派生差异模型、十五文件编码/四锚点及七新限定证据各25来源均通过。文本汇总与Check均退出0。
四旧BUU01过宽I/S已明确fail，previous pass/独立正确坐标事实和更正引用保留，当前修复七新限定证据pass；这是旧snapshot判断失效的历史记录，不是当前代码故障遗留，未删除失败或用新修复改写旧通过。
实际语义1745/有效1738、文档1856（50.43%）、F886/E1039、R7已审448/余2613、LOOP158/INPUT51/RENDER59/STARTUP118；其他域/原分母/差队列/pending18与ECX pending7/closed11和完整门禁保持。未编译、链接、运行或证明完整原父/别名/ABI/显示验收，整体目标继续active。
