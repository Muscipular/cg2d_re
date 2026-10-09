# CAA01 客户端链接与 SDL 音频依赖

> CAB06（2026-10-09）：用户随后要求“继续完善音频部分”，剩余音频开发已恢复；[旧设置五回调](cg2d_display_settings.md#audio-settings-callback-source-state)完成源码，本页“限五入口”只描述CAA01链接阶段。SDL3/WAV及空音频选择不变；其余本体/接线/运行门禁按实际推进，不以恢复范围计为完成。

> CAB02（2026-10-08最新要求）：用户改为把固定 MIDI 离线转换为 WAV，客户端后续只播放 WAV。本决议覆盖下文 CAA01 的运行时 gm.dls/MIDI 合成选择；原证据与已完成构建作为历史保留。完整 1204B 资源预渲染为 13.8 秒 WAV，嵌入客户端资源，与普通 BGM/64 音效通道独立持有。原16B资源不含完整音轨；用户随后指定第二段视为空音频，类型1停止旧播放后立即成功1，对应零时长空WAV。保留类型/重复W32、0为单次、0xFFFFFFFF为无限重复、音量表、停止/清理和原明文派发合同。离线转换用现有合成代码与本机gm.dls，无外部库；客户端不再链接合成代码或读取gm.dls。音源原效果/听感对照保留在WAV质量验收，不要求恢复运行时MIDI。只允许本次必要转换及编译/链接，不运行客户端或桌面操作。

<a id="client-link-sdl-audio-decision"></a>
## 范围与裁决

2026-10-08 用户明确要求“先完成客户端链接”，随后指定“用sdl播音频”。恢复八个实际链接缺口所需开发、配置/编译/链接及必要诊断；不运行客户端/测试/样例、不计算摘要。音频恢复限五入口与必要音乐依赖，旧7/旧8完整界面和全音频验收另行推进。沿现有 SDL3 音频设备、WAV/BGM 通道，不引入 DirectMusic/DirectSound 播放或新的外部库。

2026-10-08 用户确认保留现有 `gm.dls + SDL3` 播放链：系统 `gm.dls` 提供音色，项目 `LegacyMidiSynth` 将原 MIDI 事件合成为 PCM，由 `SdlAudioBackend` 的 SDL3 音频流播放。此处“沿用系统 MIDI”明确指沿用系统音色与现有播放链；本次不改用 Windows 自带 MIDI 合成器，不引入 SDL3_mixer 或 TiMidity++。混响、合唱、DLS 动态调制及控制器效果继续按下述缺口推进，实际声音与生命周期仍待验收。

## 宿主入口与链接边界

真实客户端继续由宿主 CRT 进入 WinMain，已批准的有序应用初始化和 SDK 提供者保持；旧 cinit/90槽表不重新启用。实际试验表明，仅函数/数据独立段与删除未引用段仍留下三个旧外部符号，不能依赖该选项。将旧进程入口和旧51/58–61目录消费按自身职责拆成独立静态库成员，保留全部原正文、状态和声明；真实客户只由正常归档器抽取被引用成员，不增加函数禁用宏、删原实现或补空90槽表/猜数组容量。此项是源码归属与宿主入口接线，不宣原端不可达、功能排除或旧生产已验收；U01/U02/C02仍按各原合同验收。若实际客户仍引用该独立成员，继续实现其真实依赖，不用链接忽略错误选项。

## 必要音频合同

原454C30先检查458DC0，随后清理旧音乐状态并初始化音乐后端；454EE0停止音乐、释放音乐持有资源，返回0。原DX/COM创建机制由真实SDL设备/音乐流能力承接，保留独立DINPUT入口观察；成功码600与失败码0/200来源必须是实际结果，不固定成功。

WAV已打开设备时借同一真实SDL设备；WAV资源初始化失败时以局部SDL设备实际探测，探测对象正常释放，再由音乐初始化独立打开实际设备，不用WAV资源失败标志禁止原固定音乐。音乐清理释放音乐流/序列/样本；没有WAV owner时一并关闭设备，有WAV owner时保留至原52B630退出位置。

50E680只消费socket后两个原S32字段：类型<1000转原PlaySe，其他转类型-1000固定音乐；现派发仍传三个数值，第三字段不被本体消费，wire/明文/分发顺序不改。固定音乐455280只有类型0/1：原556744的1204B MIDI、556BF8的16B片段，保留原借用长度和重复W32。先停止并释放旧序列，再选择类型；不得把16B片段自动补成长资源，加载失败返回0；原调用者不消费播放阶段的BOOL，加载成功后仍返回1，不能改成WAV假体或合成演示音。

SDL3音频流消费PCM；MIDI事件在本线转换为PCM后由SDL播放。使用本机系统已有GM DLS样本资源作为默认音色提供者，不下载/复制音色库；实际文件/格式/流建立失败沿明确失败返回，不提供猜测音色。DLS区域/样本、原MIDI时序/速度/音高/通道和重复由音乐后端持有，与既有WAV BGM相互独立；声音波形/效果不宣与旧驱动逐样本等价，实际播放与听感仍待运行验收。[微软默认DLS说明](https://learn.microsoft.com/en-us/windows-hardware/drivers/audio/default-sound-sample-sets)与[SDL流回调合同](https://wiki.libsdl.org/SDL3/SDL_SetAudioStreamGetCallback)只作后端设计依据，不代原程序证据。

本线解析RIFF/DLS的PCM样本、区域/键区/速度区、wsmp循环/音高/衰减与静态EG1连接；SMF 0/1按tempo合并track、保留track终点和running status。64声音、22050Hz、音符开关、通道音量/表情/声像、程序/音高和重复在SDL流锁下持有；流销毁先等待回调退出，再释放序列/样本。原固定0资源为八track/division96、程序1；固定1片段按原16B交解析，不读取其后数据。

局部音频未收口：原KS混响/合唱参数和DLS动态调制尚未逐项承接，现解析器保存MIDI控制器但不宣全部控制器均已影响PCM；GM文件实际加载、设备/声音、硬件失败/正常清理与原端差异未运行验证。保留这些开发/验收缺口，不把本次固定音乐基础播放源或客户端链接当完整音乐链/全音频验收。初始化SDL设备不可用返回1，GM资源/格式/音乐流建立失败返回2，成功返回0；原COM私有2..8阶段差异是本次SDL后端裁决，不伪造相应旧COM成功。

52C3D0沿原音频失败标志、当前BGM索引、音乐enable、原0..15钳制与同一记录音量/同一128槽表，仅在实际BGM缓冲存在时应用音量；不得把固定MIDI与普通WAV BGM当前索引合并。

<a id="client-link-sdl-audio-acceptance"></a>
## 本次完成标准

五入口均接真实SDL能力、初始化/清理、原回复分流和音量控制；客户端Debug配置、编译和链接返回0，八个原缺口不靠假体消除。构建记录保存实际结果，进度/登记/生成看板同步；只标客户端链接通过，原CRT/旧目录生产、全音频/运行与整体SPEC继续按独立门禁验收。

<a id="client-link-sdl-audio-source-state"></a>
## 实际交付与剩余（2026-10-08）

| 对照项 | 当前结果 | 尚需验收 |
|---|---|---|
| P11 / 454C30 | 系统GM DLS解析、独立MIDI流和128槽音量表已实现、已链接 | 实际资源加载/设备失败/声音；原效果及动态调制 |
| P12 / 454EE0 | 停止、销毁音乐流、释放序列/样本，按WAV owner决定设备关闭 | 回调/共享设备寿命与实际退出 |
| P13 / 458AE0 | 实际SDL设备能力与独立DINPUT入口观察；非固定成功码 | 实际设备/输入失败分支 |
| P16 / 50E680 | 原两S32字段分流，原固定MIDI字节/长度、重复、停止释放顺序 | 固定资源声音与全部控制器效果；完整murD互通 |
| P17 / 52C3D0 | 同一BGM记录/表、原钳制与真实SDL音量应用 | 实际声音变化与失败返回 |
| 旧三存储链接缺口 | 原旧入口/八窗口迁入独立归档成员，原正文不变；真实客户端未抽取 | U01/U02生产容量、旧CRT完整合同仍未闭 |
| V02链接部分 | 现有Debug配置、三库与真实客户端链接返回0；含一次全量重建 | 完整启动/退出、运行及SDK/ABI门禁仍未验 |

本轮七次构建中第3次首次链接通过；第5次全量重建通过，保留131条原有警告；第7次覆盖最后音乐序列释放改动，返回0。链接map确认五根提供者均来自LegacyAudio成员，WinMain来自ClientEntry；旧入口/目录成员未被抽取。新音乐后端及音频提供者未产生编译警告，不将全项目称为零警告。

[持久证据](cg2d_client_link_sdl_audio_evidence.json)保存原十根785条指令/27页、两原固定资源、源码迁移检查、实际构建结果和必要map文本；本轮未运行客户端/测试/算法样例或计算摘要。提交快照：`cg2d-re-client-link-sdl-audio-20261008`。五pending保留完整功能缺口，源码/链接结果单独登记。

<a id="fixed-music-wav-source-state"></a>
## CAB02 固定音乐 WAV 源码与产物（2026-10-08）

用户最新要求“midi你帮我直接转成wav吧，后续不再使用midi”覆盖同日保留系统音色/运行时合成的选择。客户端只持有内置固定WAV和SDL播放流；LegacyMidiSynth移至src/tools，仅由EXCLUDE_FROM_ALL离线转换目标编译，不链接客户端，也不参与默认构建。

| 对照项 | 本次交付 | 验证与剩余 |
|---|---|---|
| 原556744 / 1204B | 固定0转为src/audio/fixed_0.wav，13.8秒、22050Hz双声道float32，304290帧、2434376B | 实际转换、完整WAV格式/PCM和内置资源字节检查通过；未听感验收 |
| 原556BF8 / 16B | 用户指定为空音频，fixed_1.wav为0帧/56B；类型1停止旧播放后立即返回1 | 零时长成功结束，不启动空流循环；此具名差异覆盖原加载失败0 |
| 原455280 / 50E680 | 固定WAV独立通道；原额外重复W32，0单次、1共两次、全1无限；原音量表与两S32字段分流保持 | 源码分支/所有权检查及Debug链接通过；实际连续播放、通知与murD互通未验 |
| 原454C30 / 454EE0 | RCDATA 1001预载WAV；共享SDL设备，独立固定流/PCM；先销毁流再释放样本 | 内置资源与WAV直接字节一致；不运行客户端，实际设备/回调/退出未验 |
| 客户端MIDI依赖 | 移除原数组、MIDI公开接口、运行时gm.dls读取与合成器链接 | 客户端map无LegacyMidiSynth/MidiToWav，保留普通WAV/BGM及原W32音量表 |

离线转换继承当前合成链，统一增益0.448961385，源峰值2.182815790衰减到0.98；保留相对声部/声像/音量，不做逐样本裁剪或整数位深量化。WAV实际最大绝对值0.980000019、RMS 0.135786881，非有限样本0。原KS混响/合唱及DLS动态调制未因预转换自动等价；这些转为音源质量对照，仍不宣完整声音/听感验收。

本次独立按原MIDI字节核八轨终点：音乐轨均1656tick、tempo800000us、division96，对应13.8秒；CAA01旧static_midi_summary部分轨道终点不准确，本次纠正结论，原字节与历史证据保留。离线实际转换和产物检查属于用户本次明确请求，不解除客户端/桌面运行限制。

必要配置/转换器与客户端Debug编译链接已通过。第一次客户端构建发现FindResourceW使用ANSI资源类型宏，已改为MAKEINTRESOURCEW(10)，未改变全工程字符集；最终构建返回0。原工程警告保留，本步音频与转换器无新增警告。具体结果见[持久证据](cg2d_fixed_music_wav_evidence.json)，提交快照cg2d-re-fixed-music-wav-20261008。P11/P12/P16/V02及完整音频、整体SPEC不因此关闭。

<a id="streaming-bgm-prefill-decision"></a>
## CAB08 流式 BGM 首段预填与循环终点发布（2026-10-09）

当前原会话确认：52C160@52C396先把mmioRead实际PCM字节数发布给103DEA4并启动52BD40；工作线程以游标0预填nAvgBytesPerSec的一秒缓冲，再发布105083C=1。52BE60等创建入口返回后，才在52C049/52C050将原smpl终点乘blockAlign写入同一103DEA4。52BBE0后续填充沿当前终点和103CFB8回到循环起点；首段不提前使用smpl终点，也不因后续发布重新生成已排队PCM。

当前SDL载入此前在预填前即使用smpl终点，与上述正常时序不符。本步沿已批准SDL3机制修正：流式创建先用完整PCM终点、循环起点和同一ready/notice启动现有工作线程；ready后再在流锁下发布循环终点，保留已排队首段与游标。普通BGM、音效及固定WAV仍在绑定时使用自身完整帧数；原smpl终点0不另解释为完整文件，不新加钳制/异常回绕或输入保护。旧DirectSound半缓冲/事件/COM机制仍由真实SDL流/回调/工作线程承接，不伪造原句柄或未实现函数。

同一预填差异还暴露音量应用时点：原52BE60在预填之后设置缓冲音量，作用于播放数据；现有回调先把音量乘进PCM，已排队首段无法响应后来设置。按本地SDL3-3.4.8头文件SDL_audio.h@1229–1231，流增益在取出数据时应用。本步普通BGM和共用音效流改用SDL_SetAudioStreamGain、保留原百分之一分贝换算与调用门槛，删除回调里的重复音量乘法；固定WAV已使用同一流增益，不重复衰减。只改变已有音量后端，不改原控制回调/音量表/声像、清空队列或新增暂停/重启逻辑，实际设备提交延迟仍须运行验收。

52BE60/52C160/52BD40/52BBE0分别关联现有载入父、新私有流式创建适配和现有工作线程/填充回调，只表示当前SDL职责和该正常时序可追踪；原完整调用划分、全部失败残留、运行线程调度、波形与生命周期不以局部静态证据计为验收。采用WAV、第二段空音频、GBK静态文字及原网络明文格式保持；不编译、测试、运行或计算摘要。

<a id="streaming-bgm-prefill-source-state"></a>
## CAB08 实际源码与限定静态结果（2026-10-09）

已在LegacyAudio.cpp加入私有LoadStreamingBgm_52C160：SDL加载完整PCM后先发布实际读取字节数，再启动既有线程并等待ready；LoadAndPlayBGM随后按原blockAlign乘法发布终点，并经SetBgmLoopEnd只更新同一流的循环终点。普通音效及固定WAV共用绑定在回调可消费前显式写自身完整帧数，FeedChannel直接使用已发布终点，后续发布0不解释为全文件。新增接口只传长度值，不暴露SDL指针或新增资源owner。

SetBgmVolume/SetChannelVolume已改用SDL流增益，删除Channel.volume和回调音量乘法；原单位/门槛和声像保持，固定WAV既有增益不重复衰减。本地SDL头文件、三条流增益、无PCM重复音量乘法、无清队列/重置游标及未变正文的新增静态核对通过；实际设备提交延迟和声音变化未运行验收。

当前四根706条1999B已固化在[原指令与源码证据](cg2d_streaming_bgm_prefill_evidence.json)，对应原身份分开登记为SDL职责适配。新增三现源的UTF-8、完整PCM长度→线程→ready→smpl终点→原后续父序、队列/游标保持、普通通道终点及声明/定义限定静态通过；根据HEAD逐片重放新增差异，其他源码正文保持。静态结果不代当前链接、原全部失败/诊断状态、线程调度或波形/停止通知/正常退出验收，FUNC-AUDIO和V05完整门禁保持开放。

固定源码快照cg2d-re-streaming-bgm-prefill-20261009。本步未编译、运行或替换游戏目录程序；当前游戏EXE仍为CAB05网络修复版，真实登录结果待用户反馈。旧99表实际外层来源仍未确定，未以缺少引用判不可达，也未新增轮询或强转原x86地址。剩余12个音频待映射项须按实际职责/代码继续推进，不以范围恢复或局部关联计为完成。

<a id="pcm-wave-header-decision"></a>
## CAB09 共用 PCM WAV 头读取与 SDL 样本职责（2026-10-09）

原524230@52B392及52BE60@52BF3A共用52B710完整114条256B：两个20B MMCKINFO先同源清零/复制，以MMIO_FINDRIFF找WAVE，再在该父块内以MMIO_FINDCHUNK找fmt；向调用者缓冲读取恰好18B、确认W16格式1，mmioAscend成功后找data，最后才写W32长度并返回1。每个API失败/短读/非PCM均返回0；不额外判断fmt大小、预清格式输出或改成16B读取，失败不写长度。两父mmioOpenA均使用原0x10000。

现有两套手写解析分别读16B/18B，且自行扫描块，不能替原共同定位合同。本步还原私有ReadPcmWaveHeader_52B710，使用既有WinMM mmio文件API与原字段/宽度/顺序；一条现有职责内的路径包装供音效/BGM共用，保持本线探测文件使用后关闭的寿命。WinMM仅读取头部，SDL3继续实际样本加载、转换、缓冲创建和播放，不改CMake或引入依赖。原52B880缓冲创建和52B810填充/提交职责关联真实SDL LoadSample/LoadWav/LoadBgmWav，沿CAA01原生机制替换，不补伪造COM对象、锁区或空函数；原完整失败残留、线程/资源/声音与正常退出仍单独验收。

本次只修已发现的共用读取差异及具名来源，静态核对原114条/两caller窗口和现源新增差异；不编译、测试、运行或计算摘要，WAV、第二段空音频、GBK/宋体/ASCII光标与原明文网络格式保持。

<a id="pcm-wave-header-source-state"></a>
## CAB09 实际源码与限定结果（2026-10-09）

LegacyAudio.cpp已用ReadPcmWaveHeader_52B710完整体替换IsPcmWave/ReadBgmWaveDataLength两套手写扫描；ReadPcmWaveDataLength按原MMIO_ALLOCBUF打开、借18B格式缓冲和W32长度调用同一真实入口、使用后关闭HMMIO。音效524230与BGM 52BE60各接一次，SDL样本/流所有权保持。本轮不改SDL提供者或CMake，winmm已有链接；地址按x64拓宽，两个20B块结构和18B格式容量设静态断言。

三个原根186条441B及两caller窗口90条保存在[持久证据](cg2d_pcm_wave_header_evidence.json)。新增源的UTF-8、六失败/唯一成功输出、18B与同一父块、两调用接线、局部文件关闭和现有依赖静态通过；逐片对HEAD重放仅本步差异，其他正文保持。52B710按原头入口纳管，52B810/52B880只按已批准SDL样本/缓冲职责适配纳管，不生成旧COM假体，不以来源对应宣称原全部失败残留/截短PCM/提交返回或声音/寿命已通过。

源码快照cg2d-re-pcm-wave-header-20261009；三项原身份已有具名来源，固定WAV剩余九项继续开发。当前源码未编译或运行，游戏目录仍是CAB05明文连接修复版，真实登录待用户反馈；完整音频、旧页面外层、原生ABI和整体SPEC门禁保持开放。

<a id="fixed-wav-control-decision"></a>
## CAB10 固定 WAV 原控制入口与阶段接线（2026-10-09）

原455280加载成功后先455470设置完整W32额外重复次数，再4553B0播放；播放入口先检查performance/segment、写原W8[62B2DC]=0、455400(127)应用同一62B0C8音量表，再消费实际播放HRESULT符号返回0/1。父不消费重复/播放返回，仍返回1。455450在performance为空时返回0，否则停止且不消费停止HRESULT，返回1；455470在segment为空时返回0，否则设置重复且不消费其HRESULT，返回1。455400按S32 scale*同源音乐音量/15，仅原0..127域取表，不新增钳制或异常模型。

本步按原五入口划分落位SDL适配：显式设置重复与实际播放分开，重复只写当前已准备片段；播放入口清唯一原字节、应用原音量后启动。SDL已初始化固定流与已准备片段分别提供原生存在门槛，不用固定成功码补创建失败；单次/有限/全1无限重复保持。62B2DC当前原会话只查到4553CA一次写0，本轮实读加载值FFH；不把它当播放完成通知，也不借SDL feedFailed或另造消费者。停止/释放/类型选择、类型0准备失败0、类型1空音频立即1和父加载成功后的原返回保持。

原454FE0/455100音色载入下载、455260旧loader清理及455030效果参数属于剩余四个原职责，本步不补COM假体或以WAV播放自动计为完成；音源效果/离线来源和真实资源寿命继续沿既有WAV质量与SDL机制合同推进。只补当前控制源码与接线，不编译、测试、运行或计算摘要，不恢复运行时MIDI或新增依赖。

<a id="fixed-wav-control-source-state"></a>
## CAB10 实际源码与限定结果（2026-10-09）

LegacyAudio.cpp新增原4553B0/455400/455450/455470四个控制适配，455280现按停止→释放→类型/准备→重复→播放入口原序真实接入；原FF初值W8单独承载且只在本播放入口清0，不当作后端通知。SDK值接口增加准备片段查询与重复设置，PlayFixedMusic只启动播放，不覆写此前重复状态。原0..127表、127音量参数、有限/无限重复消费、各阶段返回及第二段空音频保持，SDL实际固定流/样本所有权和后端其余正文未改。

五原根146条512B、原加载字节FF及当前已查引用保存在[持久证据](cg2d_fixed_wav_control_evidence.json)。三个现源的UTF-8、五入口调用序、FF→0写点、S32音量取表、重复设置不启动/播放不重设重复、实际存在门槛/返回分工、空音频和声明/定义静态通过；逐片对HEAD重放仅本步差异，其余正文保持。五原职责按SDL适配纳管，不代原COM私有失败/全部状态、声音质量、设备/资源/正常寿命或ABI验收。

固定源码快照cg2d-re-fixed-wav-controls-20261009。P16/V05保持open，P16明确历史链接与当前未编译源码的区别；当前游戏程序仍CAB05明文修复版，未编译、运行或部署本步，真实登录待用户反馈。音色/效果/loader四原职责、旧音频设置外层及完整音频/整体SPEC继续。

<a id="fixed-wav-reverb-decision"></a>
## CAB11 固定 WAV 的原 Waves 混响（2026-10-09）

原455030的54F744 GUID实读为04CB5622-32E5-11D2-AFA6-00AA0024D8B6，与本机Windows SDK dmusicc.h的GUID_DMUS_PROP_WavesReverb相同；16B参数是输入增益0、混响混合量S32(arg1-16)转F32、衰减时间S32(arg0*100)转F32及原3A83126FH高频比例。初始化454EC7调用(25,6)，固定音乐455386调用(30,7)，分别2500ms/-10dB与3000ms/-9dB，高频比例0.001。此原入口没有合唱参数，不能将此前泛称的混响/合唱当成已证明的同一合同。

沿用户SDL3/WAV选择补原混响控制与真实处理：使用系统已安装的Waves DMO，只处理当前固定WAV的float32 PCM，不创建DirectSound设备、不打开MIDI/音色或引入第三方包。效果在独立SDL线程内创建、配置、处理和释放，COM初始化/清理在同一线程；调用方通过串行请求同步借用一块PCM，不跨线程借COM接口。SDK类型仅留平台实现，公开接口只传轻量值。原属性失败仍由实际SDK结果产生，配置失败原父不消费；处理失败不消费可能损坏的数据。

原两次配置与父顺序恢复，重复时效果状态连续；最后一遍结束后用零输入和DMO_INPLACE_ZERO排出尾音直到SDK返回S_OK，再flush SDL。停止/重准备按当前WAV已定停止合同清队列并重置效果，关闭先销毁音频流/等待回调，再停止效果线程并释放样本；普通BGM/音效不接此效果。官方[媒体类型](https://learn.microsoft.com/en-us/windows/win32/directshow/setting-media-types-on-a-dmo)、[原地处理及尾音](https://learn.microsoft.com/en-us/windows/win32/directshow/in-place-processing)只证明SDK合同，不代原驱动波形/线程调度、声音质量及实际失败验收。

原25/6与30/7及参数结构有据，系统64位CLSID已注册至dsdmo.dll；注册与源码不代实际创建、格式支持或声音通过。默认仍只做本步必要静态核对，不编译、测试、运行客户端/算法或重生成WAV；未验事项保留，原音色/loader三职责继续，不以新处理源自动关闭音频/原生ABI或整体门禁。

<a id="fixed-wav-reverb-source-state"></a>
## CAB11 实际源码与限定结果（2026-10-09）

LegacyAudio.cpp已新增原void配置适配，在初始化音量表后调用(25,6)，固定WAV准备成功后、重复/播放前调用(30,7)。新增平台LegacyWavesReverb值接口与实际提供者：独立SDL工作线程建立COM apartment、创建系统Waves对象及同线程配置/处理/释放；输入输出使用实际固定样本频率的双声道float32。流锁内串行发布请求，同步借PCM直到完成，工作线程不获取流锁/control，返回前清借用指针。只在实际配置成功后附着固定通道，普通BGM/音效正文保持；仅增加系统ole32链接，无第三方依赖或资产变更。

固定WAV重复时使用连续输出时点，最后一遍之后给零输入排尾音，S_FALSE继续、S_OK停止并flush；处理失败不向SDL提交可能损坏的PCM。停止/准备重置效果并重新应用上次成功参数，关闭先销毁音频流、等待回调退出，再join效果线程并释放样本，COM初始化成功才清理。原配置结果仍由父忽略，未用固定成功或恢复运行时MIDI/音色；SDK组件只处理PCM，播放继续由SDL3承接。

原455030完整60条208B、GUID/16B四参数、两caller、SDK声明/系统注册与静态结果见[持久证据](cg2d_fixed_wav_reverb_evidence.json)。UTF-8、两原配置/参数位样本、COM线程边界/平衡清理、PCM借用同步、尾音/失败、固定通道边界、流→线程→样本释放和普通音频其余正文限定静态通过。仅一个原职责adapted纳管，不以系统注册代实际创建/float32支持、调度/波形/原驱动听感、完整失败/通知/正常寿命或原生ABI验收。

固定快照cg2d-re-fixed-wav-reverb-20261009；当前未编译、运行、试听、部署或重生成WAV，游戏EXE仍CAB05明文修复版，实际登录待用户反馈。剩余454FE0/455100音色下载/载入、455260原loader清理和旧99外层继续，P11/P16/V05及完整音频/整体SPEC仍开放。

<a id="offline-bank-lifetime-decision"></a>
## CAB12 原音色职责与离线载入寿命（2026-10-09）

原454FE0/455100/455260三完整根138条450B：集合先GetInstrument，再DownloadInstrument，分别失败1/2、成功Release并0；载入入口先释放/清旧集合，零填350H描述，Source为空使用54F6E4原GUID和flags3，非空用260个宽字符的mbstowcs并flags32H。GetObject失败1，EnumInstrument恰S_OK才从索引0逐项调用下载，下载非0由父返回1；枚举其他结果释放/清集合并0。455260只判原loader非空后Release/清0，454EE0首先真实调用该体，再清其余原音乐对象。原默认GUID已实读F17E8673-C3B4-11D1-870B-00600893B1BD，不凭名称、当前系统文件或加载字节推定原驱动声音等价。

沿用户“不再使用MIDI”的WAV选择，音色载入/逐项变为离线LegacyMidiSynth::LoadBank/Impl::LoadDls的真实文件→样本池→instrument/region持有职责，CAB02实际转换已使用本机系统gm.dls；客户端只载入内置WAV，不新增运行时Source/COM下载入口。原loader清理关联离线Clear/析构与现有SDL固定样本/流清理，不补空原函数或造loader影子状态，三原身份仍属于音频范围而非排除。离线不同格式/实际文件/配置失败使用实际结果和RAII清理，不宣复刻原私有HRESULT/失败残留或全部DLS动态调制。

已发现LoadBank失败会保留旧impl，而原455100先释放旧集合；本步在离线LoadBank入口先Clear，使旧事件/voices/regions/样本同源释放后再加载，失败不保留旧bank。转换器最后一次PCM渲染后显式Clear再关闭输出，所有提前退出仍由同一owner析构清理。此变更只补资源寿命，既有解析/合成、统一衰减、WAV资产和客户端清理正文保持；不编译、执行转换或重生成资产。三原职责可关联来源不等于WAV质量/原驱动听感、全部DLS/失败/设备/正常寿命或整体SPEC通过，完整音频门禁继续开放。

<a id="offline-bank-lifetime-source-state"></a>
## CAB12 实际源码与限定结果（2026-10-09）

两离线现源各只新增一行：LegacyMidiSynth::LoadBank入口Clear后再读取bank/加载pending、成功才发布；MidiToWav最后Render/写PCM后Clear、再关闭输出。旧序列/voice/区域/样本归同一impl，重载先销毁；bank文件数据仅局部持有，LoadDls复制波形和区域后释放，未成功pending由RAII释放。提前退出仍由原析构释放实际owner。两处新增逐片对HEAD重放完全保持其余正文，不改解析/合成/衰减/时长/资产，工具仍EXCLUDE_FROM_ALL独立目标，客户端不链接合成器或访问gm.dls。

原三根138条450B、默认GUID与清理父86条244B、SDK两接口槽/参数，以及既有离线载入/样本区域/音符消费和SDL固定资源清理关联见[持久证据](cg2d_offline_bank_lifetime_evidence.json)。UTF-8、先清后载、实际成功发布、最后PCM后释放再close、提前退出RAII、两源其余正文和客户端链接边界静态通过；三身份按用户WAV方案adapted纳管，没有生成空原入口/COM假体或范围排除，也不凭WAV存在填成功。

固定快照cg2d-re-offline-bank-lifetime-20261009。已知AUDIO32均有具名来源，成员枚举和完整门禁仍开放，P11/P16/V05继续open；原默认波形、全部DLS动态调制/控制器、原私有失败残留、设备/通知/当前创建/格式/声音/正常寿命仍待。当前源码未编译、转换、运行、重生成WAV或部署，游戏EXE仍CAB05，旧99外层及真实本机登录继续；局部静态与CAB02历史转换不代当前或整体验收。

<a id="current-audio-debug-source-state"></a>
## CAB15 当前构建与部署结果（2026-10-09）

已按同一CAA01相关构建授权完成CAB06–14累计源码的必要Debug编译及客户端/离线工具链接。实际首轮13项错误定位为LegacyWavesReverb.cpp系统WAV格式头缺失，仅补mmsystem.h、mmreg.h；两系统头提供原需声明，PCM/效果/线程/寿命正文和用户WAV方案保持。CMake自动再配置成功，最终三库及两目标返回0，0错误/43条窗口源码增量警告；音频提供者/离线工具无诊断，详细向量与日志见[当前编译SPEC](cg2d_build_acceptance_spec.md#current-audio-debug-build)及[持久记录](cg2d_current_audio_build_evidence.json)。

MAP核实当前固定PCM混响、流式加载/共同头、设置页及两编辑回调进入客户端；离线合成和转换源没有进入客户端。8024064B客户端已复制到用户原游戏目录，保留旧版具名备份。CAB06–14的“未编译/仍CAB05游戏EXE”为各当时状态，本节是最新构建/部署事实；第二段空音频、SDL3/WAV、宋体/GBK/ASCII光标及原格式明文链保持。

只完成编译链接和部署，未运行客户端或离线工具、试听、执行测试/算法、重生成WAV或计算摘要。实际系统效果创建/float32支持、真实声音/设备/失败/通知/循环/正常退出、旧设置页外层和成员枚举仍待；IP2登录仍需使用反馈。P11/P16/V02/V05保持open，具名来源与符号不代完整功能、机器ABI或整体SPEC通过。

<a id="audio-table-x87-source-state"></a>
## CAB16 普通音频运行表数值段（2026-10-09）

[原52B9E0声像/音量表](cg2d_audio_update.md#audio-table-x87-source-state)已恢复连续x87数值段，替换double/std::log并保持同源S32表、64通道/50请求与初始化顺序；116条源投影和十处局部S64截断有原指令来源，D02/CPH01其他CRT宿主承接不变。当前客户端自动再配置/Debug编译链接通过，0错误/0增量警告，游戏目录EXE已更新并备份旧版；离线工具/固定WAV资产未改，工具最近通过仍CAB15。

[证据](cg2d_audio_table_x87_evidence.json)只证明此初始化父、计算/写序、当前链接符号和部署；AUDIO已知32→33不是完整枚举，成员库存及完整门禁保持。当前CW上游、机器状态/ABI、表值/声音/设备/通知/失败/循环/正常寿命、旧7/旧8外层和IP2登录仍待，不以0增量诊断抹掉旧窗口警告或宣布整项通过。未运行客户端/算法、测试、重生成WAV或计算摘要。

<a id="fixed-volume-x87-decision"></a>
## CAB17 固定音乐独立音量表的数值段（2026-10-09）

原454E49–454EB8从127递减至0，0是128槽表的正常成员，并非额外故障输入。当前InitializeMusic_454C30仍用double/std::log和直接S32转换，没有承接连续x87、每次原53B498的S64截断及低W32消费；不能把数学公式相似视为转换宽度或0槽等价，也不把尚未运行的差异写成已发生声音故障。

沿工程委托在既有LegacyAudioMath.asm添加私有固定音乐表接口，复用CAB16已证局部CW/S64截断展开；只借原gFixedMusicVolume62B0C8的128*S32存储。原S32计数127..0、从62B2C4向下写入、W32符号运算/上界比较、剩余x87值清栈和初始化后25/6混响配置保持，不强设PC、不先落F64或为0另加分支/默认值。本局部接口不新增原身份或通用CRT帮助体，CPH01其他host消费者保持。

SDL实际资源/设备成功失败、WAV与空音频、重复、455400消费同一表及原明文回复格式保持。只修固定表数值段，沿同一CAA01音频授权做必要Debug构建/符号检查；0槽/全表实际输出、当前CW来源/机器状态/声音/正常寿命仍分开验收，不运行客户端/算法、测试或摘要。

<a id="fixed-volume-x87-source-state"></a>
## CAB17 实际源码与限定结果（2026-10-09）

LegacyAudioMath.asm在原普通表数值段之外追加Cg2dInitializeFixedMusicVolumeX87，复用已有CW/S64截断展开，普通116条段和宏正文保持。InitializeMusic_454C30只将六行double/std::log段换为一次同步借gFixedMusicVolume62B0C8，并增加私有声明；实际SDL/WAV资源/失败返回、后续25/6混响配置、455400消费同源表和其余正文保持，没有改CMake、资产、通用CRT或网络格式。

[持久证据](cg2d_fixed_volume_x87_evidence.json)保存原完整225条679B初始化父、33条数值投影、独立128*S32表与原S64截断来源。原S32计数127..0和下降指针、W32符号/比较/上界与清剩余x87值逐项对应；0槽沿原数值路径，不加特殊常数或保护。72B正常展开帧保存RSI/RDI，S32计数、两W16控制字与S64槽不重叠；仅在转换时暂设向零并恢复原CW，不强设PC、不落F64。源投影不代0槽/全表实际输出或机器状态/ABI验收。

同一CAA01音频必要Debug增量构建返回0，0错误/0增量警告，未重新配置；MAP同时存在普通表/固定表两个实际接口，客户端仍无LegacyMidiSynth/MidiToWav。8024064B客户端已更新用户原游戏目录，旧同长版备份before-fixed-volume-x87-20261009；只核复制成功/长度，不做摘要或内容比对，旧窗口警告未因增量输出0而关闭。离线工具和固定WAV资产未改，工具最近构建仍CAB15。

只更新已有454C30语义/F来源与三限定I/W/S，语义/处置/成员和R7数量不增加，AUDIO33仍未闭枚举。P11/P16/V05/V02完整门禁保持，0槽/全表、实际音量/CW上游/机器状态/设备/失败/通知/循环/正常寿命与IP2登录仍待。未运行客户端/算法、测试、重生成WAV或摘要，快照cg2d-re-fixed-volume-x87-20261009。

<a id="spatial-sound-x87-source-state"></a>
## CAB18 空间音效距离的当前接线（2026-10-09）

按[音频局部方案](cg2d_audio_update.md#spatial-sound-x87-decision)，52C9E0请求和52CD60扫描各交既有平台ASM的真实数值接口，原浮点平方/整数平方两路径分别保持，x87/向零S64低W32及/106商已恢复；24/17条投影与完整两父155条478B见[记录](cg2d_spatial_sound_x87_evidence.json)。两原表索引/门槛、较近请求更新、扫描首次范围外立即返回与调用后标记，以及资源/SDL播放/其他CPP全文保持，不新增CRT通用体、表owner、CMake项或依赖。

当前Debug链接0错误/0增量警告、两接口和原父实际符号可追，游戏EXE已更新并备份before-spatial-sound-x87-20261009；同长8024064B只说明实际长度，未核摘要/内容。新增两个原语义/F及已知音频成员，AUDIO35仍不是完整分母，三限定I/W/S不关闭V05/V02/R01；52CDE0与完整数值/CW/机器状态/声音/失败/正常寿命、旧外层/完整枚举和IP2真实登录继续。没有运行客户端/算法、测试或重生成WAV，离线工具最近通过仍CAB15，快照cg2d-re-spatial-sound-x87-20261009。

<a id="spatial-projection-x87-source-state"></a>
## CAB19 空间音频投影记录生产者（2026-10-09）

沿[局部方案](cg2d_audio_update.md#spatial-projection-x87-decision)，52CDE0真实接入两个既有平台ASM私有接口：先按原CW将S32世界Y/X乘F32 64并先Y后X落F32，原46CE30调用保持；投影后原x87/S64低W32、同源表门禁与读X→写索引→转X→读Y→写X→转Y→写Y/距离/标记/返回全部恢复。完整52条183B/原栈字节、三F32常量及六/33条投影见[记录](cg2d_spatial_projection_x87_evidence.json)。只拓宽表寻址，原W32字段/index/Y返回保；没有额外owner、钳制/保护或CRT入口，其他音频/网络转发与资源保持。

当前Debug链接0错误/0增量警告，两接口、原生产者/转发和既有距离接口符号可追；游戏EXE已更新并备份before-spatial-projection-x87-20261009，8024064B同长只证复制/长度。一个原语义/F及AUDIO36已知成员纳管，枚举仍false；46CE30全部数值/消费者、全应用CW/机器状态、实际声音/失败/正常寿命及完整音频/V05/V02/R01和IP2登录仍待，不因局部I/W/S关闭门禁。未执行客户端/算法、测试、重生成WAV或摘要，离线工具最近构建仍CAB15，快照cg2d-re-spatial-projection-x87-20261009。

<a id="projection-x87-source-state"></a>
## CAB20 音频生产者的世界投影依赖（2026-10-09）

CAB19空间音频生产者使用的46CE30已由[真实连续x87体](cg2d_projection_sampling_return_spec.md#projection-x87-source-state)提供，反向配对入口同组恢复；正向19条/反向16条、四F32比例/两S32正常中心、参考样本与位输出顺序及native outX返回有原完整依据，当前CW保持。音频范围只关联正向实际依赖，共享投影不列为新增音频专属成员；既有记录表/门槛/字段发布及SDL/WAV资产保持。

当前Debug链接0错误/0增量警告、两个实际接口及音频生产者符号可追，游戏EXE已更新并备份before-projection-x87-20261009；8024064B同长只证复制/长度。[记录](cg2d_projection_x87_evidence.json)限定没有新语义/F/功能成员，AUDIO36/枚举false与完整门禁保持；全应用CW/所有消费者/机器ABI、画面/声音/失败/正常寿命及IP2登录仍待，未运行客户端/算法、测试、重生成WAV或摘要，离线最近构建仍CAB15。

<a id="bgm-config-scan-source-state"></a>
## CAB21 BGM 配置的原扫描接线（2026-10-09）

524060按[原扫描/别名合同](cg2d_audio_update.md#bgm-config-scan-source-state)已真实使用项目53CADC/53DEF4，不再宿主sscanf后手拼第五/第六项；同一5B字节区、原路径整DWORD/读W8/尾BYTE及字段/映射写序保持。原格式/配置asset/两配置和运行表owner、SDL/WAV方案及其他源保持；扫描器旧暂停一音频调用恢复，22来源不代完整CRT或音频。

当前Debug链接0错误/0增量警告，配置父/扫描包装/核心符号有据；8023040B游戏EXE已更新，旧8024064B备份before-bgm-config-scan-20261009，只核复制/长度。新增一个原语义/F和AUDIO37已知成员，枚举false与全配置/扫描语法/资源失败/声音/正常寿命/机器ABI/旧外层及完整门禁保持。无新版登录日志，不修改原协议或宣IP2互通；未运行/测试/重生成WAV/摘要，离线最近构建仍CAB15。

<a id="se-property-sampling-source-state"></a>
## CAB22 直接音效的属性取样（2026-10-09）

[PlaySe当前属性段](cg2d_audio_update.md#se-property-sampling-source-state)已恢复通道/音量、fresh通道/声像及单次pitch后base→fresh通道→频率的原顺序；保留现SDL3提供者/正常整数公式、原声像门槛、Play/冷却/轮换/返回。完整164条551B与局部源码保持/取样次数有[证据](cg2d_se_property_sampling_evidence.json)，不以W32样本次序代原COM owner/vtable与SDL资源寿命或整个父行为。

当前Debug0错误/0增量警告、实际父/三个属性提供者已链接，8023040B游戏EXE更新并备份旧同长版before-se-property-sampling-20261009，只核复制/长度。一个原语义/F及AUDIO38纳管但枚举false，完整声音/失败/正常寿命/机器ABI、旧7/8外层与IP2互通仍未验收；未运行客户端/算法、测试/工具、重生成WAV/摘要，离线最近通过仍CAB15，旧窗口诊断保持。

<a id="se-request-properties-source-state"></a>
## CAB23 请求音效单声道与属性合同（2026-10-09）

[现52CB50/52C680](cg2d_audio_update.md#se-request-properties-source-state)修复单声道旧请求每帧两格归中的差异，恢复原立即64；立体声仍原步进/门槛。属性目标先持有、变化才调用、调用后fresh目标写缓存，频率/循环播放各原W32样本序明确；四局部之外、SDL后端/通道owner及原轮换/清计数/失败/返回保持。两完整父257条890B与实际主循环调用链、源序/次数有[证据](cg2d_se_request_properties_evidence.json)，不代COM→SDL资源/整父/机器ABI或真实声音验收。

当前Debug0错误/0增量警告、真实提供者已链接，8023040B游戏EXE更新/备份before-se-request-properties-20261009，只核复制/长度。两个原语义/F及AUDIO40纳管但枚举false；完整寿命/声音/失败/旧7/8外层与IP2及整体SPEC未验，旧窗口诊断保持。未运行客户端/算法/工具、测试、重生成WAV或摘要，离线最近通过仍CAB15。

<a id="audio-loader-publication-source-state"></a>
## CAB24 样本与运行表的原生产顺序（2026-10-09）

[524230当前加载段](cg2d_audio_update.md#audio-loader-publication-source-state)已修复提前合并volume/pitch检查，原sampleIndex/两路径→volume检查/发布→pitch检查→loop采样/两字段发布保持；三个真实53B304路径取代宿主snprintf，BGM三个值先取再写。270静态项、64通道/两128表owner及SDL/MMIO/fileIO/频率与其余正文不改。运行尾窗351条1191B和三调用/源序有[证据](cg2d_audio_loader_publication_evidence.json)，父元数据不代完整前段或所有失败/机器ABI验收。

当前Debug0错误/0增量警告、父/原格式包装/核心已链接，8022528B游戏EXE更新/旧8023040B备份；一个原语义/F和AUDIO41已知成员纳管但枚举false。声音/正常资源寿命/完整SDK失败、旧7/8外层与IP2/整体门禁保持，未运行/测试/工具、重生成WAV或摘要，离线最近通过仍CAB15，旧窗口诊断未关闭。


<a id="offline-midi-control-decision-20261009"></a>
## CAC13 离线音乐通道控制合同与裁决（2026-10-09）

沿CAA01/CAB02/CAB12的既有离线MIDI/DLS→PCM→WAV方案及用户恢复音频开发授权，本线程按AGENTS第8条工程委托落实已列“控制器尚未全部影响PCM”中的一组实际缺口。运行客户端仍只播放WAV；本步改变离线提供者，不恢复运行时MIDI，不引入依赖，不重生成音源。

当前LegacyMidiSynth::Impl::Apply把CC120与CC123都强制设release，前者不能立即静音，后者绕过已有CC64延音；Render每帧从当前RPN选择和共享CC6/38求fine tuning，切换/注销RPN会丢失已设置微调，其他参数的Data Entry会污染微调；bend幅度固定两半音，RPN0/2尚未影响PCM。

原端音色/loader职责沿[既有CAB12合同](#offline-bank-lifetime-source-state)承接。下列控制语义依据[MIDI协会控制表](https://midi.org/midi-1-0-control-change-messages)及[消息概要](https://midi.org/summary-of-midi-1-0-messages)，CC123与延音优先关系见[MIDI 1.0 Detailed Specification的原标准文本](https://people.carleton.edu/~jellinge/cs312_w2020/pdf/_complete_midiSpec1_0.pdf)“THE RELATIONSHIP BETWEEN THE HOLD PEDAL AND ALL NOTES OFF”（检索可见，PDF正文访问受限）。这些是已批准离线替换的机制合同，不冒称cg2d原私有MIDI/COM函数体或默认驱动波形等价。

| 消息/状态 | 本步实施合同 |
|---|---|
| CC120 All Sound Off | 同通道活动voice清空，随后的PCM帧不再消费该声部；保其他通道、bank/region/sample和事件位置 |
| CC123 All Notes Off | 只将同通道活动voice的keyUp置true，交现有CC64判定进入release；不重复覆盖已经进入release的包络/计时 |
| CC101/100 RPN选择 | 保存原7位字节并切为RPN模式；选择只改变Data Entry目标，不撤销已设置音高值；127/127自然无匹配目标 |
| CC99/98 NRPN选择 | 保存原控制字节并切为NRPN模式，后续CC6/38不更新本组RPN值；未知NRPN作用继续未实现，不猜厂商参数 |
| RPN0/CC6与CC38 | 各通道独立保存半音与音分范围；初值2半音/0音分沿既有两半音范围，弯音=(bend−8192)/8192×(半音＋音分/100) |
| RPN1/CC6与CC38 | 独立保存14位微调值，初值8192；更新MSB或LSB时保留该参数另一半，贡献=(value−8192)/8192半音；注销/换参数仍保持 |
| RPN2/CC6 | 独立保存粗调，初值64；贡献=value−64半音，CC38不改变它 |
| 序列Start/循环重置 | Channel值初始化回上述既有无偏移/两半音初态，三个参数无跨序列残留；bank/样本owner和事件时序保持 |

源码仅修改离线LegacyMidiSynth.cpp的Channel成员、Apply控制分支和Render音高表达式；不改变LoadDls/LoadMidi/voice分配、波形插值、包络算法、整体增益、轨道终点或当前内置WAV。通道状态归原Impl/channels持有，不新增全局表、公开接口或资源owner；正常7位Data Entry保留，不新增钳制/默认事件/异常模拟。

本步不扩为全MIDI/DLS通过：CC121完整重置、CC96/97相对调参、其他RPN/NRPN、DLS动态调制/合唱、全部声音/失败/生命周期及听感对照继续开放。P11/P16/V05与FUNC-AUDIO完整门禁保持；不修改函数分母/原成员或宣布原新机器ABI等价。

<a id="offline-midi-control-acceptance-20261009"></a>
## CAC13 验收边界

局部I：消息→同一通道参数/voice→现有Render实际消费完整源码路径；CC120/123行为分开，RPN0/1/2独立保持与NRPN/Null隔离，重置落在既有channels初始化。局部S：仅上述三处源码差异、普通消息/解析/资源/客户端/WAV正文保持、文档/登记精确增量和UTF-8/LF核对。只做静态检查，不构建、执行转换器/算法样例/测试、试听、部署或计算摘要；实际PCM/WAV及旧驱动对照待对应授权。


<a id="offline-midi-control-source-state-20261009"></a>
## CAC13 源码实施与局部静态结果

已在LegacyMidiSynth.cpp三处落实上述控制合同：Channel新增独立RPN参数与选择状态，Apply分开CC120即时清声部/CC123普通松键并接RPN Data Entry，Render直接消费已保存粗微调/弯音范围。Null/换选择不撤销参数，NRPN消息不串写；ResetSequence原channels={}恢复值初始化，现CC64及release计时、DLS解析、事件/轨道、声部调度/包络/插值和其它PCM正文保持。

三处以外整CPP、公开头、转换器及客户端保持；限定源码/状态读写/同通道/7位拼接/正常类型和重置静态核对通过，证据见[cg2d_offline_midi_control_evidence.json](cg2d_offline_midi_control_evidence.json)。两条局部FUNC-AUDIO I/S只说明离线替换代码路径，不提升完整功能门禁或新增原身份/成员/处置；P11/P16/V05开放、所有运行限制保持。原证据不重新裁定，本步也没有观察到或宣称已修某次实际声音故障。

固定源码引用：cg2d-re-offline-midi-control-20261009。尚未构建、执行转换、测试/算法样例、重生成WAV、试听或部署；现有游戏音源保持旧版本。待运行授权后验证本组实际PCM/声音/事件重置和音源质量，其余控制/DLS/生命周期继续推进。

<a id="offline-pedal-reset-decision-20261009"></a>
## CAC14 离线选择延音与控制复位合同及裁决（2026-10-09）

沿CAA01/CAB02/CAB12和CAC13的离线适配边界，本线程按AGENTS第8条工程委托裁决本步。现Apply仅保存CC66/121字节，Render仅判断CC64，选择延音与控制复位没有实际效果。仅补现有MIDI事件→channels/voices→Render链；本步不恢复客户端MIDI，也不重生成WAV。

依据[Roland SCB-55原厂MIDI实现说明第10、12、23页](https://cdn.roland.com/assets/media/pdf/SCB-55_OM.pdf)，CC66开关阈值为64、保持踏板按下时的音符，CC123尊重两种踏板；CC121使弯音归中、表情127、相关踏板关闭、选择器注销，既有RPN/NRPN参数不变。选择延音的按键捕获边界采用[雅马哈原厂踏板说明](https://hub.yamaha.com/keyboards/k-how-to/how-to-use-keyboard-pedals/)的“当前按住的键”机制。保留音量/声像/bank/program/效果发送量的复位范围亦见[微软当前GM合成器控制说明](https://microsoft.github.io/MIDI/kb/in-box-general-midi-synth/)。这些资料只支持本离线替换设计，不证明原cg2d默认驱动实现；不采用资料中的设备容量、GS接收开关、SysEx或MIDI2行为。

| 事件/状态 | 本步合同 |
|---|---|
| CC66从0..63到64..127 | 在保存新值前读取旧值，仅此上升沿给同通道仍按键且未进入release的活动voice置sostenutoHeld；已松键的CC64保持音符和release尾音不纳入捕获 |
| CC66保持64..127 | 不重新捕获；随后Note On与被抢占重建的voice锁存位为false |
| CC66收到0..63 | 清同通道voice锁存；仍按键的音符不被强制松键，已松键音符继续尊重CC64，已进入release不重置计时 |
| Render松键 | keyUp且CC64关闭且没有sostenutoHeld才进入既有release，释放级别/计时只初始化一次；CC123沿本路径，CC120仍直接清空 |
| CC121当前已支持状态 | 同通道bend回8192、CC11回127、CC1及CC64/65/66/67回0、CC98..101回127并切回无有效RPN选择；清该通道全部sostenutoHeld，不清voice或keyUp |
| CC121保留状态 | bank、program、CC7/10/91/93及CC6/38字节、RPN0范围/RPN1微调/RPN2粗调不改；其他通道/序列位置/bank与样本owner保持 |
| 序列/声部生命周期 | 沿原值初始化和voice={}清锁存；不新增全局表、资源owner、公开接口或额外分配 |

选择延音锁存在原Voice内，比每帧只读CC66多一个布尔字段，可区分踏板前后音符；不以通道全保持替代选择延音。CC121逐项复位，不能用channel={}，否则会误撤销已设RPN、音色与音量。上述当前支持状态是具名离线设计，不声称完整RP-015/所有控制器支持或原驱动波形等价。CC96/97原推荐规范细节尚未取得，继续待证，不猜步长/边界；未实现的压力/其他踏板效果、RPN/NRPN及DLS动态调制仍待。

<a id="offline-pedal-reset-acceptance-20261009"></a>
## CAC14 验收边界

局部I限本组事件生产/原状态持有/Render消费与既有新建、抢占、Start/循环/Stop清理的源码接线；局部S限Voice一字段、Apply旧值/两个控制分支、Render释放条件的定向差异及三处外正文保持。集中静态核同通道、上升沿、后续音符隔离、两踏板联合释放、复位保留范围和登记精确增量；不执行算法、样例或测试，不构建/运行/检查产物/摘要/转换器/重生成音源/部署。全PCM/听感与真实事件时序待授权，P11/P16/V05及完整FUNC-AUDIO/总SPEC§10保持开放。

<a id="offline-pedal-reset-source-state-20261009"></a>
## CAC14 源码实施与局部静态结果

已按上述合同在原Voice加一锁存位、Apply接CC66上升沿/关闭与CC121逐项复位、Render接两踏板联合释放。只捕获仍按键且未release的同通道音符，重复On不纳入后续音符；CC66关闭/CC121不松开仍按住的键，不重启release。CC121没有整Channel重置，既有RPN值与bank/program/音量/声像/发送量保留；原新建、抢占、序列与Stop的voice值初始化自动清锁存。

三处外整CPP、公开接口、转换器及客户端保持；限定静态状态读写/同通道/边沿/联合释放/保留范围及差异核对通过，见[cg2d_offline_pedal_reset_evidence.json](cg2d_offline_pedal_reset_evidence.json)。两条局部FUNC-AUDIO I/S只登记本离线实现与接线；原身份、成员、全功能门禁和P11/P16/V05状态保持，无运行/PCM结论。

固定源码引用：cg2d-re-offline-pedal-reset-20261009。本步未构建、测试、运行、执行转换器、检查产物、计算摘要、重生成WAV或部署；当前游戏音源仍为旧版本。CC96/97、其余控制与DLS动态调制继续待，真实PCM/同刻事件/声音/失败及正常寿命待执行授权，整体SPEC未完成。

<a id="offline-dls-key-group-decision-20261009"></a>
## CAC15 离线 DLS 打击乐键组合同与裁决（2026-10-09）

沿CAA01/CAB02/CAB12既有离线MIDI/DLS→WAV及工程委托，本步补区域参数生产→新音符选区→旧声部清除→原Render的完整键组分支。当前LoadDls核rgnh长度至少12B，却只读取前8B音高/力度范围，+10的usKeyGroup没有持有或消费；Apply为所有命中区域直接分配voice，同组打击乐旧音符继续发声。

[DLS Level 1 v1.1原规范第41页的公开检索文本](https://citeseerx.ist.psu.edu/document?doi=544167bbd9658afa41ee684d91ec9827f598d071&repid=rep1&type=pdf)明确rgnh字段次序、键组0为无组、1..15为有效组，其余保留；同一打击乐音色同组旧音符应尽快停止。该原PDF正文访问返回403，本步仅采用检索可见的完整字段说明；不绕过权限，不外推未取得章节。[发布入口](https://midi.org/dls-level-1)及本机Windows SDK dls1.h的RGNHEADER/USKeyGroup和打击乐bank标志作字段交叉依据。它们支持已批准离线替换设计，不是cg2d原默认音源内部算法/实际bank键组分布或声音证据。

| 生产/消费 | 本步合同 |
|---|---|
| rgnh→Region | 从+10按LE U16保存原usKeyGroup，不猜音符名单或组值，不限制输入；缺组语义只沿文件中的0，保留值不改写 |
| Note On选区 | bank/program/音高/力度沿原匹配条件；同一条件供旧声部预处理与既有新声部创建使用，零力度仍先走原松键分支 |
| 组作用范围 | 仅原选区bank的打击乐标志存在时处理；旧voice须同通道、同bank/program、同1..15键组，且本次至少一个实际命中区域持有该组 |
| 旧声部停止 | 在本次所有新voice创建前遍历旧voice并清空匹配声部，随后的PCM不再消费它；绕过CC64/66及release，不触碰其他通道/音色/组和bank/sample owner |
| 区域分层 | 停旧与创建分两阶段；本次所有命中区域都沿原分配/抢占/值初始化创建，不互相截断，不加临时声部表或额外分配 |
| 0与保留组值 | 0不触发键组；16..65535仅保存、不赋予键组互斥语义，不钳制/映射/拒绝或模拟异常 |

“尽快停止”在本离线替换中裁决为直接清活动voice，复用现CC120即时清声部机制；不用普通keyUp/release，避免持续踏板留下被键组停止的旧声部。仅清非拥有引用，样本/区域仍归原Impl。SELFNONEXCLUSIVE同键重触发、其他DLS连接/调制/循环及完整原驱动音源仍未落实，不因本组实现称全DLS完成；同键与组语义不混用。本步不改客户端WAV方案或重生成声音资源。

<a id="offline-dls-key-group-acceptance-20261009"></a>
## CAC15 验收边界

局部I仅本字段生产、原选区与同通道/音色/组旧声部消费、两阶段创建及现Render/清理链；局部S仅Region字段、LoadDls字段写点、Apply命中条件复用和旧声部预处理。静态核字段宽度/偏移、有效与保留组边界、实际命中条件、其他声部保持、新音符分层不互斥及其余CPP/接口/资源正文。不得执行算法/样例/转换/测试、构建/运行/检查产物/摘要/重生成WAV/部署；实际PCM/音色文件分布/声音与失败/正常寿命待授权，P11/P16/V05和完整功能/总SPEC§10保持开放。

<a id="offline-dls-key-group-source-state-20261009"></a>
## CAC15 源码实施与局部静态结果

已在原Region持有U16 keyGroup，LoadDls从原12B rgnh的+10读取；Apply以原bank/program/音高/力度同义条件共用匹配，先清打击乐同通道/音色且命中1..15键组的旧voice，再沿原创建循环产生本次全部区域声部。无新临时表/分配/公开接口，原64声部池、样本与区域owner及Render消费保持；0/保留组、非打击乐、其他通道/音色/未命中旧声部保持，不猜具体音符或实际bank组值。

相对CAC14仅上述三处变化，原CPP其余正文、头/转换器与当前客户端/WAV保持；字段/边界、两阶段顺序、同域选区、两踏板绕过和原清理链限定静态核对通过，见[cg2d_offline_dls_key_group_evidence.json](cg2d_offline_dls_key_group_evidence.json)。两个局部FUNC-AUDIO I/S只记录本键组生产消费，原身份/成员和完整门禁不变，P11/P16/V05开放；本步不称完整rgnh、DLS、音源或原驱动通过。

固定源码引用：cg2d-re-offline-dls-key-group-20261009。未构建/测试/运行/转换/检查产物/摘要、重生成WAV或部署；实际PCM/声音/失败与正常寿命待授权。同键SELFNONEXCLUSIVE、其他连接/调制/循环、CC96/97及整体音频/总SPEC继续未完成。

<a id="wav-only-development-scope-20261009"></a>
## CAC16 用户再次确认 WAV 范围与推进纠偏（2026-10-09）

用户提醒“不是不用midi吗？”，确认CAB02既有要求“一次转换为WAV，后续不再使用MIDI”。CAC13–15继续完善离线合成器的推进方向不符合后续范围；其代码没有进入客户端，现WAV也没有因此更新。立即停止离线MIDI/DLS工具及CC/RPN/键组等开发，不把这些剩余机制作为整体SPEC后续开发任务；旧记录的待项按本节最新范围解释，原历史事实/限定证据保留。

本轮尚未提交的同键重触发源码、合同、登记两条增量及新证据全部撤回，相关六文件逐字恢复本轮前态，用户既有改动保持。CAC13–15已提交工具/历史记录保留，不倒改为当前客户端成果或原音源等价。当前CMake客户端使用固定WAV资源；LegacyMidiSynth/MidiToWav仅列于EXCLUDE_FROM_ALL独立转换目标，源码引用没有进入客户端。这里只核CMake/文本和本轮撤回，不执行工具/客户端、检查编译产物或重生成WAV。

后续音频工作仅为当前客户端WAV链：原调用/状态、实际旧7/8设置接线、资源/设备/效果、音量/声像/空间声效、循环/停止/通知及正常寿命。现WAV质量和适用原音源结果对照仍开放，不能借停止MIDI工具开发关闭完整声音/设备/失败/退出门禁；原音频身份/已批准WAV适配和函数分母不另排除或变更。P11/P16/V05及FUNC-AUDIO完整门禁继续open。

<a id="wav-only-scope-source-state-20261009"></a>
本步只有当前范围/规则/任务/登记的纠偏，零客户端/工具源码变更、零新软件I/W/S/R通过；软件证据保持1316，184叶整项验收0、60任务关闭11和六closure保持。固定引用cg2d-re-wav-only-scope-20261009。没有构建/测试/运行/算法/转换/产物/摘要、资源重生成或部署，整体目标仍active。
