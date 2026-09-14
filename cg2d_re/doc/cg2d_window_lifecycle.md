# 窗口生命周期还原

任务：R2.1.3.C。实现仅位于 `src/platform/WindowLifecycle.cpp`，使用既有
`cg/platform/Startup.hpp`、`cg/platform/LegacyWindow.hpp` 声明，不定义全局。
未修改共享头、全局定义、CMake 或进度；未运行、编译、测试、提交或下载。

## 证据基线

- 已按 `ida-analysis` 执行 `instance_list`、`survey_binary`，选定 `ida-30904`：
  `D:\SOFTSTAR\cg2d.exe.i64`，输入 `cg2d.exe`，32 位，基址 `0x400000`。
- `ida_nalt.retrieve_input_file_sha256().hex()`：
  `dad3f292c6df28e9b7208ad1d339f56ab68c1e728f6595e849e37296551c0f08`。
- survey SHA-256：
  `e7c2790f479862635932e043b2d7e93b9e791a1edc276a960af05f1454c54010`。
  这是已知磁盘差异，不阻断；以下以选定 IDB 的反编译及完整函数反汇编为准。
- 三个函数分别核对 132、11、45 条指令。仅执行只读 IDA 查询，没有调用目标函数或修改 IDB。

## InitializeWindow：0x4669F0

| 原地址 | 保留的行为 |
| --- | --- |
| `0x4669F0–0x466A08` | 初次模式非零时 style 为 `0x00CA0000`，调用 `0x465A30` 后重新读取模式。 |
| `0x466A0D–0x466A41` | helper 返回后模式仍非零，使用 `gWindowWidth/gWindowHeight`；变为零则使用 `640×480`，但不改掉已经选定的 style。初次模式为零则 style 为 `0x90000000`，尺寸同为 `640×480`。各分支均调用 `SetRect(&rect,0,0,width,height)`。 |
| `0x466A47–0x466A64` | 调用 `AdjustWindowRectEx(&rect,style,FALSE,0)`，随后依据全局窗口句柄分支。 |
| `0x466A66–0x466AE1` | 无窗口时重读模式：非零使用 `gStartX/gStartY`，零使用 `(0,0)`；调用 `CreateWindowExA`，类名和标题均为 `gWindowClassName`，实例为 `gInstance`，其余可选参数为零，宽高取调整后矩形差值；结果原样写回 `gLegacyWindow`。 |
| `0x466AE3–0x466AFB` | 已有窗口依次执行 `SetWindowLongA(hwnd,GWL_STYLE,style)` 和内部 `ShowWindow(hwnd,gShowCommand)`。 |
| `0x466AFD–0x466B50` | 内部显示后重读模式；非零使用起始坐标，零使用 `(0,0)`；矩形差值作为尺寸，`SetWindowPos` 使用 `HWND_NOTOPMOST` 和 `SWP_FRAMECHANGED=0x20`。 |
| `0x466B56–0x466B77` | 全部分支再次调用 `ShowWindow`，然后 `UpdateWindow`；新端返回末次调用保留的 EAX，不新增 API 失败检查、句柄检查或错误分支。 |

入口引用为 `_WinMain@16` 的 `0x4659D5` 和 `WIN_PROC` 的 `0x467213`。
实现不跨 Win32 调用缓存全局窗口句柄或模式，不合并已有窗口的两次显示。

## SetWindowedDesktopColor：0x465A30

`0x465A3D` 传入元素数量 1，`0x465A3F` 写入索引 1，
`0x465A47` 写入 `COLORREF 0x00A4A0A0`，`0x465A4F` 调用
`SetSysColors(1,&element,&color)`，直接返回其 `BOOL` 值，对应共享头的 `int`。
SDK 的索引 1 为 `COLOR_BACKGROUND`；唯一直接调用点是 `0x466A03`。

**此函数会改变系统桌面背景颜色，不是窗口客户区填充。实现保留真实副作用，
但本次绝不执行。原颜色保存及退出恢复依赖尚未完整接通；本单项不添加保存、恢复
或临时禁用逻辑，也不声称系统颜色已经具备完整恢复保障。**

## PumpWindowMessages：0x466D50

- `0x466D60–0x466D69`、`0x466D90–0x466D9D`：
  `PeekMessageA(&message,nullptr,0,0,PM_REMOVE)`，不按窗口或消息范围筛选，持续取出消息。
- `0x466D7B–0x466D80`、`0x466DAF–0x466DB7`：遇到 `WM_QUIT=0x12` 立即返回 0，
  不 Translate/Dispatch 该消息，也不继续清理它后面的消息。
- `0x466D87`、`0x466D8E`：其他消息依次 `TranslateMessage`、`DispatchMessageA`，随后继续 Peek。
- `0x466DA3–0x466DAE`：队列为空返回 1。没有 SDL 输入、单消息返回或阻塞等待替换。

直接引用为 `MainLoop@0x4570D5`、`InitLoading@0x457B4B`、
`sub_458390@0x458418`、`sub_4585F0@0x458623`，上述地址均为调用指令位置。

## 原生 ABI 与静态边界

- `Windows.h` 只在本次 `cg::platform` 实现文件引入；公开签名不新增 SDK 类型。
- `gLegacyWindow/gInstance` 按 `std::uintptr_t` 与 SDK `HWND/HINSTANCE` 互转，
  不经过原 32 位整数；`CreateWindowExA` 的返回值亦以原生宽度保存。
- `RECT/MSG` 直接使用 SDK 布局，不复制 IDB 的 28 字节消息缓冲区；
  `MSG` 的 `HWND/WPARAM/LPARAM` 跟随 Windows x64 ABI，`RECT` 字段仍为 `LONG`。
- `GWL_STYLE` 对应 32 位样式值，仅在 `SetWindowLongA` 参数处转换为 `LONG`；
  不替换成 `SetWindowLongPtrA`。`HWND_NOTOPMOST` 使用 SDK 伪句柄宏，不把 `0xFFFFFFFE` 零扩展。
- API 与类型对照本地 Windows SDK `10.0.26100.0`：
  `um/winuser.h:1878,1913,2012,3513,3541,4427,4892,4911,9637,9707,9881`；
  `shared/windef.h:154`；`shared/minwindef.h:186`。

本次核验限于源码、IDB 汇编/反编译和本地 SDK 声明静态对照，不代表编译、链接或运行验收。

主线程集成时将 `InitializeWindow` 声明从 `void` 调整为 `int`，显式返回原尾部
`UpdateWindow@0x466B6C` 留存的 32 位结果。原调用者忽略此结果，原源码返回类型未知；
新声明用于保留可证实的机器返回值，不把 IDA 的 `void` 或该新类型当成原源码证据。
