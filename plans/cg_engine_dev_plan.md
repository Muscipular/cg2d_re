# cg-engine 开发任务计划

> 目标：基于 SDL3 构建 CrossGate 客户端重写用的薄引擎层。第一阶段优先保证窗口、输入、IME、字体、地图、角色、UI、资源和协议链路跑通；第二阶段针对 16000 可见移动角色做批渲染、Atlas、裁剪和排序优化；第三阶段再评估是否引入 SDL3_GPU 后端。

---

## 0. 总体原则

### 0.1 技术路线

- 主语言：C++20。
- 平台层：SDL3。
- 初始渲染：SDL_Renderer。
- 批渲染：SDL_RenderGeometry + TextureAtlas + RenderQueue。
- 后续高性能后端：预留 SDL3_GPU，但不在第一阶段强行实现。
- 字体：SDL3_ttf 或自建字体缓存层。
- 图片辅助：SDL3_image；旧客户端资源走自定义 decoder。
- 调试 UI：后续接 Dear ImGui。
- 脚本层：后续再考虑 Lua/LuaJIT。

### 0.2 架构原则

1. 游戏逻辑不得直接依赖 `SDL_Renderer*`、`SDL_Texture*`、`SDL_Window*`。
2. 所有绘制统一走 `RenderQueue`，上层只提交 `SpriteCmd`、`TextCmd`、`RectCmd`。
3. 资源系统只暴露稳定 handle，不让外部模块直接持有底层纹理指针。
4. 内部文本统一 UTF-8；和旧协议交互时再转换 GBK/Big5/其他编码。
5. 先做可验证的小闭环，再扩大到完整客户端。
6. 每个模块必须有最小工具或测试入口，避免只在主客户端里调试。

### 0.3 进度记录与提交

1. 每次推进开发进度，必须同步记录本次修改内容、完成进度、验证结果和遗留问题。
2. 进度记录应写入对应计划、文档或提交说明，保证后续可以追踪本次变更来源。
3. 每次完成一个可验证的小步后，必须提交代码。
4. 提交前必须确认不包含构建产物、临时文件和本地依赖目录。
5. 提交信息应简洁说明完成的里程碑、任务编号或核心改动。

---

## 1. 建议目录结构

```text
cg-engine/
├─ CMakeLists.txt
├─ CMakePresets.json
├─ vcpkg.json
├─ assets/
│  ├─ fonts/
│  ├─ shaders/
│  └─ test/
├─ src/
│  ├─ platform/
│  ├─ render/
│  ├─ resource/
│  ├─ world/
│  ├─ ui/
│  ├─ net/
│  └─ app/
├─ tools/
│  ├─ asset_viewer/
│  ├─ map_viewer/
│  └─ anim_viewer/
├─ tests/
│  ├─ unit/
│  └─ perf/
└─ docs/
   ├─ architecture.md
   ├─ render.md
   ├─ resource_format.md
   ├─ protocol.md
   └─ migration_notes.md
```

---

## 2. 里程碑总览

| 里程碑 | 名称 | 目标 | 优先级 |
|---|---|---|---|
| M0 | 工程初始化 | CMake/vcpkg/SDL3 可构建可运行 | P0 |
| M1 | Platform 基础层 | window、event、timer、path、IME 基础可用 | P0 |
| M2 | Render 基础层 | IRenderer、RenderQueue、SpriteCmd、SDL_Renderer 后端 | P0 |
| M3 | Font/Input/UI 最小闭环 | 中文输入、聊天框、字体缓存 | P0 |
| M4 | Resource 基础层 | package reader、image decoder、texture cache | P0 |
| M5 | Map/Animation Viewer | 地图块、动画帧、资源查看器 | P0 |
| M6 | World 渲染与排序 | entity data、移动插值、可见性裁剪、Y-sort | P0 |
| M7 | 性能压测 | 16000 可见移动角色压测 | P0 |
| M8 | Net 基础层 | session、packet codec、协议状态机雏形 | P1 |
| M9 | UI 系统扩展 | 面板、按钮、输入框、列表、tooltip | P1 |
| M10 | 工具链完善 | asset/map/anim viewer 稳定化 | P1 |
| M11 | Hook/旧客户端对照 | 旧行为采样、差异记录、迁移验证 | P1 |
| M12 | 跨平台与打包 | Windows/Linux/macOS 构建与发布 | P2 |
| M13 | SDL3_GPU 后端评估 | 在确有性能瓶颈时实现 GPU 后端 | P2 |

---

## 3. M0：工程初始化

### 目标

建立一个可复现构建的 SDL3 C++20 工程。任何人拉代码后，可以通过 CMake Presets 构建出最小窗口程序。

### 任务

| 编号 | 任务 | 产出物 | 验收标准 |
|---|---|---|---|
| M0.1 | 创建 CMake 工程 | `CMakeLists.txt` | Windows 可 configure/build |
| M0.2 | 创建 vcpkg manifest | `vcpkg.json` | 自动安装 `sdl3`、`sdl3-image`、`sdl3-ttf` |
| M0.3 | 添加 CMakePresets | `CMakePresets.json` | 支持 Windows x64，预留 Linux/macOS |
| M0.4 | 创建最小 SDL3 app | `src/app/main.cpp` | 启动窗口、可退出 |
| M0.5 | 统一编译选项 | warning、UTF-8、C++20 | MSVC `/utf-8 /W4`，GCC/Clang `-Wall -Wextra` |
| M0.6 | 添加日志基础 | `Log.hpp/.cpp` | 能打印 SDL 错误、模块日志 |

### 验收场景

```text
1. cmake --preset windows-msvc-x64
2. cmake --build --preset windows-msvc-x64
3. 运行 cg-client.exe
4. 出现窗口
5. 点击关闭按钮正常退出
```

---

## 4. M1：platform 层

### 目标

封装 SDL3 平台能力，包括窗口、事件、输入、IME、文件路径、计时器和基础线程工具。

### 进度记录

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

### 目录

```text
src/platform/
├─ Window.hpp/.cpp
├─ Event.hpp/.cpp
├─ Input.hpp/.cpp
├─ Ime.hpp/.cpp
├─ Path.hpp/.cpp
├─ Timer.hpp/.cpp
├─ Thread.hpp/.cpp
└─ Platform.hpp
```

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M1.1 | Window 封装 | 创建/销毁窗口，尺寸、DPI、全屏切换 | 窗口可 resize，全屏切换不崩溃 |
| M1.2 | Event 分发 | SDL_Event 转内部事件 | 鼠标、键盘、窗口事件可记录 |
| M1.3 | Input 状态缓存 | key/mouse 当前状态与上一帧状态 | 支持 pressed/released/down 查询 |
| M1.4 | IME 启停 | 输入框聚焦时 `StartTextInput`，失焦时 stop | 中文输入法可弹候选 |
| M1.5 | IME 输入区域 | 根据输入框光标设置候选框位置 | 候选框跟随聊天框光标 |
| M1.6 | Text editing 事件 | composition text 和 committed text 分离 | 拼音组合态与最终文本分别显示 |
| M1.7 | Path 工具 | save/config/assets/userdata 路径 | 跨平台路径不硬编码 |
| M1.8 | Timer | delta time、fixed tick、frame cap | 支持固定逻辑步长 |
| M1.9 | Thread/Job 基础 | 可选：资源加载线程基础 | 能异步加载测试资源，不阻塞主线程 |

### 关键接口建议

```cpp
struct TextInputEvent {
    enum class Type { Editing, Commit } type;
    std::string utf8;
    int cursor = 0;
    int selectionLength = 0;
};

class ImeController {
public:
    void BeginTextInput();
    void EndTextInput();
    void SetInputArea(float x, float y, float w, float h, float cursorX);
};
```

---

## 5. M2：render 基础层

### 目标

建立和 SDL 解耦的渲染接口。第一版用 SDL_Renderer 实现，后续可切 SDL3_GPU。

### 进度记录

| 日期 | 任务 | 修改 | 验证 | 遗留 |
|---|---|---|---|---|
| 2026-05-27 | M2.1 IRenderer | 新增 `src/render/IRenderer.hpp`、`RenderTypes.hpp`、`Texture.hpp`、`Color.hpp`，定义无 SDL 依赖的渲染接口、矩形、颜色和纹理句柄 | `cmake --preset windows-msvc-x64`；`cmake --build --preset windows-msvc-x64-debug`；启动烟测；确认 `src/render` 公开头无 SDL 类型 | SpriteCmd、RenderQueue、SDL_Renderer 后端仍待后续 M2.x |

### 目录

```text
src/render/
├─ IRenderer.hpp
├─ RenderTypes.hpp
├─ SpriteCmd.hpp
├─ RenderQueue.hpp/.cpp
├─ SdlRendererBackend.hpp/.cpp
├─ Texture.hpp
├─ TextureCache.hpp/.cpp
├─ TextureAtlas.hpp/.cpp
├─ FontCache.hpp/.cpp
└─ Color.hpp
```

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M2.1 | 定义 `IRenderer` | BeginFrame/EndFrame/CreateTexture/Draw | 上层不直接调用 SDL |
| M2.2 | 定义 `SpriteCmd` | texture handle、src、dst、color、layer、sortY | 可表达地图、角色、UI sprite |
| M2.3 | RenderQueue | 收集、排序、分层提交 | 支持按 layer/y/order 排序 |
| M2.4 | SDL_Renderer 后端 | 创建 renderer、clear、present | 基础绘制可用 |
| M2.5 | `SDL_RenderTexture` 版本 | 最小 sprite 绘制 | 可画 PNG 到屏幕 |
| M2.6 | `SDL_RenderGeometry` batch | quad 转三角形批量提交 | 同 texture 多 sprite 一次提交 |
| M2.7 | TextureCache | handle 到 SDL_Texture 管理 | 不重复加载同一资源 |
| M2.8 | TextureAtlas | 简单 atlas packing 或离线 atlas 读取 | 角色帧可放入 atlas |
| M2.9 | Render stats | draw calls、sprites、texture switches | 屏幕显示或日志输出统计 |
| M2.10 | Pixel-perfect 设置 | nearest/linear、整数缩放策略 | 像素风无明显模糊 |

### 推荐命令结构

```cpp
struct SpriteCmd {
    TextureHandle texture;
    RectF src;
    RectF dst;
    ColorF color;
    uint32_t layer;
    int32_t sortY;
    uint32_t order;
};
```

### 渲染排序建议

```text
1. background layer
2. map base layer
3. map object lower layer
4. entity layer: sort by y
5. effect lower layer
6. entity name/hp layer
7. effect upper layer
8. UI layer
9. debug overlay
```

---

## 6. M3：Font/Input/UI 最小闭环

### 目标

先做一个可用的聊天输入框：能中文输入、显示 composition、提交文本、光标移动、退格、换行/发送。

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M3.1 | FontCache | 字体加载、字号、glyph cache | 可渲染中英文 UTF-8 |
| M3.2 | Text layout 基础 | 单行/多行文本测量 | 聊天框文本不越界 |
| M3.3 | InputBox 控件 | 光标、选区、退格、删除、方向键 | 基础编辑行为正确 |
| M3.4 | IME composition 显示 | 拼音/假名组合态显示在输入框内 | 输入中文时能看到组合文本 |
| M3.5 | Candidate UI 策略 | 默认用系统候选框；全屏异常时预留自绘 | 窗口模式候选框位置正确 |
| M3.6 | ChatBox | 历史消息、滚动、输入栏 | 可输入并显示消息 |
| M3.7 | 编码边界 | UTF-8 内部，发送前转旧协议编码 | 非 ASCII 不乱码 |

### 验收场景

```text
1. 启动客户端
2. 点击聊天框
3. 输入“你好 crossgate”
4. 拼音组合态显示正常
5. 确认后文本进入输入框
6. Enter 后进入聊天历史
7. Backspace、方向键、鼠标点击光标位置可用
```

---

## 7. M4：resource 基础层

### 目标

建立旧客户端资源读取和解码管线。第一阶段不追求完整格式覆盖，先支持测试资源和一类核心图片/动画资源。

### 目录

```text
src/resource/
├─ ResourceHandle.hpp
├─ ResourceManager.hpp/.cpp
├─ PackageReader.hpp/.cpp
├─ ImageDecoder.hpp/.cpp
├─ AnimationLoader.hpp/.cpp
├─ MapLoader.hpp/.cpp
├─ Palette.hpp/.cpp
└─ ResourceCache.hpp/.cpp
```

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M4.1 | ResourceManager | 统一加载入口 | 上层通过 handle 请求资源 |
| M4.2 | PackageReader | 读取资源包目录、文件偏移、大小 | 能枚举资源包内容 |
| M4.3 | ImageDecoder | 旧格式转 RGBA8888 | 能输出 width/height/pixels |
| M4.4 | Palette 支持 | 调色板、透明色、alpha 规则 | 透明边缘正确 |
| M4.5 | AnimationLoader | 帧序列、方向、帧时长 | 可播放角色动画 |
| M4.6 | MapLoader | 地图 tile、object、阻挡数据 | 可解析测试地图 |
| M4.7 | Cache 生命周期 | 引用计数或 LRU | 切图/切地图不爆内存 |
| M4.8 | Error reporting | 格式错误、缺失资源、解码失败 | 日志能定位具体资源 |

### 资源解码约定

```cpp
struct DecodedImage {
    int width = 0;
    int height = 0;
    PixelFormat format = PixelFormat::RGBA8888;
    std::vector<uint8_t> pixels;
};
```

---

## 8. M5：tools 第一轮

### 目标

先做工具，不要所有问题都在主客户端里调。资源、地图、动画必须有独立查看器。

### asset_viewer

| 编号 | 任务 | 验收标准 |
|---|---|---|
| T1.1 | 枚举 package 内容 | 左侧资源列表可搜索 |
| T1.2 | 显示图片资源 | 能查看尺寸、像素格式、透明区域 |
| T1.3 | 导出 PNG | 方便和原客户端截图对比 |
| T1.4 | 显示 palette | 能切换/查看调色板 |

### map_viewer

| 编号 | 任务 | 验收标准 |
|---|---|---|
| T2.1 | 加载地图 | 可平移/缩放查看 |
| T2.2 | 显示 tile layer | 地图块位置正确 |
| T2.3 | 显示 object layer | 遮挡层可开关 |
| T2.4 | 显示 collision | 阻挡格可开关 |
| T2.5 | 截图对比 | 能导出当前视图截图 |

### anim_viewer

| 编号 | 任务 | 验收标准 |
|---|---|---|
| T3.1 | 加载角色动画 | 可选方向、动作、帧 |
| T3.2 | 播放控制 | pause/play/frame step/speed |
| T3.3 | 显示 anchor/bounds | 锚点、包围盒可视化 |
| T3.4 | 批量检查 | 资源缺帧/异常帧可记录 |

---

## 9. M6：world 层

### 目标

建立角色、地图实体、移动插值、可见性裁剪和 Y 排序。先离线模拟，不依赖真实服务器。

### 目录

```text
src/world/
├─ EntityId.hpp
├─ EntityData.hpp/.cpp
├─ Transform.hpp
├─ Movement.hpp/.cpp
├─ AnimationState.hpp/.cpp
├─ Visibility.hpp/.cpp
├─ YSort.hpp/.cpp
├─ Scene.hpp/.cpp
└─ WorldRenderer.hpp/.cpp
```

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M6.1 | EntityId/EntityData | 角色基础数据 | 可创建/删除大量实体 |
| M6.2 | Transform | tile 坐标、像素坐标、朝向 | 坐标转换正确 |
| M6.3 | Movement interpolation | 服务器位置到客户端平滑移动 | 移动无明显跳变 |
| M6.4 | AnimationState | 动作、方向、帧更新 | 走路动画随移动播放 |
| M6.5 | Visibility culling | camera rect 裁剪 | 屏外角色不提交渲染 |
| M6.6 | Y-sort | sort key 生成与排序 | 角色遮挡顺序正确 |
| M6.7 | WorldRenderer | entity 转 SpriteCmd | 不直接调用 SDL |
| M6.8 | Synthetic population | 随机生成大量角色 | 可生成 1k/4k/8k/16k 角色 |

### 数据组织建议

初期可以用简单 vector；压测阶段改为更 cache-friendly 的结构：

```cpp
struct EntityRenderData {
    EntityId id;
    float x;
    float y;
    uint16_t animId;
    uint16_t frame;
    uint16_t direction;
    uint32_t sortKey;
};
```

---

## 10. M7：16000 角色性能压测

### 目标

验证 SDL_Renderer + SDL_RenderGeometry + Atlas 是否足够。如果不足，再决定是否进入 SDL3_GPU 后端。

### 压测场景

| 场景 | 描述 | 目标 |
|---|---|---|
| P1 | 1000 可见角色，单 sprite | 60 FPS 稳定 |
| P2 | 4000 可见角色，单 sprite | 60 FPS 稳定 |
| P3 | 8000 可见角色，单 sprite | 60 FPS 接近稳定 |
| P4 | 16000 可见角色，单 sprite | 记录瓶颈 |
| P5 | 16000 角色 + 名字关闭 | 测纯 sprite 成本 |
| P6 | 16000 角色 + 血条 | 测 UI 附加成本 |
| P7 | 16000 角色 + 多层 sprite | 测真实复杂度 |

### 必须记录的指标

```text
frame time
logic update time
animation update time
visibility culling time
sort time
render queue build time
renderer flush time
sprite count
draw call count
texture switch count
visible entity count
memory usage
```

### 优化顺序

1. 先做可见性裁剪。
2. 再做 atlas，减少 texture switch。
3. 再做 `SDL_RenderGeometry` 合批。
4. 再优化排序 key 和连续内存。
5. 再限制名字/血条/特效的显示策略。
6. 最后才考虑 SDL3_GPU。

### 决策标准

进入 SDL3_GPU 的条件：

```text
1. 纯 sprite 已经 atlas + geometry batch；
2. CPU 逻辑、排序、动画都不是主瓶颈；
3. renderer flush 或 draw submit 仍明显过高；
4. Windows/macOS/Linux 目标机器上表现不稳定；
5. 需要 shader palette/调色/后处理等能力。
```

---

## 11. M8：net 层

### 目标

建立协议层骨架，先用 mock server 或本地回放包验证状态机，再接真实服务器。

### 目录

```text
src/net/
├─ Socket.hpp/.cpp
├─ Session.hpp/.cpp
├─ Packet.hpp
├─ PacketCodec.hpp/.cpp
├─ Protocol.hpp/.cpp
├─ MessageQueue.hpp/.cpp
└─ MockServer.hpp/.cpp
```

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M8.1 | Socket 抽象 | TCP connect/read/write | 能连接测试地址 |
| M8.2 | Session | 连接状态、重连、心跳 | 状态转移可日志追踪 |
| M8.3 | PacketCodec | 封包、拆包、校验、加解密预留 | 粘包/半包处理正确 |
| M8.4 | Protocol messages | 登录、移动、聊天、实体更新等基础消息 | mock 数据可驱动 world |
| M8.5 | MessageQueue | 网络线程到主线程消息队列 | 无数据竞争 |
| M8.6 | Replay | 保存/回放网络包 | 可重现 bug 场景 |
| M8.7 | Encoding boundary | 聊天/名字编码转换 | 协议层外仍保持 UTF-8 |

---

## 12. M9：ui 层

### 目标

建立适合旧客户端复刻的 UI 系统。建议 retained/immediate hybrid：窗口和控件状态保留，绘制命令每帧即时生成。

### 目录

```text
src/ui/
├─ UiContext.hpp/.cpp
├─ Widget.hpp
├─ Panel.hpp/.cpp
├─ Button.hpp/.cpp
├─ Label.hpp/.cpp
├─ InputBox.hpp/.cpp
├─ ChatBox.hpp/.cpp
├─ ListBox.hpp/.cpp
├─ Tooltip.hpp/.cpp
├─ Layout.hpp/.cpp
└─ Theme.hpp/.cpp
```

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M9.1 | UiContext | 鼠标/键盘/焦点/hover | 控件可聚焦、点击 |
| M9.2 | Panel | 9-grid 背景、拖动、层级 | 面板显示和排序正确 |
| M9.3 | Button | normal/hover/pressed/disabled | 交互状态正确 |
| M9.4 | Label | 单行/多行/颜色 | 文本显示稳定 |
| M9.5 | InputBox | 与 M3 输入框合并完善 | IME、复制粘贴、选择文本 |
| M9.6 | ChatBox | 历史、频道、滚动、超链接预留 | 可承载真实聊天 |
| M9.7 | ListBox | 背包/角色列表基础 | 大量 item 不明显卡顿 |
| M9.8 | Tooltip | 鼠标悬停说明 | 不遮挡关键输入 |
| M9.9 | Theme | 旧客户端 UI skin 复刻 | 可替换皮肤资源 |

---

## 13. M10：Hook/旧客户端对照与迁移

### 目标

由于项目存在 IDA Pro 分析原客户端和 Hook 修改旧行为的背景，新客户端需要保留“旧行为对照”流程，避免凭感觉重写。

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M10.1 | 行为清单 | 从旧客户端记录已分析行为 | 每个行为有来源、截图或日志 |
| M10.2 | 截图对照 | 原客户端 vs 新客户端 | 同场景可并排比较 |
| M10.3 | 坐标对照 | tile/pixel/camera/anchor | 人物站位和遮挡一致 |
| M10.4 | 动画对照 | 帧率、帧序、方向、锚点 | 走路/待机/战斗动作一致 |
| M10.5 | UI 对照 | 面板尺寸、文字位置、输入行为 | 关键 UI 接近原客户端 |
| M10.6 | 协议对照 | 封包字段、状态机、边界条件 | mock/replay 可复现 |
| M10.7 | 差异文档 | `docs/migration_notes.md` | 每个差异记录是否接受或待修复 |

### 建议文档格式

```markdown
# 行为对照：角色移动

- 原客户端版本：xxx
- 观察方式：Hook / 日志 / 截图 / 封包
- 相关地址/函数：可选
- 新客户端实现模块：world/Movement.cpp
- 已知差异：xxx
- 结论：一致 / 可接受差异 / 待修复
```

---

## 14. M11：跨平台与打包

### 目标

先保证 Windows 稳定，再扩展 Linux/macOS。不要一开始把移动端和 Web 作为主目标。

### 任务

| 编号 | 任务 | 平台 | 验收标准 |
|---|---|---|---|
| M11.1 | Windows x64 构建 | MSVC | exe 可运行，DLL 路径正确 |
| M11.2 | Linux 构建 | GCC/Clang | 可启动、IME 基础可用 |
| M11.3 | macOS 构建 | Clang | 可启动、Metal/renderer 正常 |
| M11.4 | 配置文件路径 | 全平台 | user config 不写进程序目录 |
| M11.5 | 日志路径 | 全平台 | crash/bug 可收集日志 |
| M11.6 | 资源打包 | 全平台 | assets/package 可定位 |
| M11.7 | CI 构建 | GitHub Actions 或本地脚本 | 至少 Windows/Linux 自动构建 |

---

## 15. M12：SDL3_GPU 后端评估

### 目标

只有在 SDL_Renderer + batch 达不到目标时才做。不要让 GPU 后端影响前期功能交付。

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M12.1 | 定义 backend 接口差异 | IRenderer 是否足够 | 不修改 world/ui/resource 即可切换 |
| M12.2 | Shader pipeline | 基础 sprite shader | 可绘制 atlas sprite |
| M12.3 | Vertex/index buffer | 动态上传 SpriteCmd | 批量 sprite 可绘制 |
| M12.4 | Texture upload | RGBA8888 上传 | 与 SDL_Renderer 画面一致 |
| M12.5 | Blend modes | alpha/additive | 特效混合正确 |
| M12.6 | 对比压测 | 同样 16000 场景 | 记录和 SDL_Renderer 差异 |
| M12.7 | fallback | GPU 失败回退 SDL_Renderer | 老机器可启动 |

---

## 16. 推荐开发顺序

### 第一轮：最小可运行客户端

```text
M0 工程初始化
M1 platform/window/event/IME
M2 render basic
M3 chat input + font
M4 resource 最小图片加载
```

验收：打开窗口，中文输入，显示字体，加载一张资源图，画一个聊天框。

### 第二轮：资源与地图

```text
M4 package/image/animation/map
M5 asset_viewer/map_viewer/anim_viewer
M2 TextureAtlas/TextureCache
```

验收：可以查看旧资源、播放动画、显示地图。

### 第三轮：世界与角色

```text
M6 entity/movement/animation/y-sort/culling
M7 1k/4k/8k/16k 压测
```

验收：大量角色移动，遮挡顺序正确，性能数据可观测。

### 第四轮：网络和 UI

```text
M8 session/packet/protocol/mock/replay
M9 UI panels/buttons/list/chat
```

验收：mock 协议能驱动角色移动和聊天 UI。

### 第五轮：旧客户端对照和跨平台

```text
M10 behavior comparison
M11 packaging/cross-platform
M12 GPU backend optional
```

验收：关键行为有对照记录，Windows 可发布测试包，Linux/macOS 可构建。

---

## 17. 每周/迭代任务拆分建议

### Iteration 1：工程和窗口

- 创建 CMake/vcpkg 工程。
- SDL3 窗口启动和退出。
- 日志系统。
- Frame loop。
- 事件分发。

交付物：可运行空窗口。

### Iteration 2：IME 和字体

- SDL text input。
- IME composition/commit 分离。
- 字体加载。
- Text rendering。
- 输入框 demo。

交付物：中文聊天框 demo。

### Iteration 3：Renderer 抽象

- IRenderer。
- SpriteCmd。
- RenderQueue。
- SDL_RenderTexture 后端。
- Render stats。

交付物：可以用统一接口画 sprite。

### Iteration 4：Geometry Batch

- TextureAtlas 原型。
- SDL_RenderGeometry batch。
- layer/y/order 排序。
- draw call 统计。

交付物：同屏大量 sprite 基础压测。

### Iteration 5：Resource MVP

- PackageReader。
- ImageDecoder MVP。
- TextureCache。
- asset_viewer MVP。

交付物：能查看和导出旧资源图片。

### Iteration 6：Animation/Map MVP

- AnimationLoader。
- anim_viewer。
- MapLoader。
- map_viewer。

交付物：能看地图和角色动画。

### Iteration 7：World MVP

- EntityData。
- Movement interpolation。
- AnimationState。
- Visibility culling。
- Y-sort。

交付物：大量角色在地图上移动。

### Iteration 8：Performance Pass

- 1k/4k/8k/16k 压测。
- Atlas 优化。
- 排序优化。
- 名字/血条策略。
- 性能报告。

交付物：性能报告和是否需要 SDL3_GPU 的结论。

### Iteration 9：Network MVP

- Session。
- PacketCodec。
- MockServer。
- Replay。
- 聊天/移动消息。

交付物：mock 网络驱动 world 和 chat。

### Iteration 10：UI MVP

- Panel。
- Button。
- Label。
- ListBox。
- ChatBox 完善。

交付物：可用的基础客户端 UI。

---

## 18. 风险清单

| 风险 | 影响 | 缓解 |
|---|---|---|
| 旧资源格式细节不完整 | 地图/动画显示错误 | 工具优先，逐格式验证 |
| IME 全屏候选框异常 | 中文输入体验差 | 先窗口模式稳定，预留自绘候选 UI |
| 16000 角色性能不足 | 客户端极限场景不达标 | Atlas + geometry batch + culling；必要时 SDL3_GPU |
| Y-sort 与原客户端不一致 | 遮挡错误 | 旧客户端截图/Hook 对照 |
| 字体和编码乱码 | 聊天/名字不可用 | UTF-8 内部，协议边界转换 |
| 协议状态机错误 | 登录/移动/聊天异常 | mock/replay 优先 |
| UI 抽象过重 | 开发速度下降 | 先实现 CrossGate 必需控件，不做通用编辑器 |
| 过早引入 GPU 后端 | 功能进度拖慢 | SDL_Renderer 先完成，压测后决策 |

---

## 19. Definition of Done

一个模块完成必须满足：

1. 有清晰 API。
2. 有最小 demo 或工具能单独验证。
3. 有错误日志。
4. 不直接泄漏 SDL 底层对象到上层业务。
5. 有至少一个真实资源或模拟数据测试。
6. 有性能或内存基本观测指标。
7. 写入对应 docs 文档。

---

## 20. 当前最应该先做的 10 个任务

1. 创建 CMake/vcpkg SDL3 工程。
2. 实现窗口、主循环、日志。
3. 实现 SDL text input 和 IME composition/commit 分离。
4. 实现 FontCache 和基础文本绘制。
5. 实现 ChatBox/InputBox demo。
6. 定义 `IRenderer`、`TextureHandle`、`SpriteCmd`。
7. 实现 SDL_Renderer 后端和 RenderQueue。
8. 实现 `SDL_RenderGeometry` sprite batch。
9. 实现 TextureCache 和简单 atlas。
10. 做 1000/4000/8000/16000 sprite 压测 demo。

---

## 21. 最小闭环验收标准

当下面全部满足时，说明 cg-engine 第一阶段可以进入旧资源/协议接入：

```text
1. Windows 下稳定启动和退出。
2. 中文 IME 输入正常。
3. UTF-8 文本显示正常。
4. 可以加载图片并显示。
5. 可以通过 RenderQueue 绘制 sprite。
6. 可以通过 SDL_RenderGeometry 批量绘制 sprite。
7. 可以记录 draw calls、sprite count、frame time。
8. 可以运行 16000 sprite 压测。
9. 业务层没有直接依赖 SDL_Texture/SDL_Renderer。
10. asset_viewer/map_viewer/anim_viewer 至少有一个 MVP。
```
