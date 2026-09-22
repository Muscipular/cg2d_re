# cg2d 世界投影状态复制

日期：2026-09-22。任务：R6.1.3.143。证据源为 `cg2d.exe` 当前 IDB 与当前实现源码。

## 1. 函数边界

`sub_468FF0@0x468FF0` 的范围为 `[0x468FF0,0x46901D)`，共 45 字节、9 条指令、1 个基本块。函数无参数、无被调目标；唯一调用点是 `MainLoop@0x4572B9`，调用者忽略返回值。

## 2. 严格读写序列

函数依次执行：

1. R32 `0x9EE908` 到局部值 A。
2. R32 `0x9EE90C` 到局部值 B。
3. R32 `0x9EE904` 到局部值 C。
4. W32 A 到 `0xAF10C4`。
5. R32 `0x9EE8F4` 到局部值 D。
6. W32 B 到 `0xAF10CC`。
7. W32 C 到 `0xAF10D0`。
8. W32 D 到 `0xAF10D4`。
9. 返回 D。

实现不增加算术、门禁、原子操作或额外读写。

## 3. 新端存储映射

| 原地址 | 当前实现 | 初值 |
| --- | --- | ---: |
| `0x9EE908` | `cg::world::gWorldProjectionSourceA` | 0 |
| `0x9EE90C` | `cg::world::gWorldProjectionSourceB` | 0 |
| `0x9EE904` | `cg::world::gWorldProjectionSourceC` | 0 |
| `0x9EE8F4` | `cg::world::gWorldProjectionSourceD` | 0 |
| `0xAF10C4` | `cg::world::gWorldProjectionSnapshotA` | 0 |
| `0xAF10CC` | `cg::world::gWorldProjectionSnapshotB` | 0 |
| `0xAF10D0` | `cg::world::gWorldProjectionSnapshotC` | 0 |
| `0xAF10D4` | `cg::world::gWorldProjectionSnapshotD` | 0 |

生产与消费交叉引用链均落在世界等距投影和地图绘制路径，因此八个地址中性槽及复制函数由 `cg::world::SceneState` 唯一持有，不放入平台或渲染边界。

## 4. 主循环接线

`MainLoop` 保持原调用顺序：`render::DrawAllQueued_520DD0()` → `world::CopyWorldProjectionState_468FF0()` → `render::Flip_44C560()`。旧的 `cg::platform::Sub_468FF0` 未实现依赖声明已删除。
