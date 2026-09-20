# cg2d 当前保存记录持久化证据

## 函数边界、调用与引用

- 原函数：`sub_520200@[0x520200,0x5202DE)`，共 `0xDE`（222）字节、
  68 条指令、5 个基本块。
- 函数内共 7 个调用点：`__alloca_probe@0x520205`、
  `EncodeLegacyBytes_52D310@0x520250`、`sprintf@0x520280`、
  `fopen@0x520292`、`fwrite@0x5202B2`、`fclose@0x5202C5` 与
  `sub_533700@0x5202CD`。
- 原入口共有 30 个 direct xref，其中 26 个是 call、4 个是 tail jump。
- 源码在 `LoginSavedState.cpp` 匿名命名空间只保留一个
  `PersistCurrentSavedRecord_520200()` 行为核心；既有
  `cg::login::detail::Sub_520200()` 与
  `cg::character_select::detail::Sub_520200()` 都只返回该核心结果，不复制状态机。

## 局部快照、清段与编码

核心局部持有 `SavedPhysicalRecord` 快照、`path[0x400]`、
`encoded[0xA20]` 与 `encodedBytes`，四者均不预清、不预置。严格时序为：

1. 完整复制唯一 `gCurrentSavedRecord` 的 `0x510` 字节到局部快照；
2. 清零局部 `[+0x37,+0x6A)`，长度严格为 `0x33`，不追加冗余单字节写；
3. 调用
   `EncodeLegacyBytes_52D310(local,0x510,0,encoded,&encodedBytes,0xA20)`。

编码器会原地修改局部快照，唯一当前记录不受影响。此阶段不读取 selected、map 或
路径状态。

## selected、map 与磁盘路径

编码完整返回后，函数只读取一次 `gSavedState.selected`，再只读取一次
`gSavedState.logicalToPhysical[selected]`。映射结果作为物理槽下标，通过：

```text
sprintf(path, "%s\\%s", paths[physicalSlot].text, "save.dat")
```

构造路径，随后才调用 `fopen(path,"wb+")`。函数不缓存或重读 selected/map，
不检查逻辑槽、物理槽、路径长度、目录存在性、`sprintf` 或编码结果。

文件处理完成后无条件调用既有
`StoreCurrentRecordToSelectedPhysical_533700()`。该函数会独立重读 selected，并把
当前记录直接写入 `records[selected]`，完全不走 map。因此原端明确保持：磁盘路径
走 map，内存回写不走 map；两阶段甚至可以在外部状态变化时采样到不同 selected。

## 空 FILE、短写与返回值

`fopen` 返回后，结果变量先按 `file!=nullptr` 初始化为 1 或 0。但控制流不以此
短路：即使 `file==nullptr`，仍无条件执行
`fwrite(encoded,1,encodedBytes,file)`。目标 x86 CRT 对空 `FILE*` 的实际路径会在
库内解引用并崩溃；在新端这是同样不受保护的 C++/CRT 异常域，不能补安全返回。

`fwrite` 返回后再次读取 `encodedBytes`，按 W32 无符号关系比较写入量：短写只把
结果置 0，不提前返回。随后无条件执行 `fclose(file)`，忽略关闭结果；再无条件调用
`StoreCurrentRecordToSelectedPhysical_533700()`。因此：

- 打开成功且足量写入：关闭、内存回写后返回 1；
- 打开成功但短写：仍关闭、仍内存回写，最后返回 0；
- 打开失败：结果虽先置 0，但通常在空 `FILE*` 的 `fwrite` 内崩溃，不能把它解释为
  稳定的失败返回路径；若异常环境让控制继续，后续空 `fclose` 和内存回写仍在原
  控制流上。

函数不查询 `ferror`，不检查 `fclose` 返回，不创建目录，不重试、回滚或增加门禁。
两个 namespace 包装均不持有状态，也不改变现有 caller 表达式。本项不修改公共
头文件或 CMake。行为证据只取自当前 `ida-17524 / cg2d.exe`；STONAGE 不参与结论。
