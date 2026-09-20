# cg2d 保存记录音频设置恢复证据

## 函数边界与调用关系

- 原函数：`sub_5204C0@[0x5204C0,0x520502)`，共 66 字节、17 条指令、
  3 个基本块，无 callee。
- 两个 caller/callsite 分别是 `sub_464840@0x464BD2` 与
  `sub_5201B0@0x5201BE`。
- 两处原 caller 都传入当前记录 `0xFF4918`，但函数参数本身可指向任意同布局记录；
  源码因此接受 `const std::uint8_t*`，不硬编码 `gCurrentSavedRecord`。
- 源码只在 `LoginSavedState.cpp` 匿名命名空间定义私有
  `[[maybe_unused]] RestoreAudioSettings_5204C0(...)`；本项不接 caller。

## 精确读取与写入顺序

函数不检查空指针或记录长度，严格按以下顺序执行：

1. W8 读取 `currentBytes+0x9D`，立即零扩展写入 W32
   `cg::platform::gUseStereo`；
2. W8 读取 `currentBytes+0x9E`，立即零扩展写入 W32
   `cg::platform::gEffectVolume`；
3. W8 读取 `currentBytes+0x9F`，立即零扩展写入 W32
   `cg::platform::gMusicVolume`；
4. 令 `index=0`，逐轮直到 15：每轮只读取一次
   `currentBytes[0xA0+index]`，立即写入
   `gBgmFrequencySelectionPrefix[index]`，再递增 index；
5. 循环结束时 index 为 16，函数返回 16。

16 字节 BGM 前缀必须逐字节读取并立即写入，不能改成 `memcpy`、一次性预快照或
整块类型转换。该顺序会保留源指针与 live 状态发生重叠时的逐轮反馈。

## 异常与部分写入

函数不校验输入指针、容量或字段值，也不对音量、立体声或 BGM 字节钳制。若源记录
在中途地址无效并触发访问异常，异常前已经完成的 W32 字段和 BGM 前缀字节保持写入，
不存在临时对象或回滚；因此不能把实现改成“全部读成功后一次提交”。

本项只复用现有 platform live state，不改变当前记录 owner，不修改头文件、CMake
或两个 caller。行为证据只取自当前 `ida-17524 / cg2d.exe`；STONAGE 不参与结论。
