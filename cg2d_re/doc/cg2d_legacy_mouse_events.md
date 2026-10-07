# 旧鼠标六个按键事件

## 范围与证据

本项还原六个旧鼠标按键事件入口。取证只使用当前只读 IDA 实例
`ida-17524` 的 `D:\SOFTSTAR\cg2d.exe`、对应二进制和当前源码；目标为
32 位 x86、小端、基址 `0x400000`，SHA-256 为
`e7c2790f479862635932e043b2d7e93b9e791a1edc276a960af05f1454c54010`。
本项未修改 IDA，也未使用 STONAGE 作为行为证据。

| 原函数 | 半开边界 | 指令/基本块 | 新端入口 |
| --- | --- | ---: | --- |
| `MouseEvent::UpdateLDown` | `[0x506900,0x5069DD)` | 57/3 | `UpdateLegacyMouseLeftDown_506900` |
| `MouseEvent::UpdateLUp` | `[0x5069E0,0x506A8B)` | 40/1 | `UpdateLegacyMouseLeftUp_5069E0` |
| `MouseEvent::UpdateLDbClick` | `[0x506A90,0x506B44)` | 40/1 | `UpdateLegacyMouseLeftDoubleClick_506A90` |
| `MouseEvent::UpdateRDown` | `[0x506B50,0x506C32)` | 58/3 | `UpdateLegacyMouseRightDown_506B50` |
| `sub_506C40` | `[0x506C40,0x506CEB)` | 40/1 | `UpdateLegacyMouseRightUp_506C40` |
| `sub_506CF0` | `[0x506CF0,0x506DA4)` | 40/1 | `UpdateLegacyMouseRightDoubleClick_506CF0` |

六个函数的直接代码引用分别位于 `0x467431/0x467488`、
`0x4674C4/0x46751B`、`0x467564/0x4675BB`、`0x467604/0x46765B`、
`0x467697/0x4676EE`、`0x467755/0x4677AC`；IDA 均归入唯一 caller
`WIN_PROC@[0x466DC0,0x467904)`。

## 位字段与坐标槽

三个 W32 位字段仍使用 `LegacyMouseState` 的唯一存储：`heldButtons` 对应
`0xD68D98`，`edgeButtons` 对应 `0xD68D9C`，`pulseButtons` 对应
`0xD68DA0`。每次操作只置位或清除目标位，保留同一 W32 的其他位。

| 事件 | held | edge | pulse | `buttonCoordinates` |
| --- | --- | --- | --- | --- |
| 左键按下 | `| 1` | `| 1` | `| 1` | `[0]=x`、`[1]=y` |
| 左键抬起 | `& ~1` | `| 4` | `& ~1` | `[2]=x`、`[3]=y` |
| 左键双击 | `| 1` | `| 0x11` | `| 1` | `[0]=x`、`[1]=y` |
| 右键按下 | `| 2` | `| 2` | `| 2` | `[4]=x`、`[5]=y` |
| 右键抬起 | `& ~2` | `| 8` | `& ~2` | `[6]=x`、`[7]=y` |
| 右键双击 | `| 2` | `| 0x22` | `| 2` | `[4]=x`、`[5]=y` |

实现保留反汇编中的字段访问次序：左键按下按 `edge/pulse/held` 读取并按同序
写回；右键按下按 `held/edge/pulse` 读取、按 `pulse/held/edge` 写回；抬起和
双击均先读 `pulse/edge`、写 `pulse`，再读 `held`、写 `edge/held`。每个入口
都严格先对 X、后对 Y 各调用一次既有 `legacy_crypt_int::AssignFromInt`，因此每次
事件恰好消耗两次共享随机序列；没有增加坐标裁剪、校验或锚点写入。

## tick、返回值与按下追踪尾链

| 事件 | tick/elapsed | 返回值 |
| --- | --- | --- |
| 左键按下 | 坐标后调用一次 `ReadTickCount32()`，写 `leftDownTick`，`leftElapsed=0` | 追踪尾链结果 |
| 左键抬起 | `leftDownTick=0`，`leftElapsed=0` | `0` |
| 左键双击 | 坐标后调用一次 `ReadTickCount32()`，写 `leftDownTick`，`leftElapsed=0` | 刚采样的 W32 tick |
| 右键按下 | 坐标后调用一次 `ReadTickCount32()`，写 `rightDownTick`，`rightElapsed=0` | 追踪尾链结果 |
| 右键抬起 | `rightDownTick=0`，`rightElapsed=0` | `0` |
| 右键双击 | 坐标后调用一次 `ReadTickCount32()`，写 `rightDownTick`，`rightElapsed=0` | 刚采样的 W32 tick |

只有普通按下进入追踪器：左键调用
`TrackLegacyMouseDown_5067F0(x,y,0)`，右键调用
`TrackLegacyMouseDown_5067F0(x,y,1)`。返回值与唯一
`gLegacyMouseTrackerThreshold` 按 signed32 比较；小于阈值时原值返回，否则
严格先调用 `Report_512CA0(gConnectionSocket,1)` 并丢弃其返回值，再调用并返回
`ResetLegacyMouseTracker_506580()`。此路径不增加连接状态检查。抬起和双击均不
调用追踪器，也不合并左右 tick。

## 未接线范围与 x64 边界

本项只在既有 `LegacyMouse.cpp` 中实现入口并在公开头中声明，没有实现或接入
`WIN_PROC`，也没有改 CMake。原 caller 中的窗口激活门禁与坐标缩放仍属于
`WIN_PROC`，没有下沉到六个事件函数。`Report_512CA0` 只复用
`MainLoopDependencies.hpp` 的真实未实现声明，socket 只复用 `MainLoop.hpp` 的
唯一 `gConnectionSocket`；没有定义伪实现或报告前连接检查。

新端参数继续固定为 signed32，位字段、tick 和双击返回值固定为 W32；
`ReadTickCount32()` 保留 Win32 回绕。`gConnectionSocket` 仅在既有本机边界使用
`std::uintptr_t`，事件实现不截断本机句柄。追踪器的 W32 链接槽继续使用既有
handle 适配，不保存 x64 指针；六个事件没有新增指针布局或第二份 legacy 状态。

本项只做人工静态自查，未编译、测试、lint、格式化、生成代码、运行客户端或读取
任何 `.obj`、静态库及其他编译产物。

## 六事件配对坐标发布顺序恢复（BUU01，2026-10-08）

[八根基线](cg2d_mouse_coordinate_publication_spec.md#mouse-coordinate-publication-baseline)合307条1279B/八页，其中本文六事件原完整基线全部纳入；另有位置506890和正确滚轮506DB0。十四原窗口引用仅局部调用身份，不称全窗口复核。八尾65B实际NN_NOP/90无入向/后真函数，只用途。

现有六旗位读写阶段、tick/elapsed、返回与普通down的Track→fresh S32 threshold→可选Report→Reset正确全保；leftdown/普通rightdown各轴mask→encoded→count，其他事件X encoded→mask→count、Y count→encoded→mask。每轴在构造后按原序读齐三字段，再按原序发布，保目标token/reserved并析构，然后下一轴。原先通用AssignFromInt合并读写顺序，按[裁决](cg2d_mouse_coordinate_publication_spec.md#mouse-coordinate-publication-decision)改为同一临时量显式十四Ctor/十四Dtor，不增RNG/共享helper/owner。六事件与位置合一cpp七片段，滚轮正确全保不计I。

本文“WIN_PROC未接入、Report只有未实现声明”保历史：当前WindowProcedure有实际六事件调用，MainLoopDependencies通过using接network::Report_512CA0，NetworkRuntime.cpp已有本体；本步保这条真实调用及socket，不复核或声称明文互通运行。源旗位/时钟/追踪/阈值/sockets/返回及所有初始化退出不改，窗口裁决仍BUT01。

八语义/F与八R7用途补齐，LOOP144/INPUT38限定I/S各25来源，E1026；完整焦点/IME/原父-callee/owner-alias寿命、正常业务失败、ABI/链接和运行未验，异常不专项策略或模拟，网络原格式明文/音频暂停保持。

### BUU01 实际限定静态结果

八完整原根307条1279B/八页、十四Ctor/十四Dtor、四间接tick/两追踪尾链、十四引用与八65B用途，七配对原读写矩阵/同一临时量X先Y后/目标token保、整个cpp仅七片段差异、完整机器及派生差异模型、十三文件编码/四锚点和四限定I-S各25来源已通过。文本汇总与Check均退出0。
实际语义1731/有效1724、文档1842（50.05%）、F872/E1026、R7已审437/余2624、LOOP144/INPUT38；其他域/分母/差队列/pending18及ECX pending7/closed11与全门禁保。源码一cpp七配对实修，滚轮正确保持不计I；未编译、链接、运行或证明完整窗口/输入验收，整体目标继续active。

## 按下追踪尾链的原生链接承接（BUV01，2026-10-08）

[新原生链接基线](cg2d_mouse_tracker_native_links_spec.md#mouse-tracker-native-links-baseline)与[裁决](cg2d_mouse_tracker_native_links_spec.md#mouse-tracker-native-links-decision)沿BUU01正确鼠标生产后的Track/Report/Reset展开十根262条799B/十一页。LegacyMouse.cpp七配对坐标与所有旗位/时钟/阈值和Report→Reset尾链全文保，不重复BUU01实现；Report现原PGXE字串5B/长度保存/token前后fresh buffer/发送返回正确。

追踪Node32的W32 handle与解析保护已改原生非拥有T*，只拓宽已证next/prev，四S32和唯一owner不变；恢复malloc返回先发布含null再回退、pool/容量原采样/最终index。原null/哨兵分支和LRU链写序仍保，计数普通S32增一不专设回绕。本文早期“W32链接继续handle”仅历史，当前原24B固定字节观察保，x64物理节点随指针拓宽。完整所有父/提供者/焦点/owner-alias寿命/ABI/明文运行及R8继续。

### BUV01 实际限定静态结果

十完整原根262条799B/十一页、14directcall/34jump/19RET及25引用13父身份、原17B/七65B用途、两源码仅十二批准片段、原T*字段/原nullptr-哨兵/链写序及失败发布和pool-capacity采样/index返回、固定24B只读视图移交、完整机器及派生差异模型、十六文件UTF8/LF/四锚点及六限定证据各25来源已通过。文本汇总与Check均退出0。
实际语义1741/有效1734、文档1852（50.33%）、F882/E1032、R7已审444/余2617、LOOP154/INPUT47/NETWORK63/LOGIN13；原分母/其他域/差队列/pending18与ECX pending7/closed11和完整门禁保。七原根和BYTE桥实修/三旧helper退休，默认/网络配置/Report三正确正文保持不计新增I；未编译、链接、运行或证明完整窗口/登录/配置/明文互通验收，整体目标继续active。

## held/pulse取样发布按原数值地址纠正（BUW01，2026-10-08）

[本步数值原证](cg2d_cursor_flag_sampling_restore_spec.md#cursor-flag-sampling-baseline)与[纠正裁决](cg2d_cursor_flag_sampling_restore_spec.md#cursor-flag-sampling-decision)直接保存六事件36条214B数值指令窗。IDA名称gMouseBtnDown2实际D68D98 held，gMouseBtnDown实际D68DA0 pulse；原BUU01与本文关于两者读写序的名称推断错误，四过宽I/S改fail、旧pass标记和正确坐标三字段事实保留，不能继续按旧文认定旗位等价。

现六块已修：leftdown R edge/held/pulse，W edge/held/pulse；rightdown R pulse/edge/held，W held/pulse/edge；up/double R held/edge→W held→R pulse→W edge/pulse。原OR/AND/edge bit、三个W32唯一字段/布局、十四Ctor-Dtor取样发布正文、tick/Track阈值/Report→Reset及返回全文保。仅六个旗位块，不调其他消费者/位义或改网络格式。

新光标每帧计时/pulse取样也依同一D68DA0 owner修复，并撤额外state拒绝/偏移回绕helper；请求/尾及D0状态清理三正确根保。完整父/callee/字段所有别名消费者/焦点/owner寿命/机器ABI/显示运行未验，RPLAN.24异常不专项或记等价、网络明文/音频暂停保持。

### BUW01 当前修复限定静态结果

四完整原根193条708B/六页、六数值旗位36条214B编码/地址双证、原11标量88B/四28B用途、两cpp仅十三片段/正确配对与tick-tracker尾链保、光标原计时-pulse-请求-Y-X-API/公共尾阶段、完整机器/派生差异模型、十五文件编码/四锚点及七新限定证据各25来源均通过。文本汇总与Check均退出0。
四旧BUU01过宽I/S已明确fail，previous pass/独立正确坐标事实和更正引用保留，当前修复七新限定证据pass；这是旧snapshot判断失效的历史记录，不是当前代码故障遗留，未删除失败或用新修复改写旧通过。
实际语义1745/有效1738、文档1856（50.43%）、F886/E1039、R7已审448/余2613、LOOP158/INPUT51/RENDER59/STARTUP118；其他域/原分母/差队列/pending18与ECX pending7/closed11和完整门禁保持。未编译、链接、运行或证明完整原父/别名/ABI/显示验收，整体目标继续active。
