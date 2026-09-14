# cg2d_re 子项目规则

1. 本子项目位于 `cg2d_re/`；实现、头文件、证据、计划分别位于 `src/`、`include/`、`doc/`、`plan/`，按真实需要落位。
2. 旧工程、旧计划及旧进度仅供参考，不跨写旧 `src/`（含 `src/packet`）或根 CMake。
3. 唯一执行进度为 `cg2d_re/plan/cg2d_full_reconstruction_progress.md`，覆盖父级同步 `plans/progess.md` 的约定；不更新旧进度。
4. 回答与文档使用中文，文件读写显式指定 UTF-8，禁止使用 Default 编码；保持最小改动与现有风格。
5. 禁用 C++ RTTI，禁止直接引入外部依赖。
6. 以当前选定 IDA 数据库为权威，按 `ida-analysis` 逐项对照原函数划分、结构布局和内部逻辑；磁盘副本仅核对差异，不覆盖 IDA，不以功能相似替代内部对应。
7. 未决 ABI、布局、文本编码和驱动策略遵循计划门禁，不擅自裁决；未经授权不修改 IDA 数据库、原客户端或资源。
