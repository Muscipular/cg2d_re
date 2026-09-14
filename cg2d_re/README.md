# cg2d 完整复刻

原客户端为 x86，**新工程目标为 x64**；逐项对照原函数、结构与内部逻辑，必要 ABI 与指针映射单独记录。原 DX7 渲染改用 SDL3 `SDL_Renderer`，仅复刻 32 位色渲染；8/16/24 位色渲染路径排除，但保留 8 位索引图、调色板读取及转 32 位色解码。

- `src/`：按已取证的真实边界实现代码。
- `include/`：对应实现所需头文件。
- `doc/`：实际取得的基线、盘点与验证证据，由取证任务按需创建。
- `plan/`：独立执行计划与唯一执行进度。
- `tests/`：保留此前的资源名验证脚本；当前按用户要求暂停新增及执行测试用例。

入口：[执行计划](plan/cg2d_full_reconstruction_plan.md)、[独立进度](plan/cg2d_full_reconstruction_progress.md)。

已验证：[当前 IDA 基线记录](doc/cg2d_reconstruction_baseline.md)、[资源来源](doc/cg2d_resource_sources.md)、[资源名初值与布局](doc/cg2d_reconstruction_inventory.md)。

当前先完善功能，再考虑启动，不再以最小改动限制新工程实现规模。已有资源名初值与索引图解码核心，后者通过约定可定义范围内的独立静态复核；资源查询、读取、调色板与 SDL 提交适配继续推进。
新工程使用独立 x64 CMake 配置，不链接旧工程；尚未提供完整客户端入口。索引图读取可参考 cg3a，但控制字、固定颜色和调用副作用仍以 IDA 为准，不直接沿用旧实现的差异。
