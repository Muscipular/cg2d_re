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
原调用点、已定的自动 driver 策略、最终帧来源、尺寸与颜色语义、窗口缩放/DPI、设备丢失及重置事件仍待接入和运行验证。
本适配器不替代应用入口的 SDL 启动约定，不处理事件，也不声称满足原客户端画面语义。

未修改计划、进度或 CMake，未编译、运行、编写测试或提交。
当前仅完成实现和本地头文件静态核对，不代表已通过链接、外部窗口生命周期或真实画面的运行验收。

## ClearSurface@0x4512C0：原端证据与统一逻辑 Back

状态：**底层 owner 与清屏叶已落位，父级接线未完成**。下方裁决前的候选文字保留作为历史边界，当前实施口径见 2026-09-25 裁决与源码状态。

### 原端已核实事实

- `Renderer::ClearSurface@[0x4512C0,0x451306)` 为单块函数，无显式参数、无 `this`；末尾 `retn`，`EAX` 原样返回间接 `Blt` 的结果，不作成功常量替换。
- `0x4512C4–0x4512E4` 构造局部 `DDBLTFX`：100 字节清零，`dwSize=100`，从 `gBackgroundColor@0x62B0A8` 读取完整 W32 写入颜色字段。
- `0x4512D6–0x4512FE` 从 `renderer@0x627360` 的 `+0x0C` 取 Back，经其 vtable `+0x14` 间接调用 `Blt`；目标矩形、源 surface、源矩形均为 null，传入上述颜色 fx，flags 为 `0x01000400`。本体不改 Temp 或 dirty，不设置失败分支；这不等于已审完所有调用方的返回处理。
- 窗口模式的 Back 创建证据位于 `0x44B946–0x44B951`；全屏模式在 `0x44BA71–0x44BA8E` 从前表面取得 attached Back。释放位于 `0x44C4E5–0x44C4F8`；其新端统一 owner 与创建、重建、销毁接线尚未裁决。
- `MainLoop` 在 `0x457294` 先读取 `0x73ECC0`，`0x457299` 再清 `0x73ECE4`；读得 0 时于 `0x4572AF` 调用本函数，读得 3 时于 `0x4572A8` 调用 `sub_423640`，其他值不走这两个调用。汇合后依次为 `DrawAllQueued@0x4572B4`、`sub_468FF0@0x4572B9`、`Flip@0x4572BE`；这里弃置 ClearSurface 返回值。本函数在当前 IDA 有 23 处直接代码调用点，分布于 17 个函数；其余调用点的返回消费见下节。当前接线及声明见 [MainLoop.cpp](../src/platform/MainLoop.cpp) 和 [MainLoopDependencies.hpp](../src/platform/MainLoopDependencies.hpp)。

### 与当前 SDL/CPU 边界的区别

- [SdlPresenter.cpp](../src/render/SdlPresenter.cpp) 的 `SDL_RenderClear` 发生在整帧上传后的最终输出阶段，随后绘制完整纹理并提交；它不清空上层逻辑 Back，不能代替 `ClearSurface` 的当次 Back 颜色填充。
- [SurfaceState.hpp](../include/cg/render/SurfaceState.hpp) / [SurfaceState.cpp](../src/render/SurfaceState.cpp) 最初只有波纹链使用的常驻 CPU Back/Temp；2026-09-25 裁决后已改为唯一动态 owner，波纹快照复用窄接口。这只落实存储及清屏叶，不证明画面链已完成。
- 已批准 DX7 底层 `Blt` 等价映射 SDL/CPU，不要求逐码复刻 DX7 `HRESULT` 或 COM vtable；但上层触发、W32 颜色、像素内容、共享 Back/Temp 生命周期与提交顺序仍受 [渲染替换约束](../plan/cg2d_full_reconstruction_plan.md) 约束。

### Back/Temp 原生命周期的拆分

2026-09-25，当前已打开 IDA 的局部反编译与指令交叉核对：

| 阶段 | 地址 | 可见状态与门禁 |
|---|---|---|
| `Renderer::Init` | `44B946–44B956`、`44BA71–44BA93` | 窗口模式创建独立 Back；全屏模式从前表面取得 attached Back。两路非零底层结果均走本入口失败返回，不进入其后像素格式路径。 |
| `BuildSurfaceCache` | `5231B5–5231F5` | 先清三个缓存计数/状态，创建并发布 640×480 系统内存 AutoMap；失败显示提示、返回 0，此时尚未尝试 Temp。 |
| `BuildSurfaceCache` | `5231F6–523257` | 先请求 640×480 显存 Temp 并写全局槽；为空才请求系统内存 Temp，再以第二次结果覆写该槽。两次均为空则显示提示、返回 0；AutoMap 已发布且本入口未释放它。 |
| `Renderer::Destroy` | `44C436–44C45B`、`44C4E5–44C4F8` | 在纹理块释放后，依次有条件释放并清 Temp、AutoMap；在设备/调色板等释放之后，才有条件释放并清 Back。 |
| 初次初始化 | `457747–45774E` | 调 `BuildSurfaceCache` 后检查完整 EAX32，零值走本处失败退出。 |
| `Renderer::Rebuild` | `466B8C–466C1D` | 先 `Destroy` 后 `Init`，可能切换窗口模式再调一次 `Init`；无论前述消息/投递路径如何，随后调用 `BuildSurfaceCache` 且不检查其返回，继续 `InitPalette`。 |

两面逻辑缓冲不能因为新端此前是静态数组，就把原先
不同阶段的发布、失败和重建顺序写成“一次原子创建”。
DX7 显存/系统内存 caps 已获底层替换豁免，但不等于
上层可忽略初次构建的零返回门禁，或在重建时补加
原本没有的返回门禁。本节只固定时点和已见副作用；
SDL/CPU 的创建失败、剩余资源状态及驱动选择仍待 SPEC。

### 当前源码接线边界（2026-09-25 静态核对）

- `SdlPresenter::Create/Present/Destroy` 目前仅在 [SdlPresenter.hpp](../include/cg/render/SdlPresenter.hpp) 声明、[SdlPresenter.cpp](../src/render/SdlPresenter.cpp) 定义；在本子项目 `src/`、`include/` 内未找到实例创建或这些成员的调用。`Present` 内部的 SDL 清输出、上传纹理和提交不能证明原逻辑 Back 已送达最终窗口。
- Back/AutoMap/Temp 现由 [SurfaceState.cpp](../src/render/SurfaceState.cpp) 唯一私有持有；[GraphicPendingQueue.cpp](../src/render/GraphicPendingQueue.cpp) 的波纹快照及 [MainLoopSupport.cpp](../src/platform/MainLoopSupport.cpp) 的 Temp→Back 回拷都经窄接口。背景色仍由 [LoginCapability3Scene.cpp](../src/LoginCapability3Scene.cpp) 写入同一 `gBackgroundColor`。`ClearSurface` 已有定义，`DrawAllQueued/Flip` 仍只有真实声明。
- 这是当前源码接线事实，不是原端新取证；清屏叶单独落位尚不足以闭合初始化、图形队列、翻转和销毁链。

### 裁决前的 SPEC 候选（历史）

- 此前候选是将 `ClearSurface` 映射为对统一逻辑 Back 的 640×480 区域按每次调用时读取的完整 W32 背景色填充；保留调用时点及后续绘制、提交顺序，不使用固定成功返回。这部分已按下方裁决落实。
- 此前待决的统一 owner、与波纹链共享、资源顺序及自动 driver 已由下方裁决确定。原端事实和其他未决的图形、字体、帧槽及父级调用仍分别保留。

### 2026-09-25 授权裁决与实施边界

用户将上述新端选择交由本线程决定。本节自此覆盖本章先前
“待批统一 Back/Temp、driver 与失败策略”的候选状态；
此前地址事实和未实施的源码状态不因此自动变为完成。

1. `cg::render::SurfaceState` 是 CPU 逻辑 Back、AutoMap、Temp
   的唯一 owner。复用现有波纹链使用的同一 Back/Temp，
   不为 `ClearSurface`、绘制队列或 `Flip` 创建另一套缓冲；
   原 x86 的窗口 Back 与全屏 attached Back 在已获批准的
   DX7 底层替换范围内统一为一面 640×480、32 位 CPU Back。
2. Back 在 renderer 初始化阶段创建；缓存构建阶段先发布
   AutoMap、再尝试 Temp。显存/系统内存 caps 不仿真，
   CPU 分配失败返回真实失败；Temp 失败时已发布的
   AutoMap 保留至原销毁阶段，不做原子回滚。销毁按
   Temp→AutoMap→Back；重建仍先销毁、再初始化、再构建。
   初次构建的零返回按原门禁退出；重建调用不因缓存
   构建零返回新增早退。静态数组不再代表“始终已创建”。
3. SDL 最终呈现器在原生窗口有效且 SDL 主线程上创建，
   `driver=nullptr` 由 SDL 自动选择，不指定
   `direct3d11`，调用方不自行枚举、重试或回退。
   `Create/Present` 失败保留阶段与真实错误；不能固定
   返回成功、静默提交空白帧或替原上层补加状态门禁。
   精确的 `Renderer::Init` 返回 1/2 分流、窗口过程
   接线和 `Flip` 共享帧槽仍须分别按原地址实现与检查。
4. `ClearSurface@4512C0` 在当次调用读取完整 W32 背景色，
   仅填充已创建的统一 Back，不动 Temp；已有 23 个
   直接 caller 均弃其结果。新端内部以实际可用状态
   形成成功/失败，不制造固定成功值；DX7 原生
   `HRESULT` 逐码返回已获底层替换豁免，但此豁免
   不扩展到其他上层结果消费或 SDL 提交失败。

本裁决解除上述 owner 与驱动选择的设计门禁，不替代
资源初始化、图形队列、字体、纹理、逐像素绘制、最终
提交或完整客户端链接与运行的验收。

### 已落位的 CPU 面与清屏叶

[SurfaceState.cpp](../src/render/SurfaceState.cpp) 以私有
`unique_ptr` 分别持有固定 640×480、W32 的 Back、
AutoMap、Temp。`CreateCpuBackSurface` 只创建 Back；
`BuildCpuSurfaceBuffers` 先发布 AutoMap、再分配 Temp，
实际分配失败返回 false，Temp 失败不回滚 AutoMap。
`DestroyCpuCacheSurfaces` 先释放 Temp→AutoMap，
`DestroyCpuBackSurface` 单独释放 Back，以保留
`Renderer::Destroy@44C467–44C4DD` 在其间释放设备、
调色板、clipper 的阶段位置；纹理槽仍须在前一阶段之前
按 `44C412–44C433` 处理。上述创建与销毁边界尚未与
`Renderer::Init/BuildSurfaceCache/Destroy`
父入口接线，不把辅助函数当作原父函数已实现。

`ClearSurface_4512C0` 只有 Back 非空才读取完整
`gBackgroundColor` 并填满同一面；无 Back 返回非零，
成功返回零，不替换原 `HRESULT` 逐码语义。
`CopyBackToTemp` 与原波纹路径、`BlitTempToBack` 共用
同一资源状态；缺少任一面时不复制并报告失败。
`ReadCpuBackPixels` 只提供 Back 生命周期内的非拥有
只读视图，未创建时为空。当前 `MainLoop` 对清屏叶
的结果仍按原调用丢弃；AutoMap 的后续真实消费者、
SDL 帧提交、`Flip` 帧槽和父级失败分流继续待实施。

### `453DE0` 初始化颜色掩码计数叶

原 `[453DE0,453DFD)` 从 `453DE5` 读取完整 W32
实参，`453DEA–453DF9` 用从 bit 0 左移的掩码循环
32 次，每次 `test` 非零才递增 EAX，返回置位数
`0..32`。`Renderer::Init` 在 `44BB3E/44BB57/
44BB70/44BB89/44BB96/44BBAA` 六次直接调用，
用于其颜色掩码及通道位移计算。新端
`CountMaskBits_453DE0` 在既有调色板模块用
`std::popcount(uint32_t)` 保留完整 W32 位模式和
返回值；父 `Renderer::Init` 尚未落位，这六处
生产与消费未接通，不计渲染器初始化完成。

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
生命周期或绕过 SDL 提交失败处理。上述裁决已确定
统一 owner 与状态结果口径；清屏叶已落位，其他父链未完成。

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
统一 Back owner 和驱动选择已有裁决；创建/销毁接线及 SDL 错误时点仍须实施，不编码 `Flip`，
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
| 0、1 | `520E54` | 常规/裁剪图形，完整本地分流另见下文 |
| 2 | `5216E9` | 将记录与 `+8` 值传给 `522210`，后者经 `451350` 写 Back |
| 3 | `5216FB` | 将记录传 `Renderer::FillDirtyArea` |
| 4 | `521709` | 有 3D 设备时先 `EndScene`，再以记录 `+0x0C` 调文本绘制，之后有 3D 设备时重新 `BeginScene`；完整本地分流另见下文 |
| 5 | `521752` | 以记录 `+0/+4` 两值调用 `46C760` |
| 6 | `521766` | 受 `byte_C82204` 门控，裁剪后从 Temp `BltFast` 到 Back |
| 7 | `5217F2` | 受同一门控，从 Back `BltFast` 到 Temp |
| 8 | `52181E` | 调已落位的波纹入口 `521870` |
| 9 | `521828` | 调 `46E480` |
| 10、11、12 | `52143C` | 缩放分支另见下文合同 |

  默认目标 `5213D6` 在 `5213DA–5213F3` 递增并行队列游标
  与已处理数，每轮重新比较有符号 W16 计数；类型分支
  结束均汇入该处。
- `5213F9–521414` 在队列结束后若有 3D 设备则调用
  `EndScene`，返回不作门禁。`521414–521428` 仅当
  `W32[1020ECC]==0` 才将调色板版本 `W32[628FDC]`
  写入独立 `W32[58B7E0]`；随后无条件调用
  `44E230`、清 `W8[1020EDC]`，并从 `521434` 返回。
  `44E230` 的 104 项延迟老化写序见
  [DX cache 第 5.3 节](cg2d_graphic_dx_cache.md)；
  它只清过期调色板引用，不在本处释放整池。
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

## `520DD0` case 4 文本链提交

2026-09-25，当前已打开 IDA 只读核
`521709–52174D` 的实际指令。此处没有先判断
文本指针或渲染返回值：

1. `521709` 调 `hasIDirect3D7@44B610`；返回非零时，
   `521712–521720` 从 renderer `+0xA0` 取设备，
   调其 vtable `+0x18`（`EndScene`），不检查结果；
   返回零则直接到 `521723`。
2. `521723–52172C` 读取当前待提交图形记录
   `+0x0C` 的完整 W32，作为唯一实参调用
   `RenderTextToSurface@456D70`。原端这是文本链头指针；
   当前 [TextPendingQueue.cpp](../src/render/TextPendingQueue.cpp)
   的 `QueueText_521A30` 在相同字段保存一基 `headToken`，
   不能在新端将令牌直接当 x64 指针解引用。
3. `52172F` 再次调用 `44B610`，不是复用第一步
   的结果；非零时 `52173C–52174A` 再从 renderer
   `+0xA0` 取设备，调 vtable `+0x14`
   （`BeginScene`），结果同样不检查。零时从
   `521736` 跳公共计数尾；非零调用后也到同一尾。

`44B610@44B616–44B623` 只检查 renderer 的
`+0x9C` IDirect3D7 接口槽是否非零，实际设备在 `+0xA0`。
`456D70@456D78–456D93`
先从 Back `+0x0C` 获取 DC，设置透明背景；
`456D9D–456DA2` 按链头 `renderMode@+6` 分流，
最终 `456FB2–456FC3` 对 Back 释放 DC。其字体句柄、
文本字节、调色板颜色和逐节点绘制尚未在本节逐支闭合，
不能把 case 4 的局部分流取证当作 `456D70` 本体落位。
现有文本队列一基令牌、统一 Back/Temp、字体 owner 与
SDL/CPU 文本提交仍各有未决边界；本节不实现
`DrawAllQueued`，不计最终画面、完整链接或运行。

## `456D70` 文本链 Back 绘制本体

2026-09-25，当前已打开 IDA 的反编译及
`456D70–456FCD` 指令交叉核对。唯一直接 caller
为 case 4 的 `521727`；函数无返回值。

- `456D70–456D93` 从 renderer `+0x0C` 的 Back 调
  vtable `+0x44` 获取 DC，随后 `SetBkMode(DC,1)`；
  `456D99–456DA2` 先读取链头 `+6` 的 W8 模式，
  再对模式 0 的链头作零检查。尾部
  `456FB2–456FC3` 重新读取 Back，调用其 vtable
  `+0x68` 释放 DC。获取/释放与 GDI 绘制结果
  均不作为本函数的分支门禁。
- 模式 0：`456DB2–456E86` 沿节点 `+0x10C` 的
  原 W32 next 遍历。仅文本 `+9` 首字节非零时，
  从 `fontType@+5` 的无符号 W8 以 `0x18` 步长
  读取 `gFontList` 句柄并 `SelectObject`；
  若 `color@+4` 未置 `0x80`，先设颜色 0、
  再选一次同字体，以 `(x+1,y+1)` 绘阴影。
  随后按 `color & 0x7F` 取三字节颜色并在
  `(x,y)` 绘正文。每次 `TextOutA` 的长度
  均重新按原字节 NUL 扫描，坐标从 `+0/+2`
  的有符号 W16 扩展，GDI 返回不判断。
- 非零模式：`456E91–456EFB` 先设颜色 0，
  全链只对未置 `0x80` 的节点画 `(x+1,y+1)`
  阴影，不先以文本首字节为门禁。
  `456EFB–456FAC` 随后按 13 项颜色表
  `textColor@5590B8` 顺序扫描，每项都重新
  从原链头沿 `+0x10C` 遍历；仅
  `(color & 0x7F) == 当前颜色序号` 的节点
  在 `(x,y)` 绘正文。每轮首次匹配才设置
  颜色，此轮其他匹配沿用该色；进入下一轮
  重新清首次匹配标志。这里也不检查文本
  首字节，长度仍逐节点取原字节 NUL 长度。
- 两路的颜色 W32 均按表项三字节
  `byte[0] | byte[1]<<8 | byte[2]<<16` 组成；
  `456F03` 从表项 `+1` 起每轮加 3，以
  `pszFaceName@5590E0` 为不含端点，实际遍历
  `[5590B8,5590DF)` 的 13 项。字体引用沿用
  已取证的 14 项 `0x18` 原记录，不把其
  Win32 句柄压入现有 W32 文本记录。

新端已有 [TextPendingRecord](../include/cg/render/TextPendingQueue.hpp)
的 `x/y/color/fontType/renderMode/text/nextToken` 对应
字段和一基队列令牌；这只说明存储字段可定位，
不等于 Back 的 SDL/CPU 文本绘制已存在。
原 `TextOutA` 消费原字节和 NUL 长度，不能把
UTF-8 字节长度直接当成旧端长度；具体字体
owner、旧编码到现代显示边界的转换和
Back 生命周期依原 [文本编码边界 SPEC](cg2d_text_encoding_spec.md)
及各自待批门禁推进。本节不编码替代绘制，
不计 case 4 画面、完整链接或运行完成。

## `520DD0` case 0/1 常规图形分流

2026-09-25，当前 IDA `520E54–5213D4` 为跳表
0/1 的共享目标；下述只闭合父函数内这一分支，
不代表其三个图形装载叶或两种底层绘制已实现。

1. `520E54–520E7C` 从记录 `+8` 读取 S32 图号，
   仅在 `0<=index<gMAX_GRAPHIC_COUNT` 时依序调用
   `Graphic::LoadPatelet@523140(record)` 与
   `Graphic::LoadGraphic@522570(index)`；后者零返回
   直接进公共下一项。成功后 `520E82` **重读**
   `+0x24` W8 类型，仅为 0 才依序清 `+0x16/+0x14`
   W16 位置，并从 DX 项 `+4/+6` 的 S16 写记录
   `+0x18/+0x1A` W16 尺寸；类型 1 保留这四字段。
2. `520EB4–520F8B` 将上述四个 W16 按 S16
   扩展为 X/Y/W/H，先检查 `X+W>=0`、`Y+H>=0`、
   `X<=DX.w`、`Y<=DX.h`。X 负时先缩短宽度，
   再将记录 `+0` 的完整 W32 坐标减 X、局部 X
   改零；Y 负时同样缩短高度、将记录 `+4` 的
   完整 W32 坐标减 Y、局部 Y 改零。这是对队列
   记录的**原位副作用**，不是只调整临时矩形。
   再以 DX 宽高收缩右/下边；宽或高非正则直接
   跳公共下一项，不进行缓存尾部的 dirty 清理。
3. `520F91–520FC0` 读取记录 `+0x0C` 原 W32
   对象引用；为空，或对象 `+0x5C/+0x60` 两个
   F32 缩放均与原 F64 `1.0@54D5C0` 相等时，
   进入无缩放 `5211E7–5213B0`；其余进入缩放
   `520FC0–5211E2`。两路都逐块先将全局
   `state` W32 写块 `+8`，按块 `+0x0C/+0x0E`
   的 S16 偏移、全局纹理宽高与已裁剪的矩形
   作有符号相交门禁；合格才调
   `Graphic::LoadTexture@520B10(record,block)`，
   零返回跳本块，并沿块 `+0x10` next 继续。
4. 装载成功的块分别按交界推进源矩形
   left/top、缩短 right/bottom；混合 W8 的
   bit 0/1 决定 X/Y 镜像方向。无缩放路将相对
   坐标加记录 `+0/+4` 的完整 W32 原点，
   传源矩形值、块 surface/图号/调色板索引、
   混合模式和记录 `+0x1C` 颜色给
   `Renderer::RenderTextureToSurface@44D520`。
   缩放路分别用对象 `+0x5C/+0x60` 的 F32
   乘相对 X/Y、经 `__ftol` 后再加原点，改调
   `Renderer::RenderTexture@44DA90` 并同时传
   两轴系数。两路仅在完整返回恰为
   `0x887601AE` 时写现有 `gGraphicDirty=1`，
   不能以任意失败或画面相似替代。
5. `5213BB–5213D4` 在块链为空或遍历结束后
   重读 DX 项 `+8` 调色板池引用，非空才清目标
   `+0x0C` 的 W32 脏计数；**不是**清 DX 项
   `+0x0C`，也不释放资源。早期图号、装载、
   几何门禁直接进公共下一项，不能无条件执行
   这次尾清。

现有 `GraphicCacheItem32::dataBits` 仅承载原
`+0x0C` W32 位型，尚无获批的 x64 对象身份映射；
DX 表、纹理块链、两条真实绘制叶和统一 Back
生命周期亦未落位。此处不把对象位型直接强转
本机指针，不凭已核父分支伪接整个队列。

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
读取唯一队列记录（后续按 DX §15 扩为 48B）；Win32 `RECT` 及 `451350`
真实未落位声明只在 render 实现边界。包装本体已落位，
`451350` 的 32 位 Back 写入后续已落位，见文末
“`451350` 的 32 位矩形像素本体”；原队列 case 2 父入口、
Back 初始化父链与客户端完整链接/运行仍未闭合。

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
其 `44C9A0` 依赖后续已按[完整入口 SPEC](cg2d_texture_rendering.md)
落位，不替它返回固定值。
`FillRect` 原本体在没有 3D 设备、裁剪拒绝或 renderer
`+0xBC` 门禁为零时返回零，否则提交四顶点矩形并透传
`DrawPrimitive` 结果。新端复用已实现的矩形提交、三组混合及空纹理绑定，
保留两次接口检查和原 S16 裁剪，不额外拒绝反向矩形。
上层 dirty 状态写入未改：真实 Back 锁定仍映射 surface-busy，
其他 native 失败不伪装成该码。父 `DrawAllQueued`、完整画面与链接
运行尚未闭合，本体接线只作静态核对。

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

## `520DD0` case 10–12 的缩放分支

2026-09-25，当前 IDA `52143C–5216E4` 为十三项
跳表中 10/11/12 的共用目标；`52143C–52146C`
按原 W8 类型分别给 F32 缩放系数
`0.25f/0.5f/0.75f`，余值的初始 `1.0f`
不属于这三个表项。下述仅闭合本地分支，不替代
各 callee 或整个 154 块父函数。

1. `52146C–521494` 从记录 `+8` 取 S32 图号，
   必须 `0<=index<gMAX_GRAPHIC_COUNT`；先无条件
   调 `Graphic::LoadPatelet@523140(record)`，
   再调 `Graphic::LoadGraphic@522570(index)`，
   后者零返回跳公共下一项。成功后
   `52149A–5214CD` 依序清记录 `+0x16/+0x14`
   两个 W16 位置，并从 16 字节步长的
   `gGraphicInfoDxCache[index]` 的 `+4/+6`
   两个 S16 写记录 `+0x18/+0x1A` 尺寸。
   宽高非正则跳公共下一项；链头为零只跳过逐块循环，
   仍到 `5216D1` 的缓存尾部。不提前清记录或跳过
   任一装载调用。
2. `52152D–52157D` 逐块先将当前全局 `state`
   的 W32 写块 `+8`，再用块 `+0x0E` 的 S16 Y、
   `+0x0C` 的 S16 X 与 `gTextureSizeH/W`
   作有符号相交门禁；合格块才调用
   `Graphic::LoadTexture@520B10(record,block)`，
   零返回只跳过本块。`521583–5215E2` 把源矩形
   初设为 `[0,0,textureW,textureH]`，负块偏移
   推进 left/top，超出图宽高则收缩 right/bottom。
   不把这套逐块边界合并成事先裁掉整幅图。
3. `5215E6–521675` 从记录 `+0x21` 读 W8 混合
   模式：bit 0 的镜像 X 源量为图宽减块 X 再减
   源矩形 right，否则为块 X 加 left；bit 1 的
   镜像 Y 同理使用图高、块 Y 与 bottom，否则
   为块 Y 加 top。各源量乘本项 F32 系数后经
   原 `__ftol` 取整数，再分别加记录 `+0/+4`
   的完整 W32 坐标；不将这两个坐标拆成 S16。
4. `521675–5216AE` 向 `Renderer::RenderTexture@44DA90`
   传目标 X/Y、源矩形、两次相同缩放系数、块
   `+0` surface、`+4` 图号、`+0x18` 调色板
   W16、混合 W8 与记录 `+0x1C` W32 颜色。
   仅当完整 EAX32 **恰为** `0x887601AE` 时写
   现有 `gGraphicDirty=1`。`5216C0–5216E4`
   遍历块 `+0x10` next；走完后若 cache 项
   `+8` 指针非零，清其目标 `+0x0C` W32，再回
   公共下一项。跳过绘制的块不因此跳过遍历。

当前工程有队列记录及 dirty 唯一 owner，但没有
`gGraphicInfoDxCache`、块链、`state`、纹理尺寸的
统一新端 owner，三个装载叶及 `44DA90` 亦未落位。
原 DX7 `RenderTexture` 的底层返回码虽获替换，
上层精确 busy 门禁的等价映射仍需 SPEC；不接半段
父函数、不伪造装载成功或纹理资源。

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

## SDL 最终提交与 `Flip` 单一 owner SPEC（方向已裁决，接线未完成）

2026-09-26，本线程在现有 `ida-17524/cg2d.exe` 会话只读重核
`4512C0`、`44C560`、`44C400` 和 `44B660` 的相关指令，
再核当前 `SurfaceState`、`SdlPresenter` 与 `MainLoop` 接线。
`ClearSurface_4512C0` 已在同一 CPU Back 上填充背景色；
`MainLoop` 按原序调用 `DrawAllQueued→468FF0→Flip`，
但当前 `SdlPresenter::Create/Present/Destroy` 均无生产调用，
`DrawAllQueued` 与 `Flip` 仍只有真实声明。不能只实现一个
调用尚未创建的呈现器的 `Flip`，再把每帧失败当作原端状态。

**按用户委托选定的唯一 owner 与写序：**

1. `cg::render` 独占一个 `SdlPresenter` 实例，沿既有
   `Renderer::Init` 在有效 `gLegacyWindow` 所属的 SDL 主线程
   创建，传固定 640×480 帧和 `driver=nullptr`；同一
   renderer 的销毁/重建路径负责在窗口失效前销毁。
   `44C436–44C4FB` 已证 Temp、AutoMap、原生对象和 Back
   的分阶段释放；新端在释放 CPU Back 前释放 SDL 呈现器，
   保持缓存面先释放、Back 后释放，不新建另一套 Back。
   创建失败的原入口结果和清理顺序须再逐段核 `Renderer::Init`
   与调用者，不能用固定成功、自动重试或提前关闭窗口替代。
2. 独立 `W32[628FC8]` 由 `cg::render` 唯一持有，映像初值零；
   不并入 `gRendererLost`、图形队列计数或 `SdlPresenter`
   私有诊断。`Flip@44C68D–44C69A` 每次都读旧完整 W32，
   写 `old==0 ? 1 : 0`，包括呈现失败和 Back 缺失时。
   已查 `447070–447C40/521F20` 等读者应使用同一槽，
   不能复制为各绘制模块的快照。
3. `Flip` 从唯一 CPU Back 取本次整帧，交同一
   `SdlPresenter::Present`；SDL 上传、绘制和提交失败映射为
   `gRendererLost=1`，成功时不清旧 lost 值。然后执行第 2 项
   帧槽翻转。原模式 1 的客户区矩形、`GetScanLine` 等待及
   非模式 1 的 DirectDraw `Flip/Blt` 属已批准的原生替换边界；
   SDL 对外部窗口的实际客户区呈现仍须运行核对，不能据此
   宣称窗口缩放、扫描线等待或 DX `HRESULT` 逐项一致。
4. `DrawAllQueued` 必须在 `Flip` 之前把内容写入同一 CPU Back。
   `520DD0` 的 13 类批次、缓存与纹理状态不能用空绘制跳过；
   本 SPEC 不批准半个 `520DD0`、演示空帧或启动客户端。

本节确定接口和 owner 方向，不代表生产源码已接线。实施时先闭合
`Renderer::Init/Destroy` 的 SDL 创建与销毁、失败分流，
再接 `Flip` 的实际呈现和共享帧槽，最后按原绘制批次
闭合 `DrawAllQueued`。Debug 静态库编译不等于完整链接、
SDL 窗口呈现或 32 位像素对照通过。

2026-09-26，用户再次要求可行时使用 SDL，继续采用现有
SDL3 `SDL_Renderer` 加 CPU Back/Temp 替换 DX7 原生呈现；
音频按用户当前优先级暂缓。该决定只解除本节方向门禁，
不把 `BuildSurfaceCache` 后段的 DX 纹理双池、上层
`Renderer::Init/Destroy` 全部失败路径、`Flip` 帧槽消费者
或 `DrawAllQueued` 批次当作已实现，也不启动客户端。

### lost 后的恢复门禁与重建顺序

2026-09-26，只读现有 IDA 的 `44C6B0–44C6C3`：
从 renderer `+4` 取 DirectDraw7，调用 vtable `+0x68`
的 `TestCooperativeLevel`，完整返回值恰为零时返回 1，
其余返回 0。`MainLoop@457096–4570A9` 先要求
`gRendererLost@62B064` 非零，再要求该结果非零，
才调用 `466B80`；模式不匹配则走另一独立的重建入口门禁。

`466B80–466BB0` 先保存完整 `103CF10` W32，
依次调用 Destroy、Init；Init 的 `AL` 经有符号扩展
后减一判断，值 1 发布错误提示与消息，值 2 翻转
`gWindowMode` 并再次 Init。随后无论这些分流如何，
仍调用 BuildSurfaceCache 和 InitPalette，
并继续场景相关更新；不得把 SDL 呈现失败直接变成
`MainLoop` 的提前退出或单纯的下一帧重试。
`466C8A/466CAF` 在绘制两侧临时写零、恢复此前
保存的 `103CF10`，也不能由 `Flip` 私自接管。

**SDL 恢复映射（2026-09-26 按用户委托裁决）：** SDL 没有上述
DirectDraw `TestCooperativeLevel` 的同型结果。
在平台窄边界检查当前线程是 SDL 主线程，且
`gLegacyWindow` 指向仍有效的 Win32 窗口；
两者均成立时，`44C6B0` 的新端对应入口返回 1，
否则返回 0。它仅回答“可尝试 Destroy→Init”，
不伪称设备已经恢复或探测 `Present` 一定成功；
不创建新窗口、不在此调用 Create，也不清 lost。
首次 Create 或重建 Init 的实际失败仍沿各自
原调用者的返回和错误分流处理。窗口无效或不在
主线程时保留 lost、等待后续原门禁调用；窗口有效
但 SDL 持续失败时可逐帧尝试重建，不新增退避或
固定成功值。这与原端合作级别持续返回零时也
可能逐帧进入重建的条件结构相符，不宣称失败码
或时序逐项等价。新端 `44C6B0` 已在
`SdlPresenter.cpp` 中按 SDL 主线程和 `IsWindow` 门禁落位，
`gLegacyWindow` 仍由原窗口 owner 持有；不读取或清除 lost。
`466B80` 后续完整父函数已落位，见文末“重建与场景快照”；
Init、实际模式状态和完整画面仍缺，本门禁单独实现不计恢复流程完成。

### `Renderer::Init` 的实际低字节返回域

2026-09-26，在已打开的 IDA 会话只读遍历
`Renderer::Init@[44B640,44C3B7)` 的全部 14 个 `retn`。
早退点 `44B6F3/44B73C/44B7DB/44B827/44B8C0/
44B980/44B9C2/44BA70/44BABA/44BAE1/44BB1D/
44BC14` 均在返回前写 `AL=1`；正常及降级的
`44BE4B/44C3B6` 均在返回前写 `AL=0`。这里只
限定低字节，不将其上层未清理的 EAX 高位臆定为零。
`466B91–466BA8` 确实先调用 Init，再对 `AL`
有符号扩展后检查 1 和 2；但按当前本体返回点，
其中的 2 分流不可由该直接调用产生。SDL 创建
失败应落入原有 1 类失败分流，不应为激活模式
切换而伪造 2；重建父函数中该分支的指令仍须
保留，不以不可达为由删除。原 Init 一开始的
`44B66E` 还先清 `gRendererLost`，失败早退并不
自动回滚已发布的资源；底层替代的释放顺序仍
须随完整 Init/Destroy 接线核对。本节仅补证据，
不将父级生命周期或 SDL 画面记为已实现。

### SDL 首次初始化前的窗口线程门禁

2026-09-26，本机 SDL 3.4.8 的 `SDL_init.h` 将
VIDEO 主线程定义为执行其初始化的线程；同版本
`SDL.c` 的 `SDL_IsMainThread` 在尚无 VIDEO、
EVENTS、MAIN 线程 ID 时返回 true。因此
`SdlPresenter::Create` 原先在 `SDL_InitSubSystem`
之前只查 `SDL_IsMainThread`，不足以拒绝从其他线程
首次包装既有窗口。

新端先以 `GetWindowThreadProcessId(nativeWindow)`
与 `GetCurrentThreadId()` 比较；无效窗口或非创建
线程均在 SDL 初始化前失败，不夺取 VIDEO 主线程。
已经有 SDL 线程标识时仍保留 `SDL_IsMainThread`
门禁。`44C6B0` 同样要求 `gLegacyWindow` 属于
当前 SDL 主线程，返回仍只表达可尝试重建；
不读取或清除 lost，不创建资源。原 WinMain 创建
并在同一线程使用的窗口不新增分支；外部窗口
生命周期和最终呈现尚需实际运行对照。

## `451350` 的 32 位 Back 写入与 SDL/CPU 边界

2026-09-26，在现有 `ida-17524/cg2d.exe` 会话只读核对
`[451350,451D78)` 的 32 位分支与原始数据。入口按
`left>=renderer.w`、`right<0`、`top>=renderer.h`、
`bottom<0` 早退；之后原位将 RECT 限到边界，
mode 0/1 还要求裁剪后的宽、高均大于 2，mode 2
只在宽恰为零时早退。通过门禁后才 Lock Back；
Lock 失败直接返回，成功路径在 `451D5A–451D6D`
释放锁。本体为 `void`，不将 Lock 失败映射为
已获批准范围外的伪造绘制结果。

- 32 位像素起点 `45145B–451477` 是
  `lpSurface + (top*pitchPixels+left+1)*4`，
  不是 `left`；pitch 从 Lock 结果按字节除四。
- mode 0 的 `451531–451638` 先以同一颜色写四条
  水平线及四个内角，再写内侧两列和左右两列；
  `color<256` 取 `gPaletteAutoMap[color]`，
  `color>=256` 从 `556300+3*(color-256)` 连读
  R/G/B，合成 `0x00RRGGBB`。IDA 实际
  `[556300,55638C)` 含旧编码正文、换行、零字节和
  `data\update.dat` 文本，不能把这段臆造为
  46 项纯色表，也不能在未确定索引域时截断为此区间。
- mode 1 的 `451A04–451A71` 用
  `gPaletteAutoMap[color]`：首尾行从 `left+1`
  写 `width-2` 项，中间 `height-2` 行从 `left`
  写 `width` 项，均保留原连续写序。
- mode 2 的 `451C97–451CBA` 同样查色，但只在
  `top` 行从 `left+1` 写 `width=right-left` 项，
  不按矩形高度填充。故裁剪后的 `right==640`
  时末笔指向列 640；非末行会落到下一行列零，
  `top==479` 时则越过当时
  `CpuSurfacePixels[640*480]` 的末尾。这是基于
  指令和当时 CPU Back 布局的条件结论，不声称
  该极端矩形已由当前 producer 证明可达。

已批准的 SDL/CPU 底层替代不改变上述像素写序。
当时唯一 CPU Back 没有帧尾哨兵，mode 0 的高色值
源字节也不等价于现有 256 项调色板；需沿队列
producer 核实际索引域，并裁决跨行/帧尾写入的
安全承载后才能完整实现 `451350`。此前仅存证，
不将其或 `DrawAllQueued/Flip/Init` 记为已接线。

另以两个相邻、有界的 `.text` 指令扫描覆盖
`[401000,54D000)`（首段在默认 20 万条上限停止，
续段从 `4ACA19` 起），`522245/52225F/522279`
分别直接测试三个高位标志；未发现可据此限定
`522210` 参数范围的直接常量生产者。这不排除
数据表、移位或间接写入，下一步应从入队记录来源
追踪，而不是将立即数搜索当作不可达证明。

### 直接颜色来源与 CPU Back 尾槽

2026-09-26，沿原 `522180→DrawGraphic→522210→451350`
和原 `DrawGraphic3/DrawGraphic2` 的 batchType 写序追踪：
`522180@522202` 会把入队记录改为 case 2，
因此普通 `DrawGraphic3` 调用常见的 batchType 0
不能限定此路径。对 `522180` 的 115 个直接 caller
函数逐个用只读 Hex-Rays 调用表达式核得 204 处；
其中 `4BCDFF/4D1BE4/4D1E58` 的反汇编及伪代码
均明确传 `imageNo=256,step=0`。此时
`45154F–451570` 读取 `556300` 的 `A2 E8 FF`，
组合为 `0x00A2E8FF`，不是调色板第零项。

`50716D–507181` 用 `[58ACC8,58AD04)` 的 60 项
W8 表零扩展写 `gHoverImageSeq@D69768`；实际表是
`250×45,15×5,2×5,15×5`。对该 W32 的 424 个
直接 xref 中，已识别的直接写者只有 `507181`；
这不排除计算地址、整块或动态写入。`4F6220`
等 UI 路径按原位读取的 `[57CBB8,57CBD8)`
四对 W32 为 `(250,2),(254,6),(249,1),(251,3)`。
`485110/4E0C70/4E1980/4E6D70/4E7140`
的调用参数均从 `gHoverImageSeq` 传入；
`503A20` 的 48 个已识别调用也传该槽；
`5035E0` 的 221 个调用中 220 个直接传该槽，
余下 `4F7748` 的局部值此前由它赋值。
这说明已追的直接路径需要覆盖颜色 256，
但不是任意外部 W16 色号或间接写者的全域证明。

按已批准的 DX7→SDL/CPU 底层替代，
[SurfaceState.cpp](../src/render/SurfaceState.cpp) 现将
Back 的私有物理分配扩为 `640*480+1` 个 W32；
对外 `ReadCpuBackPixels` 仍仅暴露前 `640*480`，
清屏与 Back→Temp 快照也只处理可见帧。
这样将来 mode 2 在最右一列的线性写入可跨到
下一行首像素，最后一行则落入内部尾槽，
不因 C++ 越界破坏内存。尾槽不是第二张画面，
不上传 SDL，也不复制到 Temp。此步骤只落位
安全承载，不实现 `451350` 或主队列，更不证明
原 DirectDraw 帧尾之外的未定义内存效应等价。

## `Renderer::Init` 的 SDL 替换边界与模式分流

2026-09-27，在当前已打开的 `ida-17524/cg2d.exe` 会话
只读遍历 `Renderer::Init@[44B630,44C3B7)` 指令，并检查
直接 caller `4576C2/4576E9/466B91/466BBD`。以下区分
原端可保留的上层状态写入和已获批准可替换的 DX7 原生
资源操作；不将 SDL3 `Create` 的成功直接视为整个 `Init` 成功。

1. `44B66E` 首先清 `gRendererLost`；`44B694–44B6B6`
   每次重新取得窗口 DC、读取 `BITSPIXEL` 并释放 DC，
   `44B6BC–44B6CC` 仅在 renderer flag 的 bit 3 置位且
   色深小于 16 时早退。现有参数解析保存的设备色深
   不能无证替代这里的再次采样。
2. `44B701–44B715` 发布 renderer 对象后，`44B73D/44B74E`
   清两个设备槽；`44B75A/44B76A` 写逻辑宽高
   `640/480`，`44B77C/44B788` 写纹理请求尺寸 `64/48`。
   `44B792/44B7AD` 的两次 DirectDraw 创建尝试及
   `44B7DC–44BABA` 的窗口/全屏合作级别、前后表面、
   clipper 是原生替换段；但其成功后才进入像素格式和
   模式分流，任何早退都不自动清理已发布资源。
3. `44BABD–44BB1D` 对非 8 位路径查询 Back 像素格式；
   `44BB26–44BBB2` 发布三个颜色掩码、位数/位移及
   `629718/62971C` 等共享数值；`44BBBD–44BC8F`
   仅在 16 位时分配并写 PALDATA。当前只复刻 32 位
   色路径，仍需按 32 位 Back 的实际位模式保留上层
   掩码与位移消费者，不能用空字段代替。
4. `44BC9B–44BCEC` 将请求模式写入独立的
   `gSupportedRenderMode`，小于 1 或 flag bit 0
   置位时改请求和支持模式为 1；`44BCF2–44BD33`
   又检查像素格式 capability、色深是否匹配。
   请求模式高于 1 时，`44BD3B–44BF19` 尝试建立
   D3D 对象/设备，按 4、3、2 的优先次序逐级尝试；
   成功分别发布支持模式 `4/3/2`，失败汇入
   `44BD6C–44BE3F` 的降级/提示和
   `data\update.dat` 写入路径。`44C2F4–44C3AD`
   若请求和支持模式不等，还显示对应提示、写文件并
   最终改请求模式。后续 `44BF25–44C2EE` 还设置
   3D 状态、viewport、纹理尺寸与 stage；不能单靠
   一个 SDL 纹理或固定支持模式复现上层分支。
5. 原 `Init` 早退均写低 `AL=1`，两个正常/降级尾
   `44BE4B/44C3B6` 写低 `AL=0`；调用者
   `4576CD–4576F0/466BA1–466BBD` 按低字节分别走
   错误提示、保留的模式切换支路和缓存构建。
   本体已查的返回点没有 `AL=2`，不能因父函数
   保留该分支便由 SDL 失败合成 2。首次缓存构建
   `457747–45774E` 检查返回，重建 `466C11`
   不检查，仍须保持两者的差异。

**当时未决映射（后续已按文末完整父流程 SPEC 裁决）：**当时 `SdlPresenter` 只提供 32 位 2D
整帧提交；32 位色范围不自动排除上层请求模式
`2/3/4` 的 3D 分流。需要闭合这些模式的实际
绘制消费者、纹理节点和 x64 owner，才能决定 SDL/CPU
如何承载可见结果及原模式写序；也须确定 32 位 SDL
Back 的掩码发布与旧 `data\update.dat` 的写入边界。
本节不批准强制降到模式 1、不把 3D 检查固定成功、
不编码部分 `Init/Destroy/Flip` 或声称画面可运行。

后续增量：`44D520/44DA90` 的完整上层裁剪、浮点转换、顶点、
混合选择及真实变换缓存调用已落位，详见
[纹理入口 SPEC](cg2d_texture_rendering.md)。
本次进一步确认 `44B610` 查 `+9C` 的接口，而不是 `+A0` 的设备；
后续色键 Blt 已用 SDL 软件 blit 同步写入唯一 CPU Back，
节点/变换面与 Back 各自持有仅借用原数组的 SDL_Surface 包装，
销毁先释放包装再释放像素；共享纹理状态查询也已在 renderer owner 落位。
四顶点 native 提交后续已按该 SPEC 的 CPU 设备章节实现：
保留 UV 的 POINT/WRAP 采样、单行 SDL 色键/调制/三组混合写同一 Back；
设备归现有 renderer owner，Destroy 在缓存面后、Back 前释放它。
设备创建父调用、+9C 接口和模式状态生产随后按文末 Init SPEC 接入，
不代表已经可运行。
随后已补 MakeSurface 的实际尺寸规则，节点/变换面不再把请求 64×48
当作有 3D 时的固定物理大小；像素格式发布 helper 同时写唯一 bit count。
Temp 首次/失败回退后续已分别接 44C8B0/44C6D0，并按实际行距
完成无色键快照；初始化探测的 factory 后续亦已接入。
不能据上层入口存在称模式 2/3/4、RendererInit 或最终画面已完成。

## `44C560` 的 SDL 提交叶与共享帧槽

2026-09-27，本线程在当前 IDA 会话只读重核
`[44C560,44C6A5)` 完整指令。窗口模式 1 的
客户区矩形与可选扫描线等待，以及其余模式
的 3D Blt 或非 3D front Flip，均只位于
已批准替换的 DX7 原生呈现边界；三路返回
非零才在 `44C687` 写 `gRendererLost=1`，
成功不清旧值。`44C68D–44C69A` 随后
无条件读取独立 `W32[628FC8]`，写为
`old==0 ? 1 : 0`，包括提交失败路径。

[SdlPresenter.cpp](../src/render/SdlPresenter.cpp)
现持有唯一 `SdlPresenter` 实例。创建窄接口
使用已有 `gLegacyWindow`、固定 640×480
及 `driver=nullptr`，销毁窄接口在窗口仍
有效的主线程调用；最近错误保留只读入口供
未来 `Renderer::Init` 失败分流使用。
`Flip_44C560` 从[统一 CPU Back](../include/cg/render/SurfaceState.hpp)
取可见 640×480 帧，以 2560 字节 pitch
提交；缺 Back 或 SDL 上传/呈现失败时
只置 `platform::gRendererLost=1`，随后
无条件翻转 render 唯一 W32 帧槽。成功
不清此前的 lost，也不复制第二张 Back。

本项实现了提交叶及其后端创建/销毁接口，
但 `Renderer::Init/Destroy` 仍未调用这两个
接口，`DrawAllQueued` 的 13 路写 Back 尚未
完成；不能把静态库编译或 `Flip` 符号存在
误作已创建 presenter、窗口画面或完整链接。
旧 3D 模式、共享帧槽的其他读取者和外部窗口
实际缩放仍待接线/运行核对。限定
`git diff --check`、UTF-8 无 BOM/LF
和 Debug `cg2d_re_render/cg2d_re_core/
cg2d_re_startup` 静态库编译通过，无新增
告警；未运行测试或客户端，音频暂缓。

## `450470` 的 32 位调色板绑定与唯一 renderer owner

2026-09-27，本线程只读当前 `ida-17524/cg2d.exe`：
`44B6F4–44B715` 以零初始化分配发布 `gCRenderer`，
`450787–45078E` 仅用 `tPaletteInited` 决定是否首次读文件；
`45083A–45086B` 无条件从 renderer `+4` 创建调色板至
`+0x90`，`450874–4508A1` 判空、提示 Error Code = 15
并返回 0。`4508A2–4508FA` 只有 8 位分支为 Back
设置调色板；32 位仍须先创建 `+0x90`，才建立 CPU
查色表。`44C4A4–44C4BF` 在销毁中释放并清该字段，
最后 `44C531–44C553` 释放 renderer。

原 `466E42–466E63` 和 `457072–457093/
4570B1–4570D2` 先判绑定及窗口模式再调用
`SetEntries`；`45719D–4571AA/457319–457326`
直接调用后清待更新标志。以上调用均不判返回值。
新端将 `gRenderer` 移到 render 唯一 owner，
独立分配调色板绑定并复制初始 256 项，
`GetRendererPalette` 返回同一对象，
`SetPaletteEntries` 在已见的
`flags=0,first=0,count=256` 域内复制项并返回 0。
32 位 SDL/CPU 路径不从该绑定重新生成画面像素；
查色表仍由 `InitPalette32/RebuildPalette32` 持有。
异常参数的 `-1` 只是适配器失败值，不宣称等于原
DirectDraw HRESULT。

当前只落位 owner 与 `InitPalette32` 的创建/失败叶，
并提供未来 `Init/Destroy` 可调用的 owner 分配/释放
及销毁中途的绑定释放入口；
**尚无生产者调用分配入口**。因此当前完整入口仍
不能到达这段成功路径，也未证明模式 2/3/4、
初始化失败残留、SDL presenter 生命周期或画面。

## `44C3C0` 的 SDL/CPU 销毁父入口

2026-09-27，本线程只读复核完整
`Renderer::Destroy@[44C3C0,44C55C)`：
`44C3CF–44C402` 的 PALDATA 仅属于已排除的
16 位分支；`44C408–44C435` 先按实际节点数
释放非空面，再于 `44C436–44C45B` 依次释放
Temp/AutoMap。`44C461–44C4A4` 为设备与 3D
原生对象，`44C4A4–44C4BF` 清调色板，
`44C4C5–44C531` 释放 clipper、Back、front、
DDraw，`44C531–44C553` 最后清 renderer owner。
`Window::CloseAll@472190` 逐项关闭游戏内窗口链，
其直接本体没有 `DestroyWindow`；但
`47219E→472160→471270` 还经 UI 对象
`+0x38` 虚表调用清理。该间接目标及外部
Win32 HWND 的存活时序尚未全域核实，
不能仅凭直接调用排除销毁风险。

新端 `RendererDestroy_44C3C0` 复用真实节点
释放叶、CPU Temp/AutoMap 销毁、调色板绑定释放、
CPU Back 销毁，再释放 SDL 纹理/renderer/外部
窗口 wrapper，最后清 renderer owner。SDL 对
设备/3D/front/DDraw 的合并释放只属于已批准的
原生后端替换，不表示旧 3D 绘制状态已实现。
若非 SDL 主线程调用，进入前就保留全部资源；
若 SDL 销毁仍失败，保留 owner 供主线程重试，
不宣称异常线程域与 DX7 等价。

退出链 `ShutdownClient_457A40` 已接该入口，
`RebuildRenderer_466B80` 后续实现见文末，初始化与绘制下游仍缺，
`Renderer::Init` 也尚未生产 owner/SDL presenter。
完整退出链还依赖其他未落位的上游/后继，
且须核对关闭 UI 后外部 HWND 仍有效，
不能据此声称完整链接、运行或总 SPEC 完成。

## `44BB1E` 的 32 位 Back 像素格式发布

2026-09-27，本线程在当前 IDA 会话只读核
`44BABD–44BBBD`：非 8 位路径先要求 Back 非空，
`44BAE2–44BB0A` 清并查询其像素格式，
查询失败在 `44BB11–44BB1D` 以 `AL=1` 早退；
查询成功后 `44BB1E–44BB39` 才依红、绿、蓝
写三个掩码。`44BB3E–44BB83` 依次写
`8 - bitcount(R/G/B)`，`44BB89–44BBB2`
又写蓝加绿位数、蓝位数；`44BABB` 已将
`ESI=0`，故 `44BBB7` 最后写第三个位移零。
`44BBBD` 才进入仅 16 位的 PALDATA 分流。

| 原地址 | 新端字段 | SDL XRGB8888 值 |
|---|---|---:|
| `628FE4/62A660/628FC0` | `gRendererRed/Green/BlueMask` | `00FF0000/0000FF00/000000FF` |
| `6287A8/62A658/628BB8` | `gRendererRed/Green/BlueShift` | `0/0/0` |
| `629314/629718/62971C` | `gRendererRed/Green/BlueOffset` | `16/8/0` |

原端这些槽只有 `Renderer::Init` 写；`450470`
的 16 位调色板转换、`451190` 的颜色重建及
`453760` 的截图颜色拆分读取位数和偏移。
掩码另由 `453898–4538B4` 复制到截图状态。
新端的 `PublishRendererPixelFormat32_44BB1E`
复用已实现的 `CountMaskBits_453DE0`，按原
计算顺序发布同源共享值；XRGB8888 与当前 SDL
纹理及 CPU Back 的 `0x00RRGGBB` 位模式一致。
**调用点尚未接 `Renderer::Init`**，不得把这组
可发布值误记作已成功查询像素格式、完成初始化
或实现截图。

## `451350` 的 32 位矩形像素本体

2026-09-27，现有 `ida-17524` 只读核
`[451350,451488)` 的入口与 32 位指针计算、
`[45150C,45163D)` 的描边路径、`[4519F1,451A76)` 的填充路径、
`[451C84,451CBF)` 的单行路径及 `[451D5A,451D78)` 的共同出口。
这些是同一函数的公共及完整 32 位分支；8/16/24 位分支按用户范围排除，
不作为本步缺口，也不将其位宽逻辑搬入新端。

本体落在 [SurfaceState.cpp](../src/render/SurfaceState.cpp)，直接使用
唯一 CPU Back 与同一个 `gPaletteAutoMap`。既有
`Sub_522210` 的三处模式调用已解析到真实本体，包装未改逻辑。
`RECT*` 仅留在 render 实现边界，不进入上层公开接口；不新增面、
调色板副本或额外颜色转换层。现有 CMake 已列入两个源文件，无构建配置改动。

### 裁剪与门禁

`451365–451392` 按顺序拒绝 left>=w、right<0、top>=h、bottom<0；
**right/bottom 为零不在这四个门禁拒绝**。随后 `451398–4513D0`
依次原位裁剪 left、right、top、bottom，保留 caller 的 RECT 写入：
left/top 的负值改零，right/bottom 大于等于边界时改为 w/h。
新端使用已选 CPU Back 的固定 640×480，来源为初始化
`44B75A/44B76A`，不新增逻辑屏幕尺寸。

裁剪后 width=right-left、height=bottom-top。
`4513E9–451410` 对 mode=2 仅在 width==0 时提前返回；
其他 mode 在 width<=2 或 height<=2 时返回。
不额外拒绝倒置矩形，不把 mode=2 的条件改成 width<=0 或检查 height。
原 Lock/Unlock 的原生 API 已按 CPU 承接；Back 未创建时不写像素，
但保留此前 RECT 修改，不宣称原空 COM 引用的故障行为一致。

32 位起笔位置为 `Back + top*pitch + left + 1`
（`451466–45147F`），pitch 为既有 CPU 行跨度 640 个像素。
这里的 `+1` 不是现代绘图库的默认矩形规则，三个模式均保留。

### 三种像素路径

| mode | 原指令 | 写入 |
|---|---|---|
| 0，描边 | `451531–451638` | 先逐列写上方两行、下方两行各 width-2 个像素；再写原四个拐角补点；最后逐行写左右各两像素，共 height-2 行。保留重复/交叠写入顺序，不替换成通用描边 |
| 1，填充 | `451A04–451A71` | 顶行从 left+1 写 width-2；中间 height-2 行从 left 写 width；底行从 left+1 写 width-2，四个外角不写 |
| 2，单行 | `451C97–451CBA` | 先读取调色板颜色，width>0 才从 **left+1** 写 width 个像素，完全不使用 height；不改成从 left 写或写 width-1 |

mode=2 可以写到 right 对应像素，右边界为 640 时仍不按行额外裁掉；
沿用已有 Back 的 backing，不为本步扩容或新增边界恢复。
其他 mode 不写像素；保留之前裁剪和门禁，不额外报错。

### 色值来源与边界

mode=0 的 U16 索引<256 使用同一 `gPaletteAutoMap`。
`451542–451570` 对其他索引按 `(index-256)*3` 读取 RGB，
组合成 `R<<16 | G<<8 | B`。当前映像 `556300..556302`
三字节为 `A2 E8 FF`，256 号色由私有三字节表承接；
`556303` 为零，`556304` 已是另一独立提示字符串，不能自行把后续
字符串当成更多业务色表。三个通道直接 xref 都只来自本函数不同色深分支。
不新增索引拒绝、钳制或取模；越出已证表的访问按 RPLAN.24 不专项模拟。

mode=1/2 **始终**直接索引 `gPaletteAutoMap`，不借描边规则增加
256 号颜色的特殊处理。所有写入均为完整 W32，不把高字节解释为新透明度。

本步只核该像素本体及现有包装接线。DrawAllQueued、Renderer::Init、
纹理父绘制和模式 2/3/4 的混合路径仍未闭合，不能据此声称矩形已显示。
仅做本步限定差异、声明/定义/调用、编码及文件链接静态检查；
不构建、不测试、不运行客户端。

## 重建与场景快照

2026-09-27，原 `ida-17524` 只读核完整 `Renderer::Rebuild`
`[466B80,466D41)` 和场景 9 快照 `[51D640,51D6E3)`。
以下按用户工程决策委托记录实施 SPEC，不以重建父函数完成代替
RendererInit、模式 2/3/4 或最终绘制完成。

### 重建控制流

1. `466B84` 保存完整 `103CF10` 队列模式；Destroy→Init。
   第一次 Init 仅解释低 S8：1 提示、2 将 windowMode 改为旧值是否
   不等于 1 后再 Init；第二次低字节非零才提示。提示沿
   `559340` 的 23B 原字节和 `5536B8` 的 5B 原标题，
   flags=10h，随后重新读取窗口并 Post WM_CLOSE。
   这些分支之后仍继续，不提前返回或加第二次销毁。
2. `466C11` 调 BuildSurfaceCache，忽略结果；再 InitPalette。
   调色板返回 0 时 Post `(104h,0Dh,0)`，不是 WM_CLOSE，
   随后仍进入场景处理。原重建不分配或替换整张 graphic 表。
3. 只在 `466C30` 采样一次 SceneType。场景 10 顺序：
   W16 图形计数清零→第二 W32 计数清零→CacheReset→46BD40→46CF60(0)
   →读取战斗背景图号→DrawGraphic(320,240,200,image,0,nullptr)
   →Sort→Clear→queueMode=0→DrawAllQueued→Back 到 Temp 整面复制。
   复制后恢复入口保存的 queueMode，再清两计数、CacheReset，
   独立取 tick 并写截止；不调用 Flip。
4. 场景 9 调下面的完整 `51D640`；其他场景不执行上述两段。
   不把 scene 9 的固定 queueMode=1 改为恢复入口快照。
5. 场景处理后重新取当前 renderer 的 palette binding；
   仅 binding 非空且 windowMode 非零才写当前 256 色。
   随后 `522C50` 释放独立调色板→`522B30` 重新初始化，
   返回忽略；依次清原 `D68D98/D68D9C` 两个完整 W32，
   最后另取一次 tick 写截止。不可复用场景分支内的那次 tick。

### 场景 9 快照

`51D640` 顺序为 `468F70` 投影更新→清两计数→CacheReset→469030
→Sort→Clear。随后读取原 `C82204` W8 到局部，清该槽，
写 queueMode=0，再 DrawAllQueued；先恢复该 W8，后整面 Back→Temp。
之后 queueMode **固定写 1**，再清两计数、CacheReset，取 tick，
零扩展为 double 写截止并返回同一个完整 U32 tick。

唯一已见两个直接 caller 是 `466CE2/51D226`；本步接前者，
后者所属绘制父链尚未实现。绘制与复制的结果均不门控后续写入。

### 唯一状态与适配边界

- `D68D98/D68D9C` 复用已有 LegacyMouseState 的 heldButtons/edgeButtons，
  不调用会清其他字段的总鼠标重置。
- 战斗背景 `59F9F8` 是 W32 图号：原 `4214DA/421501` 从 action +2A8
  发布，`50D314` 从协议第三实参发布，`43CB30/466C61` 传入 DrawGraphic。
  原位于既有 PE 零填充区，新端由 world detail 唯一持有 S32，
  不扩宽为地址、不预设某张背景；上述生产者仍待实现，未来复用同一槽。
- 底层 `BltFast(0,0,Back,null,10h)` 采用既有批准的 CPU
  `CopyBackToTemp`，不新增 Back/Temp 副本、缩放或混合。
  保持调用及忽略结果的时点，不宣称 DX 失败码或非法资源故障等价。
- `46CF60(int)` 保留原真实依赖声明；只核本 caller 参数为零和两条
  原返回尾均经 `52CD60`，不以固定成功/空函数补掉完整地图消费者。
  `51D640` 使用已实现的投影和地图入口，DrawAllQueued 仍缺。

实现安排：重建入口位于 startup 目标下的平台源，namespace 仍为
`cg::render`，保持现有 MainLoop 与 WindowProcedure 调用声明；
场景快照留在 world 的 SceneState。先记录上述 SPEC，再编码，
不改变窗口回调、lost 门禁、首次初始化错误策略或资源 owner。
仅静态核对，不构建、测试或运行。

当前已实现 [RendererRebuild.cpp](../src/platform/RendererRebuild.cpp)
与 [SceneState.cpp](../src/world/SceneState.cpp) 中的 `51D640`，
CMake 仅登记前者；MainLoop 两处和 WindowProcedure 既有调用未改。
静态核对覆盖首次低字节 1/2/其他、重试零/非零、调色板失败投递、
场景 10/9/其他、两种 queueMode 写回差异和末尾公共清理顺序。
战斗背景仅有唯一槽及此处读取，生产者未落位；
RendererInit、46CF60、DrawAllQueued 等真实依赖仍缺，
不把重建函数定义记作已恢复设备、已有画面或完整链接通过。

## RendererInit 完整父流程实施 SPEC

2026-09-27，继四顶点 CPU native 提交，按工程决策委托落实
`[44B630,44C3B7)`。原会话只读核全段指令、调用、提示原字节及
请求/支持模式引用；不把以下 native 映射当旧 DX7 驱动的硬件能力。

### Native 映射裁决

1. RendererResources 仍为唯一 owner；Init 放在其既有源文件，
   不复制纹理状态、模式字节或窗口。原逻辑宽高沿唯一 Back 的
   640×480；纹理请求尺寸依原位发布 64/48。先清 lost、重新
   GetDC/GetDeviceCaps/ReleaseDC，再执行 flag bit 3 的低色深早退。
2. DirectDraw 创建/主表面/clipper 由已有自动 driver SDL presenter
   整体承接，不伪造三个 COM 对象或独立失败点。创建失败仍提示并
   返回低 S8=1，已发布 renderer 不自动撤销；Back 创建失败保留
   presenter，按原 Back Buffer 提示返回 1。
3. SDL presenter 增加实际窗口/全屏切换。全屏选择当前显示器最接近
   640×480、桌面刷新率的显示模式；该选择和模式设置的失败不单独
   早退，对应原 `44B9EA` 不检查 SetDisplayMode 返回。
   SDL 的实际 fullscreen 请求与同步完成检查承接合作级别失败；
   窗口模式退出 fullscreen，不新增原生窗口或更改平台窗口 owner。
   物理显示模式可由 SDL 转换，逻辑 Back 始终 32 位。
4. 早期色深采样仍用于原提示门禁；Back 创建后共享 BITSPIXEL
   改为其实际 SDL 格式位数，供后续 32 位消费者使用。
   查询失败返回 1；只承接 XRGB8888，按既有 helper 发布
   RGB 掩码/位移。8/16/24 位像素分支及 PALDATA 继续范围排除。
5. `+9C` 映射为独立的 LegacyTextureInterface，持有从实际 Back
   查询的 SDL 格式描述借用；该描述用于设备 scratch 创建，
   不是空 tag 或仅代表设备已创建的 bool。描述由 SDL 管理，
   接口在 SDL presenter 之前释放。HasLegacy3DDevice 只查这个槽。
6. 请求 4/3/2 的创建分支、各自门禁和成功发布保留。三种逻辑模式
   均用现已实现的 CPU 固定管线承接已还原的预变换顶点、色键和混合，
   不强制降到 1/2，也不宣称硬件 T&L、显存或旧 GUID 可用。
   每次尝试实际分配设备/scratch，失败继续下一个原分支；
   支持模式只在该次创建成功后发布，不由请求直接赋成功值。
7. native 能力查询读取真实 scratch 格式；色键启用通过实际 SDL
   设置检查，成功才写 +BC=1。其余原禁用的 lighting/depth/cull/
   perspective 状态由已实现的固定管线语义承接，配置真实 SDL
   调制/混合及 Back viewport，失败进入原降级。
   CPU POINT/WRAP 本身支持非幂次方和非正方形，故本后端相应
   textureCaps 限制位为零，不复制或捏造显卡 caps/GUID。
   原 +B8/+BA 的 S16 +2、能力位调整和写入次序仍保留。

以上是已批准 SDL/CPU 替换的工程实现选择，不豁免
DrawAllQueued、尚缺矩形消费者、最终画面与全屏重建的运行验收。
原 COM 内部分配、GUID 差异及逐项 HRESULT 不作伪造模拟。

### 必须保留的上层顺序

- 请求模式低 W8 按 S8 比较；<=1 才先写支持模式，<1 提示并
  依序改支持/请求为 1。flag bit 0 独立强制 1，仍保留模式
  四字节存储中未写的高字节；不把两个模式合并。
- 不兼容像素格式单独提示/改模式，不写文件；低模式正常返回 0。
  接口创建失败有 Direct3D Object 提示，随后才进入公共降级；
  后续设备/配置/探测失败无该额外提示。
- 公共降级按设备→接口释放并清槽，然后清 +BC；准备 `"1\0"`
  原字节，使用 `CreateFileA("data\\update.dat", GENERIC_WRITE,
  0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr)`。
  打开失败跳过写/关；写入长度不含 NUL，忽略写/关结果，
  随后显示模式 1 提示、写支持模式、写请求模式，返回 0。
  这不是 Init 的低 S8=1 早期失败，不改 caller 的继续行为。
- 配置成功后执行 MakeSurface(256,256,0,800h) 探测。
  创建空则跳过整段而不重写 dwStage；非空时 Set/GetTexture
  实参 stage 均为 0，而不是探测循环计数。查询持有引用先释放，
  再释放创建引用；设备绑定仍持有该面。成功后才发布共享 stage。
  原 GetTexture 成功却返回 null 后的无效虚调用属于 RPLAN.24，
  不模拟该故障现场，不将新端 null 释放合同声称为故障等价。
- 支持模式与请求相同时直接返回 0。不同则先按支持模式 3/2
  显示对应原字节提示；模式 3 提示后重读支持模式再判断 2。
  然后打开同一 update.dat，仅在打开成功后读取支持模式低 S8
  做 `%d` 格式化并写入，最后重读支持模式回写请求模式。
  打开失败仍回写，不提前缓存模式值或将写入失败变为 Init 失败。
- 本体只返回 0/1，不合成原未见的 2；父初始化/重建中的
  低字节 2 支路保留原样。任何早退不插入统一销毁或额外 Post。

### 字节与检查边界

提示按原内存至首个 NUL 读取，不能用 IDA 对非 ASCII 内容截断的
字符串视图。Caption=5536B8（4B）、低色深=5564B4（47B）、
Back=556428（21B）、Direct3D Object=556390（25B）、
三个模式提示=5563AC/556304/556340（各 56B）；
回退字=55638C 的 `31 00`。保留原编码字节，不在消息边界猜编码。

本步仅静态核对原控制流、共享写序、SDL/API 声明与生命周期；
不编译、不测试、不运行客户端、不触发实际显示模式或文件写入。
初始化本体可落位不等于完整客户端链接、全屏恢复或像素通过。

### Init 接线落实

[RendererResources.cpp](../src/render/RendererResources.cpp) 已定义完整
RendererInit、真实独立接口查询、原模式/降级/文件写入和探测控制流；
已有 InitializeClient 与 RebuildRenderer 的四处调用无需改变。
SDL presenter 的 fullscreen/sync、真实 Back 格式、单行设备配置
均已接入；正常销毁和降级均先释放设备，再释放接口。
此前 HasLegacy3DDevice 与 CreateRendererTextureDevice 的声明缺口已闭合。

本线程静态核对全部原返回和写序、探测引用、七个非 ASCII 提示常量、
接口及既有 CMake/SDL/Win32 链接声明；新增代码未编译或执行。
32 位 CPU native profile 不宣称旧硬件能力，模式 2/3/4 的画面、
全屏切换/重建和文件写回均仍缺真实运行验收。
FillRect_44C9A0 后续已接现有空纹理提交；DrawAllQueued、UI 初始化及
整体 SPEC 继续推进，矩形像素仍未运行验收。
