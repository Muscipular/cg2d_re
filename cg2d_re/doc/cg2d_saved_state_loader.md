# cg2d 保存记录载入证据

## 函数边界与调用关系

- 原函数：`sub_532ED0@[0x532ED0,0x532FB9)`，共 233 字节、80 条指令、
  7 个基本块。
- 唯一 caller 是尚未实现的 `sub_533470@0x53347A`。
- 函数共有 6 个 callsite、5 个唯一目标：
  `sprintf@0x532F01`、`fopen@0x532F10`、`fread@0x532F38`、
  两处 `fclose@0x532F4A/0x532F5E`，以及
  `sub_52D460@0x532F83`。
- 源码仅在 `LoginSavedState.cpp` 匿名命名空间定义私有
  `[[maybe_unused]] LoadSavedPhysicalRecord_532ED0(std::int32_t)`；本项不接
  caller。

## 共享 W8 状态

原 `0xFF4E28` 是一个 W8、PE virtual-zero 初值的共享状态。本项在
`cg::platform` 建立唯一 `std::uint8_t gValueFF4E28 = 0`：loader 每次入口第一步
均将其清零，未来 `InitializeClient` 路径读取它。除此之外不推定更具体语义，
也不在保存状态模块复制该字节。

## 路径、文件与解码顺序

函数保留未初始化的局部 `path[0x400]`、`encoded[0xA20]` 与
`decodedBytes`，严格执行：

1. 入口先清 `gValueFF4E28`。
2. 使用 `sprintf(path, "%s\\%s", gSavedState.paths[physicalSlot].text,
   "save.dat")` 形成路径。
3. 以 `rb+` 打开；失败直接返回 0。
4. 以元素大小 1、最大数量 `0xA20` 读取到 `encoded`。
5. 原程序读取旧 CRT `FILE+0x0C` 的 `0x20` 错误位。当前 x64 CRT 不暴露同一
   布局，因此以 `std::ferror(file)!=0` 适配其“读取错误”语义。错误路径先关闭
   文件再返回 0。
6. 无读取错误时也先关闭文件，然后才以 `mode=0` 调
   `DecodeLegacyBytes_52D460`；返回值忽略，目标直接指向正式的
   `gSavedState.records[physicalSlot]`。
7. `decodedBytes!=0x510` 返回 0；长度匹配后，仅当记录首 W32 为 `0x7002`
   返回 1，否则返回 0。

`fclose` 的返回值不参与判断，关闭一定发生在解码之前；不使用临时记录，也不回滚
解码已经写入的正式记录。

## 失败副作用与正常域

- 所有可返回路径都已经清零 `gValueFF4E28`，并在打开文件前完成路径格式化。
- 打开失败：返回 0，记录未被本函数改写。
- `fread` 报错：关闭文件后返回 0，尚未解码，记录未被本函数改写。
- 解码长度错误：文件已经关闭，正式记录可能已经部分或越界改写；直接返回 0，
  不回滚。
- magic 错误：文件已经关闭且正式记录已经解码写入；返回 0，不回滚。
- 正常成功域要求物理槽有效、路径字符串以 NUL 终止且适合 `0x400` 缓冲区、
  文件读取无错误、实际读取 `0x511` 字节，并由 mode 0 解码得到 `0x510` 字节且
  首 W32 为 `0x7002`。

实现不检查 `physicalSlot`、路径长度、`bytesRead` 是否为 `0x511`、目标容量或
codec 的除法/长度异常。零字节、1 字节、短文件和超过正常编码长度的文件均直接
进入既有 codec；其读取、异常及对相邻保存状态的写入风险保持原调用边界，不新增
保护。本项不实现 `sub_533470`、reset 或其他载入流程，不修改 CMake。行为证据
只取自当前 `ida-17524 / cg2d.exe`，STONAGE 不参与结论。
