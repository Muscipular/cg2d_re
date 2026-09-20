# cg2d 保存记录标量设置恢复证据

## 函数边界与调用关系

- 原函数：`sub_5205E0@[0x5205E0,0x52063B)`，共 91 字节、18 条指令、
  1 个基本块，无 callee。
- 两个 caller/callsite 分别是 `sub_464840@0x464BDC` 与
  `sub_5201B0@0x5201C8`。
- 两处原 caller 都传入当前记录 `0xFF4918`，但函数参数可指向任意同布局记录；
  私有 helper 不硬编码当前记录 owner，也不接 caller。

## 六项顺序、宽度与 owner

函数没有空指针或长度门禁，依次只读取一次指定 W8，并立即零扩展写入目标：

| 顺序 | 记录偏移 | 原目标 | 新端唯一 owner/宽度 | 静态初值 | exact-address direct xrefs |
|---:|---:|---:|---|---:|---:|
| 1 | `+0x0B0` | `0xD67AE4` | `platform::gValueD67AE4` / W32 | 0 | 15 / 9 个函数 |
| 2 | `+0x0B1` | `0x73ECBC` | `platform::gValue73ECBC` / W32 | 0 | 11 / 5 个函数 |
| 3 | `+0x312` | `0xD67AE8` | `platform::gValueD67AE8` / W32 | 0 | 14 / 10 个函数 |
| 4 | `+0x314` | `0x5F974C` | `platform::gValue5F974C` / W16 | 0 | 7 / 5 个函数 |
| 5 | `+0x31A` | `0x554B74` | `platform::gScanlineSyncMode` / W16 | 1 | 13 / 9 个函数 |
| 6 | `+0x31B` | `0xD68858` | `platform::gValueD68858` / W32 | 0 | 6 / 4 个函数 |

表中 xrefs 栏依次为直接引用数和涉及函数数。`0x554B74` 有 raw backing，原 W16
初值为 1；其余五项均为无 raw backing 的 virtual-only 区，运行时由 PE loader
零填充。`gValueD67AE4` 和 `gScanlineSyncMode` 复用既有 owner；本项只在
`Startup.hpp/StartupState.cpp` 建立其余四项，不创建副本。

每次读取后立即完成对应定宽写入，不预先读取后续字节，不布尔化或钳制数值，也不
重排字段。六个目标都接收源 W8 的零扩展值，其中四项落入 W32、两项落入 W16；
函数最终原样返回传入的 `source` 指针。

## 异常与消费边界

源指针为空、记录过短或中途地址无效时没有保护；异常发生前已执行的目标写入不会
回滚。由于没有预快照，源区若与 live owner 重叠，前序写入对后续读取的影响也按原
顺序保留。

本函数只恢复六个存储值，不附带 `ShowCursor`、图形、音频设备或扫描线同步等消费
行为。它不修改当前记录 owner，不修改 CMake，也不接两个 caller。行为证据只取自
当前 `ida-17524 / cg2d.exe`；STONAGE 不参与结论。
