# 能力 4 保存并切换持久槽证据

## 范围与调用关系

本文只记录当前 `ida-17524` 中 `D:\SOFTSTAR\cg2d.exe` 的
`sub_465740@[0x465740,0x465788)`。函数大小为 `0x48`，共 17 条指令；控制流
没有条件分支。唯一 caller 是 `sub_465430` 在 `0x4654BD` 的直接调用。本项只
提供 `login::SaveAndSwitchCapability4InputSlot_465740(newSlot)`，不接该 caller。

函数内先执行 4 个直接调用，再在 `0x465783` 以 `jmp sub_465320` 尾跳。新端
末尾调用既有 `InitializeCapability4Input_465320()` 后自然返回，表达相同的末步
初始化语义，不复制其实现。

## 严格保存与切换顺序

1. `0x465740` 独立读取旧 `gSelectedSavedSlot105C394@0x105C394`；把
   `gInputContext9A4D20.text` 与该快照传给 `Sub_533570@0x533570`。
2. `0x465750` 再次独立读取旧 selected；把 `gInputContext9A5960.text` 与第二份
   快照传给 `Sub_5335B0@0x5335B0`。
3. `0x465761` 第三次独立读取旧 selected；把 `gInputContext9A5678.text` 与第三份
   快照传给 `Sub_533640@0x533640`。
4. `0x465775` 调用 `Sub_533730@0x533730` 持久化记录，原返回值不参与后续控制流。
5. `0x46577A–0x46577E` 才读取参数 `newSlot` 并写入共享 selected。
6. `0x465783` 尾跳能力 4 输入初始化；新端复用唯一已实现入口并返回。

三次 selected 读取不能合并为一个快照：每个 writer 完整返回后，下一阶段都重新
读取共享值。新槽也不能在三个 writer 或持久化调用之前发布。

## 正常前提与未决保存系统

原函数不检查旧槽或 `newSlot` 的范围，不判断三个 writer 或持久化是否成功，也
没有失败回滚。新端保持这一正常前提，不增加索引保护、返回值门禁或错误处理。

`Sub_533570`、`Sub_5335B0`、`Sub_533640` 及无参数、结果忽略的副作用入口
`Sub_533730` 目前只在私有 `LoginInputDependencies.hpp` 中保留 `void` 窄声明；
IDA 自动推导的 `FILE*` 不构成稳定接口。selected 到记录的映射、记录存储、三个
writer 和文件持久化均未实现；本项不定义这些依赖、不伪造成功返回，也不建立
第二份持久状态。
