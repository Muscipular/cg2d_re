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
