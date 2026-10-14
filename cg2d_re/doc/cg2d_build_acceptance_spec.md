# 当前项目编译与链接（BWZ01 / CAA01）

## 授权与范围

2026-10-08 用户明确要求“编译当前项目”。本次对当前工作线 `cg2d_re` 执行现有 Windows/MSVC/x64 配置及 Debug 构建，覆盖原禁止构建条款；不执行测试、客户端、算法样例或文件摘要。仅使用已有本机工具链和 SDL3，不下载或引入依赖。

## 构建修复决议

首次配置成功，构建在 `LegacyFloatEnvironment.asm` 的 MASM 对象输出失败。同名 CPP/ASM 使 CMake 生成 `$(IntDir)/src/platform/LegacyFloatEnvironment.asm.obj`，而 MASM 执行时目录不存在。通过源文件 `VS_SETTINGS` 将该 ASM 的 `ObjectFileName` 设为 `$(IntDir)%(Filename).asm.obj`，保持独立对象名和原汇编正文；不修改生成项目，不依赖手工创建目录。

后续以实际编译诊断为依据修复必要的声明、类型或汇编语法，保留现有算法、字段宽度、调用顺序及已批准差异；每项修复与最终结果在本文件和唯一进度记录。

第二轮确认 MASM 输出路径已修复，随后 `mov ch, byte ptr [r9+5]` 报 A2218：REX 寻址不能与高字节寄存器 CH 同用。改为一次 `mov al, byte ptr [r9+5]` 后 `mov ch, al`，两条 MOV 均不改标志，保持原单 U8 取样和后续 CH/CL 分类运算。AL 在分类计算前本来即被 CH 覆盖，不增加读取或改变发布字段。

第三轮的必要声明和接线修复采用现有提供者：

- `LegacyException.cpp` 引入已有 `LegacyRuntimeError.hpp`；`SceneState.cpp` 引入已有 `LoginSceneDependencies.hpp`，不复制声明或增加实现。
- `MapLegacyString_543AE8`、`MapLegacyFileError_542F44` 的现正文移出匿名命名空间，匹配已有公开声明；各自私有工具/表及其余正文保留。
- 旧 `Stringify_52D2D0` 引用对齐已有 `EscapeLegacyTextAndNotify_52D2D0`；网络空叶对齐已有 `cg::NullSub_509080`，不生成假体或改协议。
- 战斗代码的 `SprimaryGraphic` 更正为已有生产变量 `primaryGraphic`；动作52的两宠物位置调用明确借用现 `world::detail::GetBattlePetPosition_405B40`。
- 窗口读取 `gValue9A6E0C` 对齐现 `world` 唯一存储；两处动画图号指针显式视为对应 S32 类型，保持现 U32 owner、W32 位样本和原消费者，不新建影子状态。

第四轮和一次全量重建确认 C++/MASM 已过，客户端还有10个链接符号缺口。必要的链接符号诊断证明其中两个已有正文：core导出的 `MeasureDefaultText_44B260` 指针参数为 `QEBD`、`Sub_451350` 为 `QEAUtagRECT`，调用声明要求 `PEBD`/`PEAUtagRECT`。这两个定义没有同签名前置声明，现 MSVC 对顶层 const 指针形参生成了不同修饰名。将两定义的指针形参去掉顶层 const，与现声明一致；不改变指向对象的 const、算法、读取宽度或调用。

其余八项中，五根（458AE0、454C30、454EE0、50E680、52C3D0）属于具名暂停音频；553000旧C++90槽表没有启用定义，CA0478目录和CB1D1C名单的总容量/生产尚无已证事实。必须按原有 SPEC 完成实际提供者后才可链接；不能凭编译授权解除音频暂停、猜数组长度、补零表或用固定返回假体填缺口。

## 验收条件

现有 `windows-msvc-x64` 配置返回零；`windows-msvc-x64-debug` 完成 core、startup、render 三库及真实 `cg2d_re_client` 的编译和链接，构建返回零。保留完整构建日志和仍存在的诊断，不将编译成功当作原新行为、机器 ABI、运行或整体 SPEC 验收。

## BWZ01 阶段结果（历史）

配置成功，当前目标列入的全部C++/MASM源已编译；含一次全量重建。最终单独构建 core、startup、render 三库返回0，客户端链接返回1（LNK1120，八个未解析符号）。两已有正文的修饰名缺口已实际消除，全部编译错误已修复；客户端尚未生成可交付EXE，本次整项目验收未通过。

| 剩余符号 | 现有范围/合同 | 必要动作 |
|---|---|---|
| `Sub_458AE0`、`InitializeMusic_454C30`、`Sub_454EE0`、`Sub_50E680`、`Sub_52C3D0` | [音频暂停根](cg2d_audio_paused_identity_spec.md)，含能力/初始化/关闭、murD回复、音量应用 | 待用户恢复必要音频范围后实现实际提供者/依赖；原网络格式和明文保持 |
| `gLegacyCppInitializationTable553000` | [标准流宿主提供者](cg2d_sdk_standard_stream_owners_spec.md)：原90槽分为70源投影、15 SDK、null1、音频暂停2、键盘排除2；旧实际表未启用 | 沿宿主生命周期合同完成旧入口边界，不补零表或重复宿主构造 |
| `gLegacyPetSkillCatalogCA0478` | [旧目录生产](cg2d_pet_catalog_production_spec.md)：总容量/运行生产未证 | 核真实原生产和容量后定义唯一存储；不可猜长度或按消费补零 |
| `gLegacyTradeTargetNamesCB1D1C` | [旧交易名单生产](cg2d_trade_target_production_spec.md)：总容量/运行生产未证 | 核真实原生产和容量后定义唯一存储；不可借现代十项名单 |

[构建记录](cg2d_build_acceptance_evidence.json)保存环境、命令/返回码、全量重建诊断、最终未解析符号及本步源码前后差异。原始完整日志仅保存在本地构建目录，不作为已验收运行证据。

## CAA01 阶段结果（2026-10-08）

用户随后要求“先完成客户端链接”“用sdl播音频”，已恢复必要五根音频开发和构建。[CAA01 SPEC](cg2d_client_link_sdl_audio_spec.md)明确SDL实际提供者与独立归档成员边界，替代上节“待恢复”的当前状态，BWZ01失败记录仍保留为历史。

现有Windows/MSVC/x64 Debug配置、core/startup/render三库及真实客户端链接均返回0，八个缺口不再出现在当前客户端链接。第5次全量重建和最后第7次增量构建通过，产物为 `build/windows-msvc-x64/Debug/cg2d_re_client.exe`。保留131条全量编译警告，未新增音乐后端/提供者警告；不以零链接错误代音频效果、完整启动退出或整体SPEC通过。

原553000表与CA0478/CB1D1C生产容量仍按各合同保留：旧入口与八个窗口正文/状态迁入独立静态库成员，正常客户端未抽取它们，未定义假数据或取消原功能。实际源码/构建/map证据见[CAA01记录](cg2d_client_link_sdl_audio_evidence.json)。未运行客户端、测试或样例，未计算摘要；V02完整任务仍open。

<a id="gbk-songti-debug-build"></a>

## CAB04 GBK、宋体与ASCII光标编译复验（2026-10-09）

用户要求“编译项目”，本次复用现有Windows/MSVC/x64配置，执行 `cmake --build --preset windows-msvc-x64-debug --parallel 8`。源码为提交 `b27173e4` / 快照 `cg2d-re-big5-to-gbk-20261009`，core/startup/render三库与真实客户端编译链接全部通过，退出码0；无需新增源码修复。产物 `build/windows-msvc-x64/Debug/cg2d_re_client.exe` 为7980032B，完整日志 `build/windows-msvc-x64/build-gbk-songti-20261009.log`。

本次增量构建记录0错误、100条既有源码警告（C4244/C4701/C4996/C4456/C4703），没有文字转码、Font.cpp或InputCaret.cpp相关诊断；增量警告数不作为全量警告总数。构建证据记入[文字更新记录](cg2d_big5_to_gbk_evidence.json)及登记簿build_validation；未重新配置、运行客户端、执行测试/算法样例或计算摘要，V02、文字画面及整体SPEC验收保持未关闭。

## CAB05 本机明文连接修复编译（2026-10-09）

按本次连接问题及既有编译授权，复用同一Debug预设编译NetworkRuntime.cpp；三库及真实客户端链接全部返回0，0错误、3条既有C4244警告，未重新配置。产物仍为7980032B，日志build/windows-msvc-x64/build-plaintext-login-20261009.log；游戏目录EXE已更新且旧版已备份。具体源差异与收发合同见[当前源码状态](cg2d_network_compatibility_spec.md#local-server-plaintext-source-state)及[CAB05记录](cg2d_local_plaintext_login_evidence.json)。未运行客户端或测试；登录互通、V02/V03及整体SPEC仍未验收。

<a id="current-audio-debug-build"></a>
## CAB15 当前音频及客户端 Debug 构建（2026-10-09）

沿AGENTS第12条既有CAA01相关构建授权，执行现有 `cmake --build --preset windows-msvc-x64-debug --target cg2d_re_client cg2d_re_midi_to_wav --parallel 4`，覆盖CAB06–14当前累计源码及必要依赖。首次构建因CMakeLists变化自动再配置/生成成功，随后LegacyWavesReverb.cpp缺少系统WAV格式声明产生13项错误；仅在Windows.h后补mmsystem.h、mmreg.h，保持PCM、效果参数、线程及资源正文，不引入依赖或修改生成项目。第二轮三库、真实客户端及独立离线转换工具编译链接全部返回0，客户端完整链接成功，13项错误全部消失。

最终日志 `build/windows-msvc-x64/build-current-audio-20261009-final.log` 记录0错误、43条WindowRuntime.cpp既有源码警告：C4244×20、C4456×2、C4701×20、C4703×1；新音频提供者和离线工具无诊断。首轮另有95条警告，两轮都是实际增量输出，不代表全量工程警告总数，尚未逐项完成原行为/ABI验收；不以补默认值、钳制或抑制警告降低标准。

客户端MAP存在实际混响、流式BGM、共同PCM头、两设置页和两摆摊回调符号；LegacyMidiSynth/MidiToWav未进入客户端，转换器保持独立目标。客户端8024064B，转换器780800B。游戏目录 `H:/Games/cgmsv_21.2a/CrossGate7.1.0/cg2d_re_client.exe` 已更新，旧7980032B版本备份为同目录 `cg2d_re_client.exe.before-current-audio-20261009`；只核复制后长度，未计算摘要或逐字节比对。

[CAB15记录](cg2d_current_audio_build_evidence.json)保存两轮实际诊断、修复、符号和部署事实，源码快照 `cg2d-re-current-audio-build-20261009`。未运行客户端/转换器、测试或算法，未重生成WAV、计算摘要。当前编译链接通过替代CAB05历史快照作为最新构建状态；实际IP2登录、系统效果创建/格式/声音、全部失败/正常寿命、V02/V05及整体SPEC仍未验收。

<a id="audio-table-x87-debug-build"></a>
## CAB16 声像音量表当前编译（2026-10-09）

沿同一CAA01音频必要构建授权，执行 `cmake --build --preset windows-msvc-x64-debug --target cg2d_re_client --parallel 4`。CMake因新增实际MASM源自动再配置/生成成功，三库/客户端编译链接全部返回0，0错误/0增量警告；日志 `build/windows-msvc-x64/build-audio-table-x87-20261009.log`，MAP包含Cg2dInitializeSoundTablesX87。仅编译当前数值段与必要依赖，不是全量诊断/机器状态/表值运行验收，CAB15的窗口警告未因此关闭。离线工具本步未构建，最近通过仍CAB15。

8024064B客户端已更新用户原游戏目录，旧同长版备份cg2d_re_client.exe.before-audio-table-x87-20261009；只核复制成功及长度，不以同长证明同内容、不计算摘要。[CAB16记录](cg2d_audio_table_x87_evidence.json)与[当前源码](cg2d_audio_update.md#audio-table-x87-source-state)保存边界，未运行客户端/工具/算法、测试或重生成WAV；IP2登录、真实音频/正常寿命及V02/整体SPEC仍未验收。

<a id="fixed-volume-x87-debug-build"></a>
## CAB17 固定音乐表当前编译（2026-10-09）

沿同一CAA01相关音频构建授权，执行 `cmake --build --preset windows-msvc-x64-debug --target cg2d_re_client --parallel 4`。本步只编译既有LegacyAudioMath.asm/LegacyAudio.cpp及必要依赖，无需再配置；三库与真实客户端链接全部返回0，0错误/0增量警告，日志 `build/windows-msvc-x64/build-fixed-volume-x87-20261009.log`。MAP含Cg2dInitializeFixedMusicVolumeX87及既有普通表接口，客户端仍未链接离线合成/转换代码。

8024064B客户端已更新原游戏目录，旧同长版备份cg2d_re_client.exe.before-fixed-volume-x87-20261009；复制成功/长度只证明部署操作，未计算摘要或比对内容。详见[源码](cg2d_client_link_sdl_audio_spec.md#fixed-volume-x87-source-state)及[记录](cg2d_fixed_volume_x87_evidence.json)。本步未运行客户端/算法、测试、重生成WAV或构建离线工具；该工具最近通过仍CAB15。0增量诊断不关闭旧窗口警告，0槽/全表、机器状态/CW、声音/正常寿命与IP2登录及整体SPEC仍未验收。

<a id="spatial-sound-x87-debug-build"></a>
## CAB18 空间音效数值段当前编译（2026-10-09）

沿同一CAA01相关构建授权执行现有Debug客户端目标，既有LegacyAudioMath.asm/LegacyAudio.cpp与必要依赖编译，三库/真实客户端链接返回0，0错误/0增量警告，无需再配置。日志build/windows-msvc-x64/build-spatial-sound-x87-20261009.log；MAP含两空间音效数值接口、52C9E0/52CD60及既有两表接口，客户端仍未链接离线合成/转换代码。本步0输出不是全量警告总数，旧窗口诊断仍待原行为/ABI验收。

8024064B客户端已更新原游戏目录，旧同长版备份cg2d_re_client.exe.before-spatial-sound-x87-20261009；只核复制成功/长度，不计算摘要或比对内容。[记录](cg2d_spatial_sound_x87_evidence.json)和[源码状态](cg2d_audio_update.md#spatial-sound-x87-source-state)保留边界：未运行客户端/算法、测试、重生成WAV或构建离线工具，后者最近通过仍CAB15。编译不代完整数值/机器状态/CW/声音/正常寿命或IP2登录与整体SPEC验收。

<a id="spatial-projection-x87-debug-build"></a>
## CAB19 空间音频记录数值段当前编译（2026-10-09）

沿同一CAA01音频必要构建授权，现有Debug客户端目标编译LegacyAudioMath.asm/LegacyAudio.cpp与必要依赖，三库/真实客户端链接返回0，无再配置，0错误/0增量警告。日志build/windows-msvc-x64/build-spatial-projection-x87-20261009.log；MAP含两新接口、52CDE0/50E6C0和既有两距离接口，运行时仍无离线合成/转换代码。增量0不代全量警告统计，旧窗口诊断未关闭。

8024064B客户端已更新原游戏目录，旧同长版备份cg2d_re_client.exe.before-spatial-projection-x87-20261009；只核复制成功/长度，不计算摘要或比对内容。[记录](cg2d_spatial_projection_x87_evidence.json)和[源码状态](cg2d_audio_update.md#spatial-projection-x87-source-state)限定当前证据；未运行客户端/算法、测试、重生成WAV或构建离线工具，后者最近通过仍CAB15。全部投影/CW/机器状态、声音/失败/正常寿命和IP2登录/整体SPEC仍未验收。

<a id="projection-x87-debug-build"></a>
## CAB20 世界投影音频依赖当前编译（2026-10-09）

沿同一CAA01必要音频依赖授权执行现有Debug客户端目标，既有MapEffectMath.asm/SceneState.cpp与必要依赖编译，三库/真实客户端链接返回0，无再配置，0错误/0增量警告。日志build/windows-msvc-x64/build-projection-x87-20261009.log，MAP有正反两个新接口、两原公开投影及已接音频生产者，运行时仍无离线合成/转换代码。0增量输出不是全量警告统计，旧窗口诊断不关闭。

8024064B客户端已更新原游戏目录，旧同长版备份cg2d_re_client.exe.before-projection-x87-20261009；只核复制成功/长度，不做摘要/内容比对。[源码](cg2d_projection_sampling_return_spec.md#projection-x87-source-state)与[记录](cg2d_projection_x87_evidence.json)保留范围：未运行客户端/算法、测试、重生成WAV或构建离线工具，后者最近通过仍CAB15。全CW上游/机器ABI、全部调用父/别名寿命、画面/声音和IP2登录及整体SPEC仍未验收。

<a id="bgm-config-scan-debug-build"></a>
## CAB21 BGM 配置扫描当前编译（2026-10-09）

沿同一CAA01音频必要授权，现有Debug客户端目标编译LegacyAudio.cpp/必要依赖，三库/客户端返回0，无再配置，0错误/0增量警告。日志build/windows-msvc-x64/build-bgm-config-scan-20261009.log；MAP含LoadBgmConfig_524060、ScanLegacyString_53CADC和ScanLegacyInput_53DEF4，客户端仍无离线合成/转换代码。旧窗口诊断不因本步增量0关闭。

8023040B新版已更新原游戏目录，旧8024064B备份cg2d_re_client.exe.before-bgm-config-scan-20261009；只核实际复制成功/长度，不计算摘要/内容比对。[记录](cg2d_bgm_config_scan_evidence.json)和[源码](cg2d_audio_update.md#bgm-config-scan-source-state)不代配置/扫描全输入、失败/声音/正常寿命/机器ABI或IP2登录与整体SPEC；未运行客户端/算法、测试、重生成WAV或构建离线工具，后者最近通过仍CAB15。

<a id="se-property-sampling-debug-build"></a>
## CAB22 直接音效属性当前编译（2026-10-09）

沿同一CAA01授权编译现有Debug客户端目标，LegacyAudio.cpp及必要三库/客户端返回0，无再配置，0错误/0增量警告。日志build/windows-msvc-x64/build-se-property-sampling-20261009.log，MAP含PlaySe_52C450及SetChannelVolume/Pan/Frequency，运行时仍无离线合成/转换代码；0增量不消除旧窗口诊断。

8023040B客户端已更新游戏目录，旧同长版备份cg2d_re_client.exe.before-se-property-sampling-20261009，仅核复制/长度，不作摘要/内容比对。[源序合同](cg2d_audio_update.md#se-property-sampling-source-state)和[记录](cg2d_se_property_sampling_evidence.json)不代完整父/实际声音/失败/正常寿命/机器ABI或IP2及整体SPEC；未运行客户端/算法、测试/工具、重生成WAV，离线最近构建仍CAB15。

<a id="se-request-properties-debug-build"></a>
## CAB23 请求音效属性当前编译（2026-10-09）

沿同一CAA01必要音频授权编译现有Debug客户端目标，LegacyAudio.cpp与必要三库/客户端返回0，无再配置，0错误/0增量警告。日志build/windows-msvc-x64/build-se-request-properties-20261009.log，MAP含两父/主循环及三个属性/播放提供者，仍无离线合成/转换代码；旧窗口诊断不因增量0关闭。

8023040B游戏客户端已更新，旧同长版备份cg2d_re_client.exe.before-se-request-properties-20261009，仅核复制/长度，不作摘要/内容比对。[原正常分支/取样](cg2d_audio_update.md#se-request-properties-source-state)与[记录](cg2d_se_request_properties_evidence.json)不代完整数值/正常资源寿命/声音/失败/机器ABI、旧外层/IP2和整体SPEC。未运行客户端/算法/工具、测试、重生成WAV；离线最近构建仍CAB15。

<a id="audio-loader-publication-debug-build"></a>
## CAB24 音频初始化当前编译（2026-10-09）

沿同一CAA01必要授权编译现有Debug客户端，LegacyAudio.cpp与三库/客户端返回0，无再配置，0错误/0增量警告。日志build/windows-msvc-x64/build-audio-loader-publication-20261009.log，MAP含Sub_524230及FormatLegacyString_53B304/FormatLegacyOutput_53EB28，客户端仍无离线合成/转换代码；旧窗口诊断不因增量0关闭。

8022528B新版已更新游戏目录，旧8023040B备份cg2d_re_client.exe.before-audio-loader-publication-20261009，仅核复制/长度，不作摘要/内容比对。[原路径/取样发布](cg2d_audio_update.md#audio-loader-publication-source-state)与[记录](cg2d_audio_loader_publication_evidence.json)限定运行尾窗/三格式点；完整父/格式/SDK失败/正常资源寿命/声音/机器ABI、旧外层/IP2和整体SPEC未验。未运行客户端/算法/工具、测试、重生成WAV，离线最近构建仍CAB15。

<a id="local-login-diagnostics-debug-build"></a>
## CAB26 本机连接诊断当前编译（2026-10-09）

沿既有BWZ01客户端连接授权，现有Debug目标编译LoginState.cpp/NetworkRuntime.cpp和必要依赖，三库/客户端返回0，无再配置，0错误、5项既有警告：2原inet_addr/gethostbyname弃用、3原派发回调宽度转换；不抑制、改契约或关闭原审查。日志build/windows-msvc-x64/build-local-login-diagnostics-20261009.log，MAP含Sub508360/508800/507DF0和日志，客户端仍无离线合成/转换代码。

8022528B新版已更新游戏目录，旧同长版备份cg2d_re_client.exe.before-local-login-diagnostics-20261009，只核复制/长度。新增[阶段日志](cg2d_network_compatibility_spec.md#local-login-diagnostics-source-state)和[证据](cg2d_local_login_diagnostics_evidence.json)限定额外诊断I/O与原逻辑保持；无客户端/试包/算法/测试/摘要运行，IP2互通/全网络/SDK失败/正常寿命/机器ABI和整体SPEC未验，旧窗口诊断保持。离线工具本步未编译/执行，最近通过仍CAB15。

<a id="dispatch-callback-width-debug-build"></a>
## CAB27 回调宽度当前编译（2026-10-09）

沿BWZ01既有客户端连接授权编译现有Debug目标，无再配置，三库/真实客户端返回0、0错误；NetworkRuntime及三个本步回调宽度C4244为0。头文件传播带来的其它重编源实际97警告：C4244共23、C4456共9、C4701共58、C4703共5、C4996共2，分别LegacyUiRecord11/LoginState2/YobiRuntime41/WindowRuntime43，均不因本步关闭或抑制。日志build/windows-msvc-x64/build-dispatch-callback-width-20261009.log，MAP含三个回调、52CDE0及连接循环/whV，仍无离线合成/转换代码。

8022528B新版已复制游戏目录，旧同长版备份cg2d_re_client.exe.before-dispatch-callback-width-20261009，仅核复制/长度。[入口宽度](cg2d_network_compatibility_spec.md#dispatch-callback-width-source-state)与[原证/静态及构建](cg2d_dispatch_callback_width_evidence.json)只证明本步边界。未运行客户端/试包/测试/算法/工具、重生成WAV或摘要；IP2互通、全部资源/正常寿命/机器ABI及整体SPEC仍未验，离线最近通过仍CAB15。

<a id="panel-local-value-debug-build"></a>
## CAB28 面板局部值当前编译（2026-10-09）

沿既有CAA01客户端/按钮音频承载链，现Debug仅重编LegacyUiRecord.cpp并链接三库/客户端，返回0，无再配置，0错误/0增量警告。该源原三C4244/八C4701已消除；CAB27其它86项（LoginState2/YobiRuntime41/WindowRuntime43）未重编核销，不称全项目无警告。日志build/windows-msvc-x64/build-panel-local-value-20261009.log，MAP七父、PlaySe52C450/5042D0及DrawText456780存在，仍无离线合成/转换代码。

8022528B新版已复制游戏目录，旧同长版备份cg2d_re_client.exe.before-panel-local-value-20261009，只核复制/长度。[局部源码](ui_windows/shared_legacy_panels.md#panel-local-value-source-state)与[证据](cg2d_panel_local_value_evidence.json)不代整父/callee/旧表、正常资源寿命/机器ABI/画面或声音与IP2互通。未运行客户端/试包/测试/算法/工具、重生成WAV或摘要；离线最近通过仍CAB15，整体未验。

<a id="login-receive-diagnostics-debug-build"></a>
## CAB29 登录接收诊断当前编译（2026-10-09）

沿BWZ01既有客户端连接授权，现Debug编译三文件及头文件传播的必要依赖，三库/客户端返回0，无再配置，0错误、87其它源警告：LoginState2/YobiRuntime41/WindowRuntime43/MainLoopSupport1（GetVersionExA弃用新重编暴露）；NetworkRuntime/新诊断及已修LegacyUiRecord为0，不关闭或抑制其余项。日志build/windows-msvc-x64/build-login-receive-diagnostics-20261009.log，MAP含512800/508360/507DF0/512E40/5081E0及whV，仍无离线合成/转换代码。

8022528B新版已复制游戏目录、旧同长版备份cg2d_re_client.exe.before-login-receive-diagnostics-20261009，仅核复制/长度。[接收诊断](cg2d_network_compatibility_spec.md#login-receive-diagnostics-source-state)与[证据](cg2d_login_receive_diagnostics_evidence.json)只证明额外日志和名称纠正、原字段/决策/明文保持。未运行客户端/试包/测试/算法/工具、重生成WAV或摘要；运行服务端身份/IP2互通、完整父-callee/正常寿命/机器ABI、声音与画面及整体仍未验，离线最近通过仍CAB15。

<a id="window-draw-width-debug-build"></a>
## CAB30 窗口正常绘制宽度当前编译（2026-10-09）

沿CAA01既有客户端/按钮音频所需窗口承载，现Debug仅重编WindowRuntime.cpp并链接三库/真实客户端，返回0，无再配置，0错误。20 C4244已消除，仍23其它窗口诊断（两C4456、二十C4701、一C4703），此前未重编其它文件44项不核销；已见总67，不称全工程无警告。日志build/windows-msvc-x64/build-window-draw-width-20261009.log，MAP十父及4771B0/520970/456780存在，仍无离线合成/转换代码。

8022528B新版已复制游戏目录，旧同长版备份cg2d_re_client.exe.before-window-draw-width-20261009，仅核复制/长度。[正常消费宽度](cg2d_window_routing_sampling_spec.md#window-draw-width-source-state)与[有限原证/实现](cg2d_window_draw_width_evidence.json)不代全父-callee/旧99路由、正常寿命/资源/机器ABI/声音/画面或IP2实际互通。未运行客户端/试包/测试/算法/工具、重生成WAV或摘要；离线最近通过仍CAB15，整体未验。

<a id="shop-inventory-scan-debug-build"></a>
## CAB31 商店按钮命中承接当前编译（2026-10-09）

沿CAA01既有客户端/按钮音频所需窗口承载，现Debug仅重编WindowRuntime.cpp并链接三库/真实客户端，返回0，无再配置，0错误。Sub_4BC660的scanSlot C4701消除；仍22窗口警告（C4456两/C4701十九/C4703一），此前其它44项不核销，已见总66。日志build/windows-msvc-x64/build-shop-inventory-scan-20261009.log，MAP4BC660/52C450/53B304提供者存在，未增离线合成/转换代码，不称全工程无警告。

8022528B新版已复制游戏目录，旧同长版备份cg2d_re_client.exe.before-shop-inventory-scan-20261009，仅核复制/长度。[一个源码片段](ui_windows/window_53.md#shop-inventory-scan-source-state)和[局部原证](cg2d_shop_inventory_scan_evidence.json)不代全父-callee/旧99路由、正常寿命/资源/机器ABI/画面音效/IP2互通或整体通过。未运行客户端/试包/测试/算法/工具、重生成WAV或摘要，离线最近通过仍CAB15。

<a id="array-assert-stack-diagnostic-20261009"></a>
## CAC01 选择人物后数组断言诊断版（2026-10-09）

用户截图显示游戏目录cg2d_re_client.exe在MSVC array:531触发array subscript out of range。新的游戏systemlog指定ASCII标签确认CONNECT OK→LOGIN QUEUE RESULT=0→SENT BYTES=63→RECEIVED BYTES=13 QUEUE=0→LOGIN REPLY RESULT=0→DISPATCH RESULT=0，随后第二次加载；这只证明本次ClientLogin往返成功，不代表角色入场或完整网络已通过。查询报错进程已不在运行，新客户端没有现成转储，具体数组/下标/调用点仍待栈，不凭截图猜修复或关闭STL断言。

仅ClientEntry.cpp增加Debug私有CRT报告回调及WinMain寿命登记：实际_RPTF0走_CrtDbgReport，SDK在回调返回0时继续原报告；回调只处理_CRT_ASSERT，捕获最多64个地址、按模块记录RVA到cg2d_assert_stack.txt，哈希输出参数为nullptr，不记录身份/正文。诊断使用原生Win32文件/模块API与宿主snprintf，不借业务数组、旧格式器或日志owner；不加载符号/依赖、运行算法或更改断言返回值，保存恢复GetLastError。Release不编译/安装该回调，原WinMain业务正文保持。额外Debug诊断文件I/O为定位本次正常操作故障的具名差异，不当越界专项模拟/钳制/默认值或原功能修复。

首次编译报报告宏名称错误，按当前SDK实际_CRT_RPTHOOK_INSTALL/REMOVE纠正后，现x64 Debug仅重编ClientEntry.cpp并链接三库/客户端，返回0、无新增警告；未生成构建日志文件。MAP记录真实_CrtSetReportHook2/UCRT与RtlCaptureStackBackTrace/Kernel32提供者。8026112B新版已复制游戏目录，旧8022528B备份cg2d_re_client.exe.before-array-assert-stack-20261009，只核复制/长度；未由本线程启动客户端、测试/算法/离线工具、重生成WAV或计算摘要。源码/链接完成不证明实际回调已触发。

已请求用户使用原批处理再次选人，自动栈文件仍待本次复现。角色入场R记录失败且不归因未定位的具体函数，V03/V04及完整门禁保持open；原格式明文/LF、GBK/宋体/ASCII光标与SDL3/WAV不变，整体目标继续。固定快照cg2d-re-array-assert-stack-20261009；待真实RVA后修实际索引/容量/接线，不将诊断版称为修复。


<a id="title-none-alias-debug-build-20261009"></a>
## CAC03 选人后称号读取修复版（2026-10-09）

用户提供真实cg2d_assert_stack.txt。重编前用本地DbgHelp离线读取CAC01的EXE/PDB，仅主模块RVA转换为首选基址；返回地址-1定位：12E0FA→TitleRecord64::operator[]/array:531，24D822→BorrowPlayerDisplayText/SceneState:8886，1D7185→442F70/YobiRuntime:6140，后续4417E0/YobiUpdateAll/DrawNormalScene/World::Update/MainLoop/WinMain。未运行或附加客户端。当前ClientLogin成功已证明，角色入场旧失败仍保留，栈未提供下标数值。

修复[已证正常-1字节别名](cg2d_yobi_runtime.md#title-none-alias-20261009)：新增私有BorrowTitleText，模式3与474FB0/478010两页面共用，原FF04F4借用第二角色记录+80h；保内容/owner、页面取样序和其它索引，不扩表、补空串/清尾部、关闭断言或添泛化保护。三源码与HEAD+明确三组修改全文一致，13增3删及UTF-8无BOM/LF、原地址算式静态核对通过。

现有windows-msvc-x64-debug客户端目标及三库返回0，无再配置；头文件传播重编产生63项既有Yobi/Window警告（C4456九、C4701四十九、C4703五），新helper无诊断，不称全工程零警告。未生成构建日志，MAP含真实BorrowTitleText。8026112B新版已直接复制游戏目录，按用户本次要求无需备份，只核复制/长度；未启动客户端/试包、测试/算法/离线工具或计算摘要。单独构建不记录日志/进度及不自动提交的偏好也写入AGENTS，当前记录属于源码修复开发步。

已更新有限源级I/S及原R失败的定位信息，未关闭V03/V04或任何完整功能门禁。已请求用户手动复验新版入场；整体SPEC、完整互通/资源/正常寿命/画面与声音仍未验。命名快照cg2d-re-title-none-alias-20261009。


<a id="character-entry-confirmed-20261009"></a>
## CAC04 用户确认选人入场正常（2026-10-09）

CAC03修复版已覆盖H:/Games/cgmsv_21.2a/CrossGate7.1.0/cg2d_re_client.exe，源码提交8d93c8d5/快照cg2d-re-title-none-alias-20261009。本线程随后请求用户重新运行cg2a - 2.bat并选择人物，用户回复原文“已正常”；结合该复验请求，确认这次修复后的选人入场及原断言故障复验通过。证据来源为用户手动运行反馈，不是本线程自动采样，也不宣称取得了下标数值或完整场景轨迹。

追加限定FUNC-CHARACTER/R通过记录，原CAC01失败和真实断言栈保留；本次故障的待验状态关闭。两称号页面、其它账号/角色/称号、全部网络与场景/资源/声音/生命周期仍按各自合同验收，完整功能门禁和整体SPEC不关闭，客户端运行权限不自动恢复。仅更新文档/登记/派生与进度，不修改源码、构建、部署、运行测试/客户端或计算摘要。

<a id="unicode-build-fixes-20261014"></a>
## UCB01 Unicode迁移编译错误修复（2026-10-14）

用户明确要求使用build.bat修复编译错误，本轮允许按脚本现有MSVC x64预设配置、编译并链接客户端；显式指定Debug避免交互暂停。先按真实诊断修正宽窄类型、接口选择、声明及链接问题，再重复构建确认；沿已批准UTL02动态UTF-16正文和GBK协议／存档边界，不通过强转或宽窄万能重载掩盖错误，不变更原地址／业务合同，不引入依赖。

本轮只恢复必要构建验证，不运行测试、客户端、算法或计算摘要，不自动部署游戏目录。源码修复同步唯一进度与关联台账；构建通过仅证明该配置编译／链接，不升级完整功能或运行验收。

本次实际结果：首轮63条编译诊断，修正宽窄实参、动态分行、七组快捷键表容量、回调签名、format声明、FindResourceW／WNDCLASSW后客户端链接通过。用户追加中文宽文案、去中间窄往返及InputContext直接宽接口后再次执行build.bat Debug；最终退出0，三个库与cg2d_re_client.exe构建成功。宽接口后续的一处全局命名空间遗漏已由诊断定位修正。过程仍有既有未用变量、可能未初始化及API弃用警告，不称零警告。日志位于忽略的build目录，证据引用见Unicode证据文档；未运行测试／客户端、部署或计算摘要，完整门禁保持。
