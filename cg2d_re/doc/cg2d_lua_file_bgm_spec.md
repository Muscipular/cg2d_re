# Lua 文件背景音乐 SPEC（LBG01）

状态：设计稿，待确认；尚未修改运行代码或引入依赖。日期：2026-10-20。

## 1. 已确认需求

- Lua 增加 MP3、WAV、OGG 文件播放能力，仅用于背景音乐，不扩展短音效接口。
- 支持替换当前场景、接管、独立播放三种模式。
- 每次成功播放返回 handle，通过 handle 停止对应音乐。
- 本轮先做 SPEC。以下接口名称、模式细则、依赖与资源策略均为具体实施建议，不将未确认的细节视为既定业务。

## 2. 当前实现与 SDL 能力

- `src/script/LuaUI.cpp` 已导出 `WinMgr.PlaySe`、`WinMgr.LoadAndPlayBGM`、`WinMgr.PlayFixedBGM`，均通过编号播放，不接受任意音频路径。
- `src/audio/LegacyAudio.cpp` 管理普通场景 BGM 请求、淡出、音量与生命周期；唯一 `gSdlAudioBackend` 实例也承接旧音效和固定音乐。
- `src/platform/SdlAudioBackend.cpp` 通过 `SDL_LoadWAV_IO` 整段加载 WAV、转换 PCM，再用 SDL 音频流播放；现有“流式 BGM”没有实现压缩文件的增量解码。
- 项目本地 SDL 为3.4.8；当前CMake未接入SDL3_mixer，未找到独立MP3/Vorbis解码依赖。
- SDL3核心已经提供设备、播放、混音、重采样和WAV读取；MP3、Ogg Vorbis等文件解码可由官方独立扩展库SDL3_mixer提供。[SDL3音频](https://wiki.libsdl.org/SDL3/CategoryAudio)、[SDL3_mixer说明](https://wiki.libsdl.org/SDL3_mixer/FrontPage)

## 3. 依赖方案与格式边界

### 3.1 推荐使用 SDL3_mixer

推荐新增SDL3_mixer 3.2.4稳定版，保留当前SDL3及旧音频实现。该版本要求SDL至少3.4.0，本地3.4.8满足声明的版本条件，但实际编译兼容性仍需实施后验证。[官方发布](https://github.com/libsdl-org/SDL_mixer/releases/tag/release-3.2.4)、[该版本构建配置](https://github.com/libsdl-org/SDL_mixer/blob/release-3.2.4/CMakeLists.txt)

建议按以下范围静态构建，并保持现有MSVC/x64及CRT配置：

| 格式 | 推荐后端 | 承诺范围 |
|---|---|---|
| WAV | SDL3_mixer内建WAVE解码 | RIFF/WAVE的PCM、IEEE Float；ADPCM等由选定版本实际支持能力及样本验收确认，不承诺所有名为.wav的编码 |
| MP3 | SDL3_mixer内置的dr_mp3路径 | 普通MPEG Layer III，包含CBR/VBR；不把AAC文件视为MP3 |
| OGG | libogg与libvorbis/libvorbisfile | Ogg Vorbis；Ogg Opus属于另一种编码，需另行确认是否纳入 |

- 只启用上述解码后端，关闭MIDI、MOD、FLAC、Opus及示例/测试目标；不恢复已停用的MIDI链。
- Ogg优先采用Vorbis官方解码库，避免轻量stb_vorbis的旧floor0及串接流限制；实际编码覆盖仍按验收样本记录。
- 保留SDL3_mixer及必要解码依赖的许可文件；锁定发布版及其依赖来源，不在普通CMake配置阶段自动联网下载。
- 新增目录建议为`cg2d_re/3rd/SDL3_mixer/`及其必要依赖。该项与项目“禁止直接引入外部依赖”的规则存在明确审批边界，当前只提案，不下载、不复制、不修改CMake。

备选为直接封装dr_wav、dr_mp3和Vorbis解码器，继续使用现有SDL输出。但多轨、循环、停止及资源管理需自行完成，维护量更高，本稿不推荐。只有现有SDL核心时不能交付完整MP3/OGG支持，也不能以只完成WAV替代验收。

### 3.2 加载策略建议

首版建议采用`MIX_LoadAudioWithProperties`：不预解码整段PCM、显式忽略文件内嵌循环信息，由本接口的loop选项控制整曲循环。每个成功播放拥有自己的track。

必须区分：**按需解码不等于按需读取文件**。SDL3_mixer普通加载接口仍会把原文件完整缓存到内存；MP3/OGG保留压缩数据，WAV的内存成本接近原文件大小。[加载行为](https://wiki.libsdl.org/SDL3_mixer/MIX_LoadAudio_IO)、[加载属性](https://wiki.libsdl.org/SDL3_mixer/MIX_LoadAudioWithProperties)

前期讨论的真正流式读取可使用`MIX_SetTrackIOStream`，但它引入播放期间磁盘I/O、文件持有和循环策略的额外权衡。本稿建议先采用官方常规加载方式，明确接受上述内存成本；若用户要求长音频也保持固定级别缓冲，则应在实施前改选真正流式方案，不能将缓存整文件标记为流式验收通过。[IO流播放](https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackIOStream)

## 4. Lua 接口建议

```lua
-- path 相对 luaUI/audio；mode 必填，loop 默认 true。
local music, err = WinMgr.PlayBGMFile("活动主题.mp3", {
    mode = "takeover", -- "replace" / "takeover" / "independent"
    loop = true,
})

if music then
    local state = music:GetState()
    music:Stop()
else
    print(err)
end
```

| 接口 | 返回与语义 |
|---|---|
| `WinMgr.PlayBGMFile(path, options)` | 成功返回`BGMHandle, nil`；打开、解码、设备或启动失败返回`nil, error`，不撤销原播放。成功表示track已启动，不是仅接受异步请求 |
| `handle:Stop()` | 停止并释放该播放，返回true；已停止、已结束或已被替换时重复调用仍返回true，不作用于其他handle |
| `handle:GetState()` | 返回`playing`、`stopped`、`finished`或`replaced`；查询不恢复或重播音乐 |

- 参数类型、mode取值错误使用既有Lua参数错误机制；文件及设备失败以返回值报告，不返回伪成功handle。
- `loop=false`完整播放一遍，结束后释放音频资源；`loop=true`整曲循环，直到Stop、替换或Lua关闭。自定义循环点、无缝循环质量、暂停/续播、定位、淡入淡出及播放列表不在本轮范围。
- 新接口在UI及战斗Lua状态注册，与现有WinMgr保持一致；两状态共享同一音频服务，handle由创建它的Lua状态持有。
- 保留现有三个编号接口的参数、返回值和路径规则，不把整数编号与文件路径重载到同一入口。

## 5. 三种模式与冲突规则（建议确认）

这里的“场景BGM”指`LoadAndPlayBGM`及`UpdateBgmRequest_52C910`管理的普通BGM；旧64路音效及`PlayFixedBGM`的独立固定音乐不属于该通道。

| 模式 | 开始后 | 后续场景实际切曲或显式编号BGM调用 | handle停止/自然结束 |
|---|---|---|---|
| `replace` | 文件音乐替代当前可听见的场景BGM | 原生正常播放流程可以覆盖本handle；handle变为`replaced` | 解除抑制，恢复当时场景BGM输出 |
| `takeover` | 文件音乐接管场景BGM输出 | 场景请求仍更新自身状态与目标，但不能盖过本handle | 解除接管，恢复当时场景BGM输出 |
| `independent` | 新建独立播放，与场景BGM及其他独立handle并存 | 不受影响 | 只停止自己 |

具体规则：

1. `replace`与`takeover`共用一个前台占用位置。新的任一前台模式成功后替换旧前台handle；失败则保持旧handle和场景输出。不会建立接管栈，旧handle不自动复活。
2. 独立模式可以有多个handle，同时播放同一个文件也产生不同播放身份；停止其中一个不影响其他播放。
3. 前台handle存活时，仅抑制原普通BGM的输出增益，原生请求、淡出、加载及存档副作用继续按原流程执行。解除抑制时恢复当前原生BGM的播放位置；不是恢复接管前旧曲目的位置，也不保证从头重播。若当前没有可播场景BGM，则保持安静。
4. `replace`在普通BGM入口通过既有编号/启用门禁后、加载新资源之前失效；特殊停止编号`-2`也终止replace，takeover则保留接管。其他无效编号不解除替换。接线不移动原函数已经存在的停止顺序和门禁；原生后续加载失败沿原错误行为，不复活已替换handle。
5. `takeover`期间普通BGM的切换只更新被抑制的原生播放，结束后能听到当时场景应播放的曲目；不保存并回写过时的场景目标。
6. Stop、自然结束、GC、运行期播放失败均按相同的前台归属规则释放抑制。只有当前前台handle能释放该占用；旧handle的延迟结束通知不能解除新handle的接管。
7. 固定音乐仍沿原通道播放；若需要连固定音乐一并接管，应在实施前明确扩展范围。

## 6. handle 与资源生命周期

- `BGMHandle`使用Lua userdata，内部保存播放身份及代次，不向Lua暴露SDL/MIX指针或数字化指针。
- 原生服务拥有track、音频资源及终止状态。资源结束即释放，仍被脚本持有的handle只保留查询状态所需的轻量记录；不得保留完整音频直到Lua GC。
- `__gc`停止对应播放并释放handle记录。因此脚本需要保存正在播放的handle；不保存返回值的播放可能在GC时结束。
- 模块卸载时脚本主动Stop；关闭一个普通窗口不会自动停止无关联handle。不擅自修改模块系统。
- `ShutdownLua`在关闭两个Lua状态前统一停止其文件BGM。客户端退出时不为恢复场景音乐而重新开设备或加载资源。
- 自然结束通知只能发布原生状态，由主线程回收资源和释放前台占用；音频线程不能调用Lua或直接改场景全局。主动Stop与自然结束竞态必须幂等。
- SDL_mixer停止回调不覆盖“直接销毁track”的全部路径，销毁与关闭也必须显式释放归属，不能只等待回调。[停止回调合同](https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackStoppedCallback)
- 实施时核对所选版本对解码失败、设备错误与正常结束的可观测信息；未能区分的底层终止不伪造详细错误原因，记录实际可取得的错误并保证清理。

## 7. 路径、音量与旧实现边界

- 建议音频根目录固定为客户端工作目录下`luaUI/audio/`，与现有`luaUI/init.lua`入口基准一致；允许根内子目录。仅本地相对路径，不接网络URL，不修改文件，不自动创建资源。
- Lua路径沿现有脚本GBK边界转换为UTF-16；仅进入SDL文件API时转换为UTF-8。中文文件名不按UTF-8猜测解释Lua字节。
- 新文件BGM遵循现有音乐开关与0～15音乐音量，不使用音效音量。建议基准档位采用旧音量表的127档上限：`index = level * 127 / 15`，复用既有百分之一分贝到增益转换；关闭音乐或level=0时静音。静音不结束handle，播放位置继续推进。
- 旧普通BGM自身的音量、淡出值与前台抑制分开存储；其有效输出为原目标增益乘以抑制系数，不能让原音量更新意外解除takeover。
- 新音频系统未准备好或设备不可用时返回失败；不得通过新的Lua路径偷偷重置旧`gAudioDisabled`或重建整个旧音频后端。
- 新模块不套用旧曲目专属采样率、声像、smpl循环点或编号映射，不修改bgm.cf及旧音源文件。

## 8. 实施位置

| 文件/模块 | 拟职责 |
|---|---|
| `src/script/LuaAudio.cpp`与现有Lua注册声明 | 参数、userdata、Stop/GetState与GC绑定 |
| `include/cg/audio/FileBgm.hpp`、`src/audio/FileBgm.cpp` | 播放身份、三模式仲裁、Lua状态归属与结束回收 |
| `include/cg/platform/FileBgmBackend.hpp`、`src/platform/FileBgmBackend.cpp` | 封装SDL3_mixer、文件加载、track和设备生命周期，公开接口不泄漏SDL/MIX类型 |
| `src/audio/LegacyAudio.cpp` | 普通BGM切曲通知、设置同步与主线程回收接线，不重写旧状态机 |
| `SdlAudioBackend.hpp/.cpp` | 普通BGM输出抑制及原目标音量保存；不暂停整个设备，以免影响音效 |
| `src/script/LuaRuntime.cpp` | Lua关闭时清理其播放 |
| `cg2d_re/CMakeLists.txt`及批准的`3rd/`目录 | 静态依赖、必要编译单元、格式开关与许可证 |

新文件播放使用独立`MIX_Mixer`逻辑设备，旧设备及音效继续由既有后端拥有；两者通过SDL默认输出播放。mixer创建/销毁放在主线程，只释放本模块拥有的资源，不调用全局SDL_Quit。[Mixer设备接口](https://wiki.libsdl.org/SDL3_mixer/MIX_CreateMixerDevice)

## 9. 验收清单

1. 三种格式分别完成正常播放、一次播放、循环和Stop；包括中文路径、单/双声道、常用采样率、MP3的CBR/VBR。OGG必须验证实际编码，不仅检查扩展名。
2. `replace`能被场景实际切曲覆盖；`takeover`跨场景仍保持文件音乐，停止后恢复当前场景输出；`independent`可与场景及另一handle同时播放。
3. 先开A再开B，A被替换后Stop/GC/延迟回调均不能停止B或解除B的接管；独立handle不受前台替换影响。
4. 新文件调用遇文件缺失、损坏、不支持编码、分配/加载/启动失败不会切断原播放；运行期间终止及设备错误不遗留前台占用和播放资源。原生编号入口的既有失败行为按第5节处理，不混同于新接口的失败保持要求。
5. 自然结束、重复Stop、GC、Lua关闭和客户端退出均正确释放；音频线程不调用Lua，旧64路音效与固定音乐继续正常工作。
6. 音乐开关/音量对三模式生效，静音不会破坏handle；原生音量/淡出更新不能绕过接管。
7. 核对选定加载策略的内存行为：预缓存方案如实记录原文件内存成本，不能验收为恒定缓冲流式方案。
8. 静态检查、依赖开关与接口核对完成后，按另行授权进行编译/链接及实际声音验收；任何缺项不以局部成功代替。

本轮只提交SPEC与进度；不下载依赖、不修改源码、不编译、不运行、不计算摘要。前一项鼠标枚举任务的build.bat授权不自动沿用。

## 10. 待确认

1. 是否允许引入SDL3_mixer 3.2.4及必要MP3/Vorbis解码依赖，并采用本稿的接口与三模式细则。
2. OGG是否仅需Ogg Vorbis；如需Ogg Opus，应同步增加对应解码依赖与验收。
3. 是否接受首版“原文件缓存、按需解码”的内存策略；若要求真正流式读取，在实施前调整第3.2节。
4. 确认建议的资源根目录、循环默认值和GC停止规则；这些尚未由用户单独指定。
