# cg2d 保存状态批量持久化证据

## 函数边界与调用关系

- 原函数：`sub_533730@[0x533730,0x533854)`，共 292 字节、88 条指令、
  11 个基本块。
- 两个 caller/callsite 分别是 `sub_464840@0x464B29` 与
  `sub_465740@0x465775`；两处都忽略返回寄存器，因此源码沿用既有窄契约
  `void Sub_533730()`，不采用 IDA 自动推导的 `FILE*` 返回类型。
- 二进制共有 7 个调用点、6 个唯一目标：编译器栈探测
  `__alloca_probe@0x533735`，以及
  `EncodeLegacyBytes_52D310@0x5337B0`、`sprintf@0x5337CB`、
  `fopen@0x5337DD`、`fwrite@0x5337FB`、成功路径
  `fclose@0x53380C` 和短写路径 `fclose@0x533841`。
- 源码在既有 `cg::login::detail` 中定义已经声明的实体；本项不修改头文件、
  caller 或 CMake。

## 固定十槽筛选

函数固定按 `physicalSlot=0..9` 递增扫描物理槽，不读取当前 selected，也不经
`logicalToPhysical` 映射。每槽严格短路执行以下三个条件：

1. `gSavedState.active[physicalSlot]` 必须严格等于 1；
2. 对应物理记录原始字节 `+0x6A` 必须非零；
3. 同一记录原始字节 `+0x37` 必须非零。

任一条件失败只跳到下一物理槽；后一个字段在前一个条件失败时不会读取。这里不把
其他非零 active 值视为有效，也不增加槽位、字符串或映射检查。

## 局部副本、清段与编码顺序

函数局部持有一个 `SavedPhysicalRecord`（`0x510`）、`encoded[0xA20]`、
`encodedBytes` 和 `path[0x400]`。后三者均不预清、不预置。槽命中后严格执行：

1. 完整复制该槽的 `0x510` 字节记录到局部记录；
2. 清零局部 `[+0x37,+0x6A)`，长度严格为 `0x33`；
3. 再冗余写一次 `local[0x37]=0`；
4. 调用
   `EncodeLegacyBytes_52D310(local,0x510,0,encoded,&encodedBytes,0xA20)`；
5. 编码完成后才执行
   `sprintf(path,"%s\\%s",paths[physicalSlot].text,"save.dat")`；
6. 最后才以 `fopen(path,"wb+")` 打开目标文件。

编码器会原地修改局部记录，但全局 `gSavedState.records[physicalSlot]` 始终只读；
清除的第二段文本与编码变换都不会写回全局记录。路径格式化和文件打开不得提前到
编码之前。

## 写入、退出和副作用

打开失败时立即从整个函数返回，后续物理槽不再处理。打开成功后执行
`fwrite(encoded,1,encodedBytes,file)`；调用返回后必须再次读取
`encodedBytes`，再按 x86 无符号关系判断是否短写：

- 写入量小于该尾读长度：先调用 `fclose`，再立即返回；
- 写入量足够：调用 `fclose` 并忽略关闭结果，然后继续扫描下一物理槽；
- 十槽扫描完毕：直接 void 返回。

函数不创建目录，不检查空路径或编码长度，不查询 `ferror`，不重试、回滚或报告
关闭失败。短写路径会关闭已打开文件；打开失败路径没有文件可关闭。除 `"wb+"`
造成的创建、截断和写入外，本函数不修改全局保存状态。

行为证据只取自当前 `ida-17524 / cg2d.exe`；STONAGE 不参与结论。
