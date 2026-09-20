# cg2d 保存状态初始化入口证据

## 函数边界与调用关系

- 原函数：`sub_533300@[0x533300,0x533358)`，共 88 字节、28 条指令、
  7 个基本块。
- 两个 caller/callsite 分别是 `sub_464840@0x464A95`，以及
  `_WinMain@16` 内的 `0x4659C6`。新端现有 `ClientEntry` 继续直接调用
  `cg::platform::InitializeSavedState_533300()`；本项不接尚未实现的
  `sub_464840`。
- 源码定义既有公开声明，不新增入口、源文件或 CMake 项。

## 调用与分支时序

函数首先无条件调用 `ResetSavedState_533360()` 并忽略其返回值，然后只调用一次
`ProbeSavedStateLocation_5333D0()`。探测结果严格分为：

1. 返回 0：直接调用 `RebuildSavedSlotMap_533470()`；
2. 返回 1：先调用 `MigrateLegacySavedFiles_533530()`，忽略返回，再调用
   `RebuildSavedSlotMap_533470()`；
3. 返回 2：先调用
   `WriteDefaultSavedFile_532FC0("data\\0\\save.dat")`，忽略返回，再调用
   `RebuildSavedSlotMap_533470()`；
4. 其他返回值：跳过迁移、默认文件写出和 map 重建，直接进入共同尾部。

三条正常分支中的 `RebuildSavedSlotMap_533470()` 返回值全部忽略。函数不根据 reset、
迁移、默认文件写出或重建结果增加失败分支，也不重试、回滚或恢复额外 live 设置。

## 共同尾部与 map[0]

分支汇合后只读取一次 `gSavedState.logicalToPhysical[0]`，把所得值直接作为物理槽
下标，完整复制：

```text
gSavedState.records[logicalToPhysical[0]]  --0x510 bytes-->
    gCurrentSavedRecord
```

复制覆盖唯一当前记录的全部 `SavedPhysicalRecord`，不只覆盖登录输入子槽。此尾部
不读取 selected，不检查 map 值、active 状态或记录 magic，最后固定返回 1。

`ResetSavedState_533360()` 会先把全部 map 项写成 -1。若 probe 异常返回 0/1/2
以外的值，default 路径会跳过 rebuild，尾部因而读到 `map[0]=-1`，并尝试从
`records[-1]` 对应的 `0x1058B88` 起复制 `0x510` 字节；该源区位于 paths 区。
原函数没有门禁，新端也不尝试以 C++ 安全代码模拟或修复这一越界异常域。

本项保持 `ClientEntry` 的既有调用表达式，不修改公开声明、CMake 或其他函数。
行为证据只取自当前 `ida-17524 / cg2d.exe`；STONAGE 不参与结论。
