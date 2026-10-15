# cg2d 完整复刻

原客户端为 x86，**新工程目标为 x64**；逐项对照原函数、结构与内部逻辑，必要 ABI 与指针映射单独记录。原 DX7 渲染改用 SDL3 `SDL_Renderer`，仅复刻 32 位色渲染；8/16/24 位色渲染路径排除，但保留 8 位索引图、调色板读取及转 32 位色解码。

- `src/`：按已取证的真实边界实现代码。
- `include/`：对应实现所需头文件。
- `doc/`：实际取得的基线、盘点与验证证据，由取证任务按需创建。
- `plan/`：独立执行计划与唯一执行进度。
- `tests/`：保留此前的资源名验证脚本；当前按用户要求暂停新增及执行测试用例。

入口：[执行计划](plan/cg2d_full_reconstruction_plan.md)、[独立进度](plan/cg2d_full_reconstruction_progress.md)。

已批准方案：[文本编码边界 SPEC](doc/cg2d_text_encoding_spec.md)。还原核心以原编码字节为权威，Unicode 仅在确有需求的现代边界集中转换；源码文件仍为 UTF-8。服务器连接提示链的原字节源码已对齐；验证结论见[独立进度](plan/cg2d_full_reconstruction_progress.md)。其他编码债务仍保留，不代表运行或整体编码完成。

已验证：[当前 IDA 基线记录](doc/cg2d_reconstruction_baseline.md)、[资源来源](doc/cg2d_resource_sources.md)、[资源名初值与布局](doc/cg2d_reconstruction_inventory.md)。

当前先完善功能，再考虑启动，不再以最小改动限制新工程实现规模。已有资源解码、查询、读取及调色板的局部实现；当前优先沿原入口、配置与参数、窗口、主循环和退出链推进。
新工程使用独立 x64 CMake 配置，不链接旧工程。真实 `WinMain`、配置读取及窗口函数已落位，部分被调函数仍未实现，尚不能链接完整客户端；不会用演示入口或空实现填补缺口，详见[入口证据](doc/cg2d_startup_flow.md)。索引图读取可参考 cg3a，但控制字、固定颜色和调用副作用仍以 IDA 为准。

## 当前代码目录

- `src/resource/`、`include/cg/resource/`：资源名／路径、图形文件与动画资源。
- `src/render/`、`include/cg/render/`：绘图、调色板、字体、队列和渲染后端。
- `src/input/`、`include/cg/input/`：输入正文、光标与按键分派；平台事件仍在platform。
- `src/audio/`、`include/cg/audio/`：音频业务与既有固定WAV资源；SDL后端仍在platform。
- `src/scene/login/`、`include/cg/scene/login/`：登录场景、登录／角色请求及保存状态。
- `src/scene/character_select/`、`include/cg/scene/character_select/`：选人、创建／删除交互及进度场景。
- `src/world/`、`include/cg/world/`：世界状态、地图、邮件、公会资料与黑名单；公会文件为GuildState。
- `src/Yobi/`、`include/cg/Yobi/`：既有Yobi动作及各动画／功能分组。
- `platform/`、`ui/`、`network/`、`text/`保留既有职责；实现私有头紧邻所属源码。

本轮迁移映射、保留理由与静态验证边界见[代码审查与组织整理SPEC](doc/cg2d_code_organization_review_spec.md)。历史文档中的旧路径可按该映射定位，历史行号不视为当前源码行号。
