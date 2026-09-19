# 客户端退出清理还原

任务：R2.2.2.C。新增 `src/platform/ClientShutdown.hpp`、
`src/platform/ClientShutdown.cpp`，实现 `cg::platform::ShutdownClient_457A40()`；
未修改 MainLoop、共享头、全局、CMake 或进度。

## 证据基线

- 按 `ida-analysis` 先执行 `instance_list`、`survey_binary`，只读实例 `ida-30904`：
  `D:\SOFTSTAR\cg2d.exe.i64`，输入 `cg2d.exe`，32 位，基址 `0x400000`。
- `ida_nalt.retrieve_input_file_sha256().hex()` 为
  `dad3f292c6df28e9b7208ad1d339f56ab68c1e728f6595e849e37296551c0f08`。
- IDA 中 `hMutex@0x9A6F3C` 类型为 `HANDLE`，`aRgbValues@0x9A6F40`
  类型为 `COLORREF`，分别对应既有 `gStartupMutex`、`gDesktopColor`。
- 本次仅作反编译、完整函数反汇编、xref、类型和导入查询；没有修改 IDB，
  没有执行退出函数、系统颜色 API、句柄操作、客户端、编译或测试。

## 2026-09-20 当前复核与架构参考边界

- 当前只读实例为 `ida-32980`：`D:\SOFTSTAR\cg2d.exe.i64`，输入名
  `cg2d.exe`，32 位，基址 `0x400000`。本次不重判历史输入哈希。
- 当前完整反汇编仍为 29 条指令：`0x457A40` 至 `0x457A96` 的调用次序和
  `ReleaseMutex(hMutex)` 不变，`0x457A9C` 跳到共享尾块；尾块在 `0x465A65`
  压入 `aRgbValues@0x9A6F40` 的地址，在 `0x465A75` 调用 `SetSysColors`。
- 新端 `gDesktopColor` 的公共类型是 `std::uint32_t`；当前 MSVC 下它与
  `COLORREF` 都是 32 位，但不是同一类型。尺寸断言不能使
  `reinterpret_cast<const COLORREF*>(&gDesktopColor)` 成为合法别名。本次改为
  `static_cast<COLORREF>` 将数值复制到局部 `COLORREF color`，再把 `&color`
  传给 `SetSysColors`；保留数值，不再把 `gDesktopColor` 的同一存储解释为
  `COLORREF` 对象，也不改公共类型。
- `D:\Source\STONAGE` 按 GBK 显式读取：`SYSTEM/MAIN.CPP:42,97` 分别声明
  `HANDLE hMutex` 并调用 `CreateMutex`；`SYSTEM/GAMEMAIN.CPP:759-785` 的
  `EndGame()` 在既有清理后于第 781 行调用 `ReleaseMutex(hMutex)`。这些内容只供
  大概架构理解和候选检索，不证明 cg2d 的互斥量释放、共享尾块、颜色恢复或版本关系。
  本项静态复核通过结论只依据当前 cg2d IDA 指令与当前实现源码。

## 调用链与签名

`sub_457A40` 的主块为 `[0x457A40,0x457AA1)`，并以
`0x457A9C -> 0x465A60` 进入共享尾块 `[0x465A60,0x465A7D)`。
主块中各直接调用之间没有压入实参，IDA 函数类型也均为无实参；返回值全部忽略。

| 调用点 | 原 callee / IDA 返回 | 新调用状态 |
| --- | --- | --- |
| `0x457A40` | `sub_4705F0` / `char` | 未实现，声明 `shutdown_detail::Sub_4705F0()` |
| `0x457A45` | `sub_522C50` / `HGLOBAL` | 未实现，真实签名声明 |
| `0x457A4A` | `sub_44E110` / `HGLOBAL` | 未实现，真实签名声明 |
| `0x457A4F` | `Renderer::Destroy@0x44C3C0` / `void __cdecl` | 未实现，声明 `RendererDestroy_44C3C0()` |
| `0x457A54` | `sub_454EE0` / `int` | 未实现，真实签名声明 |
| `0x457A59` | `sub_52B630` / `LPDIRECTSOUND` | 未实现，以 `IDirectSound*` 声明 |
| `0x457A5E` | `sub_52CCE0` / `bool` | 未实现，真实签名声明 |
| `0x457A63` | `AnimeUnload@0x45BEF0` / `int` | 未实现，真实签名声明 |
| `0x457A68` | `sub_45BF20` / `int` | 未实现，真实签名声明 |
| `0x457A6D` | `sub_45A6C0` / `FILE*` | 直接调用已实现 `resource::CloseGraphicFiles()`；原返回被忽略，新实现为 `void` |
| `0x457A72` | `sub_457670` / `void` | 未实现，真实签名声明 |
| `0x457A77` | `sub_456530` / `HFONT` | 未实现，真实签名声明 |
| `0x457A7C` | `sub_459040` / `void` | 未实现，真实签名声明 |
| `0x457A81` | `sub_404D60` / `void` | 未实现，真实签名声明 |
| `0x457A86` | `sub_5081A0` / `void` | 未实现，真实签名声明 |
| `0x457A8B` | `sub_5066D0` / `int` | 未实现，真实签名声明 |

`sub_459040` 的实际内容是 `ImmReleaseContext`、`ImmAssociateContext` 及候选列表释放，
不是 GraphicInfo 清理。现有 `GraphicInfo` 模块没有对应本链的已实现 callee，不能按地址邻近替代。
现有 `SdlPresenter::Destroy()` 也没有与原 `gCRenderer` 建立可证明的对象绑定，故不替代
`Renderer::Destroy@0x44C3C0`。

## 互斥量与共享尾块

`0x457A90` 读取 `hMutex@0x9A6F3C`，`0x457A95` 压入该值，
`0x457A96` 调用 `ReleaseMutex(HANDLE)`；返回 `BOOL` 被忽略，不检查零值，
不调用 `CloseHandle`，也不清空全局。新端以原生 `HANDLE` 解释 `gStartupMutex`，保持该顺序。

随后才进入共享尾块：`0x465A6D` 把元素值写为 1，`0x465A75` 调用
`SetSysColors(1, &element, &aRgbValues)`。元素 1 为 `COLOR_BACKGROUND`，
原端颜色指针直接指向保存值 `aRgbValues@0x9A6F40`。新端把
`gDesktopColor` 的数值复制到局部 `COLORREF` 后传入，不再对同一存储作
跨类型别名；随后直接返回 `SetSysColors` 的 `BOOL/EAX32`，因此
`ShutdownClient_457A40()` 的对外返回类型为 `int`。

原直接调用者为 `MainLoop@0x4574C6`、`InitLoading@0x4582EF`、
`sub_458390@0x4585DE`、`sub_4585F0@0x4587B7`。本项不改这些调用者。

## 未完成边界

除 `resource::CloseGraphicFiles@0x45A6C0` 外，其余十五个原 callee 只有带地址和
IDA 类型证据的声明，没有空函数、固定成功返回或替代清理。Windows SDK 类型和 API
只出现在平台实现文件；私有头仅暴露 `int ShutdownClient_457A40()`。
