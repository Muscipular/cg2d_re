# 网络协议缓冲运行时还原证据

## 范围与当前边界

本步实现 `sub_516DA0@[0x516DA0,0x517038)`，原函数共 189 条指令、11 个基本块。
闭合分析确认它的唯一直接 caller 是 `sub_516B60@0x516B60`。本步只建立该分配函数
与唯一私有运行时状态，不接 caller，不代表网络生产链或登录生产链已经完成。

状态由 `src/network/NetworkRuntime.cpp` 匿名 namespace 中唯一的
`NetworkRuntimeState` 持有，公开头只暴露
`AllocateProtocolBuffers_516DA0(std::uint32_t)`。状态初值全零，不并入
`LoginState`、`LoginConnection` 或其他 owner。

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

## 后续依赖边界

- `sub_516B60@0x516B60` 是本函数唯一 caller，后续负责其自身初始化编排；本步未实现。
- `sub_508140@0x508140` 位于上游创建链，后续必须复用本模块，不以第二份缓冲状态
  代替；本步未接入。
- `sub_4037D0@0x4037D0` 是独立网络运行期资源边界，本步未实现、未合并其状态。
- `sub_5081A0@0x5081A0` 是下游清理编排，后续必须释放这里的唯一九项 owner；本步
  未实现其释放接线。

本步未链接 `ws2_32`，未改动登录状态或依赖声明，也未宣称网络生产/清理链完成。
