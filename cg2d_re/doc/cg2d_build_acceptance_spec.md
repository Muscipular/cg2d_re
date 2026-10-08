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
