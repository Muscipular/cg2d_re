# cg2d 默认保存文件写出证据

## 函数边界与调用关系

- 原函数：`sub_532FC0@[0x532FC0,0x53319A)`，范围 `0x1DA`（474）字节、
  133 条指令、9 个基本块。
- 唯一 caller 是 `sub_533300@0x53331A`。
- 四个真实调用点依次为：
  `EncodeLegacyBytes_52D310@0x53312F`、`fopen@0x533141`、
  `fwrite@0x533168`、`fclose@0x533184`。
- 源码只在 `LoginSavedState.cpp` 匿名命名空间定义私有
  `[[maybe_unused]] WriteDefaultSavedFile_532FC0(const char*)`；本项不接 caller，
  不增加公开声明。

## 局部记录与写入顺序

函数使用独立的局部 `SavedPhysicalRecord raw`，大小严格为 `0x510`。入口先对整个
对象执行一次清零，随后仍按原指令保留以下交错读取与显式写入：

1. `raw+0x000` 写 W32 魔数 `0x7002`；
2. `raw+0x06A`、`raw+0x037` 依次写恒空字符串的 NUL；
3. 读取 `gEffectVolume` 低字节，再读取 `gMusicVolume` 低字节，随后写
   `raw+0x004=0`；
4. 读取 `gUseStereo` 低字节，再把先前读取的 effect 写到 `raw+0x09E`；
5. 独立读取 `gBgmFrequencySelectionPrefix+4` 的 DWORD，再把 stereo 写到
   `raw+0x09D`；
6. 独立读取 BGM 前缀 `+0` DWORD，再把先前读取的 music 写到 `raw+0x09F`；
7. 独立读取 BGM 前缀 `+8` DWORD，再把 BGM `+0` 写到 `raw+0x0A0`；
8. 独立读取 BGM 前缀 `+12` DWORD，再把 BGM `+4` 写到 `raw+0x0A4`；
9. 读取 `gScanlineSyncMode` 低字节，再依次把 BGM `+8`、`+12` 写到
   `raw+0x0A8`、`raw+0x0AC`；四次 DWORD 读取和写入都使用 `memcpy`，不以
   未对齐或别名不安全的整型指针访问字节区；
10. `raw+0x0B0`、`raw+0x0B1` 写零，再把先前读取的 scanline 写到
    `raw+0x31A`；
11. 两轮按每轮 `8→5→1` 的顺序写
   `raw[0x30C+i]`、`raw[0x30E+i]`、`raw[0x310+i]`；
12. 从 `raw+0x0B2` 开始执行 24 轮离散零写，每轮写零后前进 `0x19`，
   退出指针恰为 `raw+0x30A`；
13. 最后依次把 `raw+0x312`、`raw+0x313`、`raw+0x314` 写零。

其中立体声、音效、音乐、BGM 前缀和扫描线值直接读取现有 `cg::platform` live
state。局部记录既不引用也不修改 `gSavedState.records`；因此源码没有调用会写全局物理槽的
`InitializeSavedPhysicalRecord_5331A0`，也没有把两段初始化逻辑抽成共享 helper。

## 编码与文件时序

局部 `encoded[0xA20]` 和 `encodedBytes` 均不预清、不预置。完成 raw 写入后，函数
先调用：

```text
EncodeLegacyBytes_52D310(raw, 0x510, 0, encoded,
                         &encodedBytes, 0xA20)
```

编码器返回类型按已确认调用约束为 `void`；此处没有返回值可检查。正常参数产生
`0x511` 字节编码流，并会按编码算法原地改写局部 raw。只有编码全部完成后，才执行
`fopen(fileName, "wb+")`，不能把打开文件提前到编码之前。

打开失败直接返回 0。打开成功后执行
`fwrite(encoded, 1, encodedBytes, file)`，并按 x86 无符号关系判断返回的元素数是否
达到再次读取的 `encodedBytes`：

- 短写时直接返回 0，故意不调用 `fclose`，保留原函数的流句柄泄漏；
- 足量时调用 `fclose`，忽略关闭结果并返回 1。

`"wb+"` 会创建或截断目标文件并以二进制读写方式打开。本函数不检查空文件名，
不创建父目录，不对短写或关闭失败重试，不回滚已经截断或部分写入的文件，也不更新
任何全局保存状态。

本项不接 `sub_533300`，不修改公开头文件或 CMake。行为证据只取自当前
`ida-17524 / cg2d.exe`；STONAGE 不参与结论。
