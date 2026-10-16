# 代码审查与组织整理 SPEC（2026-10-16）

用户要求审查字节数组、冗余转换／包装、namespace 与目录组织，并授权按合理性逐项设计后执行。本次由当前线程完成；仅静态验证，不构建、测试、运行或计算摘要，不生成／更新 JSON 台账。

## 审查范围与裁决

初始扫描覆盖 `src/`、`include/` 的 316 个 C++ 源／头／包含文件，共 152179 行。扫描用于定位候选，下面的改动须逐一核实际生产、消费和边界，不能把扫描覆盖说成全量行为验收。

| 编号 | 问题与证据 | 实施裁决 |
| --- | --- | --- |
| COR01 | `LoginServerRecord::opaque[0xC0]` 实际由 `ParseIpOptions_507C80` 分别写地址和端口文本，查询入口分别从 0／0x80 读取 | 拆为 128B 地址、64B 端口文本，保 0xC1 步长及最后的可用字节 |
| COR02 | 相册 `gAlbumStorage[0x3C00C]` 混合头、版本及 1920 条 0x80B 记录；生产／UI 均按偏移取字段 | 建立真实存储结构及具名记录，保磁盘布局、两角色分组及调用后的角色重取 |
| COR03 | `MapEffectRecord::legacyBytes[0x44]` 通过四个宽度／偏移包装读写，渲染再作位转换 | 按已消费宽度定型，保多用途数值槽与未知区；移除偏移读写及取 next 包装，链接继续使用原生指针 |
| COR04 | 普通／公会／迷你邮件以三个大字节数组承载同种 0x140B 消息，已有 UI 只读／未读可写视图 | 核完整插入、搬移、清理、持久化后按同一消息结构定型，保持共享 owner、局部清零与视图权限 |
| COR05 | 保存编解码二次转发、可见完整类型上的比较包装和同类型转换使调用变长 | 删除无额外合同的包装；保 CryptInt 比较方向、读取次数、实参求值及返回宽度；不为删除包装暴露不完整类型 |
| COR06 | 绘制／字体度量入口藏在 `login::detail`，跨模块通过登录私有头取得声明；局部宽泛导入掩盖实际依赖 | 绘制和字体接口归 `cg::render` 及已有渲染公共头；移除相应登录声明与宽泛导入，保业务正文 |
| COR07 | 源码根目录混放资源、输入、音频、登录请求与绘制；公共头目录未反映所属模块 | 按现有职责迁移文件，更新直接／相对包含及本线 CMake；保编译单元和对象初始化顺序，不添加转发头 |

### 保留项

- 字符串、网络帧、压缩／编码／摘要缓冲、图像像素、颜色分量表及逐槽状态数组是字节数据，不为外观统一包一层结构体。
- 动画缓存 `CacheActionEntry::prefix`／`CacheSlot::remaining`、Yobi 显式未知区、音频三个仅清零的 0x80B 数组、留言板仅清零的 0x2C0B 区，当前源码不足以证明内部字段，保留并标明依据不足。
- 短函数若承担原入口身份、回调签名、编码／平台边界、失败处理、生命周期或不完整类型封装，不按行数判冗余。
- Yobi 仍有登录／场景／窗口共享历史接口；本轮 namespace 整理先消除已明确的渲染职责错位，不把整个 `login` 域机械改名，也不伪称历史耦合已全部消除。

## COR01：登录服务器记录

- `address` 为 `std::array<char, 0x80>`，`portText` 为 `std::array<char, 0x40>`，`available` 仍为 U8。
- 写入字符保原字节，截断门槛仍为 0x7F／0x3F；终止字节与 available 发布顺序保持。读取直接使用文本成员，删除原重解释转换。
- 加入成员偏移、大小、对齐、标准布局和平凡复制断言。检查所有 `opaque` 使用已迁移，原初始化范围仍为 800 × 0xC1。

## COR02：相册真实对象

- `AlbumRecord` 布局：U8 used／mark1／mark2／tribe 位于 0～3；S32 number／imageNo 位于 4／8；五个 S32 ratings 位于 0x0C，四个 S32 attributes 位于 0x20，末个 S32 value30 位于 0x30；17B name 位于 0x34，两行各29B description 位于 0x45，U8 fresh 位于 0x7F。只对消费者已明确的分组命名，不推断 value30 业务含义。
- `AlbumStorage` 为4B signature、两个S32版本、1920条连续记录，大小仍0x3C00C，对齐4；记录定位仍用 `index + selected * 960`。初始化／保存／加载只在序列化边界保留整块内存复制。
- UI 和战斗借用 `const AlbumRecord*`；每处借用调用与角色／索引取样位置保持，不缓存跨业务调用。新标记清理仍使用原独立入口。
- 加入全部字段偏移、大小、对齐与平凡复制断言；逐一核生产写序、UI独立取样、战斗短路、文件读写大小和两角色清零范围。原只读4字节等历史持久化行为本次不改。

## COR03：地图效果记录

- 在既有 `WorldUpdateDependencies.hpp` 定义完整 `MapEffectRecord`，由世界更新与渲染共享：S32 x／y／value08／value0C／speed／graphicIndex 位于 0～0x14；0x18～0x1F 保留；S32 value20／value24／value28／value2C 位于0x20～0x2C；U32 remaining 位于0x30；0x34～0x3B保留；U16 kind／phase／variant 位于0x3C／0x3E／0x40；0x42～0x43保留。
- value08／value0C 按效果类别兼作速度或定点坐标；value2C兼作更新值／绘制高度，不凭单一消费者给出排他语义。原生 prev／next 继续位于当前x64的0x48／0x50，大小0x58，未知区不猜指针。
- 四个偏移读写函数及 next 包装移除，调用点直接访问成员；S32解释直接读取，仍需U32运算的取值显式转U32，不改变无符号减法等计算。方向更新仅改触达轴，RNG调用和链操作顺序保持。
- 核所有旧偏移与字段映射、读写次数、原参数表达式、渲染像素写序和池初始化；新增逐成员、大小／对齐与标准布局断言。

## COR04：三类邮件共用消息布局

- `MailMessage` 为两个单字节 unread／kind、18B name（+2）与300B text（+0x14），大小0x140、对齐1。标记沿现有 `std::byte` 类型，UI的未读可写／其余只读视图不变。
- 普通邮件使用连续1200条消息，迷你邮件使用连续200条消息；保两角色×60联系人×10条及20联系人×10条的原索引算式。迷你清理原来循环20次，必须保留跨下一组的访问，不能因十条分组而擅改循环或增加边界门槛。
- 公会使用400条 `GuildMailEntry`，S32 key／U32 active 位于0／4，十条消息位于8，大小0xC88；保200条一组、查找最后匹配及调用后重取角色。
- 持久化改用 `sizeof` 保字节数，不能误用结构体数组的元素数；整区与局部清零、九次倒序320B复制、首字节清理、满组返回地址算术保持。字段容量来自现有固定跨度，不据此新增复制限长。
- 核消息／组／完整文件大小、迷你contacts与messages相邻偏移、借用权限、遍历方向、宽度及编码。静态断言未编译不计运行验证。

## COR05：包装与转换清理

- 删除 `EncodeSavedBytes`／`DecodeSavedBytes` 两层转发，原52D310／52D460实现直接置于 `cg::login::detail` 并在既有保存依赖头声明；相册和保存调用直达原实现，算法正文逐字保持。
- 删除 `IsPlayerNextExperienceEqual`／`IsPlayerFpLessThan`，调用方已有完整玩家结构声明，直接使用同一CryptInt成员及同向比较，不改为取值再比较；删除仅单次调用的零参数 `WindowElement_NoOp` 转发，直接调用UI原入口。
- 清理括号内完整嵌套的相同整数类型 `static_cast<T>(static_cast<T>(expr))`，只保留一次；中间含运算的窄化并非同类，保留。Yobi六处比较及鼠标坐标赋值只去除同类型S32强转。
- 保留Yobi只见前置声明的访问边界、`ContextText` 的字符串快照语义、字节宽度取样、平台／编码适配和原入口身份。短小不是删除函数的充分理由。
- 验收旧包装零引用、编解码正文／调用实参保持、比较方向与原参数类型不变，所有剩余必要窄化／转换未变。

## COR06：渲染命名空间与声明归属

- `DrawGraphic`、`DrawGraphicHover`、`SetPendingGraphicBatchTypeAtLegacyIndex` 统一归 `cg::render`，声明加入既有 `GraphicPendingQueue.hpp`；字体两个度量入口归 `cg::render` 并在 `Font.hpp` 声明。
- 删除登录依赖头中的图形描述符别名、绘制／度量声明、`DrawText`／`DrawText3` 两个纯转发inline函数；调用方直接使用原 `DrawText_456780`／`DrawText3_456560`。调用实参和函数正文保持。
- 删除 `gValue558F78` 引用别名，十处消费者直接读取同一 `gFontMetrics[0].doubleHeight`，不缓存取样。
- 悬停颜色共享槽 `gValueD69768` 同步归render并在图形队列头声明，保持唯一对象；使队列实现无需再包含登录私有头。
- 迁移调用方直接包含所属公共头；渲染实现移除已不需要的登录私有头。`GraphicHitTest.cpp` 的六处宽泛using改为明确限定名，平台鼠标调度入口本身仍归platform。
- 核声明唯一、旧限定名／别名零残留、直接包含、全部调用实参及函数正文在去除命名空间差异后等价；不新增兼容转发层。

## COR07：目录与文件组织

迁移50个文件，保持编译单元数量及CMake条目顺序，不添加转发头或兼容文件。具体归属：

| 当前根目录文件组 | 源码目标 | 公共头目标 |
| --- | --- | --- |
| Animation*、GraphicAlloc/Decode/Info/Read、ResourceNames/Paths | `src/resource/`，私有AnimationRuntimeState随实现 | `include/cg/resource/` |
| DrawGraphic、DrawGraphicHover、Palette | `src/render/` | `include/cg/render/Palette.hpp` |
| Input、InputCaret、InputKeyDispatch及私有依赖头 | `src/input/` | `include/cg/input/Input.hpp` |
| LegacyAudio、LegacySeDefinitions.inc | `src/audio/` | `include/cg/audio/LegacyAudio.hpp` |
| LoginRequest、LoginSavedState、三个Character*Request及私有依赖头 | `src/scene/login/` | Login*及三个请求头统一到`include/cg/scene/login/` |
| CharacterSelectScene公共头 | 源码维持`src/scene/character_select/` | `include/cg/scene/character_select/` |
| Blacklist | `src/world/Blacklist.cpp` | `include/cg/world/Blacklist.hpp` |
| LegacyRecordOrder（实际公会资料、成员及排序） | `src/world/GuildState.cpp` | `include/cg/world/GuildState.hpp` |
| YobiRuntime公共头 | 维持既有`src/Yobi/`分组 | `include/cg/Yobi/YobiRuntime.hpp` |

- 相对包含按“原文件位置→原目标→迁移目标→新相对路径”解析，公共包含使用新`cg/...`路径；所有原有效边在迁移后必须仍指向同一文件身份。既有循环或未闭依赖不擅自改业务。
- CMake只替换本线源路径。README补当前目录说明，当前函数表仅同步迁移路径并标明历史行号仍是原记录；历史SPEC／进度中的旧定位不全文重写，本SPEC提供迁移索引。不触碰JSON台账或历史测试脚本。
- 精确迁移正文：除包含路径外，50个文件的内容逐字保持；所有其他源文件除相应include路径外逐字保持。核完整直接包含解析、无重复／缺失目标、CMake有序列表一一映射、编码与Git重命名差异。源码根不再混放模块文件，空占位文件保留。

## 执行与验收边界

各项均在改动前补充具体字段／符号／路径和验收条件，随后按本次用户授权执行。每项完成后记录当前静态证据并精确提交。保持原函数地址注释、正常／失败分支、数值位型、编码及存储生命周期；新增断言未编译时明确标记，不以静态检查代替编译或运行验收。

## 本轮结果

COR01～COR07均已实施并逐步提交，具体记录见[进度第292节](../plan/cg2d_full_reconstruction_progress.md)。目录迁移后1287条包含边有效，198条构建源码顺序保持，3680行函数身份／名称／状态保持。代码正文、字段布局算术、限定名／实参、存储范围、编码及差异已做静态核对；COR06曾带回两处旧服务器访问，跨步骤检查已修正。

未编译、测试、运行或计算摘要；所有新增static_assert尚未编译。不生成／更新JSON台账。上述保留项仍保留，不以本轮整理声称未知字段已还原、Yobi历史namespace耦合已消除或客户端行为已通过运行验收。

## COR08：剩余内存与除法转发退休（2026-10-16）

用户指出 LegacyMemory.cpp／LegacyFormat.cpp，继续按既有授权评估并精简。当前源码已完整提供实现依据，不新增原端事实。

- LegacyMemory中的_msize、calloc、realloc、memmove、memset、memcmp六个转发只有声明与定义，无源码消费者，删除这些冗余接口。
- AllocateLegacyByteBuffer／FreeLegacyByteBuffer的真实消费者仅位于GraphicRead、SceneState、LegacyNetworkAdapter；改为直接std::malloc／std::free。参数现有U32类型或原显式U32转换保留，常量请求不变；无新乘法、对齐或窄化。不改变空指针处理、失败分支、释放顺序和分配域；原成功适配器路径没有释放，本次不补释放或改RAII。
- DivideLegacyUnsigned64_53CB10只有ReadSystemSeconds_4589D0一个调用，改为原U64高低字组合表达式除以10000000u；保留组合括号与整除，不改浮点或除法舍入。
- 删除上述两个CPP及对应两个HPP、CMake条目；消费者补所属标准头或真实LegacyFile头，不留兼容转发头。
- 原函数地址与CRT承接说明留在本SPEC／现有函数表；无消费者函数不以伪造调用维持源码占位。函数表身份和状态不变，定位改为真实调用或本节的宿主承接说明，不更新JSON台账。
- 验收：旧符号与旧include/CMake路径零残留；逐点核对分配数量、U32到size_t的现有转换、释放位置及U64运算；执行build.bat Debug验证编译／链接。不新增测试、不运行客户端、不计算摘要。收益为删除维护负担，不宣称未经测量的速度提升。

宿主承接映射：原53FCFB→MSVC _msize；53BFF8→std::calloc；53FA5B→std::realloc；541620→std::memmove（保留支持重叠）；5419D0→std::memset；54B160→std::memcmp；53CB10→原调用点U64除法。六个无消费者入口的旧窄接口随无用函数一起退休，不宣称所有size_t调用等于原U32接口。

COR08验收：上述删除与调用替换完成；静态正文对照、剩余引用及包含路径检查通过，build.bat Debug配置／编译／链接成功，退出码0。当前编译仍有未初始化变量和旧API告警；本次未运行客户端或测试，不宣称性能提升。详见唯一进度第294节。

## COR09：模块 MD5 改用 Windows CNG（2026-10-16）

用户要求确认并优化LegacyModuleDigest.cpp。当前代码的四个初始W32、64轮常量、位移和64B分组／小端长度填充均为MD5；公开头也明确标记模块MD5。唯一业务调用位于LoginScene，输出沿原32个小写十六进制字符加NUL，不改摘要类型或协议。

- 以Windows系统CNG的BCRYPT_MD5_ALGORITHM替换手写Initialize／Transform／Update／Finalize及大小端辅助函数。显式打开算法提供者，避免依赖Windows10才支持的算法伪句柄；哈希对象采用Windows7起支持的系统分配与释放。Windows/MSVC平台边界内增加系统bcrypt链接，不下载或引入第三方库。
- 每次计算使用独立算法与哈希句柄；局部不可复制RAII对象按hash→algorithm顺序释放，避免共享可变上下文、缓存摘要或引入并发同步。未做基准，不承诺系统实现一定比原实现更快；确定收益为移除私有算法维护，并由系统提供分块处理实现。
- 继续32KiB顺序ReadFile；CreateFileA路径／共享标志、原GetFileSize低W32计数、同一bytesRead槽及do-while保持，不借此修改文件失败／大文件行为。仅新增CNG操作失败的必要收尾，文件与摘要句柄均释放并沿现有false→空输出／nullptr通道返回。
- 原EncodeLowerHex和ComputeExecutableDigest_5202E0正文保持；内部16B摘要和输出33B保持。系统MD5最终化负责填充／长度编码／输出顺序，不再二次大小端转换。
- 同步原5个算法入口的函数表状态为“有承接”，明确CNG提供者；原3个文件／文本／模块父入口保留“有正文”。不把宿主承接记成原机器内部实现，也不保留无调用的手写备份。
- 延续本会话build.bat Debug编译验证；不新增／运行测试，不运行客户端、不实际计算或比对文件摘要、不记录JSON台账。摘要正确性以算法识别及官方MD5合同作静态依据，不冒称运行对比或性能验收。

接口依据：[BCryptCreateHash](https://learn.microsoft.com/en-us/windows/win32/api/bcrypt/nf-bcrypt-bcryptcreatehash)、[BCryptHashData](https://learn.microsoft.com/en-us/windows/win32/api/bcrypt/nf-bcrypt-bcrypthashdata)、[BCryptFinishHash](https://learn.microsoft.com/en-us/windows/win32/api/bcrypt/nf-bcrypt-bcryptfinishhash)。已核当前本机Windows SDK的bcrypt.h声明。

COR09验收：替换完成，源码净减144行；文件读取段、十六进制编码和模块公开函数正文静态对照一致，系统句柄逐条失败路径均有释放，CNG链接与MSVC x64 Debug编译通过。原3680入口身份／名称保持，只有上述四项状态由有正文转有承接；其余状态不变。未计算文件摘要、运行客户端／测试或测量性能，不记录JSON台账。

## COR10：移除重复点击追踪模块（2026-10-16）

用户在确认LegacyMouseTracker的用途后要求移除，沿此前SPEC合理性裁决授权执行。这是取消重复点击统计和上报的功能变更，覆盖旧鼠标输入／追踪器SPEC对该链的保留要求；普通鼠标输入仍保留。服务器是否依赖此上报尚未验证，不宣称联机行为等价。

- 删除LegacyMouseTracker.cpp／hpp、CMake条目及全部包含；退休固定／动态节点池、链管理、阈值／容量全局及无消费者的固定池字节观察接口。
- LegacyMouse的左右键按下函数保留按键位、坐标、tick和elapsed的全部更新及顺序；仅移除Track调用、阈值分支、Report(socket, 1)与重置。两个函数仅由WindowProcedure调用且返回值被丢弃，统一改为void，窗口处直接调用，不制造固定成功返回。其他按键、移动、滚轮和坐标映射保持。
- 移除客户端初始化与退出中的追踪器调用。清理LegacyMouse仅为该上报引入的登录／网络依赖，显式包含cstring供现有memset使用。
- tq仍在原位置按两个整数解码，复用既有IgnoreReplyArguments忽略配置，保留消息匹配、解码和分发返回；不新增协议或无效配置存储。Report_512CA0及其余上报消费者保持。
- 原506580／5066D0／5066F0／506720／506760／506780／5067A0／5067F0／50E7B0九项在现有Markdown函数表改记“已排除”，保留3680行身份／名称；两个鼠标按下入口仍为“有正文”并注明本次差异。旧文档保留历史，不生成／更新JSON台账。
- 验收：源码和构建列表无追踪器残留；静态逐字对照普通鼠标状态更新、其余事件函数及共用Report正文；检查tq解码类型与位置、函数表身份和状态变化；执行build.bat Debug完成编译／链接。不新增／运行测试，不启动客户端、不计算摘要。

COR10验收：模块及全部调用删除完成；8个保留源码／构建文件逐字对照确认只有上述改动，普通鼠标状态更新与其他事件、Report正文保持。源码及CMake无追踪器残留，3680行身份／名称保持，只有指定9行由有正文转已排除。build.bat Debug配置、编译及客户端链接成功，退出码0；仍有既有C4701／C4703未初始化变量告警。未运行客户端／测试或验证服务器兼容性，未计算摘要、未更新JSON台账。

## COR11：窗口浅包装精简（2026-10-16）

用户要求评估并移除LegacyWindow中的浅包装，按既有SPEC裁决授权执行。只处理当前四个接口中三项可由调用点直接表达的边界，不取消窗口功能或扩大到其他模块的包装整理。

- 删除PostLegacyWindowMessage及10个调用中的转发，改用PostMessageW；各调用原窗口、消息、WPARAM／LPARAM数值与异步语义保持。两处SceneState返回仍返回Win32 BOOL对应的32位有符号结果，不改PostQuitMessage／同步SendMessage或直接退出。
- 删除ExecuteLegacyShell及唯一调用的转发，登录场景直接ShellExecuteA，保留窗口、空operation、原URL、空parameters／directory、showCommand=1和原忽略返回行为，不改变A系字符串编码。
- 删除SetLegacyWindowText及两个调用的转发；调用点均为仍存活的std::wstring，直接以buffer.c_str()交SetWindowTextW，省去wstring_view到wstring的再次复制。保留文本生成、窗口取样与调用顺序；不以view.data()代替NUL结尾。
- 本次按用户去包装方向，将必要HWND转换与Windows SDK调用限定在8个消费者CPP内，公开业务接口不引入Win32类型；需要处显式包含Windows.h与宏防护，不创建新的等价包装。Shell32依赖由core移至实际调用所属startup目标，不新增外部依赖。
- 保留gLegacyWindow及ShowWideMessage：后者继续为任意wstring_view创建NUL结尾副本，保留父窗口、UTF-16、标志与返回；不把有字符串所有权责任的适配等同于纯转发。LegacyWindow.cpp／hpp继续承载这两项，不额外迁移或改名。
- 验收：旧三符号无源码残留；核13个调用的参数、返回使用、时序及保留正文，仅标题移除冗余分配；函数表没有这三项独立提供者，3680行及状态保持。执行build.bat Debug验证编译／链接，不新增或运行测试、不运行客户端、不计算摘要、不写JSON台账。现有其他工作区修改完整保留，提交只取本轮差异。

COR11验收：三包装删除，10处PostMessageW、2处SetWindowTextW和1处ShellExecuteA接线完成；对8个消费者反向替换后与原正文逐字一致，ShowWideMessage及窗口存储保持，Shell32依赖已随调用迁移。旧符号无残留，函数表逐字不变；build.bat Debug配置、编译及客户端链接成功，退出码0。仍有既有未初始化变量及旧网络API告警；未运行客户端／测试或测量性能，未计算摘要、未更新JSON台账。

## COR12：平台工具归位与FILE别名移除（2026-10-16）

按用户本次要求迁移GetMacAddress_534EF0、直接使用std::FILE并删除两份LegacyStdio.hpp。此前工作区中的MAC函数命名等修改已由用户提交，本次以当前已提交正文为基线，不改适配器算法。

- 本线尚无platformTool.cpp；将LegacyNetworkAdapter.cpp／hpp迁为src/platform/platformTool.cpp与include/cg/platform/platformTool.hpp，保留cg::platform及GetMacAddress_534EF0名。正文除所属include路径外逐字保持，登录场景同步包含路径，CMake在原位置替换源码路径；不留旧转发头，不附带修改分配／释放或MAC选择行为。
- 将25个文件中的79处LegacyFile类型直接改为std::FILE，包括数组、指针引用、二级指针、const与volatile声明；保留全部限定、存储、文件模式、读取顺序、错误返回及关闭时机，不改句柄所有权或结构布局。
- 删除公共LegacyStdio.hpp中的别名和仅作历史说明的注释，删除src/platform/LegacyStdio.hpp转引空壳；移除23处直接包含。使用std::FILE的文件显式包含cstdio，不依赖已删除头的间接包含；宽字符使用所需cwchar已有明确包含。
- 历史CRT提供者由本节与现有Markdown函数表承载，不为未使用服务新建代码包装。函数表15行旧Stdio定位改到本节，26行适配器提供者同步新路径／实际符号，3680行身份、名称、状态保持；旧进度／SPEC保持历史，不写JSON台账。
- 验收：旧别名、旧包含及旧构建路径无源码残留；新头／CPP与调用唯一对应；逐字核对MAC正文，规范化类型与include后核文件业务正文，检查文件指针修饰、包含路径、编码和函数表状态。执行build.bat Debug验证配置／编译／链接，不新增／运行测试，不运行客户端或算法样例，不计算摘要。

退休头中的宿主承接映射保留：53B2F1→std::fopen；53B2D1→::_fsopen；53C8B3→std::fgetc；53B226→std::fgets；53BCCD→std::fread；53C680→std::fwrite；53CFDC→std::fputc；53B356→std::fputs；53CB78→std::ftell；53BC41→std::fseek；53B27D→std::rewind；54ACF1→std::setvbuf；54463F→std::ungetc；54B239→::_ftelli64；54B3C0→::_fseeki64；54ACB7→std::fgetpos；54ACD9→std::fsetpos；54AD92→std::fputwc；54AF70→std::fgetwc；54AE87→std::ungetwc；53BB02→std::fflush；53B1D0→std::fclose；53BB99／53BBA2→::_flushall；5446AD→::_fcloseall。无当前CPP消费者的服务仍仅为已批准的MSVC宿主承接，不计新增业务接线或运行验收。

COR12验收：迁移和别名清理完成，6处补齐cstdio，旧别名／路径无源码残留；MAC正文及类型替换外的业务正文静态核对一致，259条直接本地包含边有效。41行函数表提供者更新，3680行身份／名称／状态保持。build.bat Debug配置、编译和客户端链接成功，退出码0；仍有既有未初始化变量及旧API告警。未运行客户端／测试／算法样例，未计算摘要、未更新JSON台账。

## COR13：启动公共浅包装移除（2026-10-16）

按用户要求移除StartupCommon中的目录、全局内存与调试输出包装，共6个声明／定义、3个消费者CPP中的9处调用；只消除包装层，不改变原有API族或资源生命周期。

- ReadLegacyCurrentDirectory唯一调用直接改GetCurrentDirectoryA，保留256B容量、窄字节目录和原忽略返回行为。
- 两处AllocateLockedZeroedGlobalMemory展开为GlobalLock(GlobalAlloc(GPTR, bytes))，保持原sizeof乘法、清零、失败路径与owner发布时间，不改为CRT分配或增加保护。两处解锁和两处释放分别直接GlobalUnlock(GlobalHandle(pointer))与GlobalFree(GlobalHandle(pointer))；保留各次owner取样、解锁后重读、清空时机和弃置返回。
- 窄字符调试直接OutputDebugStringA(output.data())，保留原格式化字节；宽字符唯一调用已拥有存活且NUL终止的std::wstring，直接OutputDebugStringW(message.c_str())，移除view包装内的重复复制，不以view.data()替代终止文本。
- 删除Startup.hpp对应声明与StartupCommon.cpp对应定义，保留其余启动接口；Win32调用仅进入三个CPP，按既有WIN32_LEAN_AND_MEAN／NOMINMAX约定显式包含Windows.h，公开头不引入SDK类型。仅删除不再需要的包含，不新增包装或外部依赖。
- 验收：六接口及调用无源码残留；反向展开对照9处调用，核分配域／参数／顺序、文本终止和其余正文保持；当前函数表无这些包装提供者，3680行内容与状态不变。仅做静态检查，不构建、测试、运行或计算摘要，不更新历史证据或JSON台账，保留既有Python缓存。

COR13验收：6个声明／定义已删除，9处调用全部展开；5份源码／头文件与限定替换结果逐字一致，原分配／清零、owner读写、解锁／释放和调试消息保持。旧符号零残留，新增Win32包含与本地包含有效，3680行函数表逐字不变；UTF-8无BOM／LF及Git差异静态检查通过。未构建、测试、运行或计算摘要，不更新JSON台账；精确提交本次7份文件，既有Python缓存排除。

## COR14：时间类型与接口直接使用CRT（2026-10-16）

按用户要求移除LegacyTime中的自定义类型与转发，沿既有规格裁决授权执行。当前本机UCRT的__time32_t为32位long，std::int32_t不是同一指针类型；不强转两者指针，也不改用x64默认64位time_t。

- 11处取时改为::_time32，9处本地日历转换改为::_localtime32。取时仍用nullptr参数，原8个输出槽直接接收返回值，保留失败返回的发布。仅用于CRT交互的局部时间变量及网络time62B394／日志timeCB1DF0两个内存槽改为__time32_t，保留32位大小和对齐断言；协议、文件和游戏时间计算仍维持既有S32／W32，时间戳到随机种子的取样及调用序保持。
- LegacyTm53B8C6改用std::tm，旧seconds／minutes／hours／day／month0based／yearMinus1900／weekDay／yearDay0based／dst对应tm_sec／tm_min／tm_hour／tm_mday／tm_mon／tm_year／tm_wday／tm_yday／tm_isdst。当前七段日历字节偏移／memcpy读取按各自原取样顺序改为成员访问，不依赖标准对象布局。
- 网络及日志窗口继续借用共享可变std::tm*，日志窗口的旧日期推进算法、直接修改月／年及之后重读共享指针的顺序保持，不改成每个调用独立快照。存储改由CRT线程缓冲持有，删除项目thread_local副本与九字段复制；同线程其他CRT日历调用覆盖及失败时缓冲内容沿CRT合同，不额外模拟旧私有副本隔离。本线当前没有其他直接gmtime／mktime消费者，未据此声称第三方库内部行为等价。
- 唯一文件创建时间调用内联为::_fstat32(::_fileno(file), &status)，成功才读取status.st_ctime；仍读创建时间而非修改时间，保原首文件失败判断、缺失后续文件行为和fclose顺序。ConvertLocalTime_541160没有声明或消费者，随旧文件移除；原541160的宿主_mktime32身份仅保留在本规格／函数表，不新增业务调用。
- 删除LegacyTime.cpp／hpp及CMake条目，清理全部包含；各使用处直接包含ctime、sys/stat.h或实际需要的标准头。头中仅作历史CRT导入说明的记录迁到本节，函数表定位同步，3680行身份／名称／状态保持，不生成JSON台账。
- 编译暴露SceneState数组初始化内误放cstdint／direct.h：原LegacyTime的间接包含曾令它们被头文件保护跳过。本次将两个头移到文件顶部，保留三个_mkdir消费者，数组数据保持。
- 验收：旧类型／符号／include零残留；检查全部参数、字段映射、共享指针、返回、格式串、日期取样和文件关闭路径；执行build.bat Debug配置／编译／链接。只做静态核对与编译，不新增／运行测试，不运行客户端或算法样例，不计算摘要。

宿主承接记录：原53B8C6→::_localtime32与std::tm；53BA26→::_time32；53CCD0→::_fstat32；541160→::_mktime32（无当前消费者）。旧头的53BDEF／53BE17／53CF50／54AB30／54518B／53C5BE／53DA45／53C649／53C3AC／53C0A9／54ABDD／53D26E／53BFC0／5450F0／53F9E0／544DB0／544DC0／53C790／53BEC0／53C2F0／53BE40／53C654／53DD2F及errno／_doserrno继续沿已批准的MSVC CRT直接承接，现有真实调用不变，无消费者不补调用。

接口依据：[时间戳与32位接口](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/time-time32-time64)、[本地日历与线程共享对象](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/localtime-localtime32-localtime64)、[文件状态与创建时间](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fstat-fstat32-fstat64-fstati64-fstat32i64-fstat64i32)。

COR14验收：4个旧函数、私有日历类型和两文件删除完成；11次取时／9次转换／1次文件状态查询直接使用CRT，38次字段读取按原位置映射，32位大小／对齐断言编译通过。首次构建发现日志辅助调用遗漏和既有数组内错位包含，均已修复；第二次build.bat Debug配置、编译及客户端链接成功，退出码0。55行时间提供者／承接说明更新，3680行身份、名称和状态保持；旧符号与路径无源码残留。其他已提交的网络整理保持，本轮不夹带。仍有既有未初始化变量和旧API告警，未运行客户端／测试或计算摘要，不声称CRT共享缓冲的内部行为与旧私有副本逐项等价，不更新JSON台账。
