# cg2d 完整复刻

- `src/`：按已取证的真实边界实现代码。
- `include/`：对应实现所需头文件。
- `doc/`：实际取得的基线、盘点与验证证据，由取证任务按需创建。
- `plan/`：独立执行计划与唯一执行进度。
- `tests/`：保留此前的资源名验证脚本；当前按用户要求暂停新增及执行测试用例。

## 当前代码目录

- `src/resource/`、`include/cg/resource/`：资源名／路径、图形文件与动画资源。
- `src/render/`、`include/cg/render/`：绘图、调色板、字体、队列和渲染后端。
- `src/input/`、`include/cg/input/`：输入正文、光标与按键分派；平台事件仍在platform。
- `src/audio/`、`include/cg/audio/`：音频业务与既有固定WAV资源；SDL后端仍在platform。
- `src/scene/login/`、`include/cg/scene/login/`：登录场景、登录／角色请求及保存状态。
- `src/scene/character_select/`、`include/cg/scene/character_select/`：选人、创建／删除交互及进度场景。
- `src/world/`、`include/cg/world/`：世界状态、地图、邮件、公会资料与黑名单；公会文件为GuildState。
- `src/Yobi/`、`include/cg/Yobi/`：Yobi动作及各动画／功能分组，统一使用 `cg::yobi`。
- `include/cg/core/`：跨模块的共用基础接口。
- `src/platform/RuntimeInitialization.cpp`：集中保留跨模块的原静态初始化、RNG和析构顺序。
- `platform/`、`ui/`、`network/`、`text/`保留既有职责；实现私有头紧邻所属源码。


## 其他

- 函数要写业务注释、包括不限于原有的函数来源、参数、返回值等。但业务变更需要及时更新
