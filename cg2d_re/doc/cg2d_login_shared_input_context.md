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

## `nullsub_1@0x509080` 全局核对

函数精确范围为 `[0x509080,0x509081)`，原始字节为 `C3`，唯一指令为
`retn`；CFG 只有一个大小为 1 的基本块且没有后继。除 `retn` 固有的返回地址
出栈外，函数不执行显式内存访问或数据寄存器读写，也没有 callee。特别是它不定义
`EAX`，全部直接 caller 均未把调用后的 `EAX` 当作该函数返回值使用，因此新端
契约为 `void()`，不伪造返回数值。

IDA 对 `0x509080` 共给出 62 个 xref：60 个直接代码调用点，分布在以下 38 个
caller 中：

`getCharFromBattleBuffC1`、`Battle::ParsePacket_M::TakeStr2`、
`Battle::ParsePacket_M::TakeStr3`、`Battle::ParsePacket_M::TakeStr`、
`World::DrawBattleScene`、`sub_44A1B0`、`sub_4596D0`、`sub_45C2D0`、
`sub_45C5A0`、`sub_45F190`、`sub_460FF0`、`sub_4626A0`、`sub_46E4A0`、
`sub_4810A0`、`sub_482270`、`sub_487020`、`sub_4872E0`、`sub_488B10`、
`sub_4999D0`、`sub_4A4620`、`sub_4A4660`、`sub_4A4710`、
`?classic@locale@std@@SAABV12@XZ_22`、`sub_4CAF60`、`sub_4EFB70`、
`sub_4F0460`、`sub_4F24A0`、`sub_4F2A30`、`sub_4F51D0`、`sub_4FA050`、
`sub_505C20`、`network_dispatch`、`sub_5199A0`、`World::Update`、
`World::DrawNormalScene`、`sub_520160`、`sub_52D2D0`、`Unescape2`。

调用点的 x86 栈形态并不构成一致函数原型：21 处没有栈实参，33 处在调用前
压入一个 W32，`network_dispatch` 中另有 4 处压入两个 W32、2 处压入三个
W32。`0x488C31` 与 `0x499AE6` 前还设置了 `ECX`，但函数同样不读取它。callee
只有普通 `retn`，从不代清参数；所有已压栈值均由 caller 以 `add esp, 4`，或与
相邻 cdecl 调用合并的 `add esp, 8/0x0C/0x10/0x14/0x18/0x20` 回收。其中
`0x438544→0x438549`、`0x461348→0x46134D`、`0x52D2FC→0x52D301` 均明确
使用 `add esp, 8`。

另两项数据引用具有独立 ABI 边界：`0x4FA095` 把函数地址作为 `sub_503160` 的
回调实参，该槽最终在 `sub_503160@0x503299` 以无参数 `call eax` 调用；
`0x54F8BC` 则是只读方法表形态，且已观察到 `0x488C31`、`0x499AE6` 两个直接
调用点在调用前设置 `ECX`。这些事实不为根 owner 增加参数或返回语义；未来恢复
任一间接槽位时，必须以与槽位真实类型匹配的 thunk 转调根 owner，不得把
`void() noexcept` 函数指针强转进不兼容槽。

新端由 `cg::NullSub_509080()` 唯一承接，声明位于 `cg/Input.hpp`，定义复用
`Input.cpp`；当前八个源码调用点全部直接调用该 `void()` owner。原 x86 caller
压入但 callee 不读的值不进入 x64 接口，能力 3 场景原先仅丢弃上下文参数的局部
包装已移除。

## 当前边界与未来复用

六个引用中没有正常 producer，也没有把该对象交给 `Input::SetContext` 的路径；
当前证据不能把它加入 `gInputContextFocusTable`，也不能为其推定初始化文本或焦点
状态。`DrawInputText` 与 `sub_533570` 继续按各自边界复用；`nullsub_1` 已按上述
字节级证据闭合为空操作，不因此推定该上下文的生产者或额外状态。

后续还原相关 caller 时，三种用途均须复用同一对象：空 helper 路径只执行唯一
`cg::NullSub_509080()`；绘制路径把同一完整上下文交给唯一 `DrawInputText` 边界；
持久化路径从同一对象开头读取前 51 字节交给 `Sub_533570`。不得为任一用途建立
第二份上下文或独立 51 字节缓存。
