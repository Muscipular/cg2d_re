# cg2d IME 生命周期还原

任务：R6.1.3.29、R6.1.3.44、R6.1.3.45。实现 `sub_458FD0/sub_459040` 与 IME 布局查询、
开关状态重置；退出函数接入既有 `ShutdownClient_457A40` 调用链，状态重置接入
`World::Update` 场景 5。初始化函数保留到后续初始化链精确接线。

## IDA 证据基线

- 当前只读实例为 `ida-17524`：输入 `D:\SOFTSTAR\cg2d.exe`，IDB
  `D:\SOFTSTAR\cg2d.exe.i64`，32 位，基址 `0x400000`。
- `sub_458FD0@[0x458FD0,0x459032)` 大小 `0x62`，原型 `int()`，共 27 条指令、
  3 个基本块；唯一代码引用是 `sub_4576B0@0x457804`。
- `sub_459040@[0x459040,0x459081)` 大小 `0x41`，原型 `void()`，共 18 条指令、
  3 个基本块；代码引用是 `sub_457A40@0x457A7C` 与 `sub_458FD0@0x458FEB`。
- `sub_459090@[0x459090,0x45909F)` 大小 `0x0F`，原型 `BOOL()`，共 5 条指令；
  依次调用 `GetKeyboardLayout(0)` 与 `ImmIsIME`，原样保留 32 位返回值。
- `sub_4590A0@[0x4590A0,0x4590C6)` 大小 `0x26`，原型 `BOOL()`，共 12 条指令、
  3 个基本块；唯一代码引用为 `World::Update@0x51C563`。
- 本项只查询反编译、完整反汇编和引用关系，没有修改 IDB。

## 初始化 `sub_458FD0`

原指令顺序如下：

1. `0x458FDA/0x458FDF/0x458FE4` 分别清零组合串首字节、属性首字节和
   `byte_73EFF0`。
2. `0x458FE9..0x458FEB` 仅在旧 `gHIMC` 非零时调用 `sub_459040`。
3. `0x458FF1..0x458FF6` 调用 `ImmCreateContext` 并保存新 `gHIMC`。
4. `0x459002..0x459018` 以唯一 `hWnd` 和新 `gHIMC` 调用
   `ImmAssociateContext`，保存返回的 previous context。
5. `0x45901D` 调用 `ImmGetConversionStatus`，直接刷新 conversion/sentence。
6. `0x459022..0x45902E` 以 25 次 DWORD 写零清除 `szDescription@0x73ED70`，
   即 100 字节；`0x459031` 固定返回 0。

新端对应存储为 `gImeDescription[100]`、`gImeContext`、
`gPreviousImeContext`、`gImeComposition[256]`、`gImeAttributes[256]`、
`gImeConversionMode` 和 `gImeSentenceMode`。`byte_73EFF0` 尚无足够语义证据，
仅以地址保留式名称 `gImeState73EFF0` 承接，不推定其业务含义。

## 退出 `sub_459040`

- `0x459040..0x45904D`：以唯一 `hWnd`、当前 `gHIMC` 调用
  `ImmReleaseContext`。
- `0x459052..0x45905F`：以同一窗口和 previous context 调用
  `ImmAssociateContext`。
- `0x459064..0x459076`：候选表非空时调用 `free`，随后将候选表指针置空。

原函数不增加窗口或 context 空值门禁，不调用 `ImmDestroyContext`，也不清空
当前 context、previous context、conversion、sentence 或其他 IME 状态；新实现保持这些
否定行为。所有窗口与 IME 句柄均通过 `std::uintptr_t` 承载，不经 32 位整数截断。

## 布局查询与开关状态重置

`IsImeKeyboardLayout_459090` 对应 `sub_459090`：`0x459090` 以线程号 0 调用
`GetKeyboardLayout`，`0x459099` 把所得布局传给 `ImmIsIME`，并原样返回其完整
32 位 `BOOL` 位值，不压缩成 C++ `bool`。

`ResetImeOpenStatus_4590A0` 严格保留以下顺序：

1. `0x4590A0` 调用上述共享布局查询；`0x4590A5` 只在返回值严格等于 1 时继续，
   其他值在 `0x4590C5` 原样返回。
2. `0x4590AA` 第一次独立读取 `gImeContext`，以 `FALSE` 调用
   `ImmSetOpenStatus`，丢弃结果。
3. `0x4590B7` 第二次独立读取 `gImeContext`，以 `TRUE` 调用
   `ImmSetOpenStatus`，原样返回该调用的 32 位 `BOOL`。

不增加空 context 门禁、首次调用失败早退、状态查询，也不清理组合串、候选表或
其他 context 状态。`World::Update` 场景 5 在原 `0x51C563` 调用点使用该唯一实现，
并与旧端一致忽略返回值。

## 接线边界

`ShutdownClient_457A40` 已在 `Sub_456530@0x456530` 之后、
`Sub_404D60@0x404D60` 之前调用真实 `ShutdownIme_459040`，对应原调用点
`0x457A7C`。`InitializeIme_458FD0` 本项不提前接入 `WinMain` 或 `MainLoop`；它应在
未来还原 `sub_4576B0` 时按原调用点 `0x457804` 接入。场景 5 的状态重置接线不改变
这一初始化尚未接线的限制。
