# cg2d 保存记录单字节状态恢复证据

## 函数边界与调用关系

- 原函数：`sub_520700@[0x520700,0x520712)`，共 18 字节、5 条指令、
  1 个基本块，无 callee。
- 两个 caller/callsite 分别是 `sub_464840@0x464BF0` 与
  `sub_5201B0@0x5201DC`，两处均传入当前记录 `0xFF4918` 并忽略返回值。
- `0xFF4918+0x313=0xFF4C2B`，该源字节落在既有唯一
  `gCurrentSavedRecord@[0xFF4918,0xFF4E28)` owner 内；helper 仍保留通用记录指针
  参数，不硬编码当前记录地址。
- 源码只在 `LoginSavedState.cpp` 匿名命名空间定义私有
  `[[maybe_unused]] RestoreSavedByteState_520700(...)`；本项不接 caller。

## 读取、写入与返回

函数不检查空指针、记录长度或字段范围，只从 `source+0x313` 读取一次 W8，
将其零扩展为 W32 后立即写入原 `0xD67CD0` 对应的唯一 live owner
`cg::platform::gValueD67CD0`，并返回同一个写入值，返回范围为 `0..255`。

新端在 `Startup.hpp/StartupState.cpp` 建立中性命名、零初始化的唯一 W32 owner，
不为该地址推定业务语义。逆向函数 `sub_5206F0` 把此 owner 的低字节保存到记录
`+0x313`，与本恢复方向相反。

原 `0xD67CD0` 共有 10 个 exact-address direct xrefs，其中 6 个读取、4 个写入；
`0xD67CD1..0xD67CD3` 没有额外 direct xref。该地址在 PE 中
`fileoff=-1`、`is_loaded=false`，没有 raw backing，运行时初值因此来自 loader 对
virtual-only 区域的零填充；新端 owner 对应初始化为 0。

## 异常与实现边界

函数不布尔化或钳制源字节，也没有其他写入或副作用。源指针为空、记录过短或
`source+0x313` 无效时没有保护；读取失败时不会写入 owner，读取成功后只执行一次
W32 写入，不存在临时批量提交或回滚。

本项不修改当前记录 owner、CMake 或两个 caller。行为证据只取自当前
`ida-17524 / cg2d.exe`；STONAGE 仅可作为架构参考，不参与本项行为、地址、语义
或验收结论。
