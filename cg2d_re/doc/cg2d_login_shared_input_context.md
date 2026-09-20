# 登录共享输入上下文证据

## 对象边界

本文只记录当前 `ida-17524` 中 `D:\SOFTSTAR\cg2d.exe` 的
`stru_9A4D20@[0x9A4D20,0x9A4E7C)`。IDA 将其识别为完整 `InputContext`，对象
大小为 `0x15C`，与新端 `input::InputContext` 的既有静态布局断言一致。

相邻数据项没有重叠：前一项是 `byte_9A4D10@[0x9A4D10,0x9A4D20)`，大小
`0x10`；后一项是从 `dword_9A4E7C@0x9A4E7C` 开始的 W32。新端因此只建立
`login::gInputContext9A4D20` 一份完整存储，不复用相邻对象，也不拆出 51 字节
副本。

## PE raw 与 virtual 证据

原 PE 的 `.data` 节 RVA 为 `0x153000`，`VirtualSize=0xB34B08`，
`PointerToRawData=0x153000`，`SizeOfRawData=0x40000`。按映像基址 `0x400000`
换算，文件 raw 只覆盖 VA `[0x553000,0x593000)`，而本对象完整位于该节后续
virtual 区域。IDA 对 `0x9A4D20` 与末字节 `0x9A4E7B` 查询文件偏移均返回
`-1`，证明该区间没有磁盘 raw 字节，由 PE 装载器虚拟零填充。

因此新端定义采用 `input::InputContext gInputContext9A4D20 {}`，只表达原映像
初值；本项不额外初始化或清空它。

## 六个直接引用

对完整 `[0x9A4D20,0x9A4E7C)` 逐字节查询仅得到以下 6 个 xref，全部引用
对象基址：

| 引用地址 | caller | 后续用途 |
|---:|---|---|
| `0x45C45A` | `sub_45C2D0` | 传给 `nullsub_1@0x509080` |
| `0x45C6AF` | `sub_45C5A0` | 传给 `nullsub_1@0x509080` |
| `0x45F6AA` | `sub_45F190` | 传给 `nullsub_1@0x509080` |
| `0x45FBD1` | `sub_45FA70` | 传给 `Renderer::DrawInputText@0x4568A0` |
| `0x464AFE` | `sub_464840` | 传给 `sub_533570@0x533570` |
| `0x465746` | `sub_465740` | 传给 `sub_533570@0x533570` |

`nullsub_1` 只有一条 `retn`，不读写对象。`sub_533570` 从参数基址开始逐字节
读取，遇 NUL 停止，最多读取 `0x33`（51）字节，只覆盖完整上下文开头的文本
区域。`Renderer::DrawInputText` 会使用完整布局，其最高字段访问为 `+0x158`，
仍落在对象末尾 W32 内。

## 当前边界与未来复用

六个引用中没有正常 producer，也没有把该对象交给 `Input::SetContext` 的路径；
当前证据不能把它加入 `gInputContextFocusTable`，也不能为其推定初始化文本或焦点
状态。本项不实现或接入上述 caller、`DrawInputText`、`sub_533570`、`nullsub_1`。

后续还原相关 caller 时，三种用途均须复用同一对象：空 helper 路径传递
`&gInputContext9A4D20`；绘制路径把同一完整上下文交给唯一 `DrawInputText` 边界；
持久化路径从同一对象开头读取前 51 字节交给 `Sub_533570`。不得为任一用途建立
第二份上下文或独立 51 字节缓存。
