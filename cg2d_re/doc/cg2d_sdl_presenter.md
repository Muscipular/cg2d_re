# SDL3 最终帧提交适配器

任务：R4.2.1.C。范围仅为 `SdlPresenter.hpp`、`SdlPresenter.cpp` 和本文档。

这是 Windows/x64 上的 SDL 底层适配，不是原函数还原，也不代表原渲染替换已接通。
API、属性名和返回值依据本地 `third_party/sdl/SDL3-3.4.8/include/SDL3/` 头文件核对；
不推断其他 SDK 版本，不下载依赖，不复用旧 `SdlRendererBackend` 框架。

## 接口与所有权

- 公开类型为 `cg::render::SdlPresenter`，接口只使用标准 C++ 类型，不暴露 SDL 或 Win32 类型。
- `Create(nativeWindow, width, height, driver)` 的窗口引用是未截断的外部既有 Win32 窗口；调用者保证它有效、属于当前进程的主线程，并一直存活到呈现器销毁。
- `width/height` 是固定的最终帧尺寸，必须为正且一行字节数能放入 SDL 的 `int pitch`。不在这里调整原生窗口尺寸。
- `driver` 没有默认参数，非空字符串原样传给 `SDL_CreateRenderer`；空字符串报错，只有显式 `nullptr` 表示交由 SDL 自行选择。适配器不指定 `direct3d11`，不构造驱动候选列表，不自行重试或回退；SDL 自身对传入字符串和运行环境的处理仍由 SDL 决定。
- 仅接受 SDL 的 Windows VIDEO 后端，不修改全局 hint 或强制切换 VIDEO 后端。这不是 renderer 驱动选择策略。
- 使用 `SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER` 包装窗口，并检查 `SDL_WINDOW_EXTERNAL` 标记。本类仅拥有 SDL wrapper、renderer、texture，不拥有外部原生窗口，不调用 Win32 `DestroyWindow`。
- 每个实例无论 VIDEO 是否已被其他模块初始化，都调用一次 `SDL_InitSubSystem(SDL_INIT_VIDEO)`；仅成功后记录自己持有的引用。最终只配对一次 `SDL_QuitSubSystem(SDL_INIT_VIDEO)`，不调用全局 `SDL_Quit`，不释放其他模块的引用。

## 生命周期与错误

`Create` 在临时 PIMPL 中依次持有 VIDEO 引用、窗口属性组、窗口 wrapper、renderer 和 texture。
窗口属性组在包装成功后立即释放；若任一步失败，先保存失败阶段及 SDL 错误，再由 RAII 清理全部已获得的资源。
只有所有配置成功才发布实例状态。已创建时再次 `Create` 会报错并保留现有资源，修改尺寸或驱动须先 `Destroy` 再 `Create`。

`Destroy` 按 texture、renderer、window wrapper、VIDEO 引用的顺序释放资源，未创建、创建失败后或已销毁时均可重复调用。
析构自动执行销毁；实例禁止复制和移动。外部模块不得在呈现器仍存活时强制关闭 SDL 或提前销毁原生窗口。

所有操作和析构均为 main-thread-only，不提供互斥、后台派发或事件循环。
`Create/Present/Destroy` 使用 `SDL_IsMainThread` 拒绝 SDL 已识别的非主线程调用；有资源的 `Destroy` 在线程错误时保留资源并返回 `false`。
首次初始化 VIDEO 前，SDL 不能替代调用者识别原 Win32 入口和窗口所属线程，这一前提仍由调用者保证。
持有资源却在错误线程析构属于违反接口约定，触发 `std::terminate`，不会在错误线程强行释放或静默泄漏资源。

`LastError()` 指向对象自己的固定 UTF-8 缓冲区，包含操作阶段及原因，最多保存 1023 字节，过长内容按 UTF-8 字符边界截断。
失败清理及成功 `Destroy` 不会覆盖最近错误；`Create/Present` 成功后清空错误。
诊断路径不分配内存，PIMPL 分配失败也返回 `false`。SDL 提交失败保留实例供主线程销毁或后续重新提交，不自动重建或切换驱动。

## 帧格式与提交

- 仅接受自上而下的 `std::uint32_t` 完整帧，每像素数值为 `0x00RRGGBB`。创建格式固定为 `SDL_PIXELFORMAT_XRGB8888`，占 4 字节，高 8 位不作为 alpha；不扫描或改写像素值。
- texture 使用 `SDL_TEXTUREACCESS_STREAMING`、`SDL_BLENDMODE_NONE`、`SDL_SCALEMODE_NEAREST`。不增加调色板转换、sprite 混合、图形排序或 colorkey 算法。
- `pitchBytes` 为正、为 4 的倍数且至少为 `width * 4`；允许行间 padding，不支持负 pitch 或倒置帧。
- `pixels` 必须至少覆盖 `(height - 1) * pitchBytes + width * 4` 个字节，不要求最后一行尾部 padding。实现按像素数使用先减后除的等价式校验，避免尺寸乘加或 `span::size_bytes()` 溢出；所有校验先于 SDL 上传。
- 每次调用依次执行整纹理 `SDL_UpdateTexture`、`SDL_RenderClear`、`SDL_RenderTexture`、`SDL_RenderPresent`，逐步检查 `bool` 返回值，任一步失败即停止后续调用。SDL 3.4.8 头文件明确允许 `SDL_UpdateTexture` 更新 streaming texture；这里不混用锁定接口。
- 渲染使用完整源纹理和完整默认输出区域；尺寸不一致时按 nearest 拉伸，不实现额外的宽高比、留黑或 DPI 策略。清屏后覆盖整个输出，不依赖上一帧 backbuffer 内容。
- 调用期间源数据须有效且不被并发修改，适配器不保留源 span。提交失败不承诺回滚已经上传的纹理或已执行的渲染操作。

## 尚未接入

本步没有调用者、空窗口或交互 demo，没有伪造原 `MainLoop`、窗口事件链或图形排序。
原调用点、renderer driver 策略、最终帧来源、尺寸与颜色语义、窗口缩放/DPI、设备丢失及重置事件仍待接入和运行验证。
本适配器不替代应用入口的 SDL 启动约定，不处理事件，也不声称满足原客户端画面语义。

未修改计划、进度或 CMake，未编译、运行、编写测试或提交。
当前仅完成实现和本地头文件静态核对，不代表已通过链接、外部窗口生命周期或真实画面的运行验收。

## ClearSurface@0x4512C0：原端证据与待批统一逻辑 Back SPEC

状态：**NEEDS_CLARIFICATION**。本节仅记录已核实原端事实及待批候选，不构成实现授权。

### 原端已核实事实

- `Renderer::ClearSurface@[0x4512C0,0x451306)` 为单块函数，无显式参数、无 `this`；末尾 `retn`，`EAX` 原样返回间接 `Blt` 的结果，不作成功常量替换。
- `0x4512C4–0x4512E4` 构造局部 `DDBLTFX`：100 字节清零，`dwSize=100`，从 `gBackgroundColor@0x62B0A8` 读取完整 W32 写入颜色字段。
- `0x4512D6–0x4512FE` 从 `renderer@0x627360` 的 `+0x0C` 取 Back，经其 vtable `+0x14` 间接调用 `Blt`；目标矩形、源 surface、源矩形均为 null，传入上述颜色 fx，flags 为 `0x01000400`。本体不改 Temp 或 dirty，不设置失败分支；这不等于已审完所有调用方的返回处理。
- Back 创建证据位于 `0x44B946–0x44B951`，释放位于 `0x44C4E5–0x44C4F8`；其新端统一 owner 与创建、重建、销毁接线尚未裁决。
- `MainLoop` 在 `0x457294` 先读取 `0x73ECC0`，`0x457299` 再清 `0x73ECE4`；读得 0 时于 `0x4572AF` 调用本函数，读得 3 时于 `0x4572A8` 调用 `sub_423640`，其他值不走这两个调用。汇合后依次为 `DrawAllQueued@0x4572B4`、`sub_468FF0@0x4572B9`、`Flip@0x4572BE`；这里弃置 ClearSurface 返回值。本函数在当前 IDA 有 23 处直接代码调用点，分布于 17 个函数；其余调用点的返回消费见下节。当前接线及声明见 [MainLoop.cpp](../src/platform/MainLoop.cpp) 和 [MainLoopDependencies.hpp](../src/platform/MainLoopDependencies.hpp)。

### 与当前 SDL/CPU 边界的区别

- [SdlPresenter.cpp](../src/render/SdlPresenter.cpp) 的 `SDL_RenderClear` 发生在整帧上传后的最终输出阶段，随后绘制完整纹理并提交；它不清空上层逻辑 Back，不能代替 `ClearSurface` 的当次 Back 颜色填充。
- [SurfaceState.hpp](../include/cg/render/SurfaceState.hpp) / [SurfaceState.cpp](../src/render/SurfaceState.cpp) 的 640×480 CPU Back/Temp 目前仅按已批准波纹链使用；[GraphicPendingQueue.cpp](../src/render/GraphicPendingQueue.cpp) 在波纹路径快照 Back 到 Temp 并逐条带回拷。该局部批准不自动授予全部画面统一 Back 的 owner 或生命周期，也不证明画面链已完成。
- 已批准 DX7 底层 `Blt` 等价映射 SDL/CPU，不要求逐码复刻 DX7 `HRESULT` 或 COM vtable；但上层触发、W32 颜色、像素内容、共享 Back/Temp 生命周期与提交顺序仍受 [渲染替换约束](../plan/cg2d_full_reconstruction_plan.md) 约束。

### 当前源码接线边界（2026-09-25 静态核对）

- `SdlPresenter::Create/Present/Destroy` 目前仅在 [SdlPresenter.hpp](../include/cg/render/SdlPresenter.hpp) 声明、[SdlPresenter.cpp](../src/render/SdlPresenter.cpp) 定义；在本子项目 `src/`、`include/` 内未找到实例创建或这些成员的调用。`Present` 内部的 SDL 清输出、上传纹理和提交不能证明原逻辑 Back 已送达最终窗口。
- `gCpuBackSurface/gCpuTempSurface` 的直接访问限于 [SurfaceState.cpp](../src/render/SurfaceState.cpp) 的逐行复制和 [GraphicPendingQueue.cpp](../src/render/GraphicPendingQueue.cpp) 的波纹快照；[MainLoopSupport.cpp](../src/platform/MainLoopSupport.cpp) 只通过 `BlitTempToBack` 使用此对缓冲。背景色由 [LoginCapability3Scene.cpp](../src/LoginCapability3Scene.cpp) 写入同一 `gBackgroundColor`，而 [MainLoopDependencies.hpp](../src/platform/MainLoopDependencies.hpp) 的 `ClearSurface/DrawAllQueued/Flip` 仍只有真实声明。
- 这是当前源码的接线事实，不是原端新取证，也不批准扩大波纹专用 Back 的作用域。单独填充该数组无法闭合原端在同一 Back 上的清屏、队列绘制、翻转及其创建/重建/销毁链。

### 待用户批准的 SPEC 候选

- **候选行为，未批准：**将 `ClearSurface` 映射为对统一逻辑 Back 的 640×480 区域按每次调用时读取的完整 W32 背景色填充；保留调用时点及后续绘制、提交顺序。不预设最终画面已接通，不使用固定成功返回伪造原上层返回合同。
- **必须另行裁决：**统一 Back 的唯一 owner、与现有波纹专用 CPU Back/Temp 的共享关系，以及创建、重建、销毁时点；直接 caller 的返回消费已按下节核对，替换后的返回与失败策略仍须单独裁决。SDL renderer driver 选择及失败处理也须独立裁决，不从本函数的原 DX7 `Blt` 结果推定新后端策略。
- 用户批准上述 SPEC 前，不实施源码或接线，不预记 `ClearSurface` 已实现或原画面链完成；事实进度登记不受此限。

### `4512C0` 直接调用点的返回消费

2026-09-24，当前 `ida-17524/cg2d.exe.i64` 对 `4512C0`
的 code xref 共 23 处、17 个已识别函数。逐点检查调用后
指令，并核对各函数反编译中的调用表达式，23 处均为
独立调用，不以 EAX 作条件、存储、参数或返回传递：

| 原函数 | 调用点 |
| --- | --- |
| `43CB10`、`MainLoop` | `43CB54`、`4572AF` |
| `InitLoading` | `457BBF`、`458109`、`458131` |
| `458390`、`4585F0`、`458A40` | `4584F7/4585C2`、`458703`、`458AA1` |
| `Renderer::Rebuild`、`DrawNormalScene` | `466C85`、`51C8E9/51CB97/51CF5C/51D0B1` |
| `51D640`、`51D790`、`51E0D0`、`51EC10` | `51D667`、`51DCCA`、`51E99C`、`51EE99` |
| `51EF80`、`51F610`、`51F860`、`51FA70`、`51FC90` | `51F48B`、`51F68A`、`51F8B6`、`51FAC6`、`51FCDB` |

例如 `4572AF` 后立即调用 `DrawAllQueued`，`457BBF`
后立即 `xor eax,eax`；`51DCCA` 后按 `ESI` 分支，
其伪代码中的 `ClearSurface()` 仍是独立表达式。
本结论只覆盖当前 IDA 的直接调用点，不证明无间接
调用或新端所有潜在消费都可忽略。原 `ClearSurface`
依然返回底层 `Blt` 的完整 EAX32；已批准的 DX7
底层失败码豁免不等于允许固定成功、改变统一 Back
生命周期或绕过 SDL 提交失败处理。统一 owner 与
具体等价返回策略未获批准，源码继续待确认。

## Flip@0x44C560：原端分支与共享帧槽

2026-09-25，在已打开的 `ida-17524/cg2d.exe.i64` 会话中只读核对
`Renderer::Flip@[0x44C560,0x44C6A5)` 的反编译、完整反汇编及直接引用。
本体无显式参数，原型为 `void __cdecl()`；直接调用点为
`4572BE/458264/45852E/4585CC/45873A/458ACD`，本节不推断这些
caller 的完整画面链。

1. `44C560–44C571` 读取 `gWindowMode@9A7F48`。恰为 1 时，
   `44C577–44C5B9` 以 `(0,0)` 调 `ClientToScreen`，随后
   `GetClientRect`、`OffsetRect` 得到屏幕坐标矩形；三个 API 的返回
   在本体均不判定。`word_554B74==0` 才进入 `44C5C1–44C5F1`
   的 `GetScanLine` 等待：结果为 `0x88760219` 即退出，其他非零结果
   累计至 10000 次退出，零结果继续循环。随后
   `44C5F3–44C611` 以该矩形把 renderer `+0x0C` Back
   `Blt` 到 `+0x08` 前表面，flags=`0x01000000`。
2. 模式不为 1 时，`44C61A` 调
   `hasIDirect3D7@44B610`；该叶在 `44B610–44B623` 只检查
   renderer `+0x9C` 是否非零。非零走同样的
   `GetScanLine` 等待（此分支不检查 `554B74`），再以空目标矩形
   将 Back `Blt` 到前表面，flags=`0x01000000`；为零则
   `44C671–44C680` 调前表面 `Flip(front,0,1)`。
3. 三路底层调用结果在 `44C614/44C66B/44C683` 判完整 EAX32：
   非零时 `44C687` 写 `gCRenderer::isLost@62B064=1`，零时不写。
   无论走哪一路、结果如何，`44C68D–44C699` 都将独立
   `W32[628FC8]` 写成“旧值是否为零”的 `0/1`，然后返回；
   不能将帧槽翻转放进成功分支或用固定成功结果跳过 lost 写入。

`628FC8` 在当前 IDA 有 20 处直接数据引用，除本体两处外还出现在
`447070/447230/4473B0/4474E0/447610/447820/447A20/447C40`
及 `RenderChara@521F20`。逐址检查直接引用的指令，只有
`44C69A` 写该槽，其余均为 W32 读取：`4471BA/447B5C/447D7B`
与 1 比较，`4473CD` 读后写入记录 `+0x30`，`447406` 将当前值
与该记录 `+0x30` 比较，`447A02` 也将当前值写入记录 `+0x30`；
`521F5F/521F7C` 的零/非零结果决定是否提前结束角色绘制。
这是直接引用的已查结果，不宣称没有间接别名。`628FC8`
在 IDA 的 `.data` 中 `loaded=False`，工具显示的全一值不是
原文件初值；按此前已确认的 PE 零填充范围，其映像初值为零。
它不能视为本函数局部计数器。新端
[CommandOptions.hpp](../include/cg/platform/CommandOptions.hpp) 已有
`554B74` 的唯一 W16 存储，[MainLoop.hpp](../include/cg/platform/MainLoop.hpp)
已有 `62B064` 的状态声明；`628FC8` 的唯一 x64 owner 与跨绘制消费者
尚未确定。[SdlPresenter](../include/cg/render/SdlPresenter.hpp) 仍没有
调用接线，不能仅以其 `Present` 代替上述共享写序。已批准的 DX7
底层等价替换不批准改变上层 lost/帧槽副作用；具体后端失败映射、
统一 Back 与生命周期、驱动选择继续待用户 SPEC，不编码 `Flip`，
不计主循环画面、完整链接或运行完成。

## DrawAllQueued@0x520DD0：入口、分发表与尾部边界

2026-09-25，在当前已打开的 `ida-17524` 中只读核
`Renderer::DrawAllQueued@[0x520DD0,0x521832)`。原函数无参数，
IDA 标为 `void __cdecl()`，共 154 个基本块；本节只闭合入口、
实际跳表目标及公共尾部，**不代表 13 路绘制本体已还原**。

- `520DDC` 先清 `W32[1020ECC]`，`520DF2` 对 renderer
  `+0x0C` Back 调 `IsLost`；结果非零直接到 `521434`
  返回，不进入队列、版本复制或特殊图形标记清理。结果为零时，
  `520DFD–520E15` 若 `hasIDirect3D7@44B610` 为真，
  调设备 `+0xA0` 的 `BeginScene`；其返回不作门禁。
- `520E18–520E4D` 对 `pendingGraphicCount@1020EB8` 作有符号
  W16 `>0` 门禁。每轮从并行 `GraphicCache3` 取 S16
  `cacheIndex`，按 `0x28` 步长定位 `GraphicCacheItem32`，
  将其 `batchType@+0x24` 的 W8 符号扩展，再比较上界
  `0x0C`；高于 12 的值与表外负值均去默认目标。不能
  将并行队列先按物理顺序逐项绘制，或把类型读成无符号 W8。
- `520E4D` 的跳表起点 `521834`，13 项各 W32；
  原内存核得目标如下。0/1、10/11/12 共用目标不等于
  对应分支的参数或后续行为相同：

| 类型 | 实际目标 | 本轮已核的入口行为 |
| --- | --- | --- |
| 0、1 | `520E54` | 先取记录 `+8` 图号；后续路径未在本节闭合 |
| 2 | `5216E9` | 将记录与 `+8` 值传给 `522210`，后者经 `451350` 写 Back |
| 3 | `5216FB` | 将记录传 `Renderer::FillDirtyArea` |
| 4 | `521709` | 有 3D 设备时先 `EndScene`，再以记录 `+0x0C` 调文本绘制，之后有 3D 设备时重新 `BeginScene` |
| 5 | `521752` | 以记录 `+0/+4` 两值调用 `46C760` |
| 6 | `521766` | 受 `byte_C82204` 门控，裁剪后从 Temp `BltFast` 到 Back |
| 7 | `5217F2` | 受同一门控，从 Back `BltFast` 到 Temp |
| 8 | `52181E` | 调已落位的波纹入口 `521870` |
| 9 | `521828` | 调 `46E480` |
| 10、11、12 | `52143C` | 后续路径未在本节闭合 |

  默认目标 `5213D6` 在 `5213DA–5213F3` 递增并行队列游标
  与已处理数，每轮重新比较有符号 W16 计数；类型分支
  结束均汇入该处。
- `5213F9–521414` 在队列结束后若有 3D 设备则调用
  `EndScene`，返回不作门禁。`521414–521428` 仅当
  `W32[1020ECC]==0` 才将调色板版本 `W32[628FDC]`
  写入独立 `W32[58B7E0]`；随后无条件调用
  `44E230`、清 `W8[1020EDC]`，并从 `521434` 返回。
  IDA 的 `gFrameIndex_mb` 名称不是字段用途证据。

`1020ECC` 的四处直接数据引用限本函数两处与
`Graphic::LoadTexture@520B89/520B99` 两处：加载叶先读
当前 W32，按 S32 与 10 比较，低于 10 时加一并写回。
`58B7E0` 当前仅有 `521422` 一处直接写引用，不据此排除
间接读取；`628FDC` 已由
[Palette.hpp](../include/cg/Palette.hpp) 的
`gPaletteFrameVersion` 唯一持有，不能另建“帧索引”副本。

当前 [GraphicPendingQueue.hpp](../include/cg/render/GraphicPendingQueue.hpp)
已持有两张 4096 项并行记录及特殊标记的唯一存储，
[MainLoop.cpp](../src/platform/MainLoop.cpp) 仍按原顺序调用
`ClearSurface→DrawAllQueued→468FF0→Flip`。`1020ECC`、
`58B7E0`、图形/DX cache 的上游数据以及 Back/Temp 完整生命周期
尚未映射或批准；`451350` 的 32 位色写入也未落位。
不因本轮已核入口和跳表而实现局部父函数或声明其可链接，
继续逐 case 补证并取得所需共享 owner SPEC。

## `522210` case 2 矩形绘制包装

2026-09-25，当前 IDA `sub_522210@[522210,522293)` 共 131 字节，
唯一直接 caller 为 `DrawAllQueued@5216EE`。本体两参数：
记录指针与完整 W32 图号位型；自身不直接读写全局，
`451350` 的间接画面副作用另计，返回 `void`。

`522219/52221B` 按序读取记录 `+0/+4` 的两个 W32；
`522224–52223F` 依序将 X/Y 的低 16 位符号扩展写入
局部 `RECT.right/bottom`，再将 X/Y 高 16 位算术右移所得
符号扩展写入 `RECT.left/top`。这两个 W32 是打包坐标，
不能直接将 `GraphicCacheItem32::x/y` 当完整屏幕坐标。

`522242–52228F` 对同一图号位型顺次测试
`0x20000000`、`0x10000000`、`0x40000000`：
首次命中分别调用 `451350(&rect,图号低16位,0/1/2)`，
前两路调用后立即返回；全不命中则不调用。
三路有优先级，不拆成三个独立 `if`；每次至多一次绘制。
原调用点向 `451350` 压完整 W32，后者的有效第二参数
为 U16，不能让高位模式标志流入颜色索引。

新端在 [GraphicPendingQueue.hpp](../include/cg/render/GraphicPendingQueue.hpp)
声明完整包装，实体在
[GraphicPendingQueue.cpp](../src/render/GraphicPendingQueue.cpp)
读取唯一的 40 字节记录；Win32 `RECT` 及 `451350`
真实未落位声明只在 render 实现边界。包装本体已落位，
但 `451350` 的 32 位 Back 写入、原队列 case 2 父入口、
统一画面 owner 与客户端完整链接/运行仍未闭合。

## `520AD0` case 3 dirty 矩形包装

2026-09-25，当前 IDA `Renderer::FillDirtyArea@[520AD0,520B09)`
为单参 57 字节函数，唯一直接 caller 为 `DrawAllQueued@5216FC`。
`520AD4/520AD7` 分别读取记录 `+0x21` 的 W8 混合模式、
`+0x1C` 的 W32 颜色；`520ADB/520AE0/520AE5/520AEA`
依次读取 `+6/+4/+2/+0` 的 W16。反向压栈后，
`520AEF` 的 `Renderer::FillRect@44C9A0` 实参为
`(S16[+0], S16[+2], S16[+4], S16[+6], W32[+0x1C], W8[+0x21])`。
`520AF7–520AFE` 仅当其完整 EAX32 恰为 `0x887601AE`
才写共享 `gGraphicDirty@1020EE8=1`，否则不写，随后返回
`void`。混合模式的原压栈寄存器上位未在 `FillRect`
消费，其低 W8 才是有效值。

新端现有 `GraphicCacheItem32::descriptor[8..11]` 与 `[13]`
分别承接颜色和混合模式，四坐标由 `x/y` 的各两个 S16
按原读取与传参顺序拆出；`gGraphicDirty` 复用
[GraphicPendingQueue.hpp](../include/cg/render/GraphicPendingQueue.hpp)
的唯一存储。完整包装定义在
[GraphicPendingQueue.cpp](../src/render/GraphicPendingQueue.cpp)，
只声明真实 `44C9A0` 依赖，不替它返回固定值。
`FillRect` 原本体在没有 3D 设备、裁剪拒绝或 renderer
`+0xBC` 门禁为零时返回零，否则提交四顶点矩形并透传
`DrawPrimitive` 结果；这仍需另行按 32 位色与获批 DX7
替换范围完整落位。上层 dirty 状态写入不能因不逐码复刻
底层 HRESULT 而默默删除；新 SDL/CPU 失败如何映射
本原端门禁仍待 SPEC。父 `DrawAllQueued`、完整画面与链接
运行尚未闭合。

## `46C760` case 5 地图绘制分流

2026-09-25，当前 IDA `sub_46C760@[46C760,46C837)`，
唯一直接 caller `DrawAllQueued@521759` 在
`521752–521761` 依序从记录 `+4/+0` 压入两个完整
W32 坐标；不取记录图号。`46C760` 本体分两路：

1. `46C760–46C7CE` 在完整 `W32[gMapPukMode]==1`
   时先写 `W32[55DA88]=1`，依序各一次读取玩家 Y、
   X 的 `CryptInt::GetVal`，调用
   `46C840(gMapFloor,X,Y)`；随后从
   `W16[55DA90]` **有符号扩展**成表索引，清
   `W32[55DA88]=0`，取 `55DA94+index*8`
   的 W32 宽高，将原两坐标与 `gMapBuff` 传
   `44FEB0`，透传其完整 EAX32 返回。重建结果
   不作成功门禁，表索引在调用之后才读取。
2. `46C7CF–46C836` 的其他模式先读完整
   `W32[55DA88]`：非零才依序读玩家 Y、X，
   调同一 `46C840` 并清该槽；零时不重建、
   不清槽。两路最终都以调用之后的有符号
   `W16[55DA90]` 索引同一 8 字节表项，
   将原两坐标与 `gMapBuff2` 传 `44F5C0`，
   透传其完整 EAX32。caller 不消费返回。

`55DA88` 另有 `46D084/46D5AA` 两处直接写引用；
`55DA90`、8 字节表项及两个地图缓冲亦由
`46C840` 等路径共享。当前 `cg2d_re` 没有这些
状态的唯一 owner，`46C840/44FEB0/44F5C0`
也未落位；IDA 数据项大小不能单独当作运行索引
或像素访问的边界。地图存储、跨模块接口及 SDL/CPU
绘制承接须先有 SPEC，不能私建 `55DA88` 标志、
跳过重建调用或固定返回；case 5 与父队列均未实现。

## `520DD0` case 6/7 的 Back/Temp 复制

2026-09-25，当前 IDA `521766–521819` 两路均先读
`W8[C82204]`，为零时直达队列公共下一项，
不读取位移或调用底层复制；非零时：

| case | 地址 | 实际提交 |
|---|---|---|
| 6 | `521773–5217ED` | 从 Temp 向 Back 调一次 `BltFast(destX,destY,Temp,&sourceRect,16)` |
| 7 | `5217FF–521819` | 从 Back 向 Temp 调一次 `BltFast(0,0,Back,null,16)`，整面复制 |

case 6 的 `sourceRect` 先设 `[0,0,640,480]`，
`destX=destY=0`；`521773` 读取 `C3500C` 的
S32 位移 X，正值将 `destX=X` 且 `right=640-X`，
负值将 `left=-X`，零不变。`5217AF` 再读取
`C75010` 的 S32 位移 Y，正值将 `destY=Y` 且
`bottom=480-Y`，负值将 `top=-Y`，零不变。
两轴按原次序独立计算；未另夹到合法矩形范围，
超出尺寸时仍提交原计算出的参数。两路都不检查
`BltFast` 返回，最终进入 `5213D6` 的公共计数尾。
原 DX7 标志 16 的机制已属获批替换边界，不由此
推导新的 SDL 失败策略。

[SceneState.hpp](../include/cg/world/SceneState.hpp) 已有
`C82204` 的唯一 W8 owner；`C3500C/C75010` 已在
[SceneState.cpp](../src/world/SceneState.cpp) 由 world
唯一持有。它们分别由 `51C45E/51D23B/51D671/
51D68F` 与 `468F32/468F37/468FCB/468FD5`
等已查路径写入，不可为 case 6/7 私建偏移副本。
现有 CPU Back/Temp 仅获批用于波纹，统一画面 owner、
生命周期和 world→render 窄接口仍待 SPEC；不把
这两路代码塞进已落位的波纹复制，父队列未实现。

## `46E480` case 9 提交门禁与共享 W16

2026-09-25，当前 IDA `sub_46E480@[46E480,46E499)`
只有唯一直接 caller `DrawAllQueued@521828`。
`46E480` 先比较 `W16[C8A73C]` 是否为零：零时
`46E488` 直接跳到 `retn@46E498`，不写状态；
非零时 `46E48A` 调 `453580`，随后
`46E48F` 清同一槽的低 W16，**即使下游结果非零也清**。
非零分支保留 `453580` 的完整 EAX32；零分支未定义
新的返回值，不能固定说成成功码。唯一 caller 不消费返回。

`453580@[453580,4535A8)` 按 `BITSPIXEL@554B70`
的完整 W32 分别尾跳 8/16/24 位分支，其他值尾跳
`452840`；用户确认的新端仅复刻 32 位色渲染，
因此此处所需的原 32 位分支为 `4535A3→452840`。
`452840` 的入口 `452852–452877` 构造描述符并锁
renderer `+0x0C` Back；成功后从 `C8A72C`
沿活动记录链绘图，当前未闭合其完整 1198 字节本体，
也不把低色深分支计入待实现 32 位画面。

当前 IDA 对 `C8A73C` 的八处直接引用：
`46E480` 读、`46E48F` 清；
`46E6A9` 写 BP 低 W16、`46ED26` 写 BX 低 W16；
`46E926/46EB84/46EFE7/46F611` 各写 W16 常量 1。
这六处生产点分别位于
`46E4E0/46E6C0/46E940/46EBB0/46ED50/46F1D0`，
均与[场景记录链](cg2d_scene_state.md)的活动记录生产/
更新路径相邻；直接 xref 不证明间接别名已穷尽。
该槽位于已证 `.data` 零填充范围，映像初值为零，
并未列入 `46E2A0` 的十五项显式重置；不能把 BSS
初值当成每轮清零，也不能只承载 0/1 而丢弃 BP/BX 位型。

**待确认 SPEC 候选：**由 world 记录链唯一持有
`C8A73C` 原 W16 位型，六个原生产点写该槽，
`46E480` 经 render/world 窄接口按原时点读、条件调用
和清低 W16；不在绘制队列私建另一个标志。
现有 [CommandOptions.hpp](../include/cg/platform/CommandOptions.hpp)
已持有 `554B70` 的设备色深槽，但 500 项活动记录链、
Back 32 位像素写入及 renderer 生命周期仍需各自
SPEC 与实现。此候选未获用户批准，不实施
`46E480/453580/452840` 或宣称 case 9 画面已完成。

## `452840` 32 位活动记录绘制边界

2026-09-25，当前 IDA `sub_452840@[452840,452CEE)`
共 1198 字节、67 个基本块，入口为 `453580@4535A3`
的 32 位色尾跳。下述像素坐标以
`4528C6–4528CF` 算出的记录 `+0/+4` 原点为 `(0,0)`，
`pitch` 为锁定后的字节行距右移 2。已逐项核对反编译
与各分支、写入和尾部的实际反汇编。

1. `452846–452859` 清 124 字节局部描述符并写大小 124；
   `452861–452877` 对 renderer `+0x0C` Back 调
   `Lock(null,&desc,1,0)`。非零结果直达 `452CE6`
   返回完整 EAX32；零结果取得 `lPitch >> 2` 作为
   W32 像素行步长，并在 `452882` 读活动头 `C8A72C`。
2. 每项先在 `452899–4528C0` 读取 `+0` 的 S32 X、
   `+4` 的 W32 Y；X 须有符号 `>=0` 且 `<636`，
   Y 须有符号 `>=0` 且 `<468`，否则跳过本项。
   合格项以锁定像素基址和 pitch 定位原点，
   再读取 `+0x3C` 的 W16 种类；已见 1、2、3、4
   四组写入，其他值不在此写像素。
3. 种类 1 (`4528D6–452904`) 在 `(0,0)`、`(0,1)`、
   `(1,2)`、`(1,3)` 各写一次 `gPaletteAutoMap[143]`。
   种类 2 (`452909–452A20`) 读 `+0x40` W16：
   变体 0 写 3×3，四角为 `161`，其余为 `159`；
   变体 1 写 4×4，四角为 `162`，其余为 `159`；
   其他变体不写。这里及下文数字都是
   `gPaletteAutoMap` 索引，各写入点分别取当前表值。
4. 种类 3 (`452A25–452BBB`) 同样读 `+0x40` W16，
   各变体相对原点的**完整写入位置与索引**如下；
   未列出的像素保持原值，其他变体不写：

   | 变体 | 相对像素 → 调色板索引 |
   |---|---|
   | 0 | `(-1,-1)→8, (0,-1)→255, (1,-1)→8; (-1,0)→255, (0,0)→255, (1,0)→255; (-1,1)→8, (0,1)→255, (1,1)→8` |
   | 1 | `(0,-1)→251, (-1,0)→251, (0,0)→251, (1,0)→251, (0,1)→251` |
   | 2 | `(0,0)→251, (1,0)→255, (0,1)→255, (1,1)→251` |
   | 3 | `(0,0)→255, (1,0)→251, (0,1)→251, (1,1)→255` |
   | 4 | `(0,0)→251, (1,0)→193, (0,1)→193, (1,1)→193` |
   | 5/6/7/8 | 仅 `(0,0)`，依次为 `198/193/208/251` |

   变体 0、1 的负偏移不做额外逐像素裁剪；
   入口的原点门禁不保证这些写入仍在 Back 内，
   不擅加新保护并宣称原行为等价。
5. 种类 4 (`452BC0–452CC0`) 的选色先按
   `+0x40` W16 分组，再读 `+0x3E` W16；
   表内数字为 `gPaletteAutoMap` 索引：

   | `+0x40` | `+0x3E=0` | `+0x3E=1` | 其他 `+0x3E` |
   |---|---:|---:|---:|
   | 0 | 223 | 217 | 159 |
   | 1 | 212 | 208 | 159 |
   | 2 | 130 | 147 | 159 |
   | 其他 | 116 | 99 | 159 |

   `452C5B–452CBE` 外层 `j=0..(+0x2C-1)`、
   内层 `k=0..(+0x28-1)`，两者按 S32 比较，
   宽/高非正时不写。每次只对记录 `+0x20+k`
   和 `+0x24+j` 分别检查非负且 `<640/<480`；
   通过后写到**已经带有 `+0/+4` 原点**的像素基址
   再偏移 `(+0x20+k)+pitch*(+0x24+j)`。
   `452C73–452CA1` 的实际汇编证实这次叠加；
   逐像素门禁没有检查叠加原点后的最终坐标。
   不能以入口 `636×468` 条件或通常矩形裁剪替代。
   已查 kind 4 生产者 `46ED50` 在
   `46EDD5/46EE83` 两处分支领取记录并写
   `+0/+4/+0x3C/+0x40/+0x30`，在
   `46EF9A/46EFB2` 更新 `+0x28/+0x2C`，
   均不写 `+0x20/+0x24`；`46E340` 领取、
   `46E390` 回收与 `46E2A0` 链初始化也不清
   这两个载荷。不能把初次映像零填充当作复用后
   仍为零的保证，亦未据已查路径断言间接写入穷尽。
6. `452CC0–452CC9` 每项通过 `+0x38` 读下一原
   W32 指针，零时 `452CD1–452CE5` 对同一 Back
   调 `Unlock(null)` 并透传完整结果；不把 Lock、
   遍历和 Unlock 拆成每项一次。

[Palette.hpp](../include/cg/Palette.hpp) 已持有
`gPaletteAutoMap[256]`，但活动头、500 项记录的
x64 指针布局仍受[场景证据](cg2d_scene_state.md)
的 `.184` SPEC 门禁；当前 CPU Back 也只获批用于
波纹链。这里不根据绘制函数已消费字段就断言未知记录区
无指针，不落位部分像素绘制或固定返回，不计
`452840`、case 9、统一画面链、链接与运行完成。
