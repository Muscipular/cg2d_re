# LegacyCryptInt 语义后端

## 范围与证据基线

本项还原原 x86 `CryptInt` 的定宽存储、构造、载荷赋值、读取刷新、回绕加法及
五个整数比较入口。取证使用当前只读 IDA 实例
`ida-17524` 的 `cg2d.exe`；`survey_binary` 报告 32 位 x86、小端、基址
`0x400000`，当前同名磁盘文件 SHA-256 为
`e7c2790f479862635932e043b2d7e93b9e791a1edc276a960af05f1454c54010`。
本项未修改 IDA，未读取或使用 STONAGE。

## 固定布局

IDA 中 `CryptInt` 大小为 16 字节：

| 偏移 | 宽度 | 新端字段 | 已确认含义 |
| --- | ---: | --- | --- |
| `+0x00` | W32 | `legacyVftToken` | 构造、析构写入原地址位模式 `0x0054D3A0`；新端不解引用。 |
| `+0x04` | W32 | `encoded` | 明文 `int32` 位模式与 `mask` 的异或结果。 |
| `+0x08` | W32 | `mask` | 来自共享 `rand@0x53B4C9` 的一次结果。 |
| `+0x0C` | W8 | `readCount` | 读取计数，按 U8 回绕、按 I8 判断刷新。 |
| `+0x0D..0x0F` | 3 字节 | `reserved` | 当前函数均不读写，不推断用途。 |

`LegacyCryptInt` 对大小及每个字段偏移均有静态断言。`legacyVftToken` 是旧地址
位模式，不在 x64 中冒充指针。

## 函数范围与控制流

| 原入口 | IDA 范围 | 指令/块 | 还原结论 |
| --- | --- | ---: | --- |
| `sub_401C70` | `[0x401C70,0x401C92)` | 12 / 1 | 先写 token，再取一次随机数写 mask，清 W8 count，以 0 调 SetNorVal；reserved 不写。 |
| `CryptInt_Init` | `[0x401CC0,0x401CE7)` | 13 / 1 | 与零构造相同，但 SetNorVal 使用传入 int32。 |
| `CryptInt_delete` | `[0x401CF0,0x401CF7)` | 2 / 1 | 只把 `+0` 恢复为 `0x0054D3A0`。 |
| `CryptInt::eq` | `[0x401D00,0x401D0F)` | 4 / 1 | Read 一次后作有符号相等比较。 |
| `CryptInt::gte` | `[0x401D70,0x401D7F)` | 4 / 1 | Read 一次后作有符号 `>=`。 |
| `CryptInt::Lte` | `[0x401DA0,0x401DAF)` | 4 / 1 | Read 一次后作有符号 `<=`。 |
| `CryptInt::lt` | `[0x401DB0,0x401DBF)` | 4 / 1 | Read 一次后作有符号 `<`。 |
| `CryptInt::add` | `[0x401E10,0x401E1C)` | 3 / 1 | Read 一次，再把参数按 W32 位模式相加并返回；32 位自然回绕。 |
| `sub_401EC0` | `[0x401EC0,0x401F45)` | 54 / 8 | IDA 计数包含关联异常处理块；正文先更新 W8 count，再按 I8 `>20` 选择换 mask。 |
| `CryptInt::SetNorVal` | `[0x401F50,0x401F5F)` | 5 / 1 | 只写 `encoded = bits(value) ^ mask`。 |
| `CrypyInt::lte` | `[0x401FC0,0x401FD1)` | 5 / 1 | Read 右侧一次后作有符号 `lhs <= rhs`。 |

`sub_401EC0` 的关键指令为：`0x401EE3` 读取 W8，`0x401EE9` 在 DL
内递增，`0x401EED` 先写回 W8，`0x401EF0..0x401EF3` 以 signed `CL`
和 20 比较。因此损坏计数同样先按 U8 模 256 回绕，再按 I8 判断；不能用较宽
无符号计数替代。正常第 21 次读取保存当前明文，清 count，再于 `0x401F00`
调用共享 `rand@0x53B4C9` 一次换 mask，并以相同明文重编码。

`SetNorVal` 不取随机数，也不改变 count。源码中的所有 int32/uint32 位模式转换
都使用 `std::bit_cast`，异或在 uint32 上完成，避免有符号溢出或别名未定义行为。
`Add_401E10` 同样只调用一次 `Read_401EC0`，随后在 `uint32_t` 上相加再恢复
`int32_t` 位模式；没有把鼠标坐标加 16 改成未定义的有符号溢出。

## 载荷赋值与共享随机源

`sub_4041E0` 的 `0x40426B..0x4042A1` 提供了 int 赋值的直接展开证据：
先在 16 字节临时对象上调用 `CryptInt_Init`，随后 `0x40427B..0x404297`
只复制临时对象的 `+4`、`+8`、`+C` 到目标，最后析构临时对象。目标的 token
和 `+D..+F` 保持原值；整个 `AssignFromInt` 只因临时构造取一次随机数。

实现通过既有 `MainLoopSupport.cpp` 中唯一的 `NextRandom_53B4C9` 和
`gRandomSeed` 取得 mask，没有定义第二份 PRNG。原随机状态和每个
`LegacyCryptInt` 的读取计数都是可变共享状态；当前后端不增加锁，调用方须维持
原客户端的单线程/外部串行前提，否则随机调用次序与对象刷新次序会改变。

## 已接入与未决边界

旧鼠标 X/Y 已由 `LegacyMouseState` 中的真实 `LegacyCryptInt` 承载；
`World::CursorUpdate` 的四处坐标加 16 已改接 `Add_401E10`，每处各保留一次读取
副作用。`YobiAction` 现由 `cg2d_yobi_runtime.md` 所述的原生 x64 runtime
承载；原 `+0x170` 只作为语义字段证据，不在新端复制 x86 偏移。创建时该
`LegacyCryptInt` 与原 `calloc` 对象一致保持全零，modal callback 再按原
`0x50483C/0x5048B0/0x5048EA` 的临时对象赋值形态调用 `AssignFromInt`。
`IsYobiActionReady`、相等与小于等于窄接口均已直接接入本后端，且每次比较只读取
一次。
