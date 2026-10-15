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

| 文件 | 原 namespace（匿名省略） | 处置依据 |
|---|---|---|
| `src/audio/LegacyAudio.cpp` | `cg::login::detail`、`cg::audio`、`cg::network::detail`、`cg::platform`、`cg::platform::shutdown_detail`、`cg::platform::initialization_detail` | NS02 / NS04 / NS06 |
| `src/audio/LegacySeDefinitions.inc` | 无具名 namespace | NS06：核对归属、依赖与前置声明 |
| `src/input/Input.cpp` | `cg`、`cg::input`、`cg::input::detail`、`cg::world::detail` | NS02 / NS04 / NS06 |
| `src/input/InputCaret.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/input/InputKeyDispatch.cpp` | `cg::input` | NS06：核对归属、依赖与前置声明 |
| `src/input/InputKeyDispatchDependencies.hpp` | `cg::input`、`cg::input::detail` | NS06：核对归属、依赖与前置声明 |
| `src/network/NetworkDispatchDependencies.hpp` | `cg::network::detail` | NS06：核对归属、依赖与前置声明 |
| `src/network/NetworkRuntime.cpp` | `cg::network`、`cg::network::detail` | NS06：核对归属、依赖与前置声明 |
| `src/network/NetworkRuntimeDependencies.hpp` | `cg::network::detail`、`cg::character_select::detail`、`cg::platform`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/platform/ClientEntry.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/ClientInitialization.cpp` | `cg::platform`、`cg::platform::initialization_detail` | NS06：核对归属、依赖与前置声明 |
| `src/platform/ClientShutdown.cpp` | `cg::platform`、`cg::platform::shutdown_detail` | NS06：核对归属、依赖与前置声明 |
| `src/platform/ClientShutdown.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/CommandOptions.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/ImeComposition.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/ImeComposition.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/ImeLifecycle.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/ImeLifecycle.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyAudioMath.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyAudioMath.hpp` | `cg::audio`、`cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyCryptInt.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyCursor.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyFormat.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyFormat.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyMemory.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyModuleDigest.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyMouse.cpp` | `cg::platform`、`cg::world::detail`、`cg::login::detail` | NS02 / NS05 |
| `src/platform/LegacyMouseTracker.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyNetworkAdapter.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyStdio.hpp` | 无具名 namespace | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyTime.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyWavesReverb.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LegacyWindow.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/LoadingScreen.cpp` | `cg::platform::initialization_detail` | NS06：核对归属、依赖与前置声明 |
| `src/platform/MainLoop.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/MainLoopDependencies.hpp` | `cg::render`、`cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/MainLoopState.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/MainLoopSupport.cpp` | `cg::audio`、`cg::platform` | NS02 / NS04 / NS06 |
| `src/platform/MainLoopWindowEnumeration.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/MapEffectMath.cpp` | `cg::platform::detail`、`cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/MapEffectMath.hpp` | `cg::platform::detail`、`cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/RendererRebuild.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/platform/SdlAudioBackend.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/SetupIni.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/StartupCommon.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/StartupNative.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/StartupState.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/WindowLifecycle.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/WindowProcedure.cpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/platform/WindowProcedureDependencies.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/platform/WindowProcedureState.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `src/render/BackTextDrawing.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/DrawAllQueued.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/DrawGraphic.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/DrawGraphicHover.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/Font.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/GraphicCacheState.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/GraphicDxCache.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/GraphicDxCacheState.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/GraphicHitTest.cpp` | `cg::render`、`cg::platform` | NS02 / NS04 / NS06 |
| `src/render/GraphicPaletteCache.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/GraphicPendingQueue.cpp` | `cg::render`、`cg::platform` | NS02 / NS04 / NS06 |
| `src/render/InputText.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/LegacyTextureDrawing.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/LegacyTextureDrawing.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/Palette.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/RendererResources.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/SdlPresenter.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/SurfaceState.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/render/TextPendingQueue.cpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `src/resource/AnimationCleanup.cpp` | `cg::resource::animation` | NS06：核对归属、依赖与前置声明 |
| `src/resource/AnimationList.cpp` | `cg::resource::animation` | NS06：核对归属、依赖与前置声明 |
| `src/resource/AnimationRuntime.cpp` | `cg::resource::animation::detail`、`cg::resource::animation` | NS06：核对归属、依赖与前置声明 |
| `src/resource/AnimationRuntimeState.hpp` | `cg::resource::animation::detail`、`cg::resource::animation` | NS06：核对归属、依赖与前置声明 |
| `src/resource/GraphicAlloc.cpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 |
| `src/resource/GraphicDecode.cpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 |
| `src/resource/GraphicInfo.cpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 |
| `src/resource/GraphicRead.cpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 |
| `src/resource/ResourceNames.cpp` | `cg` | NS01 |
| `src/resource/ResourcePaths.cpp` | `cg::resource`、`cg::resource::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterAuthenticationFailure.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterDeletionConfirmation.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterDeletionProgress.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterListProgress.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterModalButton.cpp` | `cg::character_select::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterSelectModal.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterSelectPreview.cpp` | `cg::character_select`、`cg::character_select::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterSelectScene.cpp` | `cg::character_select::detail`、`cg::character_select::state`、`cg::character_select` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterSelectSceneDependencies.hpp` | `cg::character_select::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterSelectSlot.cpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterSelectState.hpp` | `cg::input`、`cg::character_select::state` | NS06：核对归属、依赖与前置声明 |
| `src/scene/character_select/CharacterVersionUpgrade.cpp` | `cg::character_select`、`cg::character_select::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/CharacterCreationRequest.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/CharacterDeletionRequest.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/CharacterListRequest.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginCapability3InputController.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginCapability3Scene.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginCapability4Modal.cpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginCapability4Modal.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginCapability4Scene.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginConnection.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginInput.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginInputController.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginInputDependencies.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginMessage.cpp` | `cg::login`、`cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginMessageDependencies.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginModeScene.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginOuterScene.cpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginRequest.cpp` | `cg::login`、`cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginRequestDependencies.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginSavedState.cpp` | `cg::login::detail`、`cg::platform`、`cg::character_select::detail` | NS02 / NS04 / NS06 |
| `src/scene/login/LoginScene.cpp` | `cg::login`、`cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginSceneDependencies.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginState.cpp` | `cg::login`、`cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/scene/login/LoginStateDependencies.hpp` | `cg::login::detail` | NS06：核对归属、依赖与前置声明 |
| `src/text/Encoding.cpp` | `cg::text` | NS06：核对归属、依赖与前置声明 |
| `src/tools/LegacyMidiSynth.cpp` | `cg::platform` | 历史工具，保留 |
| `src/tools/LegacyMidiSynth.hpp` | `cg::platform` | 历史工具，保留 |
| `src/tools/MidiToWav.cpp` | 无具名 namespace | 历史工具，保留 |
| `src/ui/HitTest.cpp` | `cg::ui` | NS06：核对归属、依赖与前置声明 |
| `src/ui/LegacyUiRecord.cpp` | `cg::ui` | NS06：核对归属、依赖与前置声明 |
| `src/ui/UiControlRecord.hpp` | `cg::ui` | NS06：核对归属、依赖与前置声明 |
| `src/ui/WindowPrototypeDependencies.hpp` | `cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/WindowPrototypes.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/WindowRuntime.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/WindowRuntimeInternal.hpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/WindowRuntimeState.hpp` | `cg::login`、`cg::login::detail`、`cg::input`、`cg::world::detail`、`cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowAdventureLog.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowAlbumDetail.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowAlbumList.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowAppearance.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowAttributeTarget.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowBank.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowBattleCommand.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowBattleItems.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowBattlePets.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowBattleResult.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowBattleStateNotice.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowBlacklist.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowBottomBar.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowChat.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowConfirmation.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowContacts.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowCraft.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowCrystalExchangeShop.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowCrystalShop.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowCrystalTrade.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowDialog.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowEmote.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowGather.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowGroupSelect.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowGuildDetails.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowGuildRank.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowHotkeys.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowInventory.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowItemStorage.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowItemUse.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowMail.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowMailCompose.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowMailRead.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowMessageBoard.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowMiniMail.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowMiniMap.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowOwnProfile.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowOwnProfileCategory.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowPetAttributes.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowPetDetail.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowPetList.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowPetMailCompose.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowPetSkillChoice.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowPetSkills.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowPlayerAttributes.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowPlayerProfile.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowPlayerTitles.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowProfile.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowProfileCategory.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowProfileReply.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowProfileSearch.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowProfileView.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowQuantity.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowSettings.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowShop.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowShopChoice.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowShortcutBar.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowSkill.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowSkillResult.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowSpectator.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowStall.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowStatusBar.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowTextList.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowTextTarget.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowTimerNotice.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowTrade.cpp` | `cg::input`、`cg::ui`、`cg::ui::detail` | NS03 |
| `src/ui/windows/WindowTradeTarget.cpp` | `cg::ui`、`cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/ui/windows/WindowWorkResult.cpp` | `cg::ui` | NS06：核对归属、依赖与前置声明 |
| `src/ui/WindowStorageRecords.hpp` | `cg::ui::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/Blacklist.cpp` | `cg::blacklist` | NS06：核对归属、依赖与前置声明 |
| `src/world/GameTime.cpp` | `cg::world` | NS06：核对归属、依赖与前置声明 |
| `src/world/GuildState.cpp` | `cg::login::detail` | NS01 |
| `src/world/MailState.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/MailState.hpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/MapDrawCache.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/MapSceneDrawing.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/PetTechniqueRecord.hpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/RenderInput.cpp` | `cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/SceneAngleTable55DF10.inc` | 无具名 namespace | NS06：核对归属、依赖与前置声明 |
| `src/world/SceneState.cpp` | `cg::world`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/WorldObjectState.cpp` | `cg::world`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/WorldObjectState.hpp` | `cg::login`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/WorldUpdate.cpp` | `cg::world`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/world/WorldUpdateDependencies.hpp` | `cg::login`、`cg::render`、`cg::input`、`cg::login::detail`、`cg::platform`、`cg::world::detail` | NS06：核对归属、依赖与前置声明 |
| `src/Yobi/animation.cpp` | `cg::login`、`cg::login::detail`、`cg::world::detail` | NS01 / NS04 |
| `src/Yobi/animation.hpp` | 无具名 namespace | NS01 / NS04 |
| `src/Yobi/base.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/base.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_actor.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_actor.hpp` | 无具名 namespace | NS01 / NS04 |
| `src/Yobi/battle_attachment.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_attachment.hpp` | 无具名 namespace | NS01 / NS04 |
| `src/Yobi/battle_attack.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_attack.hpp` | 无具名 namespace | NS01 / NS04 |
| `src/Yobi/battle_base.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_base.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_cast.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_cast.hpp` | 无具名 namespace | NS01 / NS04 |
| `src/Yobi/battle_display.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_display.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_effect.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_effect.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_impact.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_impact.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_message.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_message.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_ride.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_ride.hpp` | 无具名 namespace | NS01 / NS04 |
| `src/Yobi/battle_scene.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_scene.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_skill.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_skill.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_skill_attack.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_skill_attack.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_skill_special.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_skill_special.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_skill_status.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_skill_status.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_status.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/battle_status.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/environment.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/environment.hpp` | 无具名 namespace | NS01 / NS04 |
| `src/Yobi/map_actor.cpp` | `cg::login`、`cg::login::detail`、`cg::world::detail` | NS01 / NS04 |
| `src/Yobi/map_actor.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/map_observation.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/map_observation.hpp` | 无具名 namespace | NS01 / NS04 |
| `src/Yobi/map_render.cpp` | `cg::login`、`cg::login::detail`、`cg::world::detail` | NS01 / NS04 |
| `src/Yobi/map_render.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/map_transition.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/map_transition.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/preview.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/preview.hpp` | 无具名 namespace | NS01 / NS04 |
| `src/Yobi/window.cpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `src/Yobi/window.hpp` | `cg::login`、`cg::login::detail` | NS01 / NS04 |
| `include/cg/audio/LegacyAudio.hpp` | `cg::audio` | NS06：核对归属、依赖与前置声明 |
| `include/cg/input/Input.hpp` | `cg`、`cg::input` | NS06：核对归属、依赖与前置声明 |
| `include/cg/network/NetworkRuntime.hpp` | `cg::network` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/CommandOptions.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/KeyState.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyCryptInt.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyCursor.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyMemory.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyModuleDigest.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyMouse.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyMouseTracker.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyNetworkAdapter.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyStdio.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyTime.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyWavesReverb.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/LegacyWindow.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/MainLoop.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/SdlAudioBackend.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/platform/Startup.hpp` | `cg::platform` | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/Font.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/GraphicCacheState.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/GraphicDxCache.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/GraphicPaletteCache.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/GraphicPendingQueue.hpp` | `cg::login`、`cg::render` | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/Palette.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/QueueCapacity.hpp` | 无具名 namespace | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/RendererResources.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/SdlPresenter.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/SurfaceState.hpp` | `cg::render` | NS06：核对归属、依赖与前置声明 |
| `include/cg/render/TextPendingQueue.hpp` | `cg::input`、`cg::render` | NS06：核对归属、依赖与前置声明 |
| `include/cg/resource/AnimationRuntime.hpp` | `cg::resource::animation::detail`、`cg::resource::animation` | NS06：核对归属、依赖与前置声明 |
| `include/cg/resource/GraphicAlloc.hpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 |
| `include/cg/resource/GraphicDecode.hpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 |
| `include/cg/resource/GraphicInfo.hpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 |
| `include/cg/resource/GraphicRead.hpp` | `cg::render`、`cg::resource` | NS06：核对归属、依赖与前置声明 |
| `include/cg/resource/ResourceNames.hpp` | `cg` | NS01 |
| `include/cg/resource/ResourcePaths.hpp` | `cg::resource` | NS06：核对归属、依赖与前置声明 |
| `include/cg/scene/character_select/CharacterSelectScene.hpp` | `cg::character_select` | NS06：核对归属、依赖与前置声明 |
| `include/cg/scene/login/CharacterCreationRequest.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `include/cg/scene/login/CharacterDeletionRequest.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `include/cg/scene/login/CharacterListRequest.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `include/cg/scene/login/LoginConnection.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `include/cg/scene/login/LoginInput.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `include/cg/scene/login/LoginMessage.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `include/cg/scene/login/LoginRequest.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `include/cg/scene/login/LoginScene.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `include/cg/scene/login/LoginState.hpp` | `cg::login` | NS06：核对归属、依赖与前置声明 |
| `include/cg/text/Encoding.hpp` | `cg::text` | NS06：核对归属、依赖与前置声明 |
| `include/cg/ui/HitTest.hpp` | `cg::ui` | NS06：核对归属、依赖与前置声明 |
| `include/cg/ui/WindowRuntime.hpp` | `cg::login`、`cg::ui` | NS06：核对归属、依赖与前置声明 |
| `include/cg/world/Blacklist.hpp` | `cg::blacklist` | NS06：核对归属、依赖与前置声明 |
| `include/cg/world/GameTime.hpp` | `cg::world` | NS06：核对归属、依赖与前置声明 |
| `include/cg/world/GuildState.hpp` | `cg::login::detail` | NS01 |
| `include/cg/world/SceneState.hpp` | `cg::world` | NS06：核对归属、依赖与前置声明 |
| `include/cg/Yobi/YobiRuntime.hpp` | `cg::platform`、`cg::render`、`cg::login`、`cg::login::detail` | NS01 / NS04 |
| `doc/profiling/20261013_fps/SdlOfficialScaffold.cpp` | 历史性能脚手架 | 保留，不执行 |
