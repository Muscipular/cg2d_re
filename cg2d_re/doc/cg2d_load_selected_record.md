# cg2d 载入当前选中物理记录证据

## 函数边界与调用关系

- 原函数：`sub_5336D0@[0x5336D0,0x5336F5)`，共 37 字节、13 条指令、
  1 个基本块，无 callee。
- 唯一 caller/callsite 是 `sub_464840@0x464BC8`。
- 源码只在 `LoginSavedState.cpp` 匿名命名空间定义私有
  `[[maybe_unused]] LoadSelectedPhysicalRecord_5336D0()`；本项不接 caller，
  不增加公开声明。

## 单次 selected 与复制方向

入口 `0x5336D0` 只读取一次原 `0x105C394`，源码对应一次读取
`gSavedState.selected` 到局部快照。后续寻址和返回值都复用该快照；函数不读取
`logicalToPhysical`，也不在复制前后重读 selected。

正常前提下 `selected` 直接作为物理下标，复制关系为：

```text
gSavedState.records[selected]  --0x510 bytes-->  gCurrentSavedRecord
原 0x1059098 + selected*0x510                    原 0xFF4918
```

`0x5336D7` 把 DWORD 计数设为 `0x144`，`0x5336F0` 执行 `rep movsd`，因此复制
长度严格为 `0x144*4=0x510`，覆盖完整 `SavedPhysicalRecord`，不是只复制两个
登录输入子槽。方向是物理保存记录到唯一当前记录；源记录不写，当前记录被完整覆盖。

## 返回值与 32 位算术

原 `0x5336DC` 以 `lea eax,[eax+eax*8]` 物化 `selected*9`，该值随后保留在 EAX
作为返回值；用于源记录寻址的后续缩放不改变这个返回契约。源码以 `uint32_t`
乘法保留低 32 位 bit pattern，再用 `bit_cast<int32_t>` 返回，避免宿主 C++
有符号乘法溢出。

## DF 旁证与异常域

原函数在 `rep movsd` 前没有执行 `cld`，因此它依赖正常 x86 调用约定下方向标志
DF 已清零，并按地址递增完成复制。这是复制方向的旁证，而不是要求新端暴露或模拟
EFLAGS；`memcpy` 表达同一正常域内的完整非重叠复制。

正常调用前提为 `selected∈[0,9]`。原函数不钳制、不拒绝也不检查该下标；源码同样
不增加保护。越界 selected 会使原端寻址到保存数组外，新端不尝试用 C++ 安全代码
模拟该异常内存访问；该域保持未决。精确示例：`selected=-1` 时源起点为
`0x1058B88`，即物理记录数组前方最后 `0x510` 字节，位于路径区；
`selected=10` 时源起点为 `0x105C338`，恰为 active 数组基址。返回乘法本身仍
显式保留低 32 位回绕。

本项不实现或接线 `sub_464840`，不修改头文件、CMake 或其他保存函数。行为证据
只取自当前 `ida-17524 / cg2d.exe`；STONAGE 不参与结论。
