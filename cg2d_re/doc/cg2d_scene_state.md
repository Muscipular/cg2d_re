# cg2d 基础场景状态槽与切换原语

日期：2026-09-20。任务：R6.1.1a。证据源：当前 `ida-32980` 的 `cg2d.exe` IDB 与当前实现源码。

## 范围

本步只实现四个独立 32 位场景状态槽及三个直接写入原语。函数名是新工程的语义化名称，不声明为原源码名称；不实现 `World::Update@0x51C430`、状态分发或任何 scene。

## 原函数行为

| 原函数 | 当前 IDA 指令事实 | 新实现 |
| --- | --- | --- |
| `sub_51C770@0x51C770` | `0x51C770` 读取一个 32 位参数到 EAX；`0x51C774` 写 `W32[0xFF0D64]=0`；`0x51C77E` 写 `W32[0xFF0D40]=EAX`；`0x51C783` 返回，EAX 仍为输入参数 | `SetCurrentSceneType` |
| `sub_51C790@0x51C790` | `0x51C790/0x51C794` 读取两个 32 位参数；`0x51C798` 写 `W32[0xFF0D40]=第一个参数`；`0x51C79D` 写 `W32[0xFF0D64]=第二个参数`；`0x51C7A3` 返回第一个参数 | `SetCurrentSceneTypeAndState` |
| `sub_51C7B0@0x51C7B0` | `0x51C7B0` 读取一个 32 位参数到 EAX；`0x51C7B4` 写 `W32[0xFF0D60]=0`；`0x51C7BE` 写 `W32[0xFF0D6C]=EAX`；`0x51C7C3` 返回，EAX 仍为输入参数 | `SetPendingSceneType` |

## 状态映射与初值边界

| 原地址 | 新端映射 | 边界 |
| --- | --- | --- |
| `0xFF0D40` | `cg::platform::gValueFF0D40`；`cg::world::gCurrentSceneType` 仅为引用别名 | 复用参数解析已经定义的同一份 32 位存储，不复制状态 |
| `0xFF0D60` | `cg::world::gPendingSceneState` | 独立 32 位存储 |
| `0xFF0D64` | `cg::world::gCurrentSceneState` | 独立 32 位存储 |
| `0xFF0D6C` | `cg::world::gPendingSceneType` | 独立 32 位存储 |

当前实现的四个底层存储均从静态零值开始；这是新工程的装载初值边界，不代表调用三个原语时仍为零，也不补充任何枚举含义。`ParseCmdOpt@0x465F00` 的 `0x4669D1` 也会写 `0xFF0D40=1`，因此该槽必须继续与参数解析共享。

## 未实现范围

- `World::Update@0x51C430` 的待提交状态消费、主分发表、次级分发表及全部调用仍未实现；本步不计基本状态 loop 完成。
- 未实现任何 scene；登录相关 scene 留待后续按当前 cg2d 证据逐项拆分。
- 未新增状态枚举，不给任何数值附加未经当前 cg2d 证据确认的业务名称。
