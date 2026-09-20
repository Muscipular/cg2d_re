# cg2d 当前保存记录恢复编排入口证据

## 函数边界与调用关系

- 原函数：`sub_5201B0@[0x5201B0,0x5201F2)`，共 66 字节、17 条指令、
  3 个基本块，无参数。
- 唯一 caller/callsite 是 `sub_4576B0@0x457809`。外层立即按返回值分支：
  返回 0 时进入“读取存档失败”提示并返回 0，返回非零时继续后续初始化。
- 新端在既有 `Startup.hpp` 暴露窄入口
  `cg::platform::RestoreCurrentSavedState_5201B0()`，定义仍位于
  `LoginSavedState.cpp`；本项不把它接入唯一 caller `InitializeClient`。

## guard 与返回

入口第一步在 `0x5201B0` 调用 `sub_4A4210`。该 guard
`@[0x4A4210,0x4A4216)` 只有 `mov eax, 1; ret`，当前固定返回 1；新端以私有
`ReturnOne_4A4210()` 原样表达，不引入配置项、全局状态或动态条件。

除 `0x5201B0` 的唯一 code xref 外，`sub_4A4210` 原地址另有 34 个指向它的
direct data xref，均来自 UI 原型回调字段。这些引用只说明原地址被 UI 表引用；
私有 helper 仅服务当前恢复入口，不实现或接线这些表。未来还原相关表时，必须另行
依据其地址、字段和 caller 证据完成接线，不据本项虚构动态状态。

若 guard 返回 0，入口在 `0x5201EF` 置返回值为 0 后立即返回，不调用任何恢复
helper。guard 非零时，入口忽略全部 helper 返回值，并在 `0x5201E9` 固定返回 1。
因此 0 只表示 guard 拒绝，不表示任一恢复 helper 报告失败。

## 当前记录与完整调用顺序

前四个 helper 都接收同一个唯一当前记录
`gCurrentSavedRecord@[0xFF4918,0xFF4E28)` 的首字节指针；新端只取得一次
`currentBytes` 并原样传入，未建立副本或第二份 owner。调用顺序严格如下：

| 次序 | callsite | helper | 参数与结果 |
|---|---|---|---|
| 1 | `0x5201BE` | `RestoreAudioSettings_5204C0` | `currentBytes`；返回忽略 |
| 2 | `0x5201C8` | `RestoreSavedScalarSettings_5205E0` | 同一 `currentBytes`；返回忽略 |
| 3 | `0x5201D2` | `RestoreSavedInputContexts_520690` | 同一 `currentBytes`；返回忽略 |
| 4 | `0x5201DC` | `RestoreSavedByteState_520700` | 同一 `currentBytes`；返回忽略 |
| 5 | `0x5201E4` | `RestoreCurrentSavedValue_520730` | 无参数；返回忽略 |

入口没有其他恢复调用、重排、短路或结果聚合。

## 异常与部分写入边界

入口不捕获异常，也不提供事务提交或回滚。前一个 helper 已完成的 live 状态写入会
在后一个 helper 发生访问异常时保留；单个 helper 内已经完成的逐字段、逐字节写入
也会保留。只有 guard 为 0 的早退路径保证五个恢复 helper 均未被调用。

本项不改变各 helper 已记录的有效域：尤其输入文本恢复仍按源 NUL 无界扫描并逐项
写入，异常可能发生在部分音频、标量或输入上下文已恢复之后。由于 helper 返回均被
忽略，正常返回 1 也不表达各字段的独立成功状态。

## 实现与证据边界

本项只修改既有声明与保存状态实现单元，并新增本文档；不修改 CMake、进度或唯一
外层 caller。行为证据只取自当前 `ida-17524 / cg2d.exe` 与当前实现源码；
STONAGE 仅可作为架构参考，不参与本项行为、地址、语义或验收结论。
