# 网络协议运行时创建与清理链还原证据

## 范围与当前边界

本模块当前实现并接入以下四个函数：

- `sub_4037D0@0x4037D0`：恢复八个协议 W32 常量并清零六项短状态；
- `sub_516B60@0x516B60`：安装三参数回调，复用既有 `sub_516DA0`，再分配
  18 项网络工作区；
- `sub_508140@0x508140`：启动 Winsock 1.1，创建网络工作区并设置活动门禁。
- `sub_5081A0@0x5081A0`：按活动门禁关闭 socket/Winsock、重置协议状态并释放
  九项协议缓冲及 18 槽网络工作区。

此外，`sub_517080@[0x517080,0x5170C1)`、
`sub_517690@[0x517690,0x5176AC)` 与 `sub_5170D0@[0x5170D0,0x51711C)`
以及 `sub_517190@[0x517190,0x5171A3)` 均已完成公共声明与实现。`sub_517080`
当前由 `sub_517690` 的新端实现调用 1 次；`sub_517690`、`sub_5170D0` 与
`sub_517190` 本身仍无源码调用点。私有 `sub_5176B0@[0x5176B0,0x517721)` 也已完成
唯一声明与实现，当前由 `sub_517190` 调用 1 次。它们不属于上述运行期创建与清理接线。

既有 `AllocateProtocolBuffers_516DA0` 保持为九项协议缓冲的唯一生产者。本步只扩展
`src/network/NetworkRuntime.cpp` 匿名 namespace 中原有的唯一
`NetworkRuntimeState`，没有复制九缓冲状态，也没有并入 `LoginState`、
`LoginConnection` 或其他 owner。新加协议短状态、回调槽、18 槽工作区外层指针及活动
门禁和仅按地址确认的 W32 `0xD9F2D0` 均使用地址中性私有字段名；
`login::gValue593FC0` 是独立 W32，不在本模块读写。

## `sub_4037D0` 协议状态重置

八项 W32 分别比较当前值，仅不相等时写入；比较与条件写严格保持下列顺序：

| 顺序 | 原地址 | 常量 |
|---:|---:|---:|
| 1 | `0x593FB0` | `0x1B768BA7` |
| 2 | `0x593FB4` | `0x25957EF9` |
| 3 | `0x593FB8` | `0x0FB1BFB8` |
| 4 | `0x593FBC` | `0x3039EA46` |
| 5 | `0x593FA0` | `0x135D42BA` |
| 6 | `0x593FA4` | `0x3947BF98` |
| 7 | `0x593FA8` | `0x0F1A0485` |
| 8 | `0x593FAC` | `0x259F4C5E` |

随后无条件依次清零 W16 `0x593FCC`、`0x593FCE`、`0x593FC4`、`0x593FC8`，
再依次清零 W8 `0x593FC6`、`0x593FCA`，最终返回 `0`。网络关闭入口已在关闭
Winsock 后调用本函数并忽略返回值。

## `sub_516B60` 回调与 18 槽工作区

公开回调类型严格为
`std::int32_t (*)(std::uint32_t, const void*, std::uint32_t)`。入口先无条件把私有
回调槽写为 `NULL_FUNC@0x4B0F10` 的同签名新端实现；传入回调非空时再覆盖。默认回调
忽略三个参数并返回 `0`，不通过不匹配的 `int()` 类型调用。真实回调
`sub_403E50@0x403E50` 仅在私有依赖头中按同一签名声明，本步不提供空体。

之后调用 `AllocateProtocolBuffers_516DA0(bufferLength)` 并忽略返回值。外层工作区指针
被覆盖为 `malloc(18 * sizeof(void*))` 的结果；空指针立即返回 `-1`。成功时先把
18 个原生指针槽清空，再依序对每槽执行 `malloc(bufferLength)`，先写槽再检查，不清
元素内容。任一元素分配为空时，只 `free(workspaceSlots[0])`，不释放外层表、已成功
元素或其他槽，也不清任何指针，随后返回 `-1`；18 项均成功才返回 `0`。重复调用不
释放旧值，保持覆盖与泄漏。

第三个 socket 形参由 `sub_508140` 实际压栈，但 `sub_516B60` 函数体不读取；新端保留
该参数并使用 `std::uintptr_t` 承载本机宽度。

## `sub_517080` 有界协议字节复制

完整函数范围为 `[0x517080,0x5170C1)`，共 65 字节、32 条指令和 6 个基本块；函数没有
callee，不访问全局或字符串。IDA 识别出 5 个 caller、6 个直接 callsite，六处均不消费
返回值。原 x86 ABI 为 `__cdecl`，参数依次为可写 `destination` 指针、只读 `source`
指针和有符号 W32 `capacity`；新端在 MSVC/x64 默认 ABI 下公开为
`void CopyProtocolBytes_517080(std::uint8_t*, const std::uint8_t*, std::int32_t)`，只把两项
指针适配为本机宽度，容量和下标仍保持有符号 W32，返回类型保持 `void`。

入口先按位模式计算
`limit = signed32(W32(capacity_bits - 1))`。实现使用 C++20 `std::bit_cast` 在
`std::uint32_t` 中完成减一后再解释为 `std::int32_t`，因此不以可能溢出的有符号
`capacity - 1` 代替。通常 `capacity >= 2` 时 `limit == capacity - 1`；
`capacity == INT32_MIN` 时 W32 回绕得到 `INT32_MAX`，而
`capacity` 位于 `[INT32_MIN + 1, 1]` 时 `limit <= 0`。复制下标从有符号 W32 的 0
开始；正 `limit` 路径最多递增到 `limit`，不转换为 `size_t` 或无符号容量合同。

控制流严格分为四类：

1. `limit <= 0`：完全不读取 `source`，仍无条件写 `destination[0] = 0`；
2. `limit > 0` 且首字节为 NUL：先读 `source[0]` 写入 `destination[0]`，再重读
   `source[0]` 判零，退出后再次把 `destination[0]` 写为 NUL；
3. 在正上限内遇到后续 NUL：每轮先读源字节并写目标，再重读同一源位置判零；命中
   位置的 NUL 已被复制，退出后在同一下标再次写 NUL；
4. 上限内没有 NUL：每个非 NUL 字节写入后递增有符号 W32 下标，仅当
   `index < limit` 时继续，最终在 `destination[limit]` 写 NUL。

逐字节访问顺序故意保留原函数的前向重叠行为，不使用 `memcpy`、`memmove` 或
`strncpy`；目标写入可能影响随后对同一源位置的第二次读取，或后续尚未读取的源字节。
实现不增加空指针、容量、范围或重叠保护：`limit <= 0` 仍要求 `destination[0]` 可写；正上限
路径要求实际访问到的源区间可读、目标区间可写。违反这些前置条件，以及极端容量对应的
超大访问，均保留在原合同的异常域，没有新的可移植保证。当前源码只有一处
公共声明和一处定义，并已有 1 个调用点，对应 `sub_517690` 内的 `0x5176A0`；六个
原二进制 callsite 中该处已还原，余五处留待各 caller 还原时处理。

## `sub_517690` 复制并返回目标指针

完整函数范围为 `[0x517690,0x5176AC)`，共 28 字节、12 条指令和 1 个基本块；唯一
callee 是 `sub_517080`，唯一 caller 是 `network_dispatch`。原二进制共有 85 个直接
callsite，85/85 均消费返回值：后续直接压栈 65 处，保存到 EDI 4 处、ESI 6 处、
EBX 2 处、EBP 2 处，写入栈局部 6 处。

原 x86 ABI 为 `__cdecl`，参数依次为可写 `destination` 指针、有符号 W32
`capacity` 和只读 `source` 指针；新端在 MSVC/x64 默认 ABI 下公开为
`std::uint8_t* CopyProtocolBytesAndReturn_517690(std::uint8_t*, std::int32_t,
const std::uint8_t*)`，只把指针适配为本机宽度，容量仍保持有符号 W32。函数严格先调用
`CopyProtocolBytes_517080(destination, source, capacity)`，再返回未经变换的原
`destination`；返回值不表示复制长度或成功状态。

本包装不增加空指针、容量、范围或重叠检查，也不把 `capacity` 扩宽或无符号化；
`CopyProtocolBytes_517080` 的访问顺序、前向重叠行为和异常域全部原样继承。当前源码
只有一处公共声明和一处定义，本函数的源码调用点为零；`network_dispatch` 及其 85 个
原 callsite 尚未还原，本步不猜测接线。

## `sub_5170D0` 有界协议文本追加

完整函数范围为 `[0x5170D0,0x51711C)`，共 76 字节、39 条指令和 11 个基本块；函数
没有 callee，也不访问全局。原 x86 `__cdecl` 逻辑参数顺序为 `destination`、
`source`、`capacity`；前两项分别是读写和只读本机指针，`capacity` 保持精确的有符号
W32。

`capacity <= 1` 时不读写任何缓冲，直接返回原 `capacity`。否则只在
`destination[0]` 至 `destination[capacity - 2]` 中寻找 NUL；未找到时不写入并返回
`capacity`。找到后从该位置顺序追加 `source`，追加字节最远写至
`destination[capacity - 2]`，随后在最终位置写 NUL，并返回该 NUL 的有符号 W32
下标。

实现不增加空指针、重叠、容量或异常保护，不把容量改成 `size_t` 或 `uint32_t`，也不
使用固定成功返回值。当前只存在一处公共声明和一处定义，没有源码调用点，接线留待
取得对应 caller 证据后处理。

## `sub_517190` 协议整数解析空指针门禁

完整函数范围为 `[0x517190,0x5171A3)`，共 19 字节、8 条指令和 3 个基本块。原 x86
ABI 为 `__cdecl`，唯一逻辑参数是只读 `text` 指针，真实返回类型是有符号 W32；新端
公开为 `std::int32_t ParseProtocolInteger_517190(const char*)`，仅把输入指针适配为
本机宽度，返回位宽保持 W32。

IDA 给出的指针返回类型是误推断；返回合同以唯一 callee `sub_5176B0@0x5176B0` 的
数值解析语义和 caller 对返回结果的消费共同裁决为 W32。唯一 caller 函数是
`network_dispatch`，原二进制共有 157 个直接 callsite，157/157 均消费结果：保存到
ESI 47 处、EDI 19 处、EBX 12 处、EBP 8 处，直接压栈 40 处，写入栈局部 31 处。

三个基本块的算法严格为：入口只判断 `text` 是否为空；空指针块直接返回 `0`，且不
调用依赖；非空块把原指针不变地传给 `sub_5176B0` 一次，并原样返回其 W32 结果。
包装器不访问全局、不分配内存、不增加额外校验、不写输入，也不接入 dispatcher。

`sub_5176B0` 已在私有依赖头中保留唯一声明，并在本实现文件的
`cg::network::detail` 符号域中提供真实定义；包装器仍只负责空指针门禁和一次调用，未
内联或改写解析算法。`ParseProtocolInteger_517190` 当前源码调用点为零，
`network_dispatch` 尚未接线，但其唯一解析依赖现已闭合。

## `sub_5176B0` base62 协议整数解析

完整函数范围为 `[0x5176B0,0x517721)`，共 113 字节、49 条指令和 16 个基本块。原 x86
ABI 为 `__cdecl`，唯一参数是只读 NUL 终止 `text` 指针，返回类型是有符号 W32；新端
私有合同为 `std::int32_t Sub_5176B0(const char*)`，只把指针适配为本机宽度。函数没有
callee，不访问全局或字符串，不写输入，也不分配内存。唯一 caller 是既有
`ParseProtocolInteger_517190`，当前源码保持一处声明、一处定义和一处调用。

入口以无符号字节读取首字节。首字节恰为 ASCII `'-'` 时记录负号并把游标前移一次，
否则按正数处理；`'+'` 和后续 `'-'` 均不具有符号语义。累加器从 `0` 开始，每个非 NUL
字节先在 `std::uint32_t` 中执行 `accumulator * 62`，再按下列唯一映射加入 digit：

| 输入字节 | digit |
|---|---:|
| ASCII `'0'..'9'` | `0..9` |
| ASCII `'a'..'z'` | `10..35` |
| ASCII `'A'..'Z'` | `36..61` |

字节判断全程使用无符号值；除上述三个 ASCII 区间外的任何字节（包括高位字节）均在
出现位置立即返回 `0`，此前合法前缀被丢弃。乘法与加法都按模 `2^32` 回绕。读到 NUL
后，正数返回位模式 `accumulator`，负数先在 `std::uint32_t` 中形成
`0u - accumulator`，再以 `std::bit_cast<std::int32_t>` 解释同一 W32 位模式，不触发
有符号溢出，也不进行超范围的普通无符号到有符号转换。

空串、仅含 `'-'`、合法零、任意位置含非法字节以及恰好回绕为零的合法输入都返回
`0`，函数不额外区分这些情况。实现不增加空指针门禁、长度参数、`strtol`、ctype、
locale 或错误码；无 NUL 输入和直接以空指针调用本 detail 函数仍属于原合同异常域。
公共包装器继续负责空指针返回 `0`，dispatcher 仍未接线。

## 九项分配、清零与检查

入口先保存 W32 `bufferLength`，再把九个指针全部覆盖为空；旧值不会预先释放，因此
重复调用仍会丢失旧指针并泄漏。随后必须先完成全部九次 `malloc`，再开始清零，最后
才逐项检查非空。分配、清零和非空检查均采用下表顺序：

| 原全局 | 新端承载 | 元素/字节数 |
|---|---|---:|
| `0x58B5F0` | byte buffer | `bufferLength` 字节 |
| `0x58B5F4` | byte buffer | `bufferLength` 字节 |
| `0x58B5F8` | byte buffer | `bufferLength` 字节 |
| `0x58B5FC` | byte buffer | `bufferLength` 字节 |
| `0x58B614` | `char**` pointer table | `bufferLength` 个本机指针槽 |
| `0x58B604` | triple byte buffer | `tripleLength` 字节 |
| `0x58B608` | triple byte buffer | `tripleLength` 字节 |
| `0x58B60C` | triple byte buffer | `tripleLength` 字节 |
| `0x58B610` | triple byte buffer | `tripleLength` 字节 |

`tripleLength` 先以 `std::uint32_t(bufferLength * 3u)` 形成，保留 W32 回绕。四个普通
缓冲和四个 triple 缓冲使用 `memset` 清零；pointer table 使用 `fill_n(nullptr)`。
原 x86 pointer table 每槽 4 字节，新端为了保存有效 x64 指针，申请
`size_t(bufferLength) * sizeof(char*)`，这是明确的本机指针槽适配，不保存或截断为
32 位指针位模式。

## 失败语义与有效域

每一项只在自身长度非零时清零。非零长度对应的 `malloc` 若返回空，原函数会在最终
非空检查前进入空指针清零访问；标准 C++ 不能表达该访问异常。用户已确认新端在同一
位置以 `std::terminate()` 作确定性终止，不提前统一检查、不回滚，也不改成 `-1`。

长度为零时跳过对应清零，仍执行九次分配和最终九项非空检查。任一指针为空时进入
公共失败块，严格按 `58B5F0 -> 58B5FC -> 58B5F8 -> 58B5F4 -> 58B614 ->
58B604 -> 58B608 -> 58B60C -> 58B610` 调用 `free`，不清指针、不改长度，返回
`-1`；九项均非空才返回 `0`。因此 `malloc(0)` 是否返回空会影响零长度调用结果，
保持实现库行为。

当前可移植有效域是各非零长度分配均成功，且 caller 遵守原运行期生命周期；超大
长度仍按 W32 triple 回绕和实际 x64 分配结果处理。重复初始化、分配失败后的悬空状态
以及清理后的再次读取均未增加新保证。

## `sub_508140` Winsock 启动与登录接线

入口声明未预清的局部 `WSADATA`，调用
`WSAStartup(MAKEWORD(1, 1), &data)`。返回非零时立即返回 `0`：不读取 socket，
不调用分配链，也不写活动门禁或 `world::gValueD9F2CA`。成功后调用
`AllocateNetworkWorkspace_516B60(&detail::Sub_403E50, 0x10000,
platform::gConnectionSocket)` 并忽略其返回值，再严格先把私有 W8 活动门禁
`0xD9F2C8` 写为 `1`，后把既有 `world::gValueD9F2CA` 写为 `0`，返回 `1`。本入口
不读写 `login::gRequestPollingEnabled@0xD9F2C9`。

`LoginState.cpp` 已直接调用唯一的
`cg::network::InitializeNetworkRuntime_508140()`，并删除旧私有声明；主阶段仍保持
`ShutdownNetworkRuntime_5081A0 -> blacklist::PersistAndReset_401000 ->
InitializeNetworkRuntime_508140` 的原顺序。startup 静态库既有 `ws2_32` 链接和
`/GR-` 保持不变。

## `sub_5081A0` 网络运行期关闭

原函数由 2 个 chunk 组成，共 59 条指令、6 个基本块；全程序共有 12 个原直接
callsite。入口先读取 W8 活动门禁 `0xD9F2C8`，值为 0 时立即返回，不写状态、不调用
系统 API、不重置协议状态，也不释放任何内存。

活动路径严格先后执行：把仅地址型 W32 `0xD9F2D0` 写为 0、把活动门禁写为 0、把
`login::gRequestPollingEnabled@0xD9F2C9` 写为 0、把
`world::gValueD9F2CA@0xD9F2CA` 写为 0，随后调用
`closesocket(static_cast<SOCKET>(platform::gConnectionSocket))`、`WSACleanup()`、
`ResetProtocolState_4037D0()`；后三项返回值全部忽略。

正常清理先严格按 `58B5F0 -> 58B5F4 -> 58B5F8 -> 58B5FC -> 58B614 ->
58B604 -> 58B608 -> 58B60C -> 58B610` 调用九次 `free`，再按索引 0 至 17 逐项
`free(workspaceSlots[index])`，最后 `free(workspaceSlots)`，因此动态执行 28 次
`free`。关闭函数不清 socket、回调、buffer length、九个指针、18 个元素或外层表
指针，也不增加有效性检查、RAII 或错误恢复。

用户已明确裁决“先保留” `workspaceSlots` 为空或悬空时的异常域。新端正常路径仍直接
索引该表，不增加判空、悬空跟踪或 `std::terminate()`；该异常域不获得新的可移植
保证。

当前源码中 11 个对应调用表达式均已直接改接唯一公共入口：`LoginState.cpp` 1 处、
`LoginScene.cpp` 4 处、`CharacterSelectScene.cpp` 4 处、
`platform/ClientShutdown.cpp` 1 处、`world/WorldUpdate.cpp` 1 处。12 个原 callsite 中
剩余路径位于尚未实现的 `sub_45EBB0`；该函数原有四个内部 callsite 整体留待其未来
还原，本项不猜测或修改。

## x64 适配与故意保留的缺陷

- `sub_516DA0` 的原 `4 * Size` 指针表适配为 `Size` 个本机 `char*` 槽；
- `sub_516B60` 的原 72 字节外层表适配为 `18 * sizeof(void*)`，每槽保存原生指针；
- socket 形参使用 `uintptr_t`，不把 x64 `SOCKET` 截断为 W32；回调槽保存真实本机
  函数指针；
- `sub_516B60` 故意忽略九缓冲分配结果；`sub_508140` 又故意忽略 18 槽分配结果，
  因而 Winsock 启动成功后即使分配失败，仍会写活动门禁并返回 `1`；
- 两级分配都保留重复调用覆盖旧指针并泄漏的行为；18 槽中途失败只释放第 0 项，
  保留外层表、其他元素以及悬空/空槽；
- 九缓冲正容量空分配仍按已确认策略在原清零位置 `std::terminate()`；零长度结果继续
  取决于当前 C 运行库的 `malloc(0)`；
- 关闭入口按原顺序调用 `WSACleanup`，但不增加失败回滚、指针清理、重复清理保护、
  版本协商校验或其他保护。

## 未实现边界

- `sub_403E50@0x403E50` 只保留真实声明，协议发送实现仍未落地；
- `sub_508360@0x508360` 仍未实现；
- `network_dispatch` 尚未接线，`ParseProtocolInteger_517190` 仍没有源码调用点；
- 本步没有为其余未实现边界提供空体或替代实现，不能据此宣称网络生产、发送与轮询
  生命周期已经闭合。

行为证据仅来自当前 `ida-17524/cg2d.exe` 及当前实现源码；STONAGE 未参与事实、语义或
验收判断。
