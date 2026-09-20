# cg2d 当前保存记录共享存储证据

## 对象范围与映像初值

- 原对象覆盖 `[0xFF4918,0xFF4E28)`，长度严格为 `0x510`；其布局与既有
  `SavedPhysicalRecord` 相同，即四个连续的 `0x144` 字节 part。
- IDA 对该范围报告 `fileoff=-1`、`is_loaded=false`，`get_byte=0xFF` 只是未加载
  字节的哨兵，不是运行期初值。该虚拟区没有 raw backing，运行时全零来自 PE loader
  对 virtual-only 区域的零填充。
- 前界之前的 `[0xFF4914,0xFF4918)` 是独立 4 字节项，不属于本记录；后界
  `0xFF4E28` 是独立 W8 状态，新端继续由 `cg::platform::gValueFF4E28` 持有，
  不是记录尾字节。

新端在 `LoginSavedState.cpp` 匿名命名空间定义唯一零初始化的
`SavedPhysicalRecord gCurrentSavedRecord {}`。记录类型和 owner 均不进入公共头文件，
也不与 `gValueFF4E28` 合并。

## 直接引用覆盖

对完整区间逐字节汇总得到 47 个 direct xref，分布于 22 个函数：

`sub_464840`、`sub_4CD4F0`、`sub_4CFE70`、`sub_5201B0`、`sub_520200`、
`sub_5203B0`、`sub_5203E0`、`sub_520410`、`sub_520440`、`sub_520470`、
`sub_520510`、`sub_520550`、`sub_520590`、`sub_520640`、`sub_5206F0`、
`sub_520720`、`sub_520730`、`sub_520750`、`sub_520770`、`sub_533300`、
`sub_5336D0`、`sub_533700`。

共享当前记录与物理保存记录之间有三处完整 `0x510` 字节复制边界，原端均以
`0x144` 个 DWORD 的 `rep movsd` 实现：

1. `sub_533300@0x53332E..0x533353`：把初始映射得到的物理记录复制到当前记录；
2. `sub_5336D0@0x5336D7..0x5336F0`：把当前 selected 对应的物理记录复制到
   当前记录；
3. `sub_533700@0x533707..0x533720`：把当前记录复制回 selected 对应的物理记录。

这些边界说明 `[0xFF4918,0xFF4E28)` 是一份完整共享记录，而不是若干互不相关的
登录字符串。本项只建立 owner，不实现上述复制函数。

## 两个登录输入子槽

`SavedRecordPart::text` 位于记录 `+0x04`，因此现有两个 51 字节入口的精确映射为：

| 原范围 | 结构表达式 | 记录内偏移 | 私有 accessor |
|---|---|---:|---|
| `[0xFF494F,0xFF4982)` | `parts[0].text + 0x33` | `+0x37` | `GetCurrentSavedInputValueFF494F()` |
| `[0xFF4982,0xFF49B5)` | `parts[0].text + 0x66` | `+0x6A` | `GetCurrentSavedInputValueFF4982()` |

两个 accessor 只在私有 `LoginInputDependencies.hpp` 声明，并在
`LoginSavedState.cpp` 的 `cg::login::detail` 中返回可写 `char*`。它们不公开
`SavedPhysicalRecord`，也不创建、复制或清理记录。

`LoginInput.cpp` 已删除原先两个独立 `char[51]` owner；四个现有公共读写函数改为
通过 accessor 使用上述子区间。内部读写 helper 的槽参数仅由数组引用收窄为指针，
空指针短路、最多 51 轮、NUL 先复制后返回以及返回指针约定均保持不变。

## 当前实现边界

本项不实现 `sub_5336D0`、`sub_533700`、`sub_520200` 或 `sub_533300`，也不新增
当前记录复制、保存、加载或初始化入口。`sub_520200` 未来仍应从该唯一 owner 获取
完整记录的局部编码快照，但不能为此另建共享存储。本项不修改公共头文件、CMake
或现有 caller。

行为与布局证据只取自当前 `ida-17524 / cg2d.exe`；STONAGE 不参与结论。
