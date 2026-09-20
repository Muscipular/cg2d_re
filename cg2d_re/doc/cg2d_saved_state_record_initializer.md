# cg2d 物理保存记录初始化证据

## 函数边界

- 原函数：`sub_5331A0@[0x5331A0,0x5332F6)`，共 `0x156`（342）字节、
  97 条指令、5 个基本块，无真实 callee。
- 唯一 caller 是尚未实现的 `sub_533470@0x5334F2`。
- 源码只在 `LoginSavedState.cpp` 匿名命名空间增加私有
  `[[maybe_unused]] InitializeSavedPhysicalRecord_5331A0(std::int32_t)`，不公开
  声明、不接 caller。

## 状态来源

函数直接复用 `cg::platform` 的唯一 live state：`gUseStereo`、
`gEffectVolume`、`gMusicVolume`、16 字节
`gBgmFrequencySelectionPrefix` 和 `gScanlineSyncMode`。本项不在保存状态模块
复制这些运行期状态；记录只是初始化时的快照目标。

## 完整读写顺序

取得 `gSavedState.records[physicalSlot]` 后，顺序如下：

1. 把完整物理记录 `0x510` 字节清零。
2. 向记录 `+0x00` 写 DWORD `0x7002`。
3. 依次把恒空字符串的 NUL 写到 `+0x6A`、`+0x37`。
4. 读取 effect volume 的低字节，再把恒空字符串 NUL 写到 `+0x04`。这保留了
   `0x53323A` 在最后一次空串尾复制完成前预读 effect 的原顺序。
5. 读取 stereo 低字节，写入 `+0x9D`。
6. 读取 music volume 低字节，随后把先前读取的 effect 写入 `+0x9E`。
7. 读取 BGM DWORD0，随后把 music 写入 `+0x9F`。
8. 读取 BGM DWORD1，再把 DWORD0 写到 `+0xA0`。
9. 读取 BGM DWORD2，再把 DWORD1 写到 `+0xA4`。
10. 读取 BGM DWORD3，再把 DWORD2 写到 `+0xA8`。
11. 读取 scanline W16 的低字节，再把 DWORD3 写到 `+0xAC`。
12. 显式写 `+0xB0=0`、`+0xB1=0`，再把 scanline 写到 `+0x31A`。

四个 BGM DWORD 分别以四次独立 4 字节 `memcpy` 读入临时值，写回 DWORD 也
使用 `memcpy`，避免对字节存储作别名或未对齐强转；没有合并成一次 16 字节
复制。上述显式零写即使位于整记录 `memset` 之后仍全部保留。

## 离散循环与返回值

随后执行两个离散阶段：

- 两轮循环中，每轮严格依次写
  `record[0x30C+i]=8`、`record[0x30E+i]=5`、`record[0x310+i]=1`。
- `char*` 游标从 `record+0xB2` 开始，共 24 轮；每轮先写零，再增加 `0x19`。
  最后一次增加后游标恰为 `record+0x30A`。

退出第二段循环后仍显式写 `record[0x312]`、`record[0x313]`、
`record[0x314]` 为零，并返回保持在 `record+0x30A` 的游标。

## 当前边界

正常调用前提是 `physicalSlot` 为有效物理槽 `0..9`。原函数没有门禁，本实现也
不增加 clamp 或拒绝；非法槽属于未决异常域。本项不实现 caller、loader、map、
reset 或 codec，不修改 CMake、公开接口和其他保存流程。证据只来自当前
`ida-17524 / cg2d.exe`，STONAGE 不参与结论。
