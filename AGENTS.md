# 全局规则
1. 只允许使用中文回答，输出文件默认为中文
2. 按实际需要进行编译或者执行
3. 保持代码整洁，每次都只进行最小化调整
4. 禁止直接引入外部依赖，但可以提示用户
5. 文件编码采用UTF-8，匹配现有风格
6. 禁止过度联想：仅实现用户要求；不扩展未要求的功能；不补充未说明的业务逻辑；不重构用户代码
7. 输出必须简洁精准，仅提供最小化可用代码
8. 不要隐藏困惑，呈现权衡。不确定的点停下要求用户澄清。严禁假设或猜测。
9. 读写文件必须显式指定编码，禁止使用Default编码
10. 按**流程要求**使用 subAgent 进行小颗粒度的分析、编码、验证,默认SubAgent思考强度xhigh

# 流程要求
1. 每次推进开发进度，必须同步写入 `plans/progess.md`
2. 每个可验证小步完成后提交；用户明确要求不提交时，仅记录进度，提交由主线程统一处理
3. 使用 subAgent 时，每个 agent 只处理单项任务，如Mx.x或Mx.x.x
4. subAgent 完成后及时关闭；验证交由独立 subAgent


# ida mcp
ida分析实例为cg2d.exe;

# 项目目录
1. `plans/`：开发计划与阶段任务说明
   - [cg_engine_dev_plan.md](plans/cg_engine_dev_plan.md)：项目总规划
   - [progess.md](plans/progess.md)：统一进度记录文件
2. `scripts/`：项目辅助脚本，例如依赖下载脚本
3. `src/app/`：客户端入口与应用层代码
4. `src/core/`：基础公共代码，例如日志
5. `src/platform/`：SDL3 平台封装，例如窗口、事件、输入、IME、路径、计时器、线程
6. `src/render/`：渲染基础层，例如渲染接口、队列、纹理、字体缓存
7. `src/ui/`：UI 与输入控件，例如文本布局、输入框、聊天框
8. `src/text/`：文本编码边界，例如 UTF-8 与旧协议编码转换
9. `third_party/sdl/`：本地下载的 SDL3 依赖目录，已被 `.gitignore` 忽略，不提交仓库
10. `build/`：CMake 构建输出目录，已被 `.gitignore` 忽略

# Namespace 与类型规划
1. 根 namespace 统一为 `cg`。
2. 当前已实现模块 namespace 为 `cg::app`、`cg::core`、`cg::platform`、`cg::render`、`cg::ui`、`cg::text`。
3. 接口类型优先使用 `I` 前缀，例如 `IRenderer`。
4. handle 类型用于表达稳定资源引用，例如 `TextureHandle`；上层不得长期持有底层 SDL 指针。
5. RAII 封装类型负责资源生命周期；SDL 创建/销毁逻辑封装在 `cg::platform`、`cg::render` 后端等边界内。
6. 值类型用于跨模块传递轻量数据，例如 `RectF`、`ColorF`、事件结构、编码结果等。
7. SDL 类型边界：`SDL_Window*`、`SDL_Renderer*`、`SDL_Texture*`、TTF/Win32 类型不得进入上层业务公开接口；只能出现在 `cg::platform`、`cg::render` 后端或 `cg::text` 边界实现中。
8. 编码边界：内部文本统一 UTF-8；GBK/Big5 等旧协议编码只在 `cg::text` 或协议边界转换。
9. 不要随意新增全局类型或跨模块暴露底层对象。
10. 禁用c++ rtti

# 代码风格
## 逻辑尽量语义化表达
1. 合理使用`enum`、`#define`
2. 简单逻辑如：`a.state == A_STATE_STOP`，请勿定义专门函数进行处理，如果大量重复，优先使用`#define`
3. 类型、函数、字段上标注类型/函数/字段作用及关联的类型/函数/作用，如果是复杂逻辑，可以引用doc

# 思考要求
1. 思考时需要考虑其他模块的影响
2. 避免随意改动其他关联方的函数
3. 工具函数是好，但不能滥用

# packet 规范
1. 小颗粒协议工具优先收敛到既有 `src/packet` 工具文件；避免为少量函数新增独立 hpp/cpp，确有复用范围或复杂度后再拆分。

# 常用文件
"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
"C:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\Tools\VsDevCmd.bat"
