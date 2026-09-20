# cg2d 登录保存状态连续存储证据

## 范围与初值

本项只建立原连续区间 `[0x1058698,0x105C398)` 的类型和唯一存储，区间总长
`0x3D00`。IDA 对 `0x1058698`、`0x105C394` 与末字节 `0x105C397` 报告
`fileoff=-1`、`is_loaded=false`、`get_byte=0xFF`；其中 `0xFF` 是 unloaded
哨兵，不代表文件或运行时初值。该区间没有 PE raw backing，运行时全零的依据是
PE loader 对无 raw backing 的虚拟区执行零填充。因此源码唯一实例
`gSavedState {}` 保持完整全零初值。

特别地，`newPhysical@0x105C368` 和 `logicalToPhysical@0x105C36C` 的装载初值
都是零。本项没有提前写入 `-1`；`sub_533360@0x533360` 在运行时把前者及十个
映射项写成 `-1`，这是未来 reset 行为，不属于静态存储初值。

## 精确布局

| 字段 | 相对偏移 | 原地址 | 大小 |
|---|---:|---:|---:|
| `paths[10]` | `0x0000` | `0x1058698` | `0xA00` |
| `records[10]` | `0x0A00` | `0x1059098` | `0x32A0` |
| `active[10]` | `0x3CA0` | `0x105C338` | `0x28` |
| `mappingCount` | `0x3CC8` | `0x105C360` | `0x4` |
| `visibleCount` | `0x3CCC` | `0x105C364` | `0x4` |
| `newPhysical` | `0x3CD0` | `0x105C368` | `0x4` |
| `logicalToPhysical[10]` | `0x3CD4` | `0x105C36C` | `0x28` |
| `selected` | `0x3CFC` | `0x105C394` | `0x4` |

`SavedPath` 是 `char[0x100]`，大小 `0x100`。`SavedRecordPart` 依次为
`int32@0`、`char[316]@4`、`uint8[4]@0x140`，大小 `0x144`；四份 part 构成
大小 `0x510` 的 `SavedPhysicalRecord`。十条路径、十条物理记录和尾部状态共同
构成大小 `0x3D00` 的 `SavedStateStorage`。源码用 `static_assert` 固定各类型的
标准布局、可平凡复制性质、大小及全部关键 `offsetof`。

## 直接 xref 用途

以下计数只统计 IDA 对各字段基址的直接 xref，不把数组内部地址推算为额外 xref：

- `paths` 基址有 12 处直接 xref。`sub_533360` 生成 `data\\%d` 路径，
  `sub_5333D0` 检查路径下的保存文件，`sub_533730` 用路径持久化记录。
- `records` 基址有 8 处直接 xref。`sub_533360` 循环十轮，每轮固定清 record0
  `[0x1059098,0x10595A8)`，同一记录共清十次；records[1..9] 不由该函数清理。
  `sub_533570` 至 `sub_533700` 一组 reader/writer 通过映射访问记录内容，
  `sub_533730` 读取整条记录用于持久化。
- `active` 基址有 4 处直接 xref。`sub_533470` 按物理槽写 0/1 并在分配新槽时
  写 1；`sub_533730` 以它筛选需要保存的槽。
- `mappingCount`、`visibleCount`、`newPhysical` 的基址分别有 12、8、3 处直接
  xref。`sub_533360` 负责运行时复位，`sub_533470` 建立活动槽计数和新物理槽；
  `visibleCount`、`newPhysical` 也由能力 4 场景 `sub_465430`、`sub_464840` 读取。
- `logicalToPhysical` 基址有 20 处直接 xref，是 reader/writer、保存管理函数及
  能力 4 场景使用的逻辑槽到物理槽映射。
- `selected` 基址有 22 处直接 xref，能力 4 初始化、场景和切槽包装均读取或写入
  当前逻辑槽。源码仅将现有私有符号 `gSelectedSavedSlot105C394` 定义成
  `gSavedState.selected` 的引用别名，不产生第二份 W32 存储。

## 当前边界

本项不实现 reset、load、映射构建、record reader/writer、persistence 或 current
buffer，也不增加公共 include。其余状态保持翻译单元私有；未来保存函数应在同一
翻译单元复用该存储，或仅通过有证据的窄接口访问。
