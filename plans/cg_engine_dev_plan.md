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
2. 进度记录统一写入 `plans/progess.md`，保证后续可以追踪本次变更来源。
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
| M6 | 封包解析与全量协议支持 | 209 个旧客户端封包、i62/es、内嵌 payload | P0 |
| M7 | World 渲染与排序 | entity data、移动插值、可见性裁剪、Y-sort | P0 |
| M8 | 性能压测 | 16000 可见移动角色压测 | P0 |
| M9 | Net 连接层 | session、socket、mock/replay、协议运行链路 | P1 |
| M10 | UI 系统扩展 | 面板、按钮、输入框、列表、tooltip | P1 |
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

进度统一维护在 [progess.md](progess.md)。

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

进度统一维护在 [progess.md](progess.md)。

### 目录

```text
src/render/
├─ IRenderer.hpp
├─ RenderTypes.hpp
├─ SpriteCmd.hpp
├─ RenderStats.hpp
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

建立旧客户端动态 binSet 资源读取和解码管线。M4 只做解码库，不接 viewer，不直接接主程序渲染路径。`resource` 层不暴露 `SDL_Surface*`、`SDL_Texture*`、`SDL_Renderer*`；Graphic 解码输出纯 RGBA8 数据，SDL3 上传交给 `render` 边界用现有 `TextureCache::LoadRgba8` 处理。

成功标准：

1. 可通过 API 传入多组 `BinSetDesc` 动态加载 Graphic/Anime 资源。
2. 重复 `mapNo`、`animeNo` 时后加载 binSet 覆盖前加载 binSet，并记录 warning。
3. 可按 `mapNo` 解码 Graphic 为 RGBA8，输出尺寸、偏移、像素数据。
4. 可按 `animeNo` 读取动作列表和帧列表，帧公开 `graphicSeqNo/x/y/flag`。
5. resource 公开头不包含 SDL 类型。

### 目录

```text
src/resource/
├─ ResourceTypes.hpp
├─ ResourceError.hpp
├─ BinaryReader.hpp/.cpp
├─ BinSetReader.hpp/.cpp
├─ GraphicDecoder.hpp/.cpp
├─ AnimationLoader.hpp/.cpp
├─ Palette.hpp/.cpp
├─ ResourceManager.hpp/.cpp
└─ ResourceStats.hpp
```

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M4.0 | resource 模块占位 | 新建目录并接入 CMake | 构建配置不破坏现有目标 |
| M4.1 | ResourceTypes | 定义 `BinSetDesc`、`GraphicInfo`、`GraphicRef`、`DecodedImage`、`AnimeInfo`、`AnimeAction`、`AnimeFrame` | 公开类型无 SDL 类型 |
| M4.2 | ResourceError | 定义错误码、错误文本、路径和资源 id 上下文 | 失败日志能定位文件和资源 |
| M4.3 | BinaryReader | 显式小端读取、seek/read、边界检查 | 不用 Default 编码，不用结构体硬转 |
| M4.4 | GraphicInfoReader | 读取 40 字节 `GraphicInfo` 行 | `GraphicInfo_66.bin` 可枚举 |
| M4.5 | Graphic seq 索引 | 建 `binSetIndex + seqNo -> GraphicRef` | 同 binSet 重复 seq 后者覆盖并 warning |
| M4.6 | Graphic mapNo 索引 | 建 `mapNo -> GraphicRef` | 多 binSet 重复 mapNo 后者覆盖 |
| M4.7 | Palette | 读取 `.cgp` 中间 224 色，补旧客户端固定头尾 16+16 色 | `palet_00.cgp` 得到 256 色 RGBA |
| M4.8 | Graphic 块头读取 | 读取 `RD`、version、width、height、blockSize、可选 palSize | 非 `RD` 返回格式错误 |
| M4.9 | 非压缩 Graphic | 解普通/CGEX indexed 数据 | 输出 indexed pixels |
| M4.10 | 压缩 Graphic RLE | 按 IDA `Graphic::readGraphicData` 行为解压 | 覆盖 literal、fill、zero 长短分支 |
| M4.11 | Indexed 转 RGBA8 | 默认 palette 或 CGEX 内嵌 palette 转色 | 输出 `width * height * 4` |
| M4.12 | 透明规则 | 不声明 `GraphicInfo.flag & 0x8` 语义；透明规则需 IDA 追溯后再落地 | 透明规则有可追溯证据 |
| M4.13 | GraphicDecoder | 封装 `Decode(GraphicRef)` | 输出 `DecodedImage`，不含 SDL |
| M4.14 | AnimeInfoReader | 读取 12 字节 `AnimeInfo` 行 | `AnimeInfo_4.bin` 可枚举 |
| M4.15 | animeNo 索引 | 建 `animeNo -> AnimeRef` | 多 binSet 重复 animeNo 后者覆盖 |
| M4.16 | Anime action 读取 | 读取 dir/action/duration/frameCount/pal/revert | action 数等于 info.count |
| M4.17 | Anime frame 读取 | 读取 10 字节帧 | 公开 `graphicSeqNo/x/y/flag` |
| M4.18 | Anime 帧关联 | 帧按所属 binSet 的 seq 索引找 Graphic | 找不到 warning，不崩溃 |
| M4.19 | ResourceManager | 实现 `LoadBinSets` 和查询解码 API | 调用方不接触文件句柄 |
| M4.20 | ResourceStats | 记录 binSet、graphic、anime、覆盖、错误数量 | 日志可定位加载结果 |
| M4.21 | 最小验证入口 | 覆盖 info、palette、RLE、透明、覆盖、anime frame | 不启动 SDL |
| M4.22 | 实物资源抽样 | 使用 `D:\SOFTSTAR\bin` 抽样基础/Ex/V3/Joy/PUK3 文件 | 解码样本尺寸和像素长度正确 |
| M4.23 | Texture 上传验证 | 在 render/app 边界用 `TextureCache::LoadRgba8` 上传一张 `DecodedImage` | resource 不包含 SDL 头 |
| M4.24 | 进度记录与提交 | 更新 `plans/progess.md` 并小步提交 | 不提交 build、third_party、本地资源 |

### 资源解码约定

```cpp
struct BinSetDesc {
    std::string name;
    std::filesystem::path graphicInfoPath;
    std::filesystem::path graphicDataPath;
    std::filesystem::path animeInfoPath;
    std::filesystem::path animeDataPath;
    std::filesystem::path coordinateInfoPath;
    std::filesystem::path coordinateDataPath;
};

struct DecodedImage {
    int width = 0;
    int height = 0;
    int offsetX = 0;
    int offsetY = 0;
    int mapNo = 0;
    cg::render::PixelFormat format = cg::render::PixelFormat::Rgba8;
    std::vector<std::uint8_t> pixels;
};
```

推荐最小 API：

```cpp
bool LoadBinSets(std::span<const BinSetDesc> binSets, const std::filesystem::path& defaultPalettePath);
std::optional<GraphicRef> FindGraphicByMapNo(int mapNo) const;
std::optional<GraphicRef> FindGraphicBySeq(int binSetIndex, int seqNo) const;
std::optional<DecodedImage> DecodeGraphic(const GraphicRef& ref) const;
std::optional<AnimationClip> LoadAnimation(int animeNo) const;
```

SDL3 上传示例由调用方在 `render` 边界处理：

```cpp
textureCache.LoadRgba8(key, image.width, image.height, image.pixels.data(), image.width * 4);
```

### 格式规则

1. `GraphicInfo` 磁盘行长 40 字节：`seqNo`、`offset`、`length`、`offsetX`、`offsetY`、`width`、`height`、`sizeX`、`sizeY`、`flag`、5 字节保留、`mapNo`。
2. `Graphic` 数据头：`RD`、`version`、`padding`、`width`、`height`、`blockSize`；`version & 1` 表示 RLE 压缩，`version & 2` 表示 CGEX。
3. RLE 行为以 IDA `Graphic::readGraphicData` 为准，不以 `cgrw` 的旧实现为唯一依据。
4. palette 默认使用调用方传入的 `palet_00.cgp`；CGEX 有内嵌 palette 时优先使用内嵌 palette。
5. 暂无 IDA 证据支持 `GraphicInfo.flag & 0x8` 控制色号 0 透明；不得按该位实现透明规则，后续需追溯旧客户端贴图/调色板路径后再补。
6. `AnimeInfo` 磁盘行长 12 字节：`animeNo`、`offset`、`count`、`flag`。
7. `AnimeAction` 读取 `dir`、`action`、`duration`、`frameCount`、`pal`、`revert`、`padding`；若旧格式无扩展字段，再兼容回退。
8. `AnimeFrame` 磁盘行长 10 字节，公开为 `graphicSeqNo/x/y/flag`。

### subAgent 拆分

1. 编码 subAgent A：只处理 `ResourceTypes`、`ResourceError`、`BinaryReader`。
2. 编码 subAgent B：只处理 `GraphicInfoReader`、`Palette`、`GraphicDecoder`。
3. 编码 subAgent C：只处理 `AnimeInfoReader`、`AnimationLoader`。
4. 主线程：整合 `ResourceManager`、CMake、进度记录、提交。
5. 验证 subAgent：只做验证，不修改实现文件。

---

## 8. M5：tools 第一轮

### 目标

先补配置和地图数据，再做独立查看器。M5 不在主客户端里调地图行为；所有地图、资源、动画问题先通过工具验证。地图参考 IDA `0x0046CF00`、`0x00468520`，以及 `D:\Source\cgmsv\twcg_hook\src\hooks\UI\automap.cpp`、`D:\Source\cgmsv\twcg_hook\src\hooks\UI\UIControls\MapExControl.cpp`。binSet 顺序参考 `H:\Games\cgmsv_21.2a\CrossGate7.1.0\cgmsv.yml`。

成功标准：

1. 可通过 `yaml-cpp` 读取 `cgmsv.yml`，并按 `BinLoader` 顺序生成 `BinSetDesc`。
2. 可读取旧客户端 `map\0\${floor}.dat` 和 `map\mapId\serverNo\${floor}.dat`。
3. 可生成自动地图 RGBA8 buffer，tile/object/collision 可开关。
4. 可用独立 viewer 查看 Graphic、Anime、Map，不要求接入主客户端流程。
5. `resource` 公开头不暴露 SDL 类型；SDL 上传只在 viewer/render 边界发生。

### M5.0 yaml-cpp 依赖

`yaml-cpp` 是本阶段用户明确要求的外部依赖例外，来源固定为 `https://github.com/jbeder/yaml-cpp`，版本固定 `yaml-cpp-0.9.0`。除该项外，仍不新增其他外部依赖。

| 编号 | 任务 | 验收标准 |
|---|---|---|
| M5.0.1 | 新增 `scripts/download_yaml_cpp.ps1` | 可下载源码 zip 到 `third_party/yaml-cpp/yaml-cpp-0.9.0` |
| M5.0.2 | CMake 引入本地源码 | `CG_YAML_CPP_ROOT` 指向本地目录，`add_subdirectory` 可用 |
| M5.0.3 | 关闭无关构建项 | `YAML_BUILD_SHARED_LIBS=OFF`、`YAML_CPP_BUILD_TESTS=OFF`、`YAML_CPP_BUILD_TOOLS=OFF`、`YAML_CPP_INSTALL=OFF` |
| M5.0.4 | 链接目标 | `cg-client` 可链接 `yaml-cpp::yaml-cpp` |

### M5.1 binSet yml loader

| 编号 | 任务 | 验收标准 |
|---|---|---|
| M5.1.1 | 读取 `BinLoader` 数组 | BASE/EX/V3/Puk2/Puk3/Joy/Joy_EX/Joy_CH1/TAO1/A1/TAOPuk4 顺序一致 |
| M5.1.2 | 生成 `BinSetDesc` | `Name/Graphic/GraphicInfo/Anime/AnimeInfo/Coordinate/CoordinateInfo` 字段映射正确 |
| M5.1.3 | 路径规范化 | 相对路径以 yml 所在目录为根，空路径保留为空并跳过资源 |
| M5.1.4 | 错误报告 | 缺少 `BinLoader` 或字段类型错误时返回明确错误 |

### M5.2 MapData/MapLoader

`mapId` 取值：0=固定地图，1=迷宫地图，2=玩家出租屋，3=家族地图，5=lua地图。

| 编号 | 任务 | 验收标准 |
|---|---|---|
| M5.2.1 | 定义 `MapData` | 保存 `mapId/serverNo/floor/width/height/tileData/objData/hitData`，公开头无 SDL 类型 |
| M5.2.2 | 构造地图路径 | `mapId == 0` 使用 `map\0\${floor}.dat`，否则使用 `map\mapId\serverNo\${floor}.dat` |
| M5.2.3 | 读取地图文件 | 按 `${floor}.dat` 路径校验 `MAP\1`、4 个 int header、三层 `uint16` 数据长度 |
| M5.2.4 | 边界保护 | 宽高必须大于 0，cell 数不得超过明确上限，读取失败有错误信息 |

### M5.3 自动地图规则

| 编号 | 任务 | 验收标准 |
|---|---|---|
| M5.3.1 | `DecodeMapCell` 校对 | `raw < 0x10000` 查 IDA `dword_C35010`，否则返回 raw；实现前先导出或确认查表来源 |
| M5.3.2 | 地图投影 | `screenX = 1 + 2 * (x + y)`，`screenY = mapWidth + 1 + y - x` |
| M5.3.3 | tile 颜色 | 普通地图用 `MapNoIndex[800000]`，新地图 `220000..265535` 用 `newMapRes` |
| M5.3.4 | object 覆盖 | 参考 `GraphicInfo.sx/sy` 与 `GraphicInfo::flag % 100` 计算覆盖范围，不只覆盖 object 所在格 |
| M5.3.5 | collision overlay | 只显示阻挡层开关，不扩展寻路规则 |
| M5.3.6 | RGBA8 输出 | palette 优先，缺色时按 `AUT_DATA` fallback，可上传到 `TextureCache::LoadRgba8` |

### M5.4 map_viewer MVP

| 编号 | 任务 | 验收标准 |
|---|---|---|
| M5.4.1 | 加载 yml 和 map dat | 可选择 binSet 配置和地图文件根目录 |
| M5.4.2 | 显示自动地图 | 可平移、缩放，坐标和鼠标反算坐标正确 |
| M5.4.3 | 图层开关 | tile/object/collision 可独立开关 |
| M5.4.4 | 截图导出 | 能导出当前视图用于旧客户端对照 |

### M5.5 asset_viewer MVP

| 编号 | 任务 | 验收标准 |
|---|---|---|
| M5.5.1 | 按 yml 枚举 Graphic | 可按 binSet、seqNo、mapNo 搜索 |
| M5.5.2 | 显示图片资源 | 能查看尺寸、偏移、像素格式、透明区域 |
| M5.5.3 | 显示 palette | 能查看默认 palette 和 CGEX 内嵌 palette |
| M5.5.4 | 导出 PNG | 方便和原客户端截图对比 |

### M5.6 anim_viewer MVP

| 编号 | 任务 | 验收标准 |
|---|---|---|
| M5.6.1 | 按 yml 加载 Anime | 可按 animeNo 选择动画 |
| M5.6.2 | 动作和方向选择 | 可选 dir/action/frame |
| M5.6.3 | 播放控制 | 支持 pause/play/frame step/speed |
| M5.6.4 | anchor/bounds | 锚点、包围盒可视化 |
| M5.6.5 | 异常记录 | 资源缺帧、Graphic 缺失、异常帧写 warning |

### M5.7 验证与提交

| 编号 | 任务 | 验收标准 |
|---|---|---|
| M5.7.1 | 依赖验证 | 下载脚本后存在 `third_party/yaml-cpp/yaml-cpp-0.9.0/CMakeLists.txt` |
| M5.7.2 | 配置验证 | CMake configure 能找到并链接 `yaml-cpp::yaml-cpp` |
| M5.7.3 | 实物资源验证 | 抽样读取 `cgmsv.yml`、Graphic/Anime、地图 dat |
| M5.7.4 | 对照验证 | raw cell 解码、screen 坐标、object 覆盖范围与 hook/IDA 一致 |
| M5.7.5 | 流程记录 | 每个可验证小步写入 `plans/progess.md` 并提交；验证交由独立 subAgent |

---

## 9. M6：封包解析与全量协议支持

### 目标

建立旧客户端封包解析层，支持 `plans/CG封包.xlsx` 中全部 209 个封包：客户端->服务端 100 个、服务端->客户端 109 个。M6 只负责封包编解码和内嵌 payload 结构化，不实现 TCP session、重连、网络线程；连接运行链路放到 M9。

成功标准：

1. `plans/protocol_m6_inventory.md` 列出 209 个封包，包含方向、状态、来源、format 是否为空、是否疑似内嵌 payload。
2. 默认数值编码为 `i62`，即旧端 base62：`0-9a-zA-Z`，支持负号。
3. `es` 字符串转义兼容旧 `nrproto_escapeString/descapeString` 行为。
4. 明文 token 可按空格和 `0x17` 拆分，可回写旧格式 token 串。
5. 生产代码按 `send.hpp`、`receive.hpp` 当前 per-packet struct 风格实现；封包覆盖必须包含全部 209 个封包，`CCL` 和 `TgX` 不因旧 C 分发缺失而跳过。
6. 53 个 xlsx 空 format 封包必须用 C 代码或 IDA MCP 补来源；`unknownN` 只能作为盘点/迁移临时字段，单包完成前必须明确字段来源或明确记录为可回写未知 payload。
7. 内嵌 payload 封包必须在单包目标内完成 payload 解析和组装，支持分隔符、回写和 unknown 字段保留。
8. 每个封包至少有 1 条明文 fixture；含 payload 的封包必须有二级 payload 样本，且每个 payload 封包独立验收。
9. 现有 `src/packet` 中 `send::ClientLogin`、`receive::ClientLoginResult`、`utils.hpp` TODO 被收敛进统一 codec 与具体封包结构，不另起重复协议目录。
10. 209 个封包全部完成主方向 parse 或 encode 后，才允许进入下一阶段；全量 parse/build roundtrip 通过；缺 token、非法 i62、未知包返回明确错误。

### 目录

```text
src/packet/
├─ packet.hpp
├─ send.hpp
├─ receive.hpp
├─ utils.hpp                 # i62 等小颗粒协议工具收敛于此
├─ PacketTypes.hpp
├─ LegacyStringCodec.hpp/.cpp
├─ LegacyTokenCodec.hpp/.cpp
├─ LegacyEnvelopeCodec.hpp/.cpp
├─ PacketParser.hpp/.cpp
├─ PacketBuilder.hpp/.cpp
├─ PayloadCodec.hpp/.cpp
├─ MapPayload.hpp/.cpp
├─ CharacterPayload.hpp/.cpp
├─ ItemSkillPetPayload.hpp/.cpp
├─ UiMessagePayload.hpp/.cpp
├─ BattleTradeGuildPayload.hpp/.cpp
└─ AuthCharacterPackets.hpp/.cpp

tests/packet/
├─ LegacyCodecTest.cpp
├─ PacketCoverageSchemaTest.cpp
└─ PayloadCodecTest.cpp

tests/fixtures/protocol/
├─ schema/*
└─ *.pkt
```

schema 只允许作为测试/fixture 资料存在，用于覆盖率、字段顺序、来源标记和 roundtrip 验证；不得作为生产代码构成，不生成封包类型，不替代 `send.hpp`/`receive.hpp` 风格的显式结构。

### 阶段推进门禁

1. 每个封包都是独立完成单位，不能只完成组级 codec 后推进下一阶段。
2. 客户端接收封包（服务端->客户端）必须完成完整 `Parse`；客户端发送封包（客户端->服务端）必须完成完整 `Encode`。
3. 含 payload 封包必须在同一目标内完成 payload 解析和组装；字段待补目标不能进入 parse/encode 实现验收。
4. `unknownN` 只能作为补字段过程中的临时占位；若最终仍未知，必须保留原始字段并证明 parse->encode 可无损回写，且在目标遗留中标明来源缺口。
5. 含 payload 封包按 `plans/protocol_m6_inventory.md` 的“含 payload 独立目标清单”逐个推进，目标名格式如 `M6.10.04 地图封包-aBS 服务端->客户端 SEND_MAP_DATA`。
6. 单包完成标准：代码实现、明文 fixture、payload 样本（如有）、主方向 parse/encode 验证、错误输入验证、进度记录和独立 subAgent 验证均完成。
7. M6.10-M6.15 只表示分组，不是可一次性关闭的实现目标；分组内所有单包完成后，该分组才可完成。

### 来源优先级

1. `plans/CG封包.xlsx` 主表 `封包` 的非空定义。
2. `src/packet/` 现有原型：用于确定当前实现目录、命名空间、已存在登录封包结构和 TODO 收敛点；该目录目前仅为 2 个封包草稿，未接入 CMake 或业务代码，不作为完整协议事实来源。
3. `D:\Source\cgmsv\cgmsv_server\nrproto_serv.c` 的 RECV/SEND 宏、dispatch 参数、send builder。
4. `D:\Source\cgmsv\cgmsv_server\callfromcli.c` 的业务调用语义。
5. `D:\Source\cgmsv\cgmsv_server\nrproto_util.c` 的 i62、es、token、envelope 行为。
6. IDA MCP 中旧客户端 `cg2d.exe.i64` 的客户端行为。

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M6.0 | 协议资料盘点 | 读取 xlsx、C 代码和 `src/packet` 原型，生成协议清单 | `plans/protocol_m6_inventory.md` 包含 209 个封包，并标出 `src/packet` 已有 `JFVf/whV` |
| M6.1 | 协议基础类型 | 在 `src/packet` 收敛 `PacketCode`、方向、字段类型、错误码、来源标记和基础 `Packet` 表达；具体封包继续参考 `send.hpp`、`receive.hpp` 的 struct 风格 | `PacketTypes` 公开头无 SDL 类型；`send` 表示客户端发出，`receive` 表示客户端接收；解析失败不返回半初始化对象 |
| M6.2 | i62 编解码 | 迁移 base62 数值编码；统一说明 `i62` 即旧端 base62 数值格式 | 与 `nrproto_a62toi/cnv10to62` 行为一致 |
| M6.3 | es 字符串转义 | 迁移旧字符串 escape/descape | 空格、换行、回车、反斜杠和旧 2 字节字符行为一致 |
| M6.4 | 明文 token codec | 拆分/构造旧明文封包 token | 支持空格、`0x17` 和行尾处理 |
| M6.5 | 加密压缩 envelope | encode64/decode64、jEncode/jDecode、ringo 开关 | 短包未压缩路径 roundtrip 通过；压缩路径行为明确 |
| M6.6 | 封包覆盖与测试 schema | 生产代码不做 schema 驱动；测试 schema 覆盖 209 个封包，并核对 `src/packet` 已有登录封包原型 | 测试 schema 覆盖 100 个 RECV、109 个 SEND；`CCL`、`TgX`、`JFVf`、`whV` 存在 |
| M6.7 | 外层封包解析 | token 到统一 packet 结构，并定义缺 token、非法 i62、字段数不匹配、未知 code 的错误模型 | 外层解析能力可复用；单包 Parse 完成仍按 M6.10-M6.15 独立验收 |
| M6.8 | 外层封包构造 | 统一 packet 到 token 串，并复用 M6.7 错误码表达构造失败 | 外层构造能力可复用；单包 Encode 完成仍按 M6.10-M6.15 独立验收 |
| M6.9 | 内嵌 payload 框架 | 二级分隔和转义 | 支持 `|`、`,`、`\z/\c/\y/\n` |
| M6.10 | 地图/对象 payload | 地图、对象、坐标封包按 code 独立目标推进 | 每个目标完成主方向 parse/encode、payload parse/build、fixture 和错误验证；示例：`M6.10.04 地图封包-aBS 服务端->客户端 SEND_MAP_DATA` |
| M6.11 | 角色/属性 payload | 角色、profile、属性封包按 code 独立目标推进 | 每个目标完成主方向 parse/encode、payload parse/build、fixture 和错误验证 |
| M6.12 | 物品/技能/宠物 payload | 物品、技能、宠物封包按 code 独立目标推进 | 每个目标完成主方向 parse/encode、payload parse/build、fixture 和错误验证 |
| M6.13 | 聊天/窗口/系统 payload | 聊天、窗口、系统消息封包按 code 独立目标推进 | 每个目标完成主方向 parse/encode、payload parse/build、fixture 和错误验证 |
| M6.14 | 战斗/交易/家族 payload | 战斗、交易、家族、社交封包按 code 独立目标推进 | 每个目标完成主方向 parse/encode、payload parse/build、fixture 和错误验证 |
| M6.15 | 登录/角色管理封包 | 登录、角色列表、角色管理按 code 独立目标推进；优先迁移 `send::ClientLogin` 和 `receive::ClientLoginResult` | 每个目标完成主方向 parse/encode、fixture 和错误验证；含 payload 时完成 payload parse/build；`whV.result` 枚举和 `data` 空值/`fail`/`block`/`busy` 保留 |
| M6.16 | xlsx 空字段补全 | 53 个空 format 行补来源；被单包目标依赖时先补该 code 再实现 | C/IDA 来源可追踪；`unknownN` 只作为临时占位或无损回写字段，不单独视为完成 |
| M6.17 | fixture 与回归样本 | 每个单包目标生成明文样本 | 209 条 fixture；payload 包含二级 payload 样本；fixture 必须随单包目标一起提交 |
| M6.18 | 验证入口 | 最小协议测试程序/单测 | 全量 parse/build roundtrip 通过；异常输入不崩溃 |
| M6.19 | 进度记录与提交 | 每个单包目标更新 `plans/progess.md` 并小步提交 | 每个可验证小步记录并提交；验证交由独立 subAgent |

### subAgent 拆分

1. 编码 subAgent：每次只处理一个 M6.x 小步。
2. 验证 subAgent：每次只验证刚完成的小步，不修改实现文件。
3. 主线程：统筹计划文件、整合结果、记录进度和提交。

---

## 10. M7：world 层

### 目标

建立参考旧客户端 IDA 行为的 world 渲染链路。M7 先处理 normal scene 的地板 tile atlas、地图 obj 纹理缓存、渲染分层排序，再接入角色实体、移动插值、动画、裁剪和压测数据。

成功标准：

1. 地板纹理按 map scene 切换，并缓存为常驻显存的 `sprite_atlas`。
2. 原 `SceneKey` 命名收敛为 `MapTileAtlasCacheKey`，只用于地板 tile atlas。
3. 地图 obj 纹理不进入地板 atlas，独立缓存，容量上限 100，LRU + 30 秒 idle timer 清退。
4. tile/object 入队、偏移和排序先参考 IDA，再映射到现有 `RenderQueue`。
5. world 公开头不暴露 `SDL_Window*`、`SDL_Renderer*`、`SDL_Texture*`、TTF/Win32 类型。

### 目录

```text
src/world/
├─ EntityId.hpp
├─ EntityData.hpp
├─ EntityStore.hpp/.cpp
├─ Transform.hpp/.cpp
├─ Movement.hpp/.cpp
├─ MovementAnimation.hpp/.cpp
├─ AnimationState.hpp/.cpp
├─ VisibilityCulling.hpp/.cpp
├─ MapTileAtlasCacheKey.hpp
├─ SceneGroundAtlasCache.hpp/.cpp
├─ SceneGroundSpriteBuilder.hpp/.cpp
├─ MapObject.hpp
├─ MapObjectTextureCache.hpp/.cpp
├─ MapObjectSpriteBuilder.hpp/.cpp
├─ IdaRenderRules.hpp/.cpp
├─ EntitySpriteBuilder.hpp/.cpp
├─ SyntheticPopulation.hpp/.cpp
└─ WorldRenderer.hpp/.cpp
```

### IDA 对齐依据

实现前必须先对齐以下旧客户端行为，不用自动地图规则替代主世界绘制规则：

| 行为 | IDA 来源 | M7 使用方式 |
|---|---|---|
| map cell 解码 | `0x46CF00` | `raw < 0x10000` 查表，否则原样返回 |
| normal scene 绘制链路 | `0x46CF60` | 地板 tile、地图 obj 入队顺序和分层依据 |
| 地图读取/准备链路 | `0x469030`、`0x4693F0`、`0x46D9C0` | 校对 tile/object 数据进入渲染前的处理 |
| Graphic mapNo 转索引 | `Graphic::getMapNoIndex(0x45A8B0)` | `imageNo > 999` 先转 graphic index |
| Graphic 偏移/尺寸/flag | `getXY/getWH/getFlag1/getFlag2` | sprite 位置偏移、裁剪和 object 分组 |
| 普通 graphic 入队 | `Renderer::DrawGraphic(0x521B10)` | `x/y + Graphic::getXY` 后入队 |
| 复杂 graphic 入队 | `Renderer::DrawGraphic3(0x520970)` | 仅作为后续特殊模式参考，不先扩大范围 |
| object 分组 | `sub_46BE50`、`sub_46BD60` | `MapNo == 1`、`flag2 == 3`、其他 obj 分组 |
| 渲染排序 | `Renderer::SortTexture(0x520790)`、`0x5222A0` | layer 升序，同 layer 按 cacheIndex 逆序 |

### 地板 atlas 与 obj 缓存

`MapTileAtlasCacheKey` 固定字段：

```cpp
struct MapTileAtlasCacheKey {
    int mapId = 0;
    int serverNo = 0;
    int floor = 0;
    uint32_t tileLayerMask = 0;
    uint64_t mapGeneration = 0;
    uint64_t resourceGeneration = 0;
};
```

规则：

1. `MapTileAtlasCacheKey` 只标识地板 tile atlas，不替代完整 scene 标识。
2. 地板 atlas 只包含 map tile；不包含地图 obj、角色、宠物、特效、UI。
3. tile raw 值先按 IDA `0x46CF00` 解码；`decoded <= 999` 不绘制。
4. `5000..5009`、`7664..7679` 进入 tile layer 1；其他 `decoded > 999` 进入 tile layer 5。
5. 地板 atlas 上传到显存后释放 CPU 拼图临时 RGBA；atlas 随 key 常驻，直到 map/resource generation 失效。
6. 地图 obj 纹理走 `MapObjectTextureCache`，容量 100，命中刷新 LRU，30 秒未使用清退。
7. 地图 obj 分组参考 `sub_46BE50/sub_46BD60` 和 `Graphic::getFlag2`，不得简单并入地板层。

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M7.0 | IDA 渲染行为校对 | 落地 `IdaRenderRules` 对照表，记录 tile decode、DrawGraphic、obj 分组、SortTexture 规则 | 每条规则可追溯到 IDA 地址；未确认规则不得写成业务结论 |
| M7.1 | EntityId/EntityData | 角色基础数据；`EntityData` 不保存 `objectIndex`，后续由实际地图 `Obj` 对象保持 | 可创建/删除大量实体，id 来源边界明确 |
| M7.2 | Transform | tile 坐标、像素坐标、朝向；屏幕投影先按 IDA 校对 | 坐标转换测试通过，公开头无 SDL 类型 |
| M7.3 | MapTileAtlasCacheKey | 替换方案中的 `SceneKey` 命名，定义 hash/equality | 相同 map/key 命中，不同 generation/layerMask 不复用 |
| M7.4 | SceneGroundAtlasCache | 按 `MapTileAtlasCacheKey` 构建地板 sprite atlas，上传显存并释放临时 RGBA | scene 切换命中缓存；失效时重建 |
| M7.5 | 地板 tile 入队 | 按 IDA decode 和 layer 规则生成 chunk `SpriteCmd` | `<=999` 跳过，特殊范围 layer=1，普通 tile layer=5 |
| M7.6 | MapObjectTextureCache | 地图 obj 纹理独立缓存，容量 100，LRU + 30 秒 timer 清退 | 超量淘汰最久未用；超时未用被清退 |
| M7.7 | 地图 obj 入队分组 | 参考 `sub_46BE50/sub_46BD60`、`MapNo`、`flag2` 生成 obj 渲染组 | obj 不进入地板 atlas；分组 fixture 通过 |
| M7.8 | IDA 排序映射 | 将 layer 升序、同 layer cacheIndex 逆序映射到现有 `RenderQueue` | 可用 `order = UINT_MAX - enqueueIndex` 或等价策略通过排序测试 |
| M7.9 | Movement interpolation | 服务器位置到客户端平滑移动 | 移动无明显跳变 |
| M7.10 | AnimationState | 动作、方向、帧更新，帧偏移参考 `anim_viewer` 已验证逻辑 | 走路动画随移动播放 |
| M7.11 | Visibility culling | camera rect 裁剪 tile、obj、entity | 屏外对象不提交渲染 |
| M7.12 | WorldRenderer | 地板 atlas、地图 obj、entity 转 `SpriteCmd` | 不直接调用 SDL；提交顺序稳定 |
| M7.13 | Synthetic population | 随机生成大量角色 | 可生成 1k/4k/8k/16k 角色 |
| M7.14 | 进度记录与提交 | 每个可验证小步写入 `plans/progess.md` | 文档、测试、验证结果可追踪 |

### 数据组织建议

初期可以用简单 vector；压测阶段再改为更 cache-friendly 的结构。排序 key 不直接假设等同旧端 Y-sort，必须先兼容 IDA layer/cacheIndex 语义：

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

渲染提交默认顺序：

1. 地板 tile atlas chunk。
2. 地图 obj 分组。
3. entity/角色。
4. effect/name/overlay。
5. UI。

同层具体先后以 IDA 校对结果为准；未确认角色遮挡前，不声明与旧端完全一致。

### 验收场景

1. map cell decode fixture：`raw < 0x10000` 查表，`raw >= 0x10000` 原样。
2. tile layer fixture：`5000..5009`、`7664..7679` 为 layer 1，其他 `>999` 为 layer 5。
3. `DrawGraphic` fixture：mapNo 转 graphic index 后叠加 `Graphic::getXY` 偏移。
4. sort fixture：layer 升序，同 layer cacheIndex 逆序。
5. obj cache fixture：100 容量淘汰和 30 秒 idle 清退。
6. atlas fixture：相同 `MapTileAtlasCacheKey` 命中，不同 generation/layerMask 失效。
7. world 公开头检查：不得出现 SDL/TTF/Win32 类型。

---

## 11. M8：16000 角色性能压测

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

## 12. M9：net 连接层

### 目标

在 M6 封包编解码基础上建立网络连接层，先用 mock server 或本地回放包验证状态机，再接真实服务器。

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
| M9.1 | Socket 抽象 | TCP connect/read/write | 能连接测试地址 |
| M9.2 | Session | 连接状态、重连、心跳 | 状态转移可日志追踪 |
| M9.3 | Packet stream | 接入 M6 token/envelope codec | 粘包/半包处理正确 |
| M9.4 | Protocol messages | 登录、移动、聊天、实体更新等基础消息 | mock 数据可驱动 world |
| M9.5 | MessageQueue | 网络线程到主线程消息队列 | 无数据竞争 |
| M9.6 | Replay | 保存/回放网络包 | 可重现 bug 场景 |
| M9.7 | Encoding boundary | 聊天/名字编码转换 | 协议层外仍保持 UTF-8 |

---

## 13. M10：ui 层

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
| M10.1 | UiContext | 鼠标/键盘/焦点/hover | 控件可聚焦、点击 |
| M10.2 | Panel | 9-grid 背景、拖动、层级 | 面板显示和排序正确 |
| M10.3 | Button | normal/hover/pressed/disabled | 交互状态正确 |
| M10.4 | Label | 单行/多行/颜色 | 文本显示稳定 |
| M10.5 | InputBox | 与 M3 输入框合并完善 | IME、复制粘贴、选择文本 |
| M10.6 | ChatBox | 历史、频道、滚动、超链接预留 | 可承载真实聊天 |
| M10.7 | ListBox | 背包/角色列表基础 | 大量 item 不明显卡顿 |
| M10.8 | Tooltip | 鼠标悬停说明 | 不遮挡关键输入 |
| M10.9 | Theme | 旧客户端 UI skin 复刻 | 可替换皮肤资源 |

---

## 14. M11：Hook/旧客户端对照与迁移

### 目标

由于项目存在 IDA Pro 分析原客户端和 Hook 修改旧行为的背景，新客户端需要保留“旧行为对照”流程，避免凭感觉重写。

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M11.1 | 行为清单 | 从旧客户端记录已分析行为 | 每个行为有来源、截图或日志 |
| M11.2 | 截图对照 | 原客户端 vs 新客户端 | 同场景可并排比较 |
| M11.3 | 坐标对照 | tile/pixel/camera/anchor | 人物站位和遮挡一致 |
| M11.4 | 动画对照 | 帧率、帧序、方向、锚点 | 走路/待机/战斗动作一致 |
| M11.5 | UI 对照 | 面板尺寸、文字位置、输入行为 | 关键 UI 接近原客户端 |
| M11.6 | 协议对照 | 封包字段、状态机、边界条件 | mock/replay 可复现 |
| M11.7 | 差异文档 | `docs/migration_notes.md` | 每个差异记录是否接受或待修复 |

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

## 15. M12：跨平台与打包

### 目标

先保证 Windows 稳定，再扩展 Linux/macOS。不要一开始把移动端和 Web 作为主目标。

### 任务

| 编号 | 任务 | 平台 | 验收标准 |
|---|---|---|---|
| M12.1 | Windows x64 构建 | MSVC | exe 可运行，DLL 路径正确 |
| M12.2 | Linux 构建 | GCC/Clang | 可启动、IME 基础可用 |
| M12.3 | macOS 构建 | Clang | 可启动、Metal/renderer 正常 |
| M12.4 | 配置文件路径 | 全平台 | user config 不写进程序目录 |
| M12.5 | 日志路径 | 全平台 | crash/bug 可收集日志 |
| M12.6 | 资源打包 | 全平台 | assets/package 可定位 |
| M12.7 | CI 构建 | GitHub Actions 或本地脚本 | 至少 Windows/Linux 自动构建 |

---

## 16. M13：SDL3_GPU 后端评估

### 目标

只有在 SDL_Renderer + batch 达不到目标时才做。不要让 GPU 后端影响前期功能交付。

### 任务

| 编号 | 任务 | 说明 | 验收标准 |
|---|---|---|---|
| M13.1 | 定义 backend 接口差异 | IRenderer 是否足够 | 不修改 world/ui/resource 即可切换 |
| M13.2 | Shader pipeline | 基础 sprite shader | 可绘制 atlas sprite |
| M13.3 | Vertex/index buffer | 动态上传 SpriteCmd | 批量 sprite 可绘制 |
| M13.4 | Texture upload | RGBA8888 上传 | 与 SDL_Renderer 画面一致 |
| M13.5 | Blend modes | alpha/additive | 特效混合正确 |
| M13.6 | 对比压测 | 同样 16000 场景 | 记录和 SDL_Renderer 差异 |
| M13.7 | fallback | GPU 失败回退 SDL_Renderer | 老机器可启动 |

---

## 17. 推荐开发顺序

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

### 第三轮：封包解析

```text
M6 protocol inventory/i62/es/token/payload/fixtures
```

验收：209 个旧封包可解析和回写，内嵌 payload 可保留 unknown 字段并 roundtrip。

### 第四轮：世界与角色

```text
M7 entity/movement/animation/y-sort/culling
M8 1k/4k/8k/16k 压测
```

验收：大量角色移动，遮挡顺序正确，性能数据可观测。

### 第五轮：网络和 UI

```text
M9 session/socket/mock/replay
M10 UI panels/buttons/list/chat
```

验收：mock 协议能驱动角色移动和聊天 UI。

### 第六轮：旧客户端对照和跨平台

```text
M11 behavior comparison
M12 packaging/cross-platform
M13 GPU backend optional
```

验收：关键行为有对照记录，Windows 可发布测试包，Linux/macOS 可构建。

---

## 18. 每周/迭代任务拆分建议

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

### Iteration 7：Protocol MVP

- 协议资料盘点。
- i62 编解码。
- es 字符串转义。
- 明文 token codec。
- 209 个封包 registry。

交付物：全量封包可注册、可外层解析和回写。

### Iteration 8：Payload MVP

- 内嵌 payload 二级分隔。
- 地图/对象 payload。
- 角色/属性 payload。
- 物品/技能/宠物 payload。
- 聊天/窗口/系统 payload。
- 战斗/交易/家族 payload。

交付物：含 payload 封包可结构化、保留 unknown 字段并 roundtrip。

### Iteration 9：World MVP

- EntityData。
- Movement interpolation。
- AnimationState。
- Visibility culling。
- Y-sort。

交付物：大量角色在地图上移动。

### Iteration 10：Performance Pass

- 1k/4k/8k/16k 压测。
- Atlas 优化。
- 排序优化。
- 名字/血条策略。
- 性能报告。

交付物：性能报告和是否需要 SDL3_GPU 的结论。

### Iteration 11：Network MVP

- Session。
- M6 codec 接入。
- MockServer。
- Replay。
- 聊天/移动消息。

交付物：mock 网络驱动 world 和 chat。

### Iteration 12：UI MVP

- Panel。
- Button。
- Label。
- ListBox。
- ChatBox 完善。

交付物：可用的基础客户端 UI。

---

## 19. 风险清单

| 风险 | 影响 | 缓解 |
|---|---|---|
| 旧资源格式细节不完整 | 地图/动画显示错误 | 工具优先，逐格式验证 |
| 封包格式细节不完整 | 登录、移动、聊天或 payload 解析错误 | xlsx 非空优先，空字段用 C/IDA 补齐，unknown 字段保留并记录来源 |
| 内嵌 payload 过早业务化 | 字段误判导致后续返工 | M6 只做结构化、回写和 unknown 保留，不补未确认业务逻辑 |
| IME 全屏候选框异常 | 中文输入体验差 | 先窗口模式稳定，预留自绘候选 UI |
| 16000 角色性能不足 | 客户端极限场景不达标 | Atlas + geometry batch + culling；必要时 SDL3_GPU |
| Y-sort 与原客户端不一致 | 遮挡错误 | 旧客户端截图/Hook 对照 |
| 字体和编码乱码 | 聊天/名字不可用 | UTF-8 内部，协议边界转换 |
| 协议状态机错误 | 登录/移动/聊天异常 | M6 先保证封包 roundtrip，M9 再做 mock/replay |
| UI 抽象过重 | 开发速度下降 | 先实现 CrossGate 必需控件，不做通用编辑器 |
| 过早引入 GPU 后端 | 功能进度拖慢 | SDL_Renderer 先完成，压测后决策 |

---

## 20. Definition of Done

一个模块完成必须满足：

1. 有清晰 API。
2. 有最小 demo 或工具能单独验证。
3. 有错误日志。
4. 不直接泄漏 SDL 底层对象到上层业务。
5. 有至少一个真实资源或模拟数据测试。
6. 有性能或内存基本观测指标。
7. 写入对应 docs 文档。

---

## 21. 当前最应该先做的 10 个任务

1. 按 `src/packet` 现有结构完成 M6.1 `PacketTypes` 收敛。
2. 实现 i62 编解码。
3. 实现 es 字符串转义。
4. 实现明文 token codec。
5. 实现 envelope 未压缩 roundtrip 和压缩错误边界。
6. 为测试补 209 个封包 schema/覆盖清单。
7. 按单包目标迁移 `JFVf` build 与 `whV` parse。
8. 实现外层 `PacketParser`。
9. 实现外层 `PacketBuilder`。
10. 实现 payload 二级分隔和转义框架。

---

## 22. 最小闭环验收标准

当下面全部满足时，说明 cg-engine 第一阶段可以进入 world 与网络连接层：

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
11. M6 封包清单覆盖 209 个封包。
12. i62、es、token、payload 基础 codec 有测试。
13. 209 个封包 fixture 可 parse/build roundtrip。
```
