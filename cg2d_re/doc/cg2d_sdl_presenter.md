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
