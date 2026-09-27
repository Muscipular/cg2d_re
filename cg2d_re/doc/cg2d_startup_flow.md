# 原入口调用链还原

## 当前范围

本项实现原 `WinMain` 函数体及其已取证的配置、窗口等依赖，不等于完整客户端已经贯通。
工程当前仅含 x64 静态库目标；按 2026-09-27 指示不构建、链接或运行，
不新增或执行测试用例。早期编译和独立复核文字仅为历史记录。
入口中的未完成调用保留真实依赖声明，没有空实现或固定成功返回值。

本日增量：`InitializeClient@[4576B0,457A3C)` 完整父函数已实现并接入
既有 MainLoop 调用，详见文末；渲染初始化、加载画面等下游仍缺，
不等于完整启动链闭合。仅本线程工作，不使用 subAgent 或独立复核。

基线为 `ida-30904` 中的 `cg2d.exe`，原输入 SHA-256：
`dad3f292c6df28e9b7208ad1d339f56ab68c1e728f6595e849e37296551c0f08`。
`survey_binary` 的同名磁盘副本哈希差异沿用基线记录，不覆盖当前 IDB。

## WinMain

原范围 `[0x465790,0x4659ED)`，新函数位于 `src/platform/ClientEntry.cpp`。
原 x86 入口为 `_WinMain@16`，新入口使用 SDK 的 `WINAPI WinMain` 与原生句柄。
没有另设演示 `main`、SDL 事件循环或现代启动调度器。

入口先保存显示参数、实例和命令行，再按以下原分支执行：

1. 区分大小写查找 `CGstart`。命中后清零 4096 字节命令行缓冲区和 64 字节文件名缓冲区，
   以文本模式读取 `.\data\argument.ini`。每轮清零行缓冲区的前 1025 字节，
   `fgets` 上限为 1024；累计长度达到 4096 时在追加前退出。打开失败仍采用空的文件命令行。
2. 创建名为 `XZYDRaEvyPQC`、初始拥有者为 TRUE 的互斥量。原入口既不检查重复实例错误，
   也不释放这个句柄，不另加“单实例成功”分支。
3. 命令行不含 `updated` 时，按原字节及 `0x10` 标志显示消息，随后返回 0。
4. 仅在 `previousInstance == NULL` 时填写窗口类，加载图标 `0x67` 和 11 个光标，
   清零两个原全局，使用黑色背景画刷并注册窗口类。注册失败返回 0。
5. 初始化尾部严格采用下表顺序；入口忽略各返回值，主循环返回后入口返回 0。

| 调用指令 | 原目标 | 新对应 | 当前状态 |
| --- | --- | --- | --- |
| `0x4659B2` | `0x465A20` | `CaptureDesktopColor` | 已实现 |
| `0x4659B7` | `0x465A80` | `ReadSetupIni` | 已实现，详见配置证据 |
| `0x4659BC` | `0x4659F0` | `ParseClientVersion` | 已实现 |
| `0x4659C1` | `0x532E70` | `CreateSaveDirectories` | 已实现，未执行 |
| `0x4659C6` | `0x533300` | `InitializeSavedState_533300` | 已实现，直接入口 |
| `0x4659CB` | `0x465F00` | `ParseCommandOptions` | 函数体已实现，独立复核中；IP 依赖和一处存储仍未实现 |
| `0x4659D0` | `0x456050` | `resource::BuildResourcePaths` | 已实现 |
| `0x4659D5` | `0x4669F0` | `InitializeWindow` | 已实现，详见窗口证据 |
| `0x4659DA` | `0x457020` | `MainLoop` | 函数体已实现，独立复核中；初始化、逐帧及后端依赖仍未接通 |

窗口类的回调 `WindowProcedure@0x466DC0` 也未实现，不用 `DefWindowProc` 空壳替代。
原 PE 图标、光标资源尚未接入新工程。

## 光标与路径的重叠存储

原循环从 `0x55CE88` 开始，步长 12，读取资源 ID 的低 16 位，终点为 `0x55CF0C`。
因此读取 **11 条**，不能以 IDA 标注的 120 字节数组误判为 10 条。

- 前十条记录占 120 字节，每条为三个 32 位值；后两值保持中性名称，不推断业务角色。
- 第十一条资源 ID 位于 `0x55CF00`，值为 `0x6E`。
- 第十一条的后八字节与 `gBinPath@0x55CF04` 的前八字节重叠，初始字节为 `Bin` 及零尾。
- `CursorPathStorage` 承载 `[0x55CE88,0x55CF84)` 共 252 字节；
  编译期断言记录步长、尾 ID 偏移 `0x78`、路径偏移 `0x7C` 和总尺寸 `0xFC`。
- `gBinPath` 改为此对象内 `char[128]` 的数组引用，保留路径写入与完整存储的别名关系。
  引用的目标 ABI 不等于原数据符号 ABI；不承诺原地址 Hook 或二进制直接替换。

光标加载通过完整对象的字节视图加 `memcpy` 取 16 位 ID，不在十元素子数组上越界取第十一项。
目标固定 Windows/MSVC/x64，小端字节解释与原 x86 对应，路径消费者继续使用同一个 128 字节槽。

## 共享状态与小函数

`Startup.hpp` 的注释记录各原地址，`StartupState.cpp` 保存已核实的静态初值。
实例、窗口、互斥量、光标和命令行指针采用原生宽度；模式、计数、版本和坐标不随指针扩大。
`gRequestedRenderMode@0x628FE0` 按实际字节写入保留为 `uint8_t`。
窗口类名保留原八字节及 NUL，不推测其编码或转为另一套字符串。

| 原范围 | 行为 |
| --- | --- |
| `[0x4659F0,0x465A12)` | 用 `Ver %d.%d.%d` 解析 `Ver 1.02.02`，保存三个版本值并返回 `sscanf` 结果 |
| `[0x465A20,0x465A2E)` | 读取 `COLOR_BACKGROUND`，写入原桌面颜色全局并返回该值 |
| `[0x532E70,0x532EC3)` | `_chdir("data")`，依次格式化并创建 `0` 至 `9`，再 `_chdir("..")`，最终返回 1；不检查 API 结果 |

`ParseCommandOptions` 的返回值声明为 `int`：`0x4669D8` 调用 `0x507C80` 后保留 EAX 退栈。
后者的退出路径为 `strstr` 未命中所得 0，或解析索引不小于 800 时保留的 `sscanf` 结果。
Hex-Rays 的 `char*` 推断不能作为新端指针返回契约。

## 适配与未完成

- `WNDCLASSA`、`HWND`、`HINSTANCE`、`WPARAM` 等使用 Windows x64 SDK ABI。
  SDK 类型仅在平台实现及私有头出现，不人为把新窗口类打包为原 x86 的 40 字节。
- 原 CRT 对 `FILE` 内部 EOF 位的读取以本机 `feof` 表达，字符串内联操作用对应 CRT 调用。
  原读错误未设置 EOF 时继续循环的行为未擅自修复。
- 入口核对范围要求输入及源、目标缓冲区有效，字符串在原容量内终止且 CRT 写入不重叠。
  溢出、失效指针、旧栈布局及 x86 地址回绕不在本项已覆盖范围内，也没有获批排除。
- `CreateSaveDirectories` 会改变 CWD、创建目录；窗口化 helper 会改变系统桌面颜色。
  本次只编译和静态核对，不执行这些副作用，完整恢复和退出依赖仍待接通。
- `gValueD67AE4@0xD67AE4` 是映像零值 W32；`World::CursorUpdate` 以零选择
  表驱动光标分支、非零选择原生光标分支。其余生产者仍按各自函数逐项还原。
- `gValueD68DC4@0xD68DC4` 是逐帧光标状态请求；`World::CursorUpdate` 尾部
  重新读取后清零，并把旧值写入 `gValue9A6F44@0x9A6F44`。两者仍由
  `StartupState.cpp` 唯一持有，入口写零行为不变。
- 函数体和静态库编译不证明参数、存档、窗口回调、资源初始化、主循环或退出已经贯通。
- `InitializeWindow` 与 `MainLoop` 新声明显式保留机器返回值，分别为尾部
  `UpdateWindow` 结果与两个正常出口的 0。原源码返回类型未知，原入口仍忽略这两个结果。

## InitializeClient 完整父函数

2026-09-27，现有 `ida-17524` 只读核 `[4576B0,457A3C)` 的全部 253
条指令与原消息字节，继承 [DX §30–35](cg2d_graphic_dx_cache.md) 的
初始化边界。实现位于 [ClientInitialization.cpp](../src/platform/ClientInitialization.cpp)，
保持既有 `platform::InitializeClient_4576B0` 声明及 MainLoop 调用。
没有新建启动框架、资源 owner、成功占位或失败后的自动清理。

### 控制流与共享状态

| 原指令 | 当前实现 |
|---|---|
| `4576B8–4576C2` | 清系统日志、重置鼠标追踪池，再调用真实渲染初始化目标 |
| `4576CD–4576F0` | 首次返回只解释低 S8：1 提示失败；2 将 W32 windowMode 写成旧值是否不等于 1，再重试一次；第二次只查低字节非零，其余首次结果直接继续 |
| `4576F2–45773D` | 失败提示后 PostMessage(WM_CLOSE)，**仍继续**建面；不擅自返回或调用 Destroy |
| `45773D–4577AA` | 先写 queueMode=1，BuildSurfaceCache 为零提示 Error Code=22 返回 0；再初始化调色板池/调色板、分配四表，原门禁与提示保持 |
| `4577AA–4577D8` | 失效图形引用，再 GraphicInit；只有 1/2/7 分别进入原错误分支，其余包括当前可能的 3/-1 均继续，不改为“非零失败” |
| `4577D8–457801` | 独立调色板/DBF 初始化，取一次 Tick 播种，初始化旋转表，再鼠标状态、Yobi 哨兵、字体 family=0 |
| `457804–45784C` | IME 初始化返回忽略；恢复保存记录失败时将 W8 错误码零扩展为 int，使用 1024B 栈区按原格式提示并返回 0 |
| `45784D–457896` | 在 ShowCursor(0) 前采样原 bShow；非零才依次 CryptInt add(mouseY,16)、add(mouseX,16)，入队图号 22000、层 231；随后 queueMode=0 |
| `4578A0–4578C1` | InitLoading 为零返回；日志后淡入为零返回，不跳过这两段或强行成功 |
| `4578C7–457925` | 日志→自动地图颜色→日志→AnimeInit，动画返回非零才提示并返回；随后 CoordLoadInit，参数是既有数据/Info 路径表，不按 IDA 自动名称颠倒 |
| `45792A–457951` | 原序日志→已有音频初始化→次级音乐入口(同一原生窗口引用)→日志→相册初始化；返回全部忽略，不新增音频实现或将其排除 |
| `457956–457983` | 再独立读取 Tick，按原高 W32=0 承接为正数 double；先写 NextSceneType=-1，再写 gLastRenderDt，原 double 常量 3000.0 加同一临时值后写 73ED00 |
| `457989–4579C7` | 日志后淡出，为零返回；成功才 queueMode=1、写 initGame OK 日志、调用窗口系统初始化，返回 1 |

原八个返回点中仅 `4579C7` 返回 1；新端等价分支直接返回 0/1，
不要求编译器复现原基本块或返回指令数量。失败不会回滚之前的资源、
模式写入或随机数消费，相关退出链另按原调用负责。

### 原字节消息

所有提示仍经 `ShowLegacyMessage` 的窄字节边界，flags 均为 10h；
PostMessage 继续使用同一窗口槽，且在提示返回后重新读取。
原标题 `5536B8` 为 `BD 54 BB 7B 00`。未翻译或改写原错误正文。

| 原地址 | 源常量 | 含 NUL 长度 |
|---|---|---:|
| `559340` | kRendererError | 23 |
| `559320` | kSurfaceMemoryError | 15 |
| `559314` | kAllocationError | 9 |
| `55919C` | kGraphicError | 22 |
| `559200` | kGraphicV3Error | 22 |
| `5591B4` | kGraphicInformation | 63 |
| `559218` | kGraphicVersionError | 47 |
| `5592F8` | kSavedStateErrorFormat | 26 |
| `559298` | kAnimeError | 19 |

GraphicInit=2 先显示 V3 错误，再显示原 INFORMATION/说明；
InitPalette 失败则无本函数额外提示。七条 ASCII 阶段日志的大小写、
文字和位置保持原样。原 `54DDA8` 八字节为 `00 00 00 00 00 70 A7 40`，
即精确的 3000.0，不借另一个可写帧间隔槽代替。

### 实现与缺口

已接资源、鼠标、随机数、Yobi、字体/IME、保存恢复、自动地图包装、
已有音频及真实动画/坐标初始化。CMake 仅登记新源，不配置或构建。
以下原直接目标仍只有真实声明，没有新增函数体或假成功：

- `render::RendererInit_44B630`：32 位色、SDL/CPU、模式 2/3/4 及失败发布。
- `initialization_detail::InitLoading_457B10` 和两段淡入/淡出：
  消息泵、加载阶段与真实画面消费尚缺。
- `initialization_detail::InitializeMusic_454C30`：原次级 DirectMusic
  初始化，音频继续暂缓，没有以已有 `5241A0` 替掉这个独立调用。
- `initialization_detail::InitializeWindowSystem_4705D0`：
  窗口/UI 共享状态仍须落实。

后续已按[场景 §58 的 owner SPEC](cg2d_scene_state.md)接通
`world::InitializeAlbum_518C20`，在唯一 `0x3C00C` 字节区执行
整区清零和 `CCA\0` 复制，返回完整 W32 值 4。
移除原 platform 候选声明，当前尚缺的直接目标由七个降为六个；
相册加载、保存、组清理和同步仍未实现，不将初始化当作相册整链完成。

此外已有自动地图颜色包装的下游、DrawAllQueued 和完整退出依赖仍
未闭合。本步证明的是完整父控制流已编码和现有依赖已静态接线，
**不是**可链接、可启动、资源成功加载或已有画面；不构建/测试/运行，
不使用 subAgent，不降低整体验收标准。
