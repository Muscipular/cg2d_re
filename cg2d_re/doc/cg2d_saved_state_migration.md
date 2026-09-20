# cg2d 旧保存文件迁移证据

## 函数边界与调用关系

- 原函数：`sub_533530@[0x533530,0x533563)`，共 51 字节、16 条指令、
  1 个基本块。
- 唯一 caller 是 `sub_533300@0x533324`。
- `0x533531` 从 IAT `0x54D110` 读取 `CopyFileA` 地址到寄存器，后三次均为寄存器
  间接调用，因此 IDA 的直接 callee 列表可以为空。
- 三个 callsite 依次位于 `0x533543`、`0x533551`、`0x53355F`。
- 源码只在 `LoginSavedState.cpp` 匿名命名空间增加私有
  `[[maybe_unused]] MigrateLegacySavedFiles_533530()`；本项不接 caller。

## 三次复制顺序

函数严格直线执行以下 `CopyFileA` 调用，第三参数均为 `FALSE`：

1. `data\\save.dat` → `data\\0\\save.dat`；
2. `data\\mail.dat` → `data\\0\\mail.dat`；
3. `data\\guildmail.dat` → `data\\0\\guildmail.dat`。

路径全部硬编码，不读取 `gSavedState.paths`。前两次 BOOL 结果完全忽略：无论成功
还是失败都继续下一次复制，不存在短路。函数把第三次 `CopyFileA` 的 BOOL 按
`std::int32_t` 原值返回，不把非零值另行归一成固定 1。

## 覆盖与错误语义

`bFailIfExists=FALSE` 表示目标已存在时允许覆盖；实际复制是否成功仍由 Win32 文件
权限、源文件存在性、目标目录存在性等条件决定。本函数不创建 `data\\0`，不删除、
移动或改名源文件，也不重试。

返回值只反映第三份 `guildmail.dat` 的复制结果：第三次非零即返回该非零 BOOL，
第三次为零则返回 0。第一、第二次的成败不会改变返回值；函数也不调用
`GetLastError`，因此不保存或解释任何失败原因。

除 `CopyFileA` 自身的文件系统副作用外，本函数不更新任何全局保存状态。本项不接
`sub_533300`，不修改公开声明、CMake、进度或其他迁移流程。行为证据只取自当前
`ida-17524 / cg2d.exe`，STONAGE 不参与结论。
