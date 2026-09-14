# cg2d 完整复刻

原客户端为 x86；逐项对照原函数、结构与内部逻辑。原 DX7 渲染改用 SDL3 `SDL_Renderer`，仅复刻 32 位色渲染；8/16/24 位色渲染路径排除，但保留 8 位索引图、调色板读取及转 32 位色解码。

- `src/`：按已取证的真实边界实现代码。
- `include/`：对应实现所需头文件。
- `doc/`：实际取得的基线、盘点与验证证据，由取证任务按需创建。
- `plan/`：独立执行计划与唯一执行进度。
- `tests/`：本线小步验证；当前仅验证资源名数据对象，不运行客户端。

入口：[执行计划](plan/cg2d_full_reconstruction_plan.md)、[独立进度](plan/cg2d_full_reconstruction_progress.md)。

已验证：[当前 IDA 基线记录](doc/cg2d_reconstruction_baseline.md)、[资源来源](doc/cg2d_resource_sources.md)、[资源名初值与布局](doc/cg2d_reconstruction_inventory.md)。

已有资源名连续存储区的 C++ 初值实现，x86/x64 独立数据对象与当前 IDA 逐字节一致；尚无新客户端构建配置或可运行客户端，资源解析和加载函数尚未实现。
旧工程仅供参考；数据对象验证不裁决目标 ABI、其他布局、文本编码及驱动策略，也不代表完整资源和运行环境已冻结。
