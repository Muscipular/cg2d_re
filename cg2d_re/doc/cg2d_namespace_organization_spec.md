# 全文件命名空间整理 SPEC

日期：2026-10-16。按用户“所有文件都扫一遍，尤其同文件多个 namespace”“SPEC 按合理性判断执行”授权实施。不记录 JSON 台账。

## 范围与验收

完整扫描 src、include 的 316 个 C/C++ 源文件、头文件及 inc；另核对 doc/profiling/20261013_fps/SdlOfficialScaffold.cpp，作为历史性能脚手架只保留。脚本、文档和构建配置没有 C++ namespace，不做机械替换。下表逐文件登记扫描基线，执行后补结果。

原则：一个业务实现文件由一个模块负责；同模块公开层、detail 层及匿名辅助层可以共存。外部类型前置声明允许出现在头文件，不能借此夹带外部实现或重复声明。消除冒名归属和同一命名空间无必要的反复开闭；不用全局 using namespace 掩盖依赖。

保留函数正文、数据布局、容量、读写次序、资源所有权、RNG 消费及初始化/析构顺序；不重命名原地址标识，不改变业务。跨文件搬移同步 CMake 与当前函数表的源码定位，不更新历史证据路径。最终执行 build.bat Debug，配置、编译、链接均通过才记编译验收；不运行客户端或测试。

## 分项裁决

| 项目 | 问题与实施 | 验收 |
|---|---|---|
| NS01 | Yobi 类型与运行时误属 login：迁到 cg::yobi / cg::yobi::detail；同步前置声明、函数定义、类型引用与调用；保留完整类型边界，不添加兼容 alias。公会归 cg::world::detail，资源名字表归 cg::resource。 | 旧归属无残留，声明与定义一致。 |
| NS02 | 音频播放/初始化/退出入口归 cg::audio；网络音乐回调移入 network。登录存档初始化与恢复归 login，重复 character_select 转发删除。鼠标状态 alias 与读取转发按既有类型改为直接访问；绘制队列前缀 alias 同样收敛。 | 唯一状态与入口，调用次序和取值次数不变。 |
| NS03 | UI 文件内 input 上下文实例属于窗口状态，迁至 cg::ui::detail，输入类本身仍归 input；修正统一声明及使用处，消除每个窗口的跨模块定义块。 | 不搬对象所在翻译单元，不改变构造顺序。 |
| NS04 | Input.cpp 中 world 聊天记录实现移至 world；Yobi 的世界对象协调/地图绘制实现移至 world 文件；GraphicHitTest 的鼠标调度移至 platform，绘制状态归 render。 | 逐块保留正文，直接包含所需头文件，CMake 路径有效。 |
| NS05 | LegacyMouse.cpp 的跨模块有序静态初始化独立为 platform/RuntimeInitialization.cpp；原初始化定义及排列整体保留，鼠标函数留 LegacyMouse.cpp。此初始化文件允许多个模块 namespace，因为同 TU 顺序属于行为合同。 | 初始化块逐字保持，不按模块重排，不新增状态/代理/懒初始化。 |
| NS06 | 清理重复 namespace 开闭、外部函数手写重复声明、跨模块 using 转发；同模块 detail/state、匿名实现及头文件类型前置声明按作用保留。 | 全文件复扫，每个剩余多 namespace 有具体原因，编译/链接通过。 |

## 多命名空间的保留边界

- 头文件中仅作外部类型前置声明，避免不必要的包含依赖；本模块 detail/state 分层属于同一模块。
- 匿名 namespace 用于内部链接，不能粗暴合入具名 namespace 改变链接属性。
- RuntimeInitialization.cpp 专门承载已证的跨模块初始化顺序，是业务文件规则的明确例外。
- 已停止的离线 MIDI 工具与性能脚手架只扫描，不恢复开发、构建或执行。

## 全文件扫描基线

| 文件 | 原 namespace（匿名省略） | 处置依据 | 执行结果（匿名省略） |
|---|---|---|---|
| `src/audio/LegacyAudio.cpp` | `cg::login::detail`、`cg::audio`、`cg::network::detail`、`cg::platform`、`cg::platform::shutdown_detail`、`cg::platform::initialization_detail` | NS02 / NS04 / NS06 | cg::audio |
| `src/audio/LegacySeDefinitions.inc` | 无具名 namespace | NS06：核对归属、依赖与前置声明 | 无具名 namespace |
| `src/input/Input.cpp` | `cg`、`cg::input`、`cg::input::detail`、`cg::world::detail` | NS02 / NS04 / NS06 | cg::input、cg::input::detail |
| `src/input/InputCaret.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::input::detail |
| `src/input/InputKeyDispatch.cpp` | `cg::input` | NS06：核对归属、依赖与前置声明 | cg::input |
| `src/input/InputKeyDispatchDependencies.hpp` | `cg::input`、`cg::input::detail` | NS06：核对归属、依赖与前置声明 | cg::input、cg::input::detail |
| `src/network/NetworkDispatchDependencies.hpp` | `cg::network::detail` | NS06：核对归属、依赖与前置声明 | cg::network::detail |
| `src/network/NetworkRuntime.cpp` | `cg::network`、`cg::network::detail` | NS06：核对归属、依赖与前置声明 | cg::network、cg::network::detail |
| `src/network/NetworkRuntimeDependencies.hpp` | `cg::network::detail`、`cg::character_select::detail`、`cg::platform`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::network::detail |
| `src/platform/ClientEntry.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/ClientInitialization.cpp` | `cg::platform`、`cg::platform::initialization_detail` | NS06：核对归属、依赖与前置声明 | cg::platform、cg::platform::initialization_detail |
| `src/platform/ClientShutdown.cpp` | `cg::platform`、`cg::platform::shutdown_detail` | NS06：核对归属、依赖与前置声明 | cg::platform、cg::platform::shutdown_detail |
| `src/platform/ClientShutdown.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/CommandOptions.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/ImeComposition.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/ImeComposition.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/ImeLifecycle.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/ImeLifecycle.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyAudioMath.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyAudioMath.hpp` | `cg::audio`、`cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyCryptInt.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyCursor.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyFormat.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyFormat.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyMemory.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyModuleDigest.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyMouse.cpp` | `cg::platform`、`cg::world::detail`、`cg::login::detail` | NS02 / NS05 | cg::platform |
| `src/platform/LegacyMouseTracker.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyNetworkAdapter.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyStdio.hpp` | 无具名 namespace | NS06：核对归属、依赖与前置声明 | 无具名 namespace |
| `src/platform/LegacyTime.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyWavesReverb.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LegacyWindow.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/LoadingScreen.cpp` | `cg::platform::initialization_detail` | NS06：核对归属、依赖与前置声明 | cg::platform::initialization_detail |
| `src/platform/MainLoop.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/MainLoopDependencies.hpp` | `cg::render`、`cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/MainLoopState.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/MainLoopSupport.cpp` | `cg::audio`、`cg::platform` | NS02 / NS04 / NS06 | cg::platform |
| `src/platform/MainLoopWindowEnumeration.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/MapEffectMath.cpp` | `cg::platform::detail`、`cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform::detail、cg::platform |
| `src/platform/MapEffectMath.hpp` | `cg::platform::detail`、`cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform::detail、cg::platform |
| `src/platform/RendererRebuild.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | 迁至 src/render/RendererRebuild.cpp；cg::render |
| `src/platform/SdlAudioBackend.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/SetupIni.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/StartupCommon.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/StartupNative.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/StartupState.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/WindowLifecycle.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/WindowProcedure.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/platform/WindowProcedureDependencies.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | 删除；接口归 SdlPresenter.hpp |
| `src/platform/WindowProcedureState.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `src/render/BackTextDrawing.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/DrawAllQueued.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/DrawGraphic.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/DrawGraphicHover.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/Font.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/GraphicCacheState.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/GraphicDxCache.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/GraphicDxCacheState.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/GraphicHitTest.cpp` | `cg::render`、`cg::platform` | NS02 / NS04 / NS06 | 迁至 src/platform/MouseDispatch.cpp；cg::platform |
| `src/render/GraphicPaletteCache.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/GraphicPendingQueue.cpp` | `cg::render`、`cg::platform` | NS02 / NS04 / NS06 | cg::render |
| `src/render/InputText.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/LegacyTextureDrawing.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/LegacyTextureDrawing.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/Palette.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/RendererResources.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/SdlPresenter.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/SurfaceState.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/render/TextPendingQueue.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `src/resource/AnimationCleanup.cpp` | `cg::resource::animation` | NS06：核对归属、依赖与前置声明 | cg::resource::animation |
| `src/resource/AnimationList.cpp` | `cg::resource::animation` | NS06：核对归属、依赖与前置声明 | cg::resource::animation |
| `src/resource/AnimationRuntime.cpp` | `cg::resource::animation::detail`、`cg::resource::animation` | NS06：核对归属、依赖与前置声明 | cg::resource::animation::detail、cg::resource::animation |
| `src/resource/AnimationRuntimeState.hpp` | `cg::resource::animation::detail`、`cg::resource::animation` | NS06：核对归属、依赖与前置声明 | cg::resource::animation::detail、cg::resource::animation |
| `src/resource/GraphicAlloc.cpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 | cg::resource |
| `src/resource/GraphicDecode.cpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 | cg::resource |
| `src/resource/GraphicInfo.cpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 | cg::resource |
| `src/resource/GraphicRead.cpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 | cg::resource |
| `src/resource/ResourceNames.cpp` | `cg` | NS01 | cg::resource |
| `src/resource/ResourcePaths.cpp` | `cg::resource`、`cg::resource::detail` | NS06：核对归属、依赖与前置声明 | cg::resource、cg::resource::detail |
| `src/scene/character_select/CharacterAuthenticationFailure.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 | cg::character_select |
| `src/scene/character_select/CharacterDeletionConfirmation.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 | cg::character_select |
| `src/scene/character_select/CharacterDeletionProgress.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 | cg::character_select |
| `src/scene/character_select/CharacterListProgress.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 | cg::character_select |
| `src/scene/character_select/CharacterModalButton.cpp` | `cg::character_select::detail` | NS06：核对归属、依赖与前置声明 | cg::character_select::detail |
| `src/scene/character_select/CharacterSelectModal.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 | cg::character_select |
| `src/scene/character_select/CharacterSelectPreview.cpp` | `cg::character_select`、`cg::character_select::detail` | NS06：核对归属、依赖与前置声明 | cg::character_select、cg::character_select::detail |
| `src/scene/character_select/CharacterSelectScene.cpp` | `cg::character_select::detail`、`cg::character_select::state`、`cg::character_select` | NS06：核对归属、依赖与前置声明 | cg::character_select::detail、cg::character_select::state、cg::character_select |
| `src/scene/character_select/CharacterSelectSceneDependencies.hpp` | `cg::character_select::detail` | NS06：核对归属、依赖与前置声明 | cg::character_select::detail |
| `src/scene/character_select/CharacterSelectSlot.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 | cg::character_select |
| `src/scene/character_select/CharacterSelectState.hpp` | `cg::input`、`cg::character_select::state` | NS06：核对归属、依赖与前置声明 | cg::input、cg::character_select::state |
| `src/scene/character_select/CharacterVersionUpgrade.cpp` | `cg::character_select`、`cg::character_select::detail` | NS06：核对归属、依赖与前置声明 | cg::character_select、cg::character_select::detail |
| `src/scene/login/CharacterCreationRequest.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `src/scene/login/CharacterDeletionRequest.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `src/scene/login/CharacterListRequest.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `src/scene/login/LoginCapability3InputController.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `src/scene/login/LoginCapability3Scene.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world::detail |
| `src/scene/login/LoginCapability4Modal.cpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 | cg::login::detail |
| `src/scene/login/LoginCapability4Modal.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 | cg::login::detail |
| `src/scene/login/LoginCapability4Scene.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world::detail |
| `src/scene/login/LoginConnection.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `src/scene/login/LoginInput.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `src/scene/login/LoginInputController.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `src/scene/login/LoginInputDependencies.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 | 无具名 namespace |
| `src/scene/login/LoginMessage.cpp` | `cg::login`、`cg::login::detail` | NS06：核对归属、依赖与前置声明 | cg::login、cg::login::detail |
| `src/scene/login/LoginMessageDependencies.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 | cg::login::detail |
| `src/scene/login/LoginModeScene.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `src/scene/login/LoginOuterScene.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `src/scene/login/LoginRequest.cpp` | `cg::login`、`cg::login::detail` | NS06：核对归属、依赖与前置声明 | cg::login、cg::login::detail |
| `src/scene/login/LoginRequestDependencies.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 | cg::login::detail |
| `src/scene/login/LoginSavedState.cpp` | `cg::login::detail`、`cg::platform`、`cg::character_select::detail` | NS02 / NS04 / NS06 | cg::login::detail |
| `src/scene/login/LoginScene.cpp` | `cg::login`、`cg::login::detail` | NS06：核对归属、依赖与前置声明 | cg::login、cg::login::detail |
| `src/scene/login/LoginSceneDependencies.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 | 无具名 namespace |
| `src/scene/login/LoginState.cpp` | `cg::login`、`cg::login::detail` | NS06：核对归属、依赖与前置声明 | cg::login、cg::login::detail |
| `src/scene/login/LoginStateDependencies.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 | cg::login::detail |
| `src/text/Encoding.cpp` | `cg::text` | NS06：核对归属、依赖与前置声明 | cg::text |
| `src/tools/LegacyMidiSynth.cpp` | `cg::platform` | 历史工具，保留 | cg::platform |
| `src/tools/LegacyMidiSynth.hpp` | `cg::platform` | 历史工具，保留 | cg::platform |
| `src/tools/MidiToWav.cpp` | 无具名 namespace | 历史工具，保留 | 无具名 namespace |
| `src/ui/HitTest.cpp` | `cg::ui` | NS06：核对归属、依赖与前置声明 | cg::ui |
| `src/ui/LegacyUiRecord.cpp` | `cg::ui` | NS06：核对归属、依赖与前置声明 | cg::ui |
| `src/ui/UiControlRecord.hpp` | `cg::ui` | NS06：核对归属、依赖与前置声明 | cg::ui |
| `src/ui/WindowPrototypeDependencies.hpp` | `cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui::detail |
| `src/ui/WindowPrototypes.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/WindowRuntime.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/WindowRuntimeInternal.hpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/WindowRuntimeState.hpp` | `cg::login`、`cg::login::detail`、`cg::input`、`cg::world::detail`、`cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::input、cg::world::detail、cg::ui、cg::ui::detail |
| `src/ui/windows/WindowAdventureLog.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowAlbumDetail.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowAlbumList.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowAppearance.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowAttributeTarget.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowBank.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowBattleCommand.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowBattleItems.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowBattlePets.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowBattleResult.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowBattleStateNotice.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowBlacklist.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowBottomBar.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowChat.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowConfirmation.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowContacts.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowCraft.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowCrystalExchangeShop.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowCrystalShop.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowCrystalTrade.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowDialog.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowEmote.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowGather.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowGroupSelect.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowGuildDetails.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowGuildRank.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowHotkeys.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowInventory.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowItemStorage.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowItemUse.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowMail.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowMailCompose.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowMailRead.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowMessageBoard.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowMiniMail.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowMiniMap.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowOwnProfile.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowOwnProfileCategory.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowPetAttributes.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowPetDetail.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowPetList.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowPetMailCompose.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowPetSkillChoice.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowPetSkills.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowPlayerAttributes.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowPlayerProfile.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowPlayerTitles.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowProfile.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowProfileCategory.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowProfileReply.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowProfileSearch.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowProfileView.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowQuantity.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowSettings.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowShop.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowShopChoice.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowShortcutBar.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowSkill.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowSkillResult.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowSpectator.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowStall.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowStatusBar.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowTextList.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowTextTarget.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowTimerNotice.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowTrade.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowTradeTarget.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui、cg::ui::detail |
| `src/ui/windows/WindowWorkResult.cpp` | `cg::ui` | NS06：核对归属、依赖与前置声明 | cg::ui |
| `src/ui/WindowStorageRecords.hpp` | `cg::ui::detail` | NS06：核对归属、依赖与前置声明 | cg::ui::detail |
| `src/world/Blacklist.cpp` | `cg::blacklist` | NS06：核对归属、依赖与前置声明 | cg::world::blacklist |
| `src/world/GameTime.cpp` | `cg::world` | NS06：核对归属、依赖与前置声明 | cg::world |
| `src/world/GuildState.cpp` | `cg::login::detail` | NS01 | cg::world::detail |
| `src/world/MailState.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world::detail |
| `src/world/MailState.hpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world::detail |
| `src/world/MapDrawCache.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world::detail |
| `src/world/MapSceneDrawing.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world::detail |
| `src/world/PetTechniqueRecord.hpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world::detail |
| `src/world/RenderInput.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world::detail |
| `src/world/SceneAngleTable55DF10.inc` | 无具名 namespace | NS06：核对归属、依赖与前置声明 | 无具名 namespace |
| `src/world/SceneState.cpp` | `cg::world`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world、cg::world::detail |
| `src/world/WorldObjectState.cpp` | `cg::world`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world、cg::world::detail |
| `src/world/WorldObjectState.hpp` | `cg::login`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::yobi、cg::world::detail |
| `src/world/WorldUpdate.cpp` | `cg::world`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::world、cg::world::detail |
| `src/world/WorldUpdateDependencies.hpp` | `cg::login`、`cg::render`、`cg::input`、`cg::login::detail`、`cg::platform`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 | cg::render、cg::input、cg::world::detail |
| `src/Yobi/animation.cpp` | `cg::login`、`cg::login::detail`、`cg::world::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/animation.hpp` | 无具名 namespace | NS01 / NS04 | 无具名 namespace |
| `src/Yobi/base.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/base.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_actor.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_actor.hpp` | 无具名 namespace | NS01 / NS04 | 无具名 namespace |
| `src/Yobi/battle_attachment.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_attachment.hpp` | 无具名 namespace | NS01 / NS04 | 无具名 namespace |
| `src/Yobi/battle_attack.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_attack.hpp` | 无具名 namespace | NS01 / NS04 | 无具名 namespace |
| `src/Yobi/battle_base.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_base.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_cast.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_cast.hpp` | 无具名 namespace | NS01 / NS04 | 无具名 namespace |
| `src/Yobi/battle_display.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_display.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_effect.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_effect.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_impact.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_impact.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_message.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_message.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_ride.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_ride.hpp` | 无具名 namespace | NS01 / NS04 | 无具名 namespace |
| `src/Yobi/battle_scene.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_scene.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_skill.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_skill.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_skill_attack.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_skill_attack.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_skill_special.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_skill_special.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_skill_status.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_skill_status.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_status.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/battle_status.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/environment.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/environment.hpp` | 无具名 namespace | NS01 / NS04 | 无具名 namespace |
| `src/Yobi/map_actor.cpp` | `cg::login`、`cg::login::detail`、`cg::world::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/map_actor.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/map_observation.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/map_observation.hpp` | 无具名 namespace | NS01 / NS04 | 无具名 namespace |
| `src/Yobi/map_render.cpp` | `cg::login`、`cg::login::detail`、`cg::world::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/map_render.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/map_transition.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/map_transition.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/preview.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/preview.hpp` | 无具名 namespace | NS01 / NS04 | 无具名 namespace |
| `src/Yobi/window.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `src/Yobi/window.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 | cg::yobi、cg::yobi::detail |
| `include/cg/audio/LegacyAudio.hpp` | `cg::audio` | NS06：核对归属、依赖与前置声明 | cg::audio |
| `include/cg/input/Input.hpp` | `cg`、`cg::input` | NS06：核对归属、依赖与前置声明 | cg::input、cg::input::detail |
| `include/cg/network/NetworkRuntime.hpp` | `cg::network` | NS06：核对归属、依赖与前置声明 | cg::network |
| `include/cg/platform/CommandOptions.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/KeyState.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyCryptInt.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyCursor.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyMemory.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyModuleDigest.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyMouse.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyMouseTracker.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyNetworkAdapter.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyStdio.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyTime.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyWavesReverb.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/LegacyWindow.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/MainLoop.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/SdlAudioBackend.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/platform/Startup.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 | cg::platform |
| `include/cg/render/Font.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `include/cg/render/GraphicCacheState.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `include/cg/render/GraphicDxCache.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `include/cg/render/GraphicPaletteCache.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `include/cg/render/GraphicPendingQueue.hpp` | `cg::login`、`cg::render` | NS06：核对归属、依赖与前置声明 | cg::yobi、cg::render |
| `include/cg/render/Palette.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `include/cg/render/QueueCapacity.hpp` | 无具名 namespace | NS06：核对归属、依赖与前置声明 | 无具名 namespace |
| `include/cg/render/RendererResources.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `include/cg/render/SdlPresenter.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `include/cg/render/SurfaceState.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 | cg::render |
| `include/cg/render/TextPendingQueue.hpp` | `cg::input`、`cg::render` | NS06：核对归属、依赖与前置声明 | cg::input、cg::render |
| `include/cg/resource/AnimationRuntime.hpp` | `cg::resource::animation::detail`、`cg::resource::animation` | NS06：核对归属、依赖与前置声明 | cg::resource::animation::detail、cg::resource::animation |
| `include/cg/resource/GraphicAlloc.hpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 | cg::resource |
| `include/cg/resource/GraphicDecode.hpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 | cg::resource |
| `include/cg/resource/GraphicInfo.hpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 | cg::resource |
| `include/cg/resource/GraphicRead.hpp` | `cg::render`、`cg::resource` | NS06：核对归属、依赖与前置声明 | cg::render、cg::resource |
| `include/cg/resource/ResourceNames.hpp` | `cg` | NS01 | cg::resource |
| `include/cg/resource/ResourcePaths.hpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 | cg::resource |
| `include/cg/scene/character_select/CharacterSelectScene.hpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 | cg::character_select |
| `include/cg/scene/login/CharacterCreationRequest.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `include/cg/scene/login/CharacterDeletionRequest.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `include/cg/scene/login/CharacterListRequest.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `include/cg/scene/login/LoginConnection.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `include/cg/scene/login/LoginInput.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `include/cg/scene/login/LoginMessage.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `include/cg/scene/login/LoginRequest.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `include/cg/scene/login/LoginScene.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `include/cg/scene/login/LoginState.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 | cg::login |
| `include/cg/text/Encoding.hpp` | `cg::text` | NS06：核对归属、依赖与前置声明 | cg::text |
| `include/cg/ui/HitTest.hpp` | `cg::ui` | NS06：核对归属、依赖与前置声明 | cg::ui |
| `include/cg/ui/WindowRuntime.hpp` | `cg::login`、`cg::ui` | NS06：核对归属、依赖与前置声明 | cg::yobi、cg::ui |
| `include/cg/world/Blacklist.hpp` | `cg::blacklist` | NS06：核对归属、依赖与前置声明 | cg::world::blacklist |
| `include/cg/world/GameTime.hpp` | `cg::world` | NS06：核对归属、依赖与前置声明 | cg::world |
| `include/cg/world/GuildState.hpp` | `cg::login::detail` | NS01 | cg::world::detail |
| `include/cg/world/SceneState.hpp` | `cg::world` | NS06：核对归属、依赖与前置声明 | cg::world |
| `include/cg/Yobi/YobiRuntime.hpp` | `cg::platform`、`cg::render`、`cg::login`、`cg::login::detail` | NS01 / NS04 | cg::platform、cg::render、cg::yobi、cg::yobi::detail |
| `doc/profiling/20261013_fps/SdlOfficialScaffold.cpp` | 无 namespace | 保留，不执行 | 已扫描，保持 |


## 拆分细化

NS04将世界对象协调、世界对象绘制和聊天存储分别落入 WorldActor.cpp、WorldActorDrawing.cpp、ChatHistory.cpp；输入的历史浏览／提交仍属 input，通过 ChatHistory.hpp 借用原历史记录与游标，不新增代理状态。共用空通知函数保留原空操作，改由 core/LegacyNoOp.hpp 提供 inline 定义，移除 Input.cpp 的根 namespace 实现块。以上是既有函数与存储迁移，无新业务。

NS05将原 LegacyMouse.cpp 初始化前缀逐字搬到 RuntimeInitialization.cpp，保留 CMake 原位置和内部排列；鼠标函数正文逐字保留在 LegacyMouse.cpp。该专用文件保留 platform/world 两模块 namespace，其余业务 CPP 不再混入其他模块定义。

## 最终归属补充

NS06同时修正单域错位：InputCaret 归 cg::input::detail，黑名单归 cg::world::blacklist，RendererRebuild 移入 render。渲染设备、时间转换、存档、音频及登录请求声明集中到各自所属头；删除平台目录仅转述渲染接口的 WindowProcedureDependencies.hpp。移除模块级跨域 using 转发；函数内部为局部业务读取使用的 using 保留在其作用域内。

首次编译暴露的遗漏已补齐：LocalTime、Yobi 注册项清理、固定音乐播放和世界W16状态声明。原458AE0探测需要访问音频模块私有后端，因此最终归 audio 并在原文件实现；平台调用所属接口，不暴露私有后端。此前静态拆分中的平台定位由本裁决覆盖。快捷键窗口两处旧 login 悬停值引用修正为 render。

新增文件：

| 文件 | namespace | 职责 |
|---|---|---|
| `src/scene/login/LoginSavedState.hpp` | cg::login::detail | 既有实现／声明按职责归属 |
| `src/world/WorldActor.cpp` | cg::world::detail | 既有实现／声明按职责归属 |
| `src/world/WorldActorDrawing.cpp` | cg::world::detail | 既有实现／声明按职责归属 |
| `src/world/ChatHistory.cpp` | cg::world::detail | 既有实现／声明按职责归属 |
| `src/world/ChatHistory.hpp` | cg::world::detail | 既有实现／声明按职责归属 |
| `include/cg/core/LegacyNoOp.hpp` | cg::core | 既有实现／声明按职责归属 |
| `src/platform/RuntimeInitialization.cpp` | cg::platform、cg::world::detail | 原顺序构造／析构例外 |

## 剩余多模块 namespace 的逐文件理由

当前322份源／头／inc已复扫，另检查历史性能脚手架。9份头文件的其他模块块只含类型前置声明；只有1份CPP有跨模块定义，即专门承载有序初始化的RuntimeInitialization.cpp。同模块公开／detail／state及匿名辅助层不视为跨模块混杂。

| 文件 | 保留原因 |
|---|---|
| `src/scene/character_select/CharacterSelectState.hpp` | 外部类型的前置声明；实现和状态定义仍由本模块持有。 |
| `src/ui/WindowRuntimeState.hpp` | 外部类型的前置声明；实现和状态定义仍由本模块持有。 |
| `src/world/WorldObjectState.hpp` | 外部类型的前置声明；实现和状态定义仍由本模块持有。 |
| `src/world/WorldUpdateDependencies.hpp` | 外部类型的前置声明；实现和状态定义仍由本模块持有。 |
| `include/cg/render/GraphicPendingQueue.hpp` | 外部类型的前置声明；实现和状态定义仍由本模块持有。 |
| `include/cg/render/TextPendingQueue.hpp` | 外部类型的前置声明；实现和状态定义仍由本模块持有。 |
| `include/cg/resource/GraphicRead.hpp` | 外部类型的前置声明；实现和状态定义仍由本模块持有。 |
| `include/cg/ui/WindowRuntime.hpp` | 外部类型的前置声明；实现和状态定义仍由本模块持有。 |
| `include/cg/Yobi/YobiRuntime.hpp` | 外部类型的前置声明；实现和状态定义仍由本模块持有。 |
| `src/platform/RuntimeInitialization.cpp` | 集中维持原CRT构造、RNG及逆序析构顺序；不可按模块拆开。 |

## 验收结果

2026-10-16：NS01～NS06全部完成。build.bat Debug 第二次执行配置、编译和客户端链接成功，退出码0；首次发现的接口遗漏已修正。仍有C4701/C4703未初始化变量及C4996旧API告警，未以本次命名空间整理改变这些业务分支，也未据构建通过声明运行验收完成。

静态验收：rg枚举与322文件清单一致；9份多模块头的跨域内容全部为类型前置声明；模块／全局作用域无using namespace；项目内引号include与CMake源码路径全部有效；3680行函数身份／状态保持。当前函数表同步本轮迁移定位，历史行号仍需按符号查找。源码UTF-8无BOM、LF，差异检查通过。未运行客户端／测试，未计算摘要，未生成或修改JSON台账。

## 2026-10-17 第二轮优化方案（已确认，实施中）

本节对应本轮命名空间审查及用户“你优化一下”。用户已回复“y”确认本节方案，按 NS07～NS11 分步修改、静态核对并提交。

当前 AGENTS.md 禁用空 namespace 定义和保护，覆盖上文保留匿名 namespace 的旧裁决。本轮不继承上次 build.bat 授权；只做静态检查，不构建、测试、运行或检查编译产物，不计算摘要。旧函数表与 JSON 台账不更新。

### 当前证据

- 本轮审查覆盖 src、include 的346份C/C++源／头文件，不含第三方；发现105个文件共144处匿名 namespace，其中 battle_display.cpp 有一处完全空块。统计包含 src/tools 中已停用的历史工具，不代表全部参与客户端构建。
- YobiRuntime.hpp 的 detail 块从第38行延续至文件末尾，包含被其他模块调用的创建、更新、绘制与字段访问接口；GuildState.hpp 整组接口同样位于 world::detail。Input.hpp 的 Sub_44A410 也被 world 场景调度直接调用。
- WorldUpdateDependencies.hpp 被82个文件直接包含，跨10个源码目录；16个文件共90处 using namespace world::detail，LuaData.cpp 在文件作用域导入。
- WindowRuntime.cpp 同时定义 cg::ui 和 cg::script 函数。ReadMousePhase、ConsumeLuaDrop 只由脚本 UIControl 使用，实际操作的是本文件静态拖动状态；LuaHost.hpp 提供两条脚本声明。
- RuntimeInitialization.cpp 的跨模块定义用于保持原CRT动态初始化、RNG消费及析构顺序；外部类型前置声明、同模块公开／内部层次另有明确用途。

### 目标与边界

1. 一个业务实现文件由一个模块负责；外部模块前置声明和有序初始化专用文件保留现有例外。
2. 实际跨模块使用的接口归所属模块命名空间。detail 仅承载该模块内部跨文件共享的实现声明，不作为所有旧函数和共享状态的统一容器。
3. 文件私有函数与变量保留内部链接；共享对象保持唯一实体、原翻译单元、初始化表达式、定义相对顺序及函数内静态首次构造时点。
4. 保留业务正文、地址标识、签名、默认参数、字段布局、别名、读写及回调顺序。不借整理新增状态副本、兼容别名、批量访问器或抽象层。
5. 只处理当前客户端源码和头文件；第三方、历史性能脚手架及已停用的离线 MIDI 工具保留。历史工具中的匿名 namespace 单独登记为范围排除，不能宣称整个 src 零残留。

### 实施步骤

| 步骤 | 具体调整 | 完成条件 |
|---|---|---|
| NS07 文件归属及空块 | 删除 battle_display.cpp 真正空块。将 WindowRuntime.cpp 末尾两个脚本桥接实现归回 cg::ui，名称统一为 WindowDrag_ReadMousePhase、WindowDrag_ConsumeDrop；在现有 WindowRuntime.hpp 声明，UIControl.cpp 直接调用，删除 LuaHost.hpp 原声明，不留同义转发。 | WindowRuntime.cpp 只定义UI模块成员；拖动状态仍为原文件静态对象；phase判断、回调参数及回调后重置次序保持。 |
| NS08 对外接口分层 | 按当前定义和直接消费者逐项划分 YobiRuntime.hpp：模块外使用的函数／共享状态归 cg::yobi；仅Yobi内部使用的声明移至已有 base.hpp 或对应功能头，继续放 cg::yobi::detail。GuildState.hpp 跨模块合同归 cg::world，内部声明归既有 world 实现头；Sub_44A410 归 cg::input。同步定义、前置声明和全部调用。 | 公共接口不再强迫调用方进入上述 detail；内部载荷、私有实现声明不因迁移全部公开；旧限定名没有未处理引用，不增加兼容alias。 |
| NS09 世界声明按职责拆分 | 将 WorldUpdateDependencies.hpp 中地图／移动、玩家／技能／宠物、战斗、图鉴四组声明分别收敛至 src/world 的 WorldMapState.hpp、PlayerState.hpp、BattleState.hpp、AlbumState.hpp；邮件、聊天、世界对象及宠物技术记录优先复用已有 MailState.hpp、ChatHistory.hpp、WorldObjectState.hpp、PetTechniqueRecord.hpp。WorldUpdateDependencies.hpp 仅保留 world 更新编排所需声明，不作为全量聚合入口。跨模块使用的声明归 cg::world，纯 world 内部共享声明保留 detail。 | 每组声明具有唯一所属头、完整直接依赖与原定义；消费者包含实际所需头，不能只在旧头转包含所有新头；不搬状态定义或改变初始化顺序。 |
| NS10 显式依赖 | 移除客户端源码中模块／文件作用域的 using namespace，包括 world::detail 及脚本私有实现域的整域导入；使用实际归属限定名。函数内跨模块 using namespace world::detail 同步改为限定名，不把隐式导入转成更外层导入。 | 调用点可辨认状态／函数所属模块；保持原重载目标及参数，不批量删除同模块合理的类型别名或局部实现层。 |
| NS11 文件私有符号 | 按文件清除客户端匿名 namespace。文件私有函数／变量及其前置声明显式使用 static；已有内部链接常量不扩大可见范围。文件私有类型保留在所属模块、仅在CPP定义；重名时按文件职责补唯一语义前缀，不另建大量 *_detail 命名空间。合并无必要的同域反复开闭。 | 客户端范围无匿名／完全空 namespace；无私有变量意外外部化、跨文件重名定义、声明／定义链接不一致；动态初始化顺序逐项保持。 |

NS09 的职责分组依据现有源码注释及消费者，不依据地址相近推断业务归属。不能确认归属的声明先列出具体符号和消费者，确认前不搬移，也不把该组记为完成。新头文件仅承接上述已有的大组声明，不按单个函数碎片化拆文件。

NS07 只调整拖动桥接的函数归属。WindowRuntime.cpp 已有 PrepareLuaWindowFrame、MergeLuaWindowOrder 等脚本调度调用属于现行集成合同，本轮不借移除跨模块定义改变调度方向或删除必要包含。

### 必须保留的例外与取舍

- RuntimeInitialization.cpp 保留跨模块定义及其交错排列；NS11可以调整文件私有声明的写法，但不得按 namespace 分组重排对象，也不得拆成多个翻译单元。
- 头文件中其他模块的类型前置声明继续保留；AnimationRuntime.hpp 中 detail 不完整类型的指针边界不机械上提。
- Yobi载荷等模块内部跨文件类型仍可使用 detail。公开接口迁出后，不要求所有 detail 清零。
- 匿名 namespace 中的类／枚举不能直接以 static 修饰类型声明。改为CPP内唯一具名类型后，其类型身份及相关符号限定名会变化，不能宣称类型链接属性完全不变。必须核对同模块重名类型、模板实例、内联成员、ADL与对外签名；不允许通过导出同名类型或放到公共头规避检查。
- 命名空间调整会改变C++修饰符号；本轮按全项目源码消费者同步迁移，不承诺兼容旧目标文件。后续实际编译／链接验收需要用户明确恢复权限。

### 验收与提交

每一步先核对当前工作区，保留用户既有 AGENTS.md 修改、采样目录及 Python 缓存；只精确暂存本步文件，在既有 cg2d_re 分支提交。同步本SPEC的执行状态与唯一进度，不改根计划。

静态验收包括：声明／定义／使用处及include的一致性；新旧函数正文除限定名与必要链接说明外的差异；可变对象唯一性、静态构造顺序与回调次序；默认参数唯一声明、类型身份和名称冲突；匿名／空块／跨模块定义／整域导入的范围复扫；UTF-8与原换行风格、Git差异检查。统计必须区分客户端和排除的历史工具，不仅以命中数量下降判定完成。

静态通过只记“源码调整及静态核对完成”。编译／链接与实际运行分别保持待验收，不继承2026-10-16的通过结果，不以本轮限制降低原验收标准。

### 本轮执行记录

NS07：已删除 battle_display.cpp 空匿名块；拖动阶段读取与阶段4消费归 cg::ui，公开头声明及脚本控件调用同步，删除脚本域旧声明／定义。原三项拖动状态仍为 WindowRuntime.cpp 静态对象，回调参数、阶段判断及回调后重置保持；UI现有脚本窗口调度调用保留。仅静态核对，编译／链接待验收。
