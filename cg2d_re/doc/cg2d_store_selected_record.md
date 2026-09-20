# cg2d 当前记录写回选中物理槽证据

## 函数边界与调用关系

- 原函数：`sub_533700@[0x533700,0x533725)`，共 37 字节、13 条指令、
  1 个基本块，无 callee。
- 唯一 caller/callsite 是 `sub_520200@0x5202CD`。
- 源码只在 `LoginSavedState.cpp` 匿名命名空间定义私有
  `[[maybe_unused]] StoreCurrentRecordToSelectedPhysical_533700()`；本项不接
  caller，也不增加公开声明。

## 单次 selected 与复制方向

入口 `0x533700` 只读取一次原 `0x105C394`，源码对应一次读取
`gSavedState.selected` 到局部快照。后续目标寻址和返回值都复用该快照；函数不在
复制前后重读 selected。

正常前提下复制关系为：

```text
gCurrentSavedRecord  --0x510 bytes-->  gSavedState.records[selected]
原 0xFF4918                            原 0x1059098 + selected*0x510
```

`0x533707` 把 DWORD 计数设为 `0x144`，`0x533720` 执行 `rep movsd`，因此复制
长度严格为 `0x144*4=0x510`。写入覆盖完整 `SavedPhysicalRecord`；函数不会另外
修改 `active[selected]`，也不会只挑选登录输入子槽。

## 返回值与映射不对称

原 `0x53370C` 以 `lea eax,[eax+eax*8]` 物化 `selected*9`，该低 32 位 bit
pattern 最终从 EAX 返回。源码以 `uint32_t` 乘法和 `bit_cast<int32_t>` 表达，避免
宿主 C++ 有符号乘法溢出。

本函数把 selected 直接当作物理数组下标，完全不读取 `logicalToPhysical`。这与
若干按逻辑槽经映射访问记录的 reader/writer 不对称；该不对称是原二进制事实，
不能在此函数中擅自补映射、槽一致性检查或 active 更新。

唯一 caller `sub_520200` 在 `0x520255` 读取 selected，`0x520264` 取
`logicalToPhysical[selected]`，再于 `0x520270/0x520273` 以映射后的物理号选择
`paths[...]` 构造磁盘路径；但它随后在 `0x5202CD` 调用 `sub_533700` 时，后者却
直接把当前记录写入 `records[selected]`。因此原端的精确不对称是：磁盘路径走
map，内存回写不走 map。

## DF 与异常地址

原函数在 `rep movsd` 前没有 `cld`，依赖正常 x86 调用约定下 DF 已清零并按地址
递增复制。源码使用 `memcpy` 表达正常域内的完整非重叠复制，不模拟 DF 异常状态。

正常调用前提为 `selected∈[0,9]`，本函数不检查或钳制。精确异常示例：

- `selected=-1` 时目标起点为 `0x1058B88`，即物理记录数组前方最后 `0x510`
  字节，位于路径区；
- `selected=10` 时目标起点为 `0x105C338`，恰为 active 数组基址。

原端会从这些异常地址开始覆盖完整 `0x510` 字节；新端不尝试以 C++ 安全代码模拟
越界写入，该域保持未决。返回乘法仍显式保留低 32 位回绕。

本项不实现或接线 `sub_520200`，不修改头文件、CMake 或其他保存函数。行为证据
只取自当前 `ida-17524 / cg2d.exe`；STONAGE 不参与结论。
