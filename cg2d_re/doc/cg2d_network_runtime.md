# 网络协议运行时创建与清理链还原证据

## 范围与当前边界

本模块当前实现并接入以下四个函数：

- `sub_4037D0@0x4037D0`：恢复八个协议 W32 常量并清零六项短状态；
- `sub_516B60@0x516B60`：安装三参数回调，复用既有 `sub_516DA0`，再分配
  18 项网络工作区；
- `sub_508140@0x508140`：启动 Winsock 1.1，创建网络工作区并设置活动门禁。
- `sub_5081A0@0x5081A0`：按活动门禁关闭 socket/Winsock、重置协议状态并释放
  九项协议缓冲及 18 槽网络工作区。

此外，`sub_517080@[0x517080,0x5170C1)`、`sub_5170D0@[0x5170D0,0x51711C)`、
`sub_517120@[0x517120,0x517160)`、`sub_517160@[0x517160,0x517186)`、
`sub_517190@[0x517190,0x5171A3)`、`sub_5171F0@[0x5171F0,0x5172CE)`、
`sub_5172D0@[0x5172D0,0x5173A6)`、
`sub_517480@[0x517480,0x517502)`、
`sub_517670@[0x517670,0x51768A)` 与
`sub_517690@[0x517690,0x5176AC)`
均已完成公共声明与实现。`sub_517080` 当前由 `sub_517120` 与 `sub_517690` 的新端实现
调用 2 次；`sub_5170D0` 分别由 `sub_517120`、`sub_517160` 调用 1 次；
`sub_517670` 由 `sub_517120` 调用 1 次，`sub_5171F0` 由 `sub_517160` 调用 1 次；
`sub_517120`、`sub_517160`、`sub_517190`、`sub_5172D0` 与 `sub_517690`
本身仍无源码调用点。
私有 `sub_5176B0@[0x5176B0,0x517721)` 也已完成唯一声明与实现，当前由
`sub_517190` 调用 1 次；`sub_517730@0x517730` 仅保留声明。它们不属于上述运行期
创建与清理接线。

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
公共声明和一处定义，并已有 2 个调用点，分别对应 `sub_517120` 内的 `0x51713B` 与
`sub_517690` 内的 `0x5176A0`；六个原二进制 callsite 中两处已还原，余四处留待各
caller 还原时处理。

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
使用固定成功返回值。当前只存在一处公共声明和一处定义，并有 2 个源码调用点，分别
对应 `sub_517120` 内的 `0x517152`、`sub_517160` 内的 `0x51717A`；其余接线留待取得
对应 caller 证据后处理。

## `sub_517120` 协议整数令牌格式化包装

完整函数范围为 `[0x517120,0x517160)`，共 64 字节、19 条指令和 1 个基本块，没有分支。
原 x86 ABI 为 `__cdecl`，唯一逻辑参数是有符号 W32 `value`，返回值是协议缓冲指针；
新端公开为
`std::uint8_t* FormatProtocolIntegerToken_517120(std::int32_t)`，只把返回指针适配为
本机宽度。

原函数严格按下列顺序执行：第一次读取 `0x58B5EC`，以同一 W32 位模式作为有符号容量；
调用 `FormatProtocolInteger_517670(value)`；第一次读取 `0x58B5FC`，把该目标、格式化
结果和第一次容量传给 `CopyProtocolBytes_517080`；第二次独立读取 `0x58B5EC` 并再次
按位解释为有符号容量；第二次读取 `0x58B5FC`，把该目标、单个 ASCII 空格字符串和
第二次容量传给 `AppendProtocolText_5170D0` 并忽略返回值；最后第三次读取
`0x58B5FC` 并返回。新端将 `0x58B5EC` 映射为 `bufferLength`，每次均使用
`std::bit_cast<std::int32_t>` 保留 W32 位模式，将 `0x58B5FC` 映射为
`buffer58B5FC`。各次读取和 helper 调用均为独立语句，没有依赖函数参数求值顺序，也
没有合并两次容量或三次目标指针读取。

原字符串 `0x55C9BC` 的精确字节是 `20 00`；新端用函数内静态常量表示相同的 ASCII
空格加 NUL，不保留原虚拟地址身份。正常有效域内，目标共享缓冲最终包含格式化整数和
一个尾随空格，返回值只是第三次读取到的共享缓冲指针，不表示长度或成功状态。

包装器不增加判空、容量检查、错误码、owner、私有缓冲、锁、`thread_local`、清理或
容错。整数 formatter 的共享 scratch 会被后续格式化覆盖，`buffer58B5FC` 也由整个
网络运行期共享；独立读取还保留并发或重入期间目标指针可能变化的原语义，因此函数
继续非重入且非线程安全。`INT32_MIN`、formatter 失败、无效容量、无效指针和 helper
异常域均原样继承。

IDA 识别出 68 个 caller、160 个直接 callsite，原返回值均作为随后一次
`AppendProtocolText_5170D0` 的 `source`。这些原 caller 本步全部不接线，当前源码只有
一处公共声明和一处定义，`FormatProtocolIntegerToken_517120` 的源码调用点为零。
本节证据只确认该包装器自身的指令、全局读取和调用边界，不据此推断 160 个 callsite
所在协议的更高层语法。

`FormatProtocolInteger_517670` 仍依赖仅声明、未定义的 `Sub_517730`；本包装器没有为
其补实现或 stub，因此即使本函数已经复刻，整数格式化链的最终链接仍未闭合。当前
helper 源码调用计数为：`FormatProtocolInteger_517670` 1 次、
`CopyProtocolBytes_517080` 2 次、`AppendProtocolText_5170D0` 2 次、
`EscapeProtocolText_5171F0` 1 次。

## `sub_517160` 协议文本令牌格式化包装

当前 `ida-17524/cg2d.exe` 中，完整函数范围为 `[0x517160,0x517186)`，共 38 字节、
14 条指令和 1 个基本块，没有分支。原 x86 ABI 为 `__cdecl`，唯一逻辑参数是只读文本
指针，返回值是原转义缓冲指针；新端公开为
`std::uint8_t* FormatProtocolTextToken_517160(const char*)`，只把指针适配为本机宽度。
原函数的两个 callee 依序是 `sub_5171F0@0x5171F0` 和 `sub_5170D0@0x5170D0`，
对应直接 call 指令为 `0x517166`、`0x51717A`。

原函数先将参数原样交给 `EscapeProtocolText_5171F0`；随后仅在 `0x51716B` 读取一次
W32 容量 `0x58B5EC`，按同一位模式解释为有符号容量；以转义返回
指针为 `destination`、`0x55C9BC` 处字节 `20 00`（ASCII 单空格加 NUL）为 `source`
调用 `AppendProtocolText_5170D0`，忽略该调用的返回值，最后返回原转义指针。新端用
`NetworkRuntimeState::bufferLength` 和 `std::bit_cast<std::int32_t>` 保留 W32 位模式，
使用函数内静态常量承载同样的空格字符串，不读取或复制其他协议缓冲。

IDA 识别出 22 个原 caller、31 个直接 callsite；逐处紧邻指令显示，31/31 均把返回的
EAX 指针作为随后一次 `AppendProtocolText_5170D0` 的 `source`。这仅确认调用点局部的
返回值消费，不代表已经展开所有 caller 的完整业务。当前新端只有一处公共声明、一处
定义，`FormatProtocolTextToken_517160` 本身没有源码调用点；`EscapeProtocolText_5171F0`
和 `AppendProtocolText_5170D0` 分别新增这一个源码调用点。

包装器不增加空指针、容量预检、同步、私有缓冲或编码转换。容量不足时只保留既有
`AppendProtocolText_5170D0` 的行为；前置转义本身不检查容量。无效输入或目标、越界、
源目标重叠、并发/重入，以及孤立高位尾字节继续属于既有 helper 的异常域。原
`Report_512CA0` 的直接链不含本函数；本步不接上游 caller，主循环上报链仍未闭合。

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

## `sub_5171F0` 共享缓冲协议文本转义

当前 `ida-17524/cg2d.exe` 中，函数完整范围为 `[0x5171F0,0x5172CE)`，共 222 字节、
66 条指令和 17 个基本块；原 x86 ABI 为 `__cdecl`，唯一参数是 `char*` 输入，EAX
返回 `0x58B5F8` 中的共享缓冲指针，没有 callee。唯一原 caller 是
`sub_517160@0x517160`，直接 callsite 为 `0x517166`。新端公开为
`std::uint8_t* EscapeProtocolText_5171F0(const char*)`，只把指针适配为本机宽度，
复用现有 `NetworkRuntimeState::buffer58B5F8`；其分配仍只由
`AllocateProtocolBuffers_516DA0` 负责。

入口在 `0x5171F0..0x5171F8` 先读取共享指针并写 `destination[0] = 0`，随后
`0x5171FA..0x517201` 才读取输入首字节。空串路径 `0x5172BE..0x5172CD` 再读共享
指针、再次写 `destination[0] = 0`，最后重新读取共享指针返回。非空路径按下列顺序
逐单元处理，不预扫描长度：

1. `0x51720A..0x517224`：高位首字节 `0x80..0xFF` 连同紧随的下一字节原样成对
   复制；先写首字节，再读取并写第二字节，写第二字节前不判断其是否为 NUL。
2. `0x517247..0x5172B9`：其余字节依次判断 ASCII 反斜杠、空格、LF、CR，分别
   输出 `\\`、`\S`、`\n`、`\r`；其余 ASCII 原样复制。转义两次写入之间分别
   重新读取共享指针，不缓存整轮目标。
3. 每个单/双字节单元都经过 `0x51722D..0x517234`：先预读当前源位置的下一个
   字节，再递增输出下标和源游标；预读为零时，经 `0x517236..0x517246` 再读共享
   指针追加 NUL，并再次读取共享指针返回。原函数只多次读取 `0x58B5F8` 指针，
   不读取容量，也没有分配、错误码或锁。

有效比对域要求共享目标已经分配且足以容纳转义输出与尾部 NUL，输入指针非空且指向
可读的 NUL 终止字节串（允许空串），并且每个高位首字节后都有非 NUL 后继。孤立高位
尾字节会先复制 NUL，然后越过该 NUL 继续预读；空指针、越界、源目标重叠和并发/重入
均属于原合同异常域，新端不添加可移植保证，也不作 UTF-8、GBK 或 Big5 转换。当前
仅有一处公共声明和一处定义，当前由 `FormatProtocolTextToken_517160` 调用 1 次；
`Report_512CA0` 的直接链不含本函数，本步不接入主循环上报路径，也不宣称该链已闭合。

## `sub_5172D0` 共享缓冲协议文本反转义

当前 `ida-17524/cg2d.exe` 中，函数完整范围为 `[0x5172D0,0x5173A6)`，共 214 字节、
72 条指令和 20 个基本块；原 x86 ABI 为 `__cdecl`，唯一参数是非空原始字节串指针，
EAX 返回全局指针 `0x58B5F8` 的当前值，没有 callee。唯一原 caller 是
`sub_5171B0@0x5171B0`，直接 callsite 为 `0x5171D9`，该 caller 仅在参数非空时进入
本函数。新端公开为 `std::uint8_t* UnescapeProtocolText_5172D0(const char*)`，复用
唯一私有 `NetworkRuntimeState::buffer58B5F8` owner；它的分配仍只由
`AllocateProtocolBuffers_516DA0` 负责，不另建缓冲。

入口 `0x5172D0..0x5172E4` 先读共享指针并写首字节 `0`，然后才读输入首字节；
空串走 `0x517395..0x5173A5`，再次写首字节 `0`，重读共享指针返回。非空路径逐轮按
以下指令顺序处理，不预扫长度：

1. `0x5172EB..0x5172F6` 读取并分类当前字节。高位首字节 `0x80..0xFF` 走
   `0x5172F8..0x51730D`，先复制首字节、推进源游标，再读取并原样复制紧随字节；
   写第二字节前不判断其是否为 NUL。普通非反斜杠 ASCII 经 `0x51732C` 的比较后
   跳到 `0x517307` 原样复制。
2. ASCII 反斜杠经 `0x517331..0x517337` 预读后继。后继为 NUL 时由
   `0x517339..0x517343` 输出独立反斜杠，不吞掉终止符。非 NUL 后继为 `S`、`n`、
   `r` 时，`0x517345..0x51737F` 分别输出空格、LF、CR，并推进源游标跳过后继。
   其他后继均走 `0x517381..0x517390`：只输出反斜杠并吞掉后继，包括后继本身
   也是反斜杠的情形；`0x517381` 的比较之后没有条件跳转，不产生另一映射。
3. 每轮均在 `0x517311..0x517318` 先预读下一源字节，再推进源游标并测试；
   预读为零时由 `0x51731A..0x51732B` 重新读取共享目标指针追加 NUL，再次读取
   共享指针返回。原函数不读取容量，也不分配、加锁或返回错误码。

有效比对域要求共享目标已分配且足以容纳输出与尾部 NUL，输入指针非空且指向可读的
NUL 终止字节串（允许空串），每个高位首字节后均有非 NUL 后继，并且源目标不重叠、
无并发。孤立高位尾字节会先复制 NUL，再越过它预读；无效指针、越界、重叠和并发均
维持原异常域，不增加保护或 UTF-8、GBK、Big5 转换。此函数不能复用
`CharacterListRequest.cpp` 的 `UnescapeLegacyDetail`：后者原地改写且转义映射不同。
当前仅有一处公共声明、一处定义，没有新端源码调用点；本步不接
`sub_5171B0` 或 `network_dispatch`。

## `sub_517480` 协议发送前日志、观察与行尾包装

完整函数范围为 `[0x517480,0x517502)`，共 130 字节、49 条指令和 8 个基本块。
原 x86 ABI 为 `__cdecl`，两个参数依次为 W32 `socketValue` 与可写的 NUL 终止
`buffer` 指针，返回值为有符号 W32。新端公开为
`std::int32_t SendProtocolBuffer_517480(std::uint32_t, char*)`，只把指针适配为
本机宽度。IDA 识别出 83 个 caller、84 个直接 callsite；按 caller 聚合后，
82 个 caller 各含 1 个 callsite，其余 1 个 caller 含 2 个 callsite。这一
分类只确认直接调用关系；当前源码仍无本函数的调用点，不在本步猜测上游接线。

日志路径 `0xDA3B58` 由唯一私有 `char[0x400]` 字节数组承载，依靠
`NetworkRuntimeState` 整体零初始化，不公开 setter。入口只检查路径首字节；
非零时以精确模式 `"a+"` 调用 `fopen`，打开成功后严格执行
`fprintf(file, "->%s\n", buffer)`，再执行 `fclose`，两个返回值均忽略；
打开失败只跳过日志，不改发送结果。观察回调 `0x58B61C` 同样由零初始化的
私有 `void (*)(std::uint32_t, char*)` 槽承载，不公开 setter；每次调用只读取
一次该槽，非空时以方向 `1` 和原 `buffer` 调用，无返回值可消费。
日志和观察均先于长度计算与行尾写入，因而两者看到的都是尚未追加 LF 的原缓冲。

之后调用 `strlen(buffer)`，再显式窄化为 W32 无符号长度 `L`。
`bufferLength` 只独立读取一次得到 W32 `N`，门槛以
`std::uint32_t(N * 3U)` 形成，保留模 `2^32` 回绕。仅当无符号比较
`L < 3N` 成立时，依次写 `buffer[L] = '\n'`、`buffer[L + 1U] = '\0'`，
再以 W32 语义执行 `++L`；否则不改缓冲或长度。最后不判空地调用现有
`gNetworkRuntimeState.callback(socketValue, buffer, L)`，并原样返回其有符号 W32
结果。

实现不检查 `buffer`、socket、主回调、NUL 终止或实际容量，也不将
`3N` 门槛当作可证明的分配长度。尤其 `L == 3N - 1` 时仍会写入下标
`3N` 的尾 NUL；若调用方只提供精确 `3N` 字节，该写入即越界。
`strlen` 结果超过 W32、`N * 3U`、`L + 1U` 或 `++L` 的回绕，以及日志、
观察或主回调中的重入/并发，都不获得新的可移植保证。无效指针、缺失 NUL、
容量不足、空主回调与上述越界全部继续属于原合同异常域，不增加 RAII、
异常转换或容错。

## `sub_517510` 只读分析与待决 SPEC

**R6.1.3.148只读分析、未编码，SPEC待确认。** 本节仅记录分析代理 Turing 对当前
cg2d IDA 的交接证据，文档记录任务为 `R6.1.3.148.D`；未重新取证，也不改变既有
callee 的历史证据。分析结论为 `NEEDS_CLARIFICATION`，不具备代码门禁，不表示
实现、审查或验证通过。

完整函数范围为 `[0x517510,0x51766C)`，共 348 字节、113 条指令、14 个基本块，
单个连续 chunk。原 x86 ABI 为 `int __cdecl(char* Buffer, const void* command)`，
正常返回有符号 W32 `0`，对应 `0x517631` 的 `xor eax, eax`。容量读取次数为零，
不调用 `sub_517080`、`sub_5170D0` 或 `sub_517480`。有 83 个直接 caller；
`0x512C6B` 调用点传入共享 buffer 和 `"DE"`，不据此扩展其余 caller 的业务语义。

交接确认的处理顺序如下：

1. `0x517513` 读取 `0x593850` 首字节写入栈临时区首字节，再扫描 `command`
   长度；`0x517534` 清零临时区首字节之后的 4 字节。栈留出 8 字节，但并非全清。
   `0x51753F`、`0x51754D` 分 DWORD 与余字节复制 `strlen(command)` 字节，不含
   NUL；第一次读取 W16 模式 `0xDA3B22` 位于两段复制之间的 `0x517546`。
2. `0x517579` 调用 `sub_4038A0`：模式为 0 时参数为
   `(temp, 25, 0x58B62C, 120, 0x58B638)`，非 0 时为
   `(temp, 31, gPassword, 121, gAccount)`，返回值被丢弃。常量 `0x58B62C`
   的字节为 `75 43 59 50 57 64 6A 73 00`，`0x58B638` 的字节为
   `4D 73 49 74 4D 56 68 67 62 57 50 00`。
3. `0x517590` 执行 `sprintf(Buffer, "%s ", temp)`，没有容量参数。
   `0x517595` 再次读取 `0x593850`，初始化复用的第二参数栈槽首字节；
   `0x5175B0` 无条件计算口令长度。随后依次清零 W16 `0x593FC4`、临时槽偏移
   1 与 2 的字节、W8 `0x593FC6`。
4. `0x5175CD..0x5175EC` 在 `temp` 非空时逐字节累加 `(byte & 0x7F)`；
   每次仅在 `sum > 255` 时减去 255，等于 255 不减。`0x5175EE` 第二次读取
   模式：为 0 时异或 59；非 0 时先把零口令长度替换为 1，再以有符号 W32
   `82 % len` 定位口令字节，将该字节符号扩展至 W16，再零扩展至 W32 后异或。
5. `0x517625` 执行 `_itoa(value, tempSlot, 16)`；
   `0x517649`、`0x517650` 把格式化文本的长度字节复制到 `0x593FC4`，不含 NUL。
   随后从该全局地址重新扫描长度，在 `0x517660` 写入 W16 `0x593FCC`，最终返回 0。

新增重大布局冲突：当前 `NetworkRuntimeState` 将 `0x593FC4`、`0x593FCC`、
`0x593FC6` 分别映射为 `protocolHalfWord2`、`protocolHalfWord0`、`protocolByte0`，
并非原连续存储，不能对 `protocolHalfWord2` 取址后当作文本。`sub_4037D0` 会重置
这些值；`sub_403E50` 在 `0x403F04` 取得 `0x593FC4` 地址，在 `0x404033` 读取
`0x593FCC`，相关访问必须使用唯一 owner。高位口令字节可使十六进制结果达到 4 位，
复制覆盖至 `0x593FC7`；复用的临时参数槽也存在跨槽风险。不能据此擅定 3 字节缓冲、
补写 NUL 或将输入限制为 ASCII。

待用户确认的 SPEC 草案仅提出以下要求，均未批准或实施：

- 连续共享字节存储：需要由唯一 owner 承载相关原地址字节关系，使重置、文本写入、
  长度写入与后续消费保持一致；具体覆盖范围、布局及既有字段迁移方式仍待确认，
  不另建与短状态脱节的文本副本，不改写已通过 callee 的历史证据。
- 模式与依赖 owner：W16 `0xDA3B22` 当前无源码 owner，其直接 writer
  `0x5086AD`、`0x508836` 尚未展开，需后续取证或裁决。账号、口令复用
  `LoginScene.hpp` 中已有 `gValueDA38DC`、`gValueDA3A5C`。`sub_4038A0` 当前
  无源码，另任务 149 正在取证，不能将其视为已实现。`0x593850` 已知初值为零、
  未发现直接写入，但存在表或间接传播，不能当作恒定空串。
- 异常域：高位口令导致的 4 位结果、全局复制不含 NUL 后的长度扫描、临时参数槽
  跨槽及未全清临时区，需要进一步取证或用户裁决；未决前不擅自补零、截断、扩大
  缓冲或收窄有效输入域。本节没有批准异常域的实现策略。

socket ABI 仍是独立待用户确认事项，本分析不作裁决，也不改变其余边界的当前事实。

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

## `sub_517670` 共享缓冲整数格式化包装

完整函数范围为 `[0x517670,0x51768A)`，共 26 字节、8 条指令和 1 个基本块。原 x86
ABI 为 `__cdecl`，唯一逻辑参数是有符号 W32 `value`，返回值是共享格式化缓冲的
指针；新端公开为
`std::uint8_t* FormatProtocolInteger_517670(std::int32_t)`，只把返回指针适配为本机
宽度。唯一 caller 是 `sub_517120`，唯一直接 callsite 为 `0x51712B`；唯一 callee 是
`sub_517730@0x517730`。

原共享 scratch 精确覆盖 `[0xDA3F58,0xDA3F98)` 的 64 字节，当前 IDA 中仅有
`0x517676` 和 `0x517684` 两处交叉引用。新端在私有 `NetworkRuntimeState` 中使用独立
`std::uint8_t integerFormatScratch[64]` 承载，并依靠状态整体零初始化自然清零；
`0xDA3F98` 对应的 `workspaceSlots` 仍是随后独立的外层工作区指针，不与 scratch
合并或别名。

包装器只调用一次
`detail::Sub_517730(value, gNetworkRuntimeState.integerFormatScratch, 64)`，显式忽略
callee 返回值，再返回 `gNetworkRuntimeState.integerFormatScratch`。它不预清缓冲、
不增加判空或失败处理，也不转换容量；所有调用都返回同一共享别名，后续调用会覆盖
先前内容，因此保持非重入且非线程安全，不增加 `thread_local` 或锁。

`sub_517730` 当前只在私有依赖头中声明，没有定义或 stub；其格式化算法及
`INT32_MIN` 行为待独立还原，因此最终链接依赖尚未闭合。
`FormatProtocolInteger_517670` 当前有 1 个源码调用点，对应已复刻但尚无 caller 接线的
`FormatProtocolIntegerToken_517120`。

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

## `KeepAlive@0x457AB0` 主循环保活门禁

当前 `ida-17524/cg2d.exe` 的原函数范围为 `[0x457AB0,0x457B03)`，共 83 字节、
23 条指令、6 个基本块；唯一直接 callsite 位于 `MainLoop` 的 `0x4570E2`。入口读取 W8
`0xD9F2C8`，为零时直接返回，不取 tick。两个映像初值为零的 W32 `0x73ED1C`
（基线）与 `0x73ED20`（最近值）全部 xref 均在本函数，故新端由
`MainLoopSupport.cpp` 私有状态承载，只通过网络运行期的只读 getter 访问活动门禁。

基线为零时只调用一次 `GetTickCount`，依次写最近值、基线并返回；非零时只取一次
tick，以 W32 回绕减旧基线，再按 signed32 比较 `60000`，且比较前总是先写最近值。
达到门槛时按 `0x457AF0..0x457AFC` 先读取当前 socket、再写基线，调用
`sub_512C60(socket)` 并忽略返回。门禁关闭或重开均不重置两 tick；tick 为零仍保留
零基线哨兵。不额外检查 socket 或发送结果。原 callee 为 `int __cdecl(int)`，新端仅
将 socket 参数拓宽为 `std::uintptr_t` 并保留真实未定义声明。

## 未实现边界

- `sub_403E50@0x403E50` 只保留真实声明，协议发送实现仍未落地；
- `sub_512C60@0x512C60` 只保留真实声明，保活发送链的最终链接仍未闭合；
- `sub_517730@0x517730` 只保留真实声明，`sub_517670` 与 `sub_517120` 的格式化链及
  最终链接仍未闭合；
- `sub_517510@0x517510`（R6.1.3.148）仅完成只读分析记录，未编码；连续共享存储、
  模式 owner 与异常域 SPEC 待确认，`sub_4038A0` 仍无源码，不具备代码门禁；
- `sub_508360@0x508360` 仍未实现；
- `network_dispatch` 尚未接线，`ParseProtocolInteger_517190` 仍没有源码调用点；
- 本步没有为其余未实现边界提供空体或替代实现，不能据此宣称网络生产、发送与轮询
  生命周期已经闭合。

行为证据仅来自当前 `ida-17524/cg2d.exe` 及当前实现源码；STONAGE 未参与事实、语义或
验收判断。
