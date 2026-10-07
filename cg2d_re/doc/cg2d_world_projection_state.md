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

## BUR01 投影生产与重绘消费补齐（2026-10-08）

[当前基线](cg2d_map_projection_cache_sampling_spec.md#map-projection-cache-baseline)补原点投影468F70完整28条116B与既有复制468FF0完整9条45B的正式身份，三个源C00/F70/FF0正文正确全保。原半值54DD28已加载000000000000E03F；两个F32零先投影，X加F64半值并首次整数转换，Y加半值后先pubX至A，再转换Y，后C/快照A/B/D/快照B及两W32零原序保。现S64宿主转换/lowW32承接不代全x87精度/异常/ABI验收。

复制严格A/B/C三读→A发布→D读取→B/C/D发布→返D，与本页既有合同一致，无批量复制/新owner。实际地图重绘469030只一初始坐标片段恢复C在X符号修正后读取、D在Y三倍项与后乘16之间读取；column0后才held spanY减1作row，后续对角tile遍历/裁剪/层号/步长/返回不改。MainLoop和地图父局部后窗有据，不重复原46CF60完整绘制或主循环总根；完整像素/寿命/数值与运行继续。

### BUR01 本步实际结果

必要静态通过：四完整原根178条620B/五页、六局部后窗/半值8B/hit父局部/四尾36B，源码全文仅重绘初始坐标一片段、两机器台账和五文档/四任务完整模型核定；四锚点/25来源及UTF-8无BOM/LF通过。文本汇总与-Check通过，完整派生快照其余对象保持：语义1719/有效1712、文档1830（49.73%）、处置860/E1014、R7已审425/余2636，LOOP131/MAP126、NETWORK61保持。两I两S仅本片段开发/静态，四任务open、完整叶验收0、整体未验收；三个正确叶和既有I/O/渲染/网络全文保，未编译、测试或运行，不代全部父/callee、x87/ABI、寿命/文件/像素/明文互通及R8。命名快照cg2d-re-map-projection-cache-sampling-20261008，整体目标active。
