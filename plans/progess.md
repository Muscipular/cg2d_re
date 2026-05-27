# 项目进度记录

> 总规划见 [cg_engine_dev_plan.md](cg_engine_dev_plan.md)。后续开发进度统一记录在本文件。

## 流程调整

| 日期 | 任务 | 修改 | 验证 | 遗留 |
|---|---|---|---|---|
| 2026-05-27 | 进度记录迁移 | 新增 `plans/progess.md`，将 M1/M2 进度记录从 `plans/cg_engine_dev_plan.md` 迁入统一进度文件；总规划仅保留链接 | 文档检查；未编译 | 后续推进继续写入本文件 |
| 2026-05-27 | 更新项目规则文档 | `AGENTS.md` 已按当前项目更新，补充当前目录、进度记录文件和 subAgent 流程规则 | 文档检查；未编译 | 无 |
| 2026-05-27 | 补充 namespace 与类型规划 | `AGENTS.md` 新增 namespace 与类型规划约束，明确根 namespace、模块 namespace、接口、handle、RAII、值类型、SDL 类型边界和编码边界 | 文档检查；未编译 | 无 |

## M1：platform 层

| 日期 | 任务 | 修改 | 验证 | 遗留 |
|---|---|---|---|---|
| 2026-05-27 | M1.1 Window 封装 | 新增 `src/platform/Window.hpp/.cpp`，封装窗口创建/销毁、尺寸、像素尺寸、DPI scale、全屏切换和窗口 ID；`main.cpp` 改用 `Window` 持有窗口 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | 事件、输入、IME、路径、计时器、线程仍待后续 M1.x |
| 2026-05-27 | M1.2 Event 分发 | 新增 `src/platform/Event.hpp/.cpp`，将 SDL quit、window、keyboard、mouse 事件转换为平台层 `Event`；`main.cpp` 改用 `EventPump` | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | 输入状态缓存、IME、路径、计时器、线程仍待后续 M1.x |
| 2026-05-27 | M1.3 Input 状态缓存 | 新增 `src/platform/Input.hpp/.cpp`，基于 `Event` 缓存键盘和鼠标 down/pressed/released 状态；`main.cpp` 接入帧级输入更新 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | IME、路径、计时器、线程仍待后续 M1.x |
| 2026-05-27 | M1.4 IME 启停 | 新增 `src/platform/Ime.hpp/.cpp`，按窗口 ID 封装 SDL 文本输入启停和状态查询；`main.cpp` 绑定窗口并退出时关闭文本输入 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | 输入区域、text editing、路径、计时器、线程仍待后续 M1.x |
| 2026-05-27 | M1.5 IME 输入区域 | 扩展 `ImeController::SetInputArea`，用窗口坐标设置 `SDL_SetTextInputArea` 候选框参考区域；`main.cpp` 设置默认输入区域 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | text editing、路径、计时器、线程仍待后续 M1.x |
| 2026-05-27 | M1.6 Text editing 事件 | 扩展 `Event` 支持 `TextEditing`、`TextInput` 和候选项事件，区分 IME composition 与 committed text | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | 路径、计时器、线程仍待后续 M1.x |
| 2026-05-27 | M1.7 Path 工具 | 新增 `src/platform/Path.hpp/.cpp`，封装 base/assets/config/save 路径，基于 SDL base/pref path，不硬编码平台路径 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | 计时器、线程仍待后续 M1.x |
| 2026-05-27 | M1.8 Timer | 新增 `src/platform/Timer.hpp/.cpp`，提供帧 delta/累计时间、帧率上限等待和固定步长累加器；`main.cpp` 用 `FrameTimer` 限制 60 FPS | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | 线程仍待 M1.9 |
| 2026-05-27 | M1.9 Thread/Job 基础 | 新增 `src/platform/Thread.hpp/.cpp`，提供最小 `JobQueue`，支持后台 worker、入队、停止和 join | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | M1 已完成，后续进入 M2 前可补工具化验证 |
| 2026-05-27 | M1 收尾 | 新增 `src/platform/Platform.hpp` 聚合头，覆盖 M1 平台层所有公开头文件 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | `JobQueue` 的任务异常和 worker 内自调用 `Stop()` 风险已记录，后续按使用场景再收敛 |

## M2：render 基础层

| 日期 | 任务 | 修改 | 验证 | 遗留 |
|---|---|---|---|---|
| 2026-05-27 | M2.1 IRenderer | 新增 `src/render/IRenderer.hpp`、`RenderTypes.hpp`、`Texture.hpp`、`Color.hpp`，定义无 SDL 依赖的渲染接口、矩形、颜色和纹理句柄 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测；确认 `src/render` 公开头无 SDL 类型 | SpriteCmd、RenderQueue、SDL_Renderer 后端仍待后续 M2.x |
| 2026-05-27 | M2.2 SpriteCmd | 新增 `src/render/SpriteCmd.hpp`，定义 sprite 绘制命令，包含 texture、source/destination、color、layer、sortY、order | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测；确认公开头无 SDL 类型 | RenderQueue、SDL_Renderer 后端仍待后续 M2.x |
| 2026-05-27 | M2.3 RenderQueue | 新增 `src/render/RenderQueue.hpp/.cpp`，收集 `SpriteCmd`，按 layer、sortY、order 稳定排序并提交给 `IRenderer` | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测；确认 render 队列无 SDL 依赖 | SDL_Renderer 后端、颜色提交和批处理仍待后续 M2.x |
| 2026-05-27 | M2.4 SDL_Renderer 后端 | 新增 `src/render/SdlRendererBackend.hpp/.cpp`，实现 clear、present、纹理句柄管理和 `SDL_RenderTexture` 绘制；`main.cpp` 改用后端清屏提交 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | 像素上传、PNG 绘制、颜色提交和批处理仍待后续 M2.x |
| 2026-05-27 | M2.5 SDL_RenderTexture | 扩展 `IRenderer::UpdateTexture` 和 SDL 后端像素上传，`main.cpp` 创建内存测试纹理并用 `DrawTexture` 绘制 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | PNG 绘制、颜色提交和批处理仍待后续 M2.x |
| 2026-05-27 | M2.6 SDL_RenderGeometry batch | 扩展 `IRenderer::DrawSprites`，SDL 后端用 `SDL_RenderGeometry` 批量绘制连续同纹理 sprite，`RenderQueue` 改为按纹理分段提交 batch | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | PNG 绘制、TextureCache、Atlas 和统计仍待后续 M2.x |
| 2026-05-27 | M2.7 TextureCache | 新增 `src/render/TextureCache.hpp/.cpp`，按字符串 key 缓存纹理句柄并负责销毁；`main.cpp` 通过缓存加载测试 RGBA 纹理 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | PNG 绘制、Atlas 和统计仍待后续 M2.x |
| 2026-05-27 | M2.8 TextureAtlas | 新增 `src/render/TextureAtlas.hpp/.cpp`，保存 atlas 纹理和 key 到源区域映射；`main.cpp` 通过 atlas 创建测试 `SpriteCmd` | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测 | Render stats、Pixel-perfect 仍待后续 M2.x |
| 2026-05-27 | M2.9 Render stats | 新增 `src/render/RenderStats.hpp`，`IRenderer` 暴露统计读写接口；SDL 后端统计 draw calls、sprites、texture switches；`main.cpp` 每 120 帧输出统计日志 | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；VS CMake configure/build `windows-msvc-x64-debug`；启动烟测确认 `Render stats` 日志 | Pixel-perfect 仍待后续 M2.x |
| 2026-05-27 | M2.10 Pixel-perfect 设置 | 新增 `TextureScaleMode` 和 `LogicalPresentationMode`；纹理默认 nearest，SDL 后端应用 nearest/linear，`main.cpp` 设置 1280x720 integer scale | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；VS CMake configure/build `windows-msvc-x64-debug`；启动烟测确认 `Render stats` 日志 | M2 基础层完成；PNG/FontCache 后续阶段处理 |
| 2026-05-27 | M2 收尾 | 独立 reviewer 总体验证 M2.1-M2.10：渲染接口、队列、批绘制、纹理缓存、Atlas、统计和 Pixel-perfect 均存在；公开头未暴露 SDL 类型 | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；VS CMake configure/build `windows-msvc-x64-debug`；DLL 检查；4 秒启动烟测确认 `Render stats` 日志 | M2 已完成；后续进入 M3 Font/Input/UI 最小闭环 |

## M3：Font/Input/UI 最小闭环

| 日期 | 任务 | 修改 | 验证 | 遗留 |
|---|---|---|---|---|
| 2026-05-27 | M3.1 FontCache | 新增 `src/render/FontCache.hpp/.cpp`，封装 SDL_ttf 字体加载和 UTF-8 文本栅格化上传；`main.cpp` 尝试加载 Windows 中文字体并绘制 `你好 CrossGate` demo 文本 | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；VS CMake configure/build `windows-msvc-x64-debug`；4 秒启动烟测确认 `FontCache demo loaded` 和 `Render stats` 日志 | M3.2 及后续任务未处理 |
| 2026-05-27 | M3.2 Text layout 基础 | 扩展 `FontCache::MeasureText`，新增 `src/ui/TextLayout.hpp/.cpp`，支持 UTF-8 文本按 `\n` 分段、按 codepoint 边界保守换行并统计行尺寸；`main.cpp` 增加轻量布局日志 | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；VS CMake configure/build `windows-msvc-x64-debug`；4 秒启动烟测确认 `FontCache demo loaded`、`TextLayout demo` 和 `Render stats` 日志 | M3.3 及后续任务未处理 |
| 2026-05-27 | M3.3 InputBox 控件 | 新增 `src/ui/InputBox.hpp/.cpp`，维护单行文本、焦点、提交标记和 UTF-8 字节光标；支持鼠标聚焦、TextInput commit 插入、退格、删除、左右键和回车提交；`main.cpp` 增加轻量日志 demo | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；VS CMake configure/build `windows-msvc-x64-debug`；4 秒启动烟测确认 `FontCache demo loaded`、`TextLayout demo`、`InputBox demo` 和 `Render stats` 日志 | M3.4 及后续任务未处理 |
| 2026-05-27 | M3.4 IME composition 显示 | 扩展 `InputBox` 保存 IME composition 文本、光标和选区长度，`TextEditing` 更新组合态、`TextInput` 提交后清空组合态；新增 `GetDisplayText()` 将组合文本插入当前光标位置；`main.cpp` 输出 composition 和 display 日志 | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；VS CMake configure/build `windows-msvc-x64-debug`；4 秒启动烟测确认 `FontCache demo loaded`、`TextLayout demo`、`InputBox demo` 和 `Render stats` 日志 | M3.5 及后续任务未处理 |
| 2026-05-27 | M3.5 Candidate UI 策略 | 扩展 `InputBox` 保存 IME 候选列表、选中项和横向/纵向状态；新增 `CandidateDisplayMode`，默认使用系统候选框；`main.cpp` 输出候选状态日志并继续设置输入区域 | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；VS CMake configure/build `windows-msvc-x64-debug`；4 秒启动烟测确认 `FontCache demo loaded`、`TextLayout demo`、`InputBox demo` 和 `Render stats` 日志 | 未做自绘候选窗；M3.6 及后续任务未处理 |
| 2026-05-27 | M3.6 ChatBox | 新增 `src/ui/ChatBox.hpp/.cpp`，保存聊天历史、滚动偏移和输入栏状态；提交非空输入后加入历史并清空输入；`main.cpp` 改为通过 ChatBox 转发输入事件并输出 history/visible/input 日志 | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；VS CMake configure/build `windows-msvc-x64-debug`；4 秒启动烟测确认 `FontCache demo loaded`、`TextLayout demo`、`ChatBox demo` 和 `Render stats` 日志 | 未做聊天绘制和复杂虚拟列表；M3.7 及后续任务未处理 |
| 2026-05-27 | M3.7 编码边界 | 新增 `src/text/TextEncoding.hpp/.cpp`，提供 UTF-8 与 GBK/Big5 的转换接口；Windows 使用系统 code page，非 Windows 明确返回失败；`main.cpp` 增加 GBK 往返转换 demo 日志 | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；`cmake --preset windows-msvc-x64` 因 PATH 无 cmake 失败；VS CMake configure/build `windows-msvc-x64-debug`；4 秒启动烟测确认 `Encoding demo`、`ChatBox demo` 和 `Render stats` 日志 | M3.8 及后续任务未处理 |
| 2026-05-27 | M3 收尾 | 独立 reviewer 总体验证 M3.1-M3.7：FontCache、TextLayout、InputBox、IME composition、候选策略、ChatBox 和编码边界均存在；公开头未暴露 SDL/TTF/Win32 类型 | `powershell -ExecutionPolicy Bypass -File scripts/download_sdl3.ps1`；VS CMake configure/build `windows-msvc-x64-debug`；DLL 检查；4 秒启动烟测确认 `FontCache demo loaded`、`TextLayout demo`、`ChatBox demo`、`Encoding demo` 和 `Render stats` 日志 | M3 已完成；IME composition/候选仍需后续人工输入法交互验证 |

## M4：resource 基础层

| 日期 | 任务 | 修改 | 验证 | 遗留 |
|---|---|---|---|---|
| 2026-05-27 | M4 动态 binSet 基础 | 更新 `plans/cg_engine_dev_plan.md` 的 M4 细分计划；新增 `src/resource`，实现 `ResourceTypes`、`ResourceError`、`BinaryReader`、`Palette`、`GraphicDecoder`、`AnimationLoader`、`ResourceManager`；CMake 接入 resource 源文件；resource 公开头不暴露 SDL 类型，Graphic 解码输出 RGBA8 纯数据 | 编码 subAgent A/B/C 分别处理基础类型、Graphic/Palette、Anime；验证 subAgent 只读检查通过：公开头无 SDL、无结构体硬转读取、`GraphicInfo` 40 字节、`AnimeInfo` 12 字节、`AnimeFrame` 10 字节、Anime action 的 duration/frameCount 为 32 位；`D:\SOFTSTAR\bin` 下 11 个 `GraphicInfo/AnimeInfo` 文件长度整除检查通过；VS CMake configure/build `windows-msvc-x64-debug` 通过 | Coordinate/MapData、viewer、主程序资源接入、TextureCache 上传示例和更细粒度单测仍待后续 M4/M5 小步 |
| 2026-05-27 | M4 Anime/Graphic 格式校对 | 按 `cgrw`、`twcg_hook` 和 IDA 校对修正 Anime/Graphic：Anime action 由 `AnimeInfo.flag` 判定 12/20 字节头，`dir/action` 改为有符号精确匹配；补 `FindAnimeAction`、action 帧 span、帧到 Graphic 查询；Graphic 解码结果带出 RD `version`、属性、压缩、内置 palette 和 palette size；CGEX palette 从解码后的像素尾部读取；补 `asGround`、全局 graphic index 和 raw/internal animeNo 双索引 | 验证 subAgent 只读检查通过：`git diff --check` 通过；resource 公开头无 SDL；Anime `flag0/flagExt` 抽样 36 条通过；CGEX 抽样 24 条通过；VS CMake build `windows-msvc-x64-debug` 通过 | Coordinate/MapData、viewer、主程序资源接入、TextureCache 上传示例和更细粒度单测仍待后续 M4/M5 小步；少数 CGEX 记录存在 `blockSize > GraphicInfo.length` 风险，后续需确认兼容策略 |
| 2026-05-27 | M4 索引/Header 完整保存 | 补齐 `GraphicInfo` 原始 reserved 字节；新增 `GraphicDataHeader` 保存 RD/CGEX data header 的 magic、version、attributes、尺寸、blockSize、paletteSize 和 headerSize；新增 `AnimeActionHeader` 保存 Anime data action header 的 12/20 字节完整字段、revert 原值和 headerSize；`DecodedImage`、`AnimeAction` 持有对应 header | 验证 subAgent 只读检查通过：`git diff --check` 通过；resource 公开头无 SDL；结构字段与读取路径核对通过；VS CMake build `windows-msvc-x64-debug` 通过 | 少数 CGEX 异常 blockSize 兼容策略仍待后续确认 |
| 2026-05-27 | M4 BASE 首尾记录验证 | 读取 `H:\Games\cgmsv_21.2a\CrossGate7.1.0\cgmsv.yml` 的 `[BASE]`；首条 Graphic/Anime 与末条 Graphic/Anime 均完成只读验证；末条 Anime 原始 `flag=64`，按旧客户端规则应视为非扩展头，修正 `AnimationLoader` 仅在 `flag == 1` 时读取 20 字节 action header | 验证 subAgent 只读检查通过：`diff --check` 通过；`flag == 1` 源码核对通过；末条 Anime 12 字节 header 连续读取到文件尾；VS CMake build `windows-msvc-x64-debug` 通过 | 后续如需可追加全量 AnimeInfo flag 分布扫描 |
| 2026-05-27 | M4 IDA 字段类型对齐 | 将 Graphic/Anime 磁盘结构按 IDA 字段类型和长度收敛：补 `CG_GraphicVersion`、`CG_Direction`、`CG_ActionType` 固定底层类型；`GraphicInfo`、`GraphicDataHeader`、`GraphicCgexDataHeader`、`AnimeInfo`、`AnimeActionHeader`、`AnimeFrame` 保持原始字段；派生状态移入 metadata/ref/clip 层 | 验证 subAgent 只读检查通过：`diff --check` 通过；IDA 类型核对通过；`[BASE]` 首尾 Graphic/Anime 读取通过，末条 Anime `flg=64` 按 12 字节头；构建 subAgent 验证 `windows-msvc-x64-debug` 通过 | 无 |
