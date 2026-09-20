# 能力 4 登录输入初始化证据

## 范围与调用关系

本文只记录当前 `ida-17524` 中 `D:\SOFTSTAR\cg2d.exe` 的
`sub_465320@[0x465320,0x465429)`。该函数大小为 `0x109`，共 69 条指令、
1 个基本块，原型为 `void()`。

IDA 确认两个直接代码引用：`sub_464840` 在 `0x464A9A` 以 `call` 调用，
`sub_465740` 在 `0x465783` 以 `jmp` 尾跳转入。当前只提供
`login::InitializeCapability4Input_465320()`，不修改或接入这两个 caller。

## 六个调用点

函数内共有 6 个调用点、5 个唯一目标：

| 地址 | 原目标 | 新端目标 |
|---:|---|---|
| `0x465327` | `sub_45C970` | `resource::ProbePaletteCapability_45C970` |
| `0x465362` | `sub_533680` | `detail::Sub_533680`，仅真实声明 |
| `0x465391` | `sub_449CF0` | `input::InitializeContext_449CF0` |
| `0x4653C6` | `sub_5335F0` | `detail::Sub_5335F0`，仅真实声明 |
| `0x4653F6` | `sub_449CF0` | `input::InitializeContext_449CF0` |
| `0x46540F` | `Input::SetContext` | `input::SetContext_44B330` |

## 严格初始化顺序

1. 单个 `char[256]` 本地缓冲在入口仅零初始化一次；调用
   `ProbePaletteCapability_45C970`，把原返回值写入唯一
   `render::gPaletteCapability@0x9A7F64`。
2. 第一次读取 `gSelectedSavedSlot105C394@0x105C394` 快照。把
   `gInputContext9A5678` 的坐标槽写为 `(187,358)`，严格按
   `text[0]→totalByteLength(+0x12C)→caretByteOffset(+0x128)` 清零，再调用
   `Sub_533680(buffer,snapshot1)`。reader 返回后重新读取 Y、X 坐标槽，各加 2，
   调用 `InitializeContext_449CF0(ctx,189,360,0,0,3,buffer,1,23,0,0)`。
3. 第一个初始化器完整返回后，第二次读取选中槽快照。把
   `gInputContext9A5960` 的坐标槽写为 `(187,389)`，按同序预清，调用
   `Sub_5335F0(buffer,snapshot2)`；随后重新读取 Y、X 坐标槽并各加 2，调用
   `InitializeContext_449CF0(ctx,189,391,0,0,3,buffer,1,20,0,1)`。
4. 第二个初始化器返回后，先把 `gInputContextFocusTable[1]` 读取到局部，再依次
   写 `gValue9A6E20=1`、调用 `SetContext_44B330(local)`、写共享
   `gValue9A6200=255`。

本函数不在初始化器返回后补写 `encrypt`，不跨两阶段缓存选中槽，也不在第二次
reader 前重新清空缓冲。

## 安全差异与未决边界

原函数只分配一个未初始化的 256 字节栈缓冲，并由两个 reader 顺序复用。新端
保留单缓冲和不二次清空的行为，只在入口对该缓冲零初始化一次。这是与既有输入
初始化一致的最窄安全差异：正常 reader 写入的零结尾文本不变；短写或异常缺少
终止符时，不再让后续初始化器扫描未初始化栈字节。

`Sub_533680`、`Sub_5335F0` 与 `gSelectedSavedSlot105C394` 仅在私有
`LoginInputDependencies.hpp` 中保留真实声明；本项不实现 reader、不定义持久
状态、不提供固定返回，也不增加额外门禁。
