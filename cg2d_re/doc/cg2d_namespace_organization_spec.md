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
