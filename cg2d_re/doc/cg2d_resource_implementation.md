# 资源读取与调色板实现

日期：2026-09-14。基线为当前 `ida-30904` 中的 x86 `cg2d.exe`，IDB 原始输入 SHA-256 为
`dad3f292c6df28e9b7208ad1d339f56ab68c1e728f6595e849e37296551c0f08`。
新端使用 x64；只扩大实际指针，不改变文件字段、索引、计数和颜色值宽度。
本文件记录源码与局部静态证据，不代表完整加载初始化、运行资源集或客户端流程已完成。

## 1. 函数映射

| 任务 | 原函数范围（右端不含） | 新实现 |
|---|---|---|
| R3.2.3 | `[0x456050,0x456189)` | `resource::BuildResourcePaths` |
| R3.2.3 | `[0x45A900,0x45A9DE)` | `resource::ReadGraphicDataByIndex` |
| R3.2.3 | `[0x45A6C0,0x45A704)` | `resource::CloseGraphicFiles` |
| R3.2.4 | `[0x450470,0x450A78)` | `render::InitPalette32`，共享逻辑及 32 位分支，原生替换见第 5 节 |
| R3.2.4 | `[0x450A80,0x450B6A)` | `render::ReadPalette` |
| R3.2.4 | `[0x450B70,0x450BAD)` | `render::SelectPalette` |
| R6.1.3.136 | `[0x450BB0,0x45118B)` | `render::UpdatePalette_450BB0`，共享过渡及 32 位路径 |
| R3.2.4 | `[0x451220,0x451270)` | `render::RebuildPalette32` |

代码分别位于 `ResourcePaths`、`GraphicRead`、`Palette` 的头文件及源文件。
`platform::ShowLegacyMessage` 是 Win32 边界封装，不计为另一个已还原原函数。

## 2. 路径构造

`gBinPath@0x55CF04` 为 128 字节：`Bin`、终止符及全零尾。
六组路径各为九个 256 字节缓存及九个指针槽，原起点及用途见[资源来源](cg2d_resource_sources.md)第 3 节。
新端保持每组缓存容量及指向槽内字符的别名关系，但不声明分散全局之间的地址邻接关系已完整恢复。

`0x456054..0x4560A9` 先完成全部指针绑定；随后逐槽以 Graphic、GraphicInfo、Anime、AnimeInfo、
Coordinate、CoordinateInfo 的顺序执行六次 `sprintf("%s\\%s")`。
原函数末尾不改写 EAX，因此新函数返回最后一次 `sprintf` 的结果。
这里不展开星号别名、不解析版本号，也不改变 CWD；这些属于尚未还原的调用前置流程。
调用者须提供已终止的名称，拼接结果连同 NUL 不超过目标槽；原无界写入与跨槽破坏未覆盖。

## 3. 索引读取与关闭

`0x45A907..0x45A916` 使用有符号图形号边界；成功后复制整个 32 字节元信息记录，
但文件槽号仍从原表 `+0x14` 经 `movsx` 读取，而非从不同布局的磁盘记录读取。
源记录类型及七个查询函数见[图形信息证据](cg2d_graphic_info.md)。

`0x45A94D` 的 `fseek` 仍采用有符号 32 位偏移，忽略返回值；
`0x45A966` 以记录长度为单项大小、项数为 1 执行 `fread`，仅返回项数等于 1 才继续。
非法图形号、空文件句柄或短读直接返回 0，不提前清空调用方输出。
成功读取后在 `0x45A97D` 先把输出指针改为共享解压区，再调用[解码核心](cg2d_graphic_decode.md)。
核心失败时该指针修改仍保留；省略调色板长度输出时改用局部数值槽。最终返回 0 或 1。

新端两块共享承载区各为 1,000,000 字节，保持每次读取复用及覆盖的关系：

- 读取承载范围选取原 `0x833CB0..0x927EF0`，右端为相邻 `fd1` 的地址。
- 解压承载范围为原 `0x73F138..0x833378` 的同长跨度。
- IDA 对 `tmp_buffer` 的类型注释只标了 100,000 字节，不能把该注释或上述邻接跨度冒充原源码数组声明。
- 目前只覆盖实际读写落在这些承载区内的调用。未知的原数组声明、跨界别名、覆盖相邻全局和地址回绕保留待还原状态。
- 短读保留已读字节及旧缓冲尾部，不额外清零、重试或转换为另一种资源格式。

`CloseGraphicFiles@0x45A6C0` 先 `free` MapNo 索引并清空指针，再依次关闭九个非空文件句柄并置空。
它不释放图形主表、共享缓冲或其他缓存；完整分配、初始化和退出清理仍须接入原调用链。
新 `FILE*` 交给目标 CRT 管理，不强转或复制原 x86 CRT 的内部 FILE 布局。

## 4. 调色板

原 `0x554B78..0x556252` 的 45 个物理记录保持 130 字节步长，包括有符号门槛、128 字节路径及全零尾。
`SelectPalette` 保留原上界 39 和门槛判断；接受时记录活动编号，steps 非正只把全局步数改为 1。
拒绝返回原 index，接受返回原 steps，不能把返回值规范化为成功布尔量。
负索引、越出物理表的直接读取以及损坏路径不属于当前已核查的有效内存范围。

`InitPalette32` 保留原栈上 32 个固定颜色及两轮头尾赋值：
首尾十项 flags 为 2，另六项为 5。颜色值直接取原指令，不沿用 cg3a 固定表。
仅首次打开 0 号文件，从文件头执行 672 次 `fgetc`，按 B、G、R 顺序写入 16..239 号并设置 flags=5。
原端没有短读检测，EOF 转为字节后仍为 `0xFF`，不改成加载失败。
首次副本保存在 168 号置黑之前；每次初始化均重新写固定颜色，并把当前 168 号 RGB 置零。

32 位颜色为 `0x00RRGGBB`；每个纯黑先变为 1，最后仅第 0 项清零。
高字节为零不代表 alpha 全透明，`PaletteEntry::flags` 也不是 alpha。
首次初始化按蓝、绿、红指针的写序逐项建立三张恒等映射，最后设置 initialized=1、changing=0。
三个原指针 `0x628FFC/0x6290FC/0x629200` 在原 `gBGR_Data` 中依次是红、绿、蓝通道表；
新端按该原指针次序承载，指针槽适配为 x64。

`ReadPalette` 在成功打开后先读取中间 224 色并关闭文件，再从当前调色板复制头尾各 16 项。
它不额外置黑 168、不选择活动调色板、不重建 32 位表。
`RebuildPalette32@0x451220` 单独保留原完整函数，返回原循环最终 EAX `0x400`。

### 4.1 动态过渡 `UpdatePalette_450BB0`

本项依据当前 `ida-17524/cg2d.exe` 的 357 条指令核对 `[0x450BB0,0x45118B)`。
`0x5562F4` 初值为 -1，`0x62B0A0/0x62B09C` 为零；活动编号改变时先比较 130 字节记录的门槛，
再用 512 字节栈路径以 `rb` 打开文件。门槛拒绝返回 `130*index`，打开失败返回零且不更新
`0x5562F4`，所以后续帧会重试。没有新增负索引门禁。成功后依次消费 224×3 次 `fgetc`，
EOF 仍写成 `0xFF`；168 号 B/G/R 随后置零，目标区 `0x626C60` 的第四字节不被本函数写入。

每色先把当前调色板 B/G/R 存成 float32，再把目标与当前字节之差除以有符号步数，
按蓝、绿、红顺序存入三个 float32 增量表。文件关闭后才设置进行中、帧版本 1、节流 0、
已加载编号；若 `tColor@0x5562E4` 指向原自有色调 `0x5562E0`，还按 45×3 原始字节表
`[0x556254,0x5562DB)` 计算 B/G/R 色调斜率。外部色调指针路径把六个色调浮点状态清零，
不改原目标索引。自有色调 RGB 初值均为 `0x80`，指针初值指向自有色调。

每次参与过渡先令帧计数加一；有符号 `step > steps` 时复制目标中间 224 色，
自有色调直接取目标三字节，清进行中及 changing，并给帧版本加 10。
否则逐项以 float32 状态推进：蓝通道写字节前使用 x87 加法尚未存为 float 的结果，
绿/红使用已存的 float32；`__ftol@0x53B498` 向零截断为 64 位整数后只取低 8 位。
色调也逐通道以 float32 累加并截断，自有指针之外不写色调字节；未收尾时置 changing=1。

32 位路径随后复用 `RebuildPalette32`。节流计数先加一；步数 `<=120` 时每次给帧版本加一，
仅计数达到 2 时先清节流；步数 `>120` 时每到有符号 `steps/20`，清节流并给帧版本加一。
仅当入口活动编号或入口帧版本与末值不同，才依次重建三张通道表：
`gBGR_Data.r` 取 `tColor+2`、`.g` 取 `+1`、`.b` 取 `+0`；色调为 `0x80` 时恒等映射，
否则 `min((索引*色调字节)>>7,255)`。不重建时返回入口帧版本；重建时返回末张蓝表循环
遗留的 EAX（恒等路径 256，缩放路径为最后一个映射值），不能统一改为 `0x400`。
活动编号相同且不在过渡时，入口直接返回零。上述提前返回均不会重建查色表或通道表。

两条文件打开失败路径保留原正文的实际字节、标题和 `0x10` 标志，通过 `MessageBoxA` 边界输出；
不根据 IDA 显示的汉字猜测或重新编码。原 hWnd 由原生 x64 窗口引用承载，窗口初始化尚未接入。

## 5. 替换与验收范围

- `InitPalette32` 不调用 DX7 `CreatePalette`，但按原 `45083A–4508A1` 在 32 位路径也创建独立 CPU 调色板绑定；失败显示原 Error Code = 15 并返回 0。32 位索引颜色仍由 CPU 查表提供。
- CPU 绑定的分配失败不等同 DX7 HRESULT 或对象布局；`Renderer::Init/Destroy` 尚未调用唯一 owner 的分配/销毁入口，完整生命周期和替换对照仍待完成，详见[SDL 呈现器证据](cg2d_sdl_presenter.md)。
- 8/16/24 位渲染分支按用户要求不实现，索引、调色板文件和转 32 位色的共享逻辑保留。
- 动态调色板过渡的共享逻辑及 32 位分支已实现；2026-09-27 后续贴图缓存、节点上传和 `GraphicInit` 已有局部本体，见 [DX 证据](cg2d_graphic_dx_cache.md)、[GraphicInit](cg2d_graphic_info.md)。完整源色键提交、父绘制和初始化接线仍未完成，不沿用早期“本体完全未实现”的状态。
- 2026-09-14 独立静态代理复核 R3.2.4 的四函数、45 条文件记录、固定颜色、消息字节与通道指针，在上述范围内通过。
- R3.2.3 独立复核确认三函数源码在已声明范围内一致；补全文件槽下标、输出槽可写及承载范围注释后再次复核通过，未修改函数行为。
- 主线程独立 x64 Debug/Release 编译通过；本功能阶段未新增或执行测试用例，未运行客户端。编译和静态复核不替代后续原新运行对照。

## 调色板与独立调色板索引文件接线实施 SPEC（2026-10-03）

按第8条工程委托，迁移Palette.cpp三个调色板文件入口和GraphicRead.cpp的
522900独立调色板DBF索引载入。四个局部文件流统一LegacyFile*，
23个原格式/文件调用全部接已有真实53B304/53B2F1/53C8B3/53BCCD/53B1D0。
只迁移文件链，不重做或扩大已确认的共享调色板/32位渲染替换、过渡与DBF容器合同。

| 原父范围 | 原指令/直接入向 | 原文件/格式调用点 |
|---|---:|---|
| 450470..450A78 | 387/2 | 4507AB→53B304、4507BD→53B2F1、4507F6/4507FF/450807→53C8B3、450821→53B1D0 |
| 450A80..450B6A | 77/1 | 450AA9→53B304、450AB8→53B2F1、450AFC/450B05/450B0D→53C8B3、450B23→53B1D0 |
| 450BB0..45118B | 357/6 | 450C24→53B304、450C33→53B2F1、450C4D/450C5A/450C67→53C8B3、450D44→53B1D0 |
| 522900..522B29 | 170/1 | 522989→53B304、52299B→53B2F1、5229BB/5229FE→53BCCD、522AAB→53B1D0 |

四父991条仅统计原完整范围，十个直接入向及所有调用已取证；
本步逐指令核文件链的450780..450840、450A80..450B6A、
450BB0..450D55和完整522900..522B29，不把局部核对转记全部991条重新验收。
既有色深排除、浮点过渡、Renderer绑定、CPU查色表和容器分配的历史边界保持。

450470仅首次初始化才格式化512B未清path、gBinPath及第0条路径，以rb打开；
失败保留原MessageBoxA旧字节/标题和返0。成功逐项读取16..239共224色，
每色B→G→R三次getc，分别消费S32返回的低U8，EOF(-1)仍成为FF而不是新错误。
flags=5、关闭后备份全256色、再把168号置黑的顺序保持，已初始化路径不打开文件。

450A80使用256B未清path及实参index路径；rb失败保留原提示返0。
成功同序读取中间224色并写flags=5，关闭后才从当前调色板复制头尾各16色，
返回1；不置黑168、不修改活动索引、不加短读/EOF拒绝。
450BB0使用512B未清path及入口活动索引，保留加载状态/能力门槛；
rb失败返0且不发布已加载索引。每色三次getc之后才168号黑色修正及
当前色/步长计算，关闭后重新读取活动编号，再发布过渡与已加载状态。
不把九处getc替换为一块读取，不调整既有浮点计算或跨调用采样次序。

522900保留两个256B零初始化局部、原目录第255字节检查及追加反斜杠、
共享gDataPath和%s%s\%s/CGLoadSpr.dbf。rb失败退出；
先读(1,20H)至唯一共享头，忽略读取数，短读保留头后缀。
magic!=00464244H直接退出且不关闭文件，保留原未释放流，不新增清理。
成功沿原两次裸字节分配、读(1,compressedLength)、53AF90解码、
释放压缩块、保留解码块、payload+8/唯一键覆盖及count/stride发布，
之后才关闭，再沿有序索引加入独立调色板队列。所有正文与旧分配适配保持，
头长度/未知20B及compressed/decoded字段不随FILE拓宽，不加输入/失败保护。

内部LegacyFormat/LegacyStdio头仅由两个cpp引用，不改变公开渲染/资源接口。
GraphicRead的九个共享图形文件槽及其生产/定位/读取/释放仍是另一条待迁移链，
不得只改公开std::FILE类型或把该链混接LegacyFile。本步不碰这些槽或音频。

实际含NUL字节：553514=25735c257300（%s\%s），554920=726200（rb），
55D064=257325735c257300（%s%s\%s），58D364=43474c6f61645370722e64626600（CGLoadSpr.dbf）。
原消息字节、表路径、索引资源、共享状态与文本边界均保持，不转码或清未知局部。
仅静态核指定文件指令/入向/调用/原字节、生命周期/窄化/失败/备份/发布次序、
限定源码/映射差异、合同仅追加和唯一进度；完整CRT/模式3/文件内容及运行验收仍待。
不构建、链接、查产物、新增/执行测试、读写调色板样例/客户端或校验哈希。

## 2026-10-04：图形索引与DBF容器生命周期增量

[本组子SPEC](cg2d_resource_container_lifecycle_spec.md#resource-container-source-closure)已补四容器的唯一被动存储、具名构造/退出、即时宿主登记和同源消费。DBF先map后vector构造；正常退出逐轮释放解压块、归空集合/清树，再释放集合容量和空树，不提前由522C50析构。原图形加载/文件、解压、索引/队列/双池和载荷分配域保持；全应用原序、其他owner/完整清理、构建和运行仍待，局部证据不代整个资源功能验收。


## CPH01 调色板正常转换/计数与编译器提供者（2026-10-05）

[当前具名合同](cg2d_compiler_helper_spec.md#compiler-helper-source-state)撤TruncatedLowByte的非有限/越S64返回0模拟，保正常S64后低U8；Add32撤U32/bit_cast回绕，保S32计数和1/10实际increment。六颜色转换和五加法、蓝/绿/红及色调F32取样、原节流/发布和其余正文保持。

53B498/53BDC0仅按D02登记本机编译器host职责：明确类型转换与自动局部帧/探页。没有同名假函数/手工栈操作或ABI通过声明；本步原两体32条86B、六转换去重28条150B与计数邻窗只作限定静态，完整357条父/所有浮点/资源/画面/运行仍待，异常域不保护或模拟/称等价。

## BOL01 图形读盘与解码正常取样（2026-10-06）

[具体SPEC](cg2d_graphic_read_decode_sampling_spec.md#graphic-read-decode-sampling-decision)/[完整证据](cg2d_graphic_read_decode_sampling_evidence.json)。原读盘75条222B与核心182条461B共257条683B/4页，4call-32内部分支/5ret无switch；读盘4普通call/4父，核心2普通call/同一父45A900分列。读盘二处恢复32B快照后同一实际记录地址读S16 fileIndex、缓存已检查FILE seek→重取FILE→快照length读取，以及可选palette判定在共享解码区指针发布之前取样；严格read返回1、失败不清输出/不回滚、缺省局部palette与两原core目标保持。核心正确CPP整文保持：初始目标/头宽高缓存、raw不读header.length且前向逐字节、RLE固定边界/仅命令间检查/value先长度/20优先10、literal只10及原FFFFF比较、原pal和length→width→height发布序不改。

167/168两个已有原ID完整历史深拷贝续记，旧167 FILE调用事实保；1399语义/1394有效ID/1512关联(41.09%)不变，分类510→512、RESOURCE29→31枚举open、四独立正常局部static I/S E609→613，完整gates空。七open只加当前进展/引用，队列175/R7区间67/3061、60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它功能保持。

撤旧接口及旧解码SPEC额外的两两不重叠前提，按原实际取样/字节处理/发布顺序保正常别名，不用附加前提缩原范围；旧限定范围下的历史验证保但不追认当前运行。正常W32/native地址注释按RPLAN.24，不增溢出/越界/补零/钳制/短读修复或异常等价。未构建/调试/测试/运行/hash；全部caller、当前所有正常别名/真实资源内容-调色板/draw/主表FILE owner及画面/R1/R7/R8继续，网络原格式明文/内存CryptInt保/音频暂停。

## BOM01 独立调色板生产与版本刷新（2026-10-06）

[具体SPEC](cg2d_independent_palette_production_refresh_spec.md#independent-palette-production-refresh-decision)/[完整证据](cg2d_independent_palette_production_refresh_evidence.json)。原生产313条1163B及刷新30条87B，共343条1250B/五页/3call-34内部分支/4ret，无switch；入向生产1/1父、刷新2/1父分列。完整原体含已排除16/24位四区间541B，只共享及32色实施。七生产补丁撤跨调用entry长借，失败/成功表与像素源先当前图号pub，复制后补清新表，固定实际目标/逐项新源record、R-B-G原色取样与R表→record-R/G表-G/B字节→B表取样，完整W32颜色先写、纯黑再W32=1，两W16版本各自新表先值再record写。原loaded不置位/计数/768B和L/3清段保，刷新两正确正文/原+22 S16与U8=0/1/2、仅2清零写回保持。

原5562E8/EC/F0三槽B-G-R/初值628FFC-6290FC-629200的12B与完整数据引用已归档；ColorChannelMaps及唯一位置初始化恢复B-G-R native顺序/布局断言，具名三通道仍各指同一数组，PaletteTone成员指针消费者/GPC01源码保，不重新全面复验其它父。

新1399/1400两原ID独立纳管：语义1399→1401/有效ID1394→1396、分类512→514、合同关联1512→1514/3680=41.14%；刷新两实现体同一原ID，历史直接定义1/2保。RESOURCE31→33及RENDER26→28枚举open，两根各域I/S共八新局部static证据E613→621，仍两原函数/三实现体；member_inventory_closed=false/gates空，七open仅进展/引用，175队列/R7区间67-3061、60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它历史功能保持。

仅已批准共享及32位正常局部静态，不代全部Tone/数组初始化父、读取/分配-回收/重入owner、上传像素/画面和R1/R7/R8。未编译/调试/测试/运行/hash；RPLAN.24正常原宽/已证地址，不新增异常保护/回绕或故障等价，网络原格式明文/内存CryptInt保/音频暂停。

## BON01 调色板过渡、通道与查色取样（2026-10-06）

[具体SPEC](cg2d_palette_transition_channel_sampling_spec.md#palette-transition-channel-sampling-decision)/[证据](cg2d_palette_transition_channel_sampling_evidence.json)。完整过渡357条1499B与查色23条80B，共380条1579B/六页/15call-40内部跳/3ret，无switch；主根6普通引用/5父、查色1/1父分列。两个低色深调用分流区各20B仅纳盘；9B格式/模式与135B ToneTargets共144B，三元表逐项一致保持。12补丁(主11/查色1)恢复初始化颜色及目标差、默认指针先状态pub、Tone三Byte/索引/差/浮点、steps先Framepub、完成Tone目标B/G先Bpub/R先Gpub、版本先两状态清、Tone G/R浮点先B/GBytepub、callee后活动快照及节流counter/steps先pub并同样本判定、查色R/G中间整数先B样本。

723/172两个已有ID及完整旧记录深拷贝续记，FILE及CPH六正常转换/五加法历史保；语义1401/有效ID1396/1514关联(41.14%)不变，分类514→516、RESOURCE33→35/RENDER28→30枚举open、八两域局部static I/S E621→629。历史直接定义1/0保持；两个原函数不当八函数，member_inventory_closed=false/gates空、七open仅进展/引用，队列175/R7区间67-3061/60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它历史功能保持。

原512B未清path/三getc低U8/168黑色、色调helper逐项重取与业务上限255、查色完整W32颜色后纯黑1/透明0和400H返回保持；只共享/32正常局部，不代全部caller/PC53/私有ABI/数组及通道初始化-owner/像素画面和R1/R7/R8运行。未编译/调试/测试/运行/hash；RPLAN.24不增溢出/越界/补零/故障策略或回绕等价，网络原格式明文/内存CryptInt保/音频暂停。

## BOO01 初始化与读取取样（2026-10-06）

[BOO01 SPEC](cg2d_palette_initialization_read_sampling_spec.md#palette-initialization-read-sampling-decision)/[原证据](cg2d_palette_initialization_read_sampling_evidence.json)。完整初始化387条1544B与读取77条234B，共464条1778B/六页/19call-27内跳/5ret、三普通入向。原8/16/24区85条289B按既定范围排除；固定128条写入、128B局部色表与当前逐字节一致，八段原常量100B保留。Init仅1处恢复R→G→中间整数→B查色取样；W32完整色/纯黑1/透明0保持。Read原逐项DWORD头尾复制保持，不改Byte分写；旧FILE链、备份先168置黑、实际独立CPU绑定、中性通道顺序保。

169/170两已有ID完整历史深拷贝续记，语义1401/有效ID1396及1514关联41.14%不变；分类516→518、RESOURCE35→37/RENDER30→32仍枚举open，八两域局部static I/S E629→637。历史定义0/0保持，两个原函数不计八函数；七open及完整门禁保持，不代全部父/owner/正常别名/像素运行。

本步静态与汇总仅验上述范围；未构建/测试/运行/hash。原DX机制沿已有批准承接，不猜HRESULT或补COM模拟，不扩溢出/越界策略。网络原格式明文，音频暂停；继续沿实际初始化/资源消费/owner推进整体SPEC。

## BOP01 自动地图调色板取色生产（2026-10-06）

[BOP01 SPEC](cg2d_automap_palette_color_production_spec.md#automap-palette-color-production-decision)/[证据](cg2d_automap_palette_color_production_evidence.json)。45A190→ReadPalette/451D80→ReadGraphicDataByIndex/451E90三原根完整273条768B/五页，5call-27内跳/5ret、四普通入向。Get恢复蓝累计在count之后发布、蓝/绿中间组包先红平均两处阶段；Nearest去通用闭包，首色R²+B²+G²与候选R²+G²+B²及取样各按原顺序表达。现有正常数值保持，不误报取色值错误；Sub45A190正确体保，原1024B局部palette/500000B色表清零及两map区、调用后fresh GraphicInfo不改。

三既有源码根新增机器语义1401/1402/1403，语义1401→1404/有效ID1396→1399，关联1514→1517（41.14%→41.22%），分类518→521；RESOURCE37→40/RENDER32→34仍枚举open，各成员独立I/S共10条static E637→647。历史直接定位各1/旧行号保；三个原函数不当十函数，七open仅进展/引用，60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0保持。

只全根正常局部文字static；共享解码区单次借用/height入口样本与每像素fresh width、U32除法/低U8、严格<先见/候选16..<count-16保持。auto.dat父/实际图形与地图画面/共享owner运行继续推进，未构建/测试/运行/hash；网络原格式明文、内存CryptInt保、音频暂停，不新增溢出/越界策略。

## BOQ01 DBF索引生产的共享比较字与记录取样（2026-10-06）

[BOQ01 SPEC](cg2d_palette_dbf_index_production_spec.md#palette-dbf-index-production-decision)/[原证据](cg2d_palette_dbf_index_production_evidence.json)。522900完整170条553B/三页、16call-11内跳/1ret，唯一普通入向522B8E/父522B30。五段原数据31B；58D360在.data，初值44424600、唯一直接引用5229C5为W32读。新增私有U32承载1、headerMagic→共享比较字和stride→fresh payload→S32偏移→native record地址两正文补丁。原正常值、两清零path/第255Byte判断、短读共享头/魔数不符不关闭、两分配及解码/旧块与键/重复覆盖/count-stride后pub/关闭后迭代入队保。

724已有ID完整历史深拷贝续记，语义1404/1399有效ID及1517关联41.22%不变；分类521→522、RESOURCE40→41枚举open，独立局部I/S E647→649。历史直接定义1/旧FILE属性保持；七open仅引用/进展、60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它门禁保持。

仅全根正常局部static；W32比较字非地址不扩宽，不据无直接写引用宣只读或所有alias闭合。私有vector/map沿已有批准宿主承接，不以本根代原容器机制/53AF90或SDK-owner运行。未构建/测试/运行/hash；网络原格式明文、音频暂停，不新加异常/越界/故障或关闭策略。

## BOR01 独立调色板初始化与索引发布（2026-10-06）

[BOR01 SPEC](cg2d_independent_palette_initialization_sampling_spec.md#independent-palette-initialization-sampling-decision)/[本步证据](cg2d_independent_palette_initialization_sampling_evidence.json)。1237/522B30完整94条286B/两页6call-7内跳/1ret，两个普通入向；原指令继承PGM01完整归档，本轮核边界/入向及缺口取样。hidden58B7E8..58D348全7008B/1752W32与60源段初值逐项一致，保持可写表。初始化落实queueCount/首分配、clearCount先索引pub/实际取得表清零、recordCount与lock后initialCount先记录pub、逐项records→index→recordpub→fresh index loaded0、尾fresh count先index++比较。正文1/来源注释1，正常值和旧SDK适配/释放根保。

1237完整旧PGM01记录/分类保留续记，既有source2/历史定义1不变，语义1404/有效ID1399及1517关联41.22%、分类522/RESOURCE41保持；两局部static I/S E649→651。没有新增原函数或成员，七open仅进展/引用，60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及全门禁保持。

仅正常局部static，native index16B仅首T*拓宽、loaded仍U8、记录0xB04无指针/count S32保，字节量在已批native API边界。保两GPTR/GlobalLock后才判初始数量，不新做空指针/失配/溢出保护、旧池回收/回滚。全部父/真实SDK/queue-DBF两表owner-正常别名/画面运行继续推进；未构建/测试/运行/hash，网络原格式明文、音频暂停。

## 2026-10-11 PTR01：地图回复调色板消费

[PTR01](cg2d_map_reply_palette_spec.md#map-reply-palette-source-state)补齐509550回复生产与51D6F0调色板消费，两完整原根102条325B/三页、7E8五目标/七跳三RET。恢复地图名整词→状态→尾字节发布及atoi持值判负，两组两处四注释；原提取失败读序/负值覆盖和消费者正确body保持。两新F两adapted，NETWORK106/MAP191/RENDER118，三限定S零I八open；资源/全provider/owner/SDK与运行未闭。
