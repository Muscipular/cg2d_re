# cg2d 保存槽映射重建证据

## 函数边界与调用关系

- 原函数：`sub_533470@[0x533470,0x533528)`，共 184 字节、50 条指令、
  9 个基本块。
- 唯一 caller 是 `sub_533300@0x533329`。
- 三个 callee 分别是保存记录 loader `sub_532ED0@0x53347A`、空闲物理槽查找
  `sub_5338F0@0x5334CD` 和记录初始化 `sub_5331A0@0x5334F2`。
- 源码只在 `LoginSavedState.cpp` 匿名命名空间增加私有
  `[[maybe_unused]] RebuildSavedSlotMap_533470()`；本项不接 caller。

## 十槽扫描

`physicalSlot` 从 0 到 9 严格执行十轮，每轮先调用
`LoadSavedPhysicalRecord_532ED0(physicalSlot)`：

- loader 返回非零时，先第一次独立读取 `mappingCount`，以该值为下标写
  `logicalToPhysical[count]=physicalSlot`；再写 `active[physicalSlot]=1`。
  随后第二次独立读取 `mappingCount`，再读取一次 `visibleCount`，分别加一，
  先物化 `nextMappingCount`、`nextVisibleCount`；两次加法都完成后，才严格按
  `mappingCount`、`visibleCount` 的顺序写回。
- loader 返回零时只写 `active[physicalSlot]=0`；不清记录、不清映射项，也不修改
  两个计数。

两次 `mappingCount` 读取没有合并：第一次只服务于映射下标，第二次发生在映射项
和 active 写入之后，服务于计数递增。

## 扫描后的新槽分配

扫描完成后重新以有符号数读取 `mappingCount`：

1. `mappingCount>10` 时跳过整个分配阶段。
2. 其余情况，包括 `mappingCount==10`，均调用
   `FindFreeSavedPhysicalSlot_5338F0()`。
3. 返回 `-1` 时不写任何新槽状态，直接进入末尾返回判定。
4. 返回有效物理槽时，第一次读取 `mappingCount` 作为映射下标并写入 free 槽，
   再写 `active[free]=1`。
5. 调用 `InitializeSavedPhysicalRecord_5331A0(free)` 并忽略其返回指针。
6. 初始化完整返回后，重新独立读取 `mappingCount`、`visibleCount`；随后分别加一
   并物化 `nextMappingCount`、`nextVisibleCount`。两次加法都完成后，才严格依次写
   `newPhysical=free`、`mappingCount=nextMappingCount`、
   `visibleCount=nextVisibleCount`。

函数末尾再次读取 `mappingCount`，按有符号 `>0` 返回 1，否则返回 0。

## 正常初始化链结果

原 `sub_533300` 在本函数前先由 `sub_533360` 把 `mappingCount`、
`visibleCount` 清零，把 `newPhysical` 设为 `-1`，并清十个 active 槽。在此前提下：

- 若载入成功 `N=0..9` 条记录，扫描后计数为 N；随后选择首个空闲物理槽并创建
  一条新记录，最终 `mappingCount=visibleCount=N+1`、`newPhysical=free`，返回 1。
- 若十条记录都载入成功，扫描后两个计数均为 10；条件仍会调用空闲槽查找，
  该查找返回 `-1`，因此不创建新记录，`newPhysical` 保持 `-1`，最终返回 1。
- 特别地，全部载入失败时会初始化物理槽 0，建立映射 0，两个计数均为 1。

## 异常别名与 C++ 边界

实现不检查计数、下标或加一溢出。正常域要求映射写入下标位于 `0..9`；若状态
已不一致，负下标或大于 9 的下标在原 32 位平坦地址模型中可能别名到相邻字段，
而对当前定长 C++ 数组而言属于越界未定义行为。本实现不增加安全模拟或归一化。
`mappingCount==10` 仍查找空闲槽；正常十槽全 active 时不会写 `map[10]`，但异常
状态若同时存在空闲槽，则后续该写入进入上述未定义域。

`mappingCount+1` 与 `visibleCount+1` 使用 C++ 有符号运算；若输入已到
`INT32_MAX`，其溢出同样属于未定义行为，而不是为原 x86 回绕添加额外 helper。
独立重读顺序仍被保留，以免在异常别名状态下把不同阶段错误合并。本项不实现
reset、caller 或其他保存流程，不修改 CMake。行为证据只取自当前
`ida-17524 / cg2d.exe`，STONAGE 不参与结论。
