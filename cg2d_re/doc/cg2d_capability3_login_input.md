# 能力 3 登录输入初始化证据

## 范围与调用关系

本文只记录当前 `ida-17524` 中 `D:\SOFTSTAR\cg2d.exe` 的
`sub_45F970@[0x45F970,0x45FA6C)`。该函数大小为 `0xFC`，共 63 条指令、
1 个基本块，原型为 `void()`。

两个直接调用点均位于尚未实现的 `sub_45F190@[0x45F190,0x45F93D)`：

| 调用地址 | caller |
|---:|---|
| `0x45F689` | `sub_45F190` |
| `0x45F734` | `sub_45F190` |

当前只提供唯一公共入口 `login::InitializeCapabilityInput_45F970()`，不提前接入
这两个调用点。

## 六个调用点

函数内六个调用点及新端映射如下：

| 地址 | 原目标 | 新端目标 |
|---:|---|---|
| `0x45F977` | `sub_45C970` | `resource::ProbePaletteCapability_45C970` |
| `0x45F9AC` | `sub_520440` | `ReadInputValueFF4982_520440` |
| `0x45F9D6` | `sub_449CF0` | `input::InitializeContext_449CF0` |
| `0x45FA0A` | `sub_5203E0` | `ReadInputValueFF494F_5203E0` |
| `0x45FA33` | `sub_449CF0` | `input::InitializeContext_449CF0` |
| `0x45FA52` | `Input::SetContext` | `input::SetContext_44B330` |

## 严格初始化顺序

1. 调用 `ProbePaletteCapability_45C970`，把原返回值写入唯一的
   `render::gPaletteCapability@0x9A7F64`。
2. 为 `gInputContext9A5960@0x9A5960` 写坐标 `(257,358)`；严格按
   `text[0]→totalByteLength(+0x12C)→caretByteOffset(+0x128)` 预清；从
   `0xFF4982` 的唯一持久槽读取初值；调用
   `InitializeContext_449CF0(ctx,257,358,0,0,3,text,1,20,0,1)`；返回后
   强制写 `encrypt=0`。
3. 为 `gInputContext9A5678@0x9A5678` 写坐标 `(257,389)`；按同一顺序预清；
   从 `0xFF494F` 的唯一持久槽读取初值；调用
   `InitializeContext_449CF0(ctx,257,389,0,0,3,text,1,20,0,0)`；返回后
   强制写 `encrypt=1`。
4. 把共享 `gValue9A6E20@0x9A6E20` 写为 0。
5. 读取原焦点表 `0x559FFC` 第 0 项并调用 `SetContext_44B330`。该项仍指向
   `gInputContext9A5960`，未建立第二份焦点表。
6. 把共享 W32 `gValue9A6200@0x9A6200` 写为 255。

这里的持久槽映射相对上下文地址发生交换：`0x9A5960` 读取 `0xFF4982`，
`0x9A5678` 读取 `0xFF494F`。两个 `encrypt` 值也必须在各自初始化器返回后再
反向写成 0 与 1；不能只依赖初始化器的末参数。

## 共享 W32 归属

`0x9A6200` 的映像初值为 0。除本函数在 `0x45FA5A` 写 255 外，IDA 还确认：

- `sub_45F190` 在 `0x45F1E6` 写 0，并在 `0x45F897` 读取；
- 已实现的 `sub_465320` 在 `0x465417` 同样写 255，证据见
  [`cg2d_capability4_login_input.md`](cg2d_capability4_login_input.md)。

因此新端只定义一个地址型共享量 `gValue9A6200`；能力 4 初始化已复用该唯一
存储，后续还原 `sub_45F190` 时继续复用，不建立场景私有副本。

## 安全差异

原函数复用一个未初始化的 256 字节栈缓冲。新端沿用 R6.1.3.32 已采用的最窄
安全差异，为两次持久槽读取分别使用零初始化的 256 字节本地缓冲。正常含 NUL
的 51 字节槽行为不变；仅当槽的 51 字节全部非 NUL 时，缓冲 `[51]` 保证为
NUL，避免后续初始化器继续扫描未初始化栈。除此之外不增加空槽、能力值、上下文
或焦点表门禁。
