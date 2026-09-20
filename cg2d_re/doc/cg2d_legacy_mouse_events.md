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
