# 网络协议运行时创建链还原证据

## 范围与当前边界

本步实现并接入以下三个函数：

- `sub_4037D0@0x4037D0`：恢复八个协议 W32 常量并清零六项短状态；
- `sub_516B60@0x516B60`：安装三参数回调，复用既有 `sub_516DA0`，再分配
  18 项网络工作区；
- `sub_508140@0x508140`：启动 Winsock 1.1，创建网络工作区并设置活动门禁。

既有 `AllocateProtocolBuffers_516DA0` 保持为九项协议缓冲的唯一生产者。本步只扩展
`src/network/NetworkRuntime.cpp` 匿名 namespace 中原有的唯一
`NetworkRuntimeState`，没有复制九缓冲状态，也没有并入 `LoginState`、
`LoginConnection` 或其他 owner。新加协议短状态、回调槽、18 槽工作区外层指针及活动
门禁均使用地址中性私有字段名；`login::gValue593FC0` 是独立 W32，不在本模块读写。

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
再依次清零 W8 `0x593FC6`、`0x593FCA`，最终返回 `0`。本入口当前只建立真实状态
生产者；`sub_5081A0` 尚未实现，因此尚未接入其清理编排。

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
`Sub_5081A0 -> blacklist::PersistAndReset_401000 -> InitializeNetworkRuntime_508140`
的原顺序。startup 静态库仅新增 `ws2_32` 链接，`/GR-` 保持不变。

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
- 本步不增加失败回滚、`WSACleanup`、指针清理、版本协商校验或其他保护。

## 未实现边界

- `sub_403E50@0x403E50` 只保留真实声明，协议发送实现仍未落地；
- `sub_5081A0@0x5081A0` 仍未实现，后续必须复用本模块的活动门禁、协议状态、九缓冲
  与 18 槽工作区完成原清理链；
- `sub_508360@0x508360` 仍未实现；
- 本步没有为上述边界提供空体或替代实现，不能据此宣称网络生产、
  发送、轮询与清理生命周期已经闭合。

行为证据仅来自当前 `ida-17524/cg2d.exe` 及当前实现源码；STONAGE 未参与事实、语义或
验收判断。
