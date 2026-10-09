# CRT 宿主承接总 SPEC

> 2026-10-10当前错误/退出边界：[MC01-C3](cg2d_crt_host_migration_spec.md#msvc-crt-exit-source-state-20261010)已将CRT私有退出/信号/消息与钩子、console及重复filter层交MSVC，删除纯CRT收尾回调/唯一登记；真实业务std::atexit与数组构析保持。两致命入口真实_Exit(255)不输出旧R600x/MessageBox，数组二次异常真实terminate；旧库字段/表与原filter生命周期只作固定历史，不再要求复刻。heap/multibyte前置与所有适用业务/owner/SDK/正常快速退出运行门禁仍待，未编译/运行。

> 2026-10-10当前数值库边界：[MC01-C2](cg2d_crt_host_migration_spec.md#msvc-crt-numeric-source-state-20261010)已退休无现业务调用的旧浮点格式/96位转换/环境与pow错误私有组，数值格式/数学及库内状态交MSVC。实际地图/音频数学源与控制作用域保持，两Shop/NLS低宽读取移回既有模块，计时U64除法保留；旧gate/数学记录/CPU探测和其余内部CPP/ASM源码状态属固定历史，不再要求复刻。全部适用业务/owner/SDK正常结果与运行门禁保持，未编译/运行。

> 2026-10-10当前C++边界：[MC01-C1](cg2d_crt_host_migration_spec.md#msvc-crt-cpp-source-state-20261010)已退休无现组外消费者的六组私有C++库及NativeCppException头。当前采用真实MSVC标准流/Init/locale及库内string/facet/codecvt/异常支撑，现流配置/缓冲借用/刷新、业务构析与退出登记保持。下文旧Legacy对象/方法表/私有登记/异常payload及其源码状态属固定历史，不再要求复刻；完整业务/owner/未知原alias与SDK实际运行仍待，未编译/运行。

> 2026-10-09用户最新CRT要求：当前按[MC01](cg2d_crt_host_migration_spec.md#msvc-crt-direct-decision-20261009)直接采用MSVC。标量C服务、32位时间及字符串格式/扫描已转真实宿主提供者；下文旧算法/共享错误槽/逐指令保持条款在该具名范围只作历史。FILE/lowio及C++对象剩余组仍处迁移期，业务格式/编码/字段、owner和全部适用运行门禁保持。

> 2026-10-09最新通用堆合同：[HMU01](cg2d_heap_malloc_unification_spec.md#heap-malloc-source-state)已按用户malloc方向与第8条工程委托将全部通用模式统一malloc/calloc/realloc/free/_msize，初始化借用真实CRT句柄；原模式只保请求数量策略。具名覆盖下文旧阶段“模式1/2、私有池及元数据保持”，12旧池接口/8布局类型/10状态槽已退役，原身份及旧实施证据保留为历史。裸字节及其它独立域保持；完整owner/间接alias/SDK/正常寿命与C01/C08/C11/V02仍未闭，当前未编译/运行。

> 2026-10-09最新数值/源合同：[ACP01-C](cg2d_asm_to_cpp_migration_spec.md#asm-cpp-crt-source-state)已取消本组ASM/私有ST0机器ABI，使用CPP/double/标准数学与宿主浮点环境承接；旧x87/PC53/T80/FLAGS数值保证由用户最新许可覆盖。下文原指令、ASM实现与机器状态证据属固定历史；原业务字段/整数读取与错误记录/errno/系统回调保持，当前未编译/链接/运行，完整验收未闭。

> 2026-10-02提案；2026-10-04依用户本轮“未决议内容你做决定”授权，由本线程采用下列受限宿主承接方案。**已决议，尚未迁移/验收**；不是原CRT逐指令等价。
> 压缩前全文见Git命名快照 `cg2d-re-spec-history-20261004-before-closure-compression` 的本路径；后续见[收尾C01–C12](../plan/cg2d_spec_closure_checklist.md)。

## 已采用裁决

经实际调用/数据归属证明确认为C/C++运行库的底层依赖，由本机MSVC/UCRT/STL承接，接受下表库内实现差异，不逐指令重建旧x86 CRT。既有真实业务/兼容层/共享数据保留，不借此删除或回退已还原实现。

本轮明确接受下表CRT私有实现边界差异，授权与分项决议见[决议总SPEC](cg2d_closure_decisions_spec.md)。原2026-09-27一般委托/四入口许可不曾自动批准此提案；本轮新授权不改变历史状态。业务契约和完整运行门禁保持。

## 已有证据支持的差异范围

| 范围 | 已知原/宿主差异 | 本轮接受的库边界 |
|---|---|---|
| FILE/低层文件 | 原静态流20/3、条件关闭与宿主无条件收尾、缓冲/描述符发布、锁和部分错误返回不同 | 宿主FILE/描述符寿命及库内错误处理，不复刻旧私有流表 |
| 格式化 | 原支持%n而宿主默认禁用；精度163/512、宽串转换失败计数、浮点机制不同 | 宿主格式/错误合同，不全局启旧选项或重建输出引擎模拟旧版 |
| 单字符/多字节转换 | 旧locale/代码页与宿主分离；宿主UTF-8、清零、校验/续接状态不同 | CRT内部用宿主转换状态；应用协议/资源仍按原字节 |
| C++私有实现 | 字符串/流/容器布局、分配与共享不同 | 用宿主标准库对象，但应用实际借用/别名/寿命逐项保留 |
| 编译器与异常框架 | x86栈步长、寄存器返回、SEH/C++支撑与x64不同 | 本机编译器/运行库提供，业务位宽和真实指针保留 |
| 浮点启动/格式链 | 原trap槽、P5探测、FNCLEX和私有转换链 | 宿主服务，不复刻旧CPU探测/私有槽；已批PC53业务合同保留 |
| CRT私有内存 | 堆表、分配序、错误/异常和释放策略不同 | 标准库内部归宿主；已还原应用分配协议/owner保留 |

证据：[stdio](cg2d_stdio_contract.md)、[多字节](cg2d_legacy_multibyte.md)、[启动/浮点](cg2d_startup_flow.md)、[内存](cg2d_crt_memory_contract.md)。这些是真实差异，不是等价证明。

## 始终保留的业务边界

1. 逐项有入口/调用/数据归属证据，不按地址段、名称或IDA库标志批量归CRT，也不自动包含第三方/业务。
2. 保留业务参数、格式、调用序、真实返回的消费、资源owner、RNG及非地址字段位宽；不加默认值/保护/重试、固定成功或吞错误。
3. 接受宿主库错误/终止/编码状态不等于可丢业务读取的旧槽；直接可见状态必须有明确适配，不能永久留默认值。
4. 线上封包、资源/存档与已证正常业务结果继续原合同；宿主差异破坏这些边界时局部兼容或保留原实现，不准协议变形、丢字、换编码或改数值。
5. LegacyTime/LegacyStringCompare/LegacyMultibyte/LegacyMemory等已有兼容实现保留，不为了统一宿主而删除。
6. 六补值、4626A0 gate、5202E0路径差异另由D01具名裁决；网络按用户最新要求原格式明文传送；未知容量/生产及缺失资源仍取证，音频暂停和两加解密叶排除不变。
7. 不引入依赖，不解除不构建/测试/运行/计算摘要要求。

## 子 SPEC 与实施顺序

按[启动与生命周期子SPEC](cg2d_crt_startup_lifecycle_spec.md)处理宿主入口、原表分类、单owner构析及图形双树；按[运行库边界子SPEC](cg2d_crt_runtime_boundary_spec.md)处理堆模式3、异常/ThrowInfo、FILE/格式与codecvt。保留原身份/调用关系，不把每个旧内部槽强制当新链接目标；真实提供者/业务冲突仍逐项处理。

优先接口、真实数据与接线，按可验证小步提交；已有string/streambuf/filebuf/ios/locale、原堆/异常数据及父源码不回退。C++90表/四Init/登记按活动提供者分类，模式3和异常私有内核按宿主职责承接；已有兼容路径、业务可见状态、避免双执行和分配域迁移仍须闭合，不能提前关闭。

## 状态与验收

本轮只完成决议和拆分，不改变实现/链接/运行状态。活动提供者未接、分配域/错误/异常/业务结果未核均保持未完成；库内差异按获批替代单列，整体仍须总SPEC所有适用门禁。

## MFA01 保留专用幂路径兼容边界（2026-10-05）

[两原活动点](cg2d_map_effect_arithmetic_spec.md#map-effect-arithmetic-decision)确认46F1D0的底数恒F64 1.25、指数signedLevel−1；现有Cg2dMapEffectDivisor按场景§20.19以x87承接53D000→53D022正常正底数路径，本步保持其不落F64和PC53/控制字，不能因D02擅换std::pow或造同名包装。

两原ID只登记实际正底数内联路径的adapted，不记整个pow宿主迁移或原一般数学内核等价；53D019/负底数53D1F5及未知间接调用、错误/浮点环境/实际ABI与业务RNG结果仍未闭。原内核含54348E共享尾块、53D019→53D022是flow，不猜成直接call或新的业务边。

## BPJ01 实际专用幂帮助体的独立基线（2026-10-07）

[BPJ01](cg2d_map_effect_arithmetic_spec.md#pow-twotos-source-state)将原__twoToTOS/5433F0完整11条21B与唯一53D071调用独立纳管，映射到既有Cg2dMapEffectDivisor十条x87内联。归属adapted/CRT，非host或范围排除；只现有1.25底数/S32指数的专用正常域，源码与PC53/连续结果消费保持。

CRT149→150枚举仍未闭，单一局部S不代新增实现、一般_pow/负底数及其余浮点私有链、全环境/机器ABI和场景RNG运行。D02/MFA01其余边界继续有效。

## BPM01 专用幂输入及控制字身份补齐（2026-10-07）

[BPM01](cg2d_map_effect_arithmetic_spec.md#pow-input-control-source-state)补543405/543435完整27条90B、五call/两父邻窗及既有两个1.25活动点的取参依据，两项CRT/adapted。原默认027F绕过543405，当前固定PC53只正常阶段；543435只53D04C活动域，特殊F80及其余三点保原证据，不称泛用源码已实现。ASM/头/场景源保持。

CRT152→154枚举仍open，E728单一局部S只证明身份/来源。一般_pow/负底数和其余浮点链、全部环境/间接alias/owner寿命、实际机器ABI与场景RNG及运行仍未闭；D02/MFA01/BPJ01的边界及完整验收标准继续有效。

<a id="msvc-crt-direct-decision-20261009"></a>
## MC01：CRT直接采用MSVC（2026-10-09）

用户最新指令“CRT 直接用msvc代替”是本轮范围授权，覆盖旧阶段必须保留自实现LegacyTime/StringCompare/Memory/格式核心及逐指令重建CRT私有设施的要求。当前工程统一采用已安装MSVC的UCRT、VCRuntime和STL；旧CRT身份、原字节及实施证据保留为历史，不再因其私有算法、旧CPU/SEH/堆/FILE/locale表未复刻而追加开发债务。业务函数、第三方和来源不明项不按名字或地址段批量算作CRT。

调用方所需的旧函数名可保留为薄适配入口，实际操作必须委托真实MSVC服务；不复造旧CRT私有内核、返回固定成功或强转旧对象为宿主对象。宿主参数校验、错误、locale、线程局部状态、缓冲与格式实现及数值结果采用本机合同；不全局开启旧%n或模拟旧机器故障。CRT内部差异记为获批替代，不称原体等价。已有double/标准数学许可保持，原业务F32及非地址字段不扩宽。

线上协议、资源/存档结构、原业务输入字段/调用序与真实返回消费、RNG调用数量及资源owner仍按业务SPEC；转换类型/分配域须整组迁移，不能混用宿主块与旧释放器或旧FILE字段。业务可见字节与正常结果若受库替代影响，按具体来源核对，不自行改变格式串、字节编码、字段或补值。MSVC的rand系列与原业务RNG分开，不借CRT替换重置或改变业务随机序列。

实施分组：

1. MC01-A先迁移标量C服务：字符串/比较/解析/内存、目录、32位时间和字符串格式/扫描；旧错误槽访问改实际线程的errno/_doserrno，不保全局影子值。时间仍用_time32/_localtime32/_mktime32；业务九S32日历对象逐字段适配，不强转std::tm，不改32位时间字段。裸字节分配/释放成对采用malloc/free。
2. MC01-B成组迁移FILE/描述符、格式流、缓冲和相应业务字段消费，再退休无消费者的旧stdio/lowio/浮点格式与扫描内部核；不得只替换fopen而保留旧缓冲/flags读取或旧fclose。
3. MC01-C收敛C++字符串/流/locale/异常与初始化退出设施到MSVC对象和编译器；业务初始化/退出回调独立保留，避免双构造、重复注册或过早清理。已由宿主承接的堆、标准流、throw/catch不重做。

这三组是实施次序，不是缩小用户要求。尚未迁移的现提供者只能作过渡状态，不能继续写成必须永久保留的设计要求；每组同步真实提供者/当前语义/功能与任务登记。全CRT迁移未完成前不得宣布完成；当前不构建、测试、运行、查产物或计算摘要，实际链接/运行及全部适用业务门禁仍待授权验收。

<a id="msvc-crt-scalar-acceptance-20261009"></a>
## MC01-A：静态核对范围

确认每个变化入口只调用对应MSVC服务或做必要类型/宽度适配，原调用方格式串和数据写入序保持；宽度和指针借用不以原布局强转。错误访问每次查询当前CRT线程，不在模块初始化时缓存TLS指针。对旧解析、字符串复制/复读、时区缓存及分词续读实现确认无当前消费者后退休，原身份转宿主职责或适配并保留之前记录。

内存复制采用memmove以支持重叠；比较直接消费宿主返回符号，现已知caller的零比较保持。字符串格式用vsprintf、bounded用_vsnprintf，采用MSVC容量耗尽/返回合同，不改成会返回理论长度的vsnprintf；扫描用vsscanf，不补未写目标。目录用_mkdir/_chdir，截图目录已存在判断从同一CRT errno读取。原参数无额外保护、重试或默认值；宿主内部校验和标准异常语义按本决议。

限定静态证据只证明源码和接线，不能替代当前版本的编译、链接、格式/时间/目录结果、owner与完整业务运行。C01、C04、C08、C10–C12及V02/V03保持开放。

<a id="msvc-crt-scalar-source-state-20261009"></a>
## MC01-A 当前源码状态

33个入口已核，其中GetEnv/Atol/Malloc/Free四个此前已交宿主；本次29个入口转MSVC：13个字符串/解析、三内存、三长度/忽略大小写比较、两目录、三32位时间/日历、三字符串格式/扫描、裸字节分配释放两项。旧接口名只是实际服务和定宽适配；不创建原地址跳板。八无当前消费者的解析/数字转换/时区日历/私有比较辅助退休，旧F身份和实现前态保留。源码及SDK声明详见[完整前后态证据](cg2d_msvc_crt_scalar_evidence.json)。

errno/_doserrno每次返回MSVC当前线程真实槽，所有现读写成组替换，截图目录已存在判断同源；不缓存TLS指针。日历九S32逐字段复制，线程局部借用不强转std::tm；time32参数用本机准确类型局部承载，结果保持S32。裸字节malloc/free配对转移，不再使用HeapAlloc/HeapFree。原调用方格式串、输入字段、RNG和协议字节无本步改动。

FILE/lowio仍用过渡期旧类型与表，旧流格式核心/扫描核心和C++库对象尚未全部迁移；因此MC01整体未完成。SDK头核到真实入口声明及stdio inline委托，未构建/链接/测试/运行/查产物/计算摘要；只登记两限定I/S，完整CRT、owner、协议及正常寿命门禁未关闭。固定引用cg2d-re-msvc-crt-scalar-20261009。

<a id="msvc-crt-file-decision-20261009"></a>
## MC01-B：FILE、描述符与流格式整组迁移

依据用户CRT直接采用MSVC指令及MC01分组，当前全部LegacyFile指针改为实际std::FILE指针的类型别名；规范声明落在公开platform头，旧私有头只转引。业务/资源对象只借用该指针，不读或强转MSVC FILE内部布局；旧32B/现48B FILE、20/512索引表、8B/16B描述符记录、手工CRLF/Ctrl-Z/缓冲表与锁归MSVC，不建新的影子表。

打开/共享打开、读行/字节/元素、输出、刷新、位置/回推/宽字符和关闭直接采用fopen/_fsopen、fgets/fgetc/fread/fwrite、fputs/fputc/fflush、ftell/fseek/_ftelli64/_fseeki64/fgetpos/fsetpos、ungetc及宽字符服务；流格式/扫描核心改vfprintf/vfscanf，不经旧状态机/浮点格式六槽。只保留有当前消费者或真实MSVC操作的接口，旧Acquire/缓冲分配/临时缓冲/格式字节私有核退休，不定义空初始化器或固定成功体。全体刷新/关闭直接调用_flushall/_fcloseall，采用真实返回与本机收尾范围。

旧FPosition两个W32字段按实际MSVC fpos_t的8B数值逐半转换；目标只在fgetpos成功时发布，不强转旧对象或伪造失败位置。公开描述符消费用_open/_filelength/_close，原数值flags/permissions和业务返回消费不变；私有句柄/继承块/descriptor表初始化及读写算法不再复刻。auto.dat创建时间用同一个打开FILE的_fileno交_fstat32，成功才发布ctime，S32保存字段不扩宽。

原EOF/错误位的业务读改为feof/ferror，保持原条件、查询时点与分支顺序；实际读取/格式串、资源/协议/存档字节、偏移与元素宽度、失败消费及RNG不改。窄旧filebuf不再绑定FILE内部游标，保自身真实游标槽，读写经公开FILE服务；其完整C++类型/codecvt迁移仍按MC01-C继续。旧标准FILE数组引用改为直接stdin/stdout/stderr，库拥有其生命周期，不复制FILE或增fake槽。

MSVC入口已完成lowio/stdio初始化：撤下实际模块初始化中的重复调用，同时退休无当前调用的旧私有进程入口/CRT初始化器及C表，不把旧x86私有入口接到新客户端；业务C++初始化/真实WinMain、窗口/主循环/资源回调和现异常过滤器保持。原入口、原表及所有身份留在迁移前Git证据，不能按名称或无xref扩大业务排除。

FILE一经fclose即失效，包括刷新失败的返回路径。当前已证日志页日期切换先关闭流、再调用会再次关闭该全局槽的重载函数；迁移时清空已关闭owner槽，防止把已释放MSVC对象再次交fclose。动画按关闭标记收尾的槽和过渡filebuf也在关闭后释放其FILE引用；关闭返回仍据真实MSVC结果消费。这里只处理已证拥有者配对，不额外跳过未知失败或造新生命周期，旧静态FILE地址关闭后仍可读的库内机制由宿主取代；具名覆盖旧保留关闭槽的内部兼容要求。

宿主CRT的errno/locale、容量/位置、共享与校验/错误/关闭机制按MC01接受；不全局开%n或旧格式选项，不改变调用方格式串或补未写参数。格式结果、文件状态和普通/快速退出、全部owner及正常业务运行依总SPEC适用门禁待验；当前不构建/测试/运行/查产物/计算摘要，单线程。

<a id="msvc-crt-file-acceptance-20261009"></a>
## MC01-B 静态核对

逐项核唯一当前FILE别名、所有前置声明/字段读和标准流引用；全体提供者与业务读/写/EOF/错误/位置/关闭均属同MSVC域，旧文件/描述符表及初始化无调用或假体。核SDK实际API声明、fpos_t/time32宽度及创建时间成功发布，所有退休源码符号和台账引用；保留用户改动。日志切换的两次关闭用真实已证调用链说明，清空owner与返回消费核到具体源点。完整功能/业务、SDK实际链接/运行及六closure不因静态迁移关闭。

<a id="msvc-crt-file-source-state-20261009"></a>
## MC01-B 当前源码状态

全部现文件指针已统一为规范公共platform头的std::FILE别名，四原struct前置声明与旧私有头已对齐。33实际服务中字符串扫描此前已交MSVC，本次32项文件/流格式/描述符/ctime服务转真实宿主；所有原FILE EOF/错误字段消费已改feof/ferror，六标准流构造引用直接stdin/stdout/stderr。fpos_t按SDK实值__int64保8B数值适配，业务两个W32及创建时间S32不扩宽，fgetpos/_fstat32成功才发布；[_fileno/FILE/描述符前后态及关闭链](cg2d_msvc_crt_file_evidence.json)可追。

39私有定义退休，含库内缓冲/表/格式字节/低层句柄读写和旧进程入口；其中共享calloc只退休stdio索引表消费者，现通用业务分配保留，53DE8C仅原入口片段、不造新F。37原私有F职责登记host，不造同名函数或原地址调用；其实际服务来源只表示库职责承接，不宣机器ABI一致。93现语义来源/规范声明/旧caller与当前提供者对齐，完整前态沿迁移前固定Git快照及原索引读取，不复制历史链。

旧私有进程入口源与CMake源项/C表撤下；实际MSVC WinMain、全部业务C++模块初始化/回调和现filter保留，停止手工lowio/stdio重复初始化。日志页日期事件关闭后清空owner再重载，动画按标记关闭后清空FILE槽；过渡filebuf的fclose失败也释放已失效FILE引用，同时返回真实失败。窄filebuf不再借库内游标，其完整C++对象/facet/codecvt迁移继续沿MC01-C，不把现过渡类型当永久设计。

原格式串/字段/协议/资源/存档/RNG未改，未知原caller/间接alias和完整owner/业务/SDK运行仍待。两限定I/S仅当前源码/接线静态，完整功能与总SPEC门禁保持；未构建/测试/运行/算法/查产物/计算摘要，无新依赖。固定引用cg2d-re-msvc-crt-file-20261009，MC01整体及总SPEC仍未完成。

<a id="msvc-crt-cpp-decision-20261010"></a>
## MC01-C1：C++库对象与私有异常整组收敛（2026-10-10）

按用户CRT直接采用MSVC指令，退休LegacyNarrowString、LegacyStreamBuffer、LegacyFileBuffer、LegacyIos、LegacyLocale、LegacyException六组私有CPP/头及NativeCppException头。当前源码的库导出函数、类型、共享槽和方法表没有组外消费者；组外只剩LegacyMouse的旧头引用，以及供旧控制器展开使用的UnwindLegacyStandardStream适配。现活动标准流已使用MSVC完整对象，晚段真实ios_base::Init和locale::classic提供者已接，不再保两套私有对象/计数/登记/facet/codecvt/异常payload来模拟旧库。

真实std::cin/cout/cerr/clog及四宽流、库拥有的缓冲/locale/异常机制由已安装MSVC STL与VCRuntime持有。原应用配置仍按现源码设置tie、error的unitbuf、log借用error缓冲，并按原无sentry的pubsync顺序刷新；晚段Init、经典locale查询、所有业务静态初始化和std::atexit登记保持。删除旧头引用和失去唯一消费者的展开适配，不删除真实单RET的54843B退出回调或改变其登记次数；不得把未调用的旧私有函数改为同名空体、固定成功或SDK对象强转。

旧string共享U8/容量、streambuf/filebuf游标和虚表、ios私有节点/计数、locale引用/codecvt及六自定义异常payload/类型描述符/捕获表归宿主库内部。现组外无这些对象的读取、抛出或捕获，整组迁移不新增业务异常类、消息副本或新throw；实际MSVC消息、分配、校验、复制、展开与生命周期采用本机合同，接受原私有ABI和内部结果差异，不称逐指令等价。原机器身份、原调用/字段证据和全部实施前态沿固定Git快照保留，未知原端间接调用事实不据当前无消费者判为不可达或排除。

共享裸字节分配/释放、实际数组构析、业务字段/参数/返回消费、RNG及协议/资源/存档字节保持。六库删除仅退休这些共享服务的旧库消费者，不扩大到其他提供者；旧浮点格式/数值、剩余多字节与初始化/错误/退出设施继续MC01-C后续小步，不能把旧CRT私有内核未复刻列为永久开发债务。

历史源码附件按本步迁移前固定Git快照核存在；汇总器仅为明确列出的退休附件增加历史路径解析，当前语义来源必须改为实际MSVC职责边界或仍存在的业务提供者。原功能分母、身份、原证据、历史构建与运行记录保持，历史引用不等于当前可调用源；不降低缺源码/身份的校验标准。当前不构建/测试/运行/查产物/计算摘要，无新依赖，全CRT与总SPEC仍未完成。

<a id="msvc-crt-cpp-acceptance-20261010"></a>
## MC01-C1 静态核对范围

逐项核退休函数、公开类型、数据与方法表的组外消费者及include/CMake残留，核当前MSVC完整标准流/Init/locale提供者与SDK声明。对LegacyMouse只删除旧include和展开适配；其余应用初始化、guard、即时登记、两个实际空退出回调、流配置/借用/刷新及业务代码逐字保持。全部现语义、共享消费者和功能/任务登记对齐；历史附件只查固定快照可读性，不允许当前映射仍指向退休对象。库内替代限定I/S不关闭完整业务/owner/SDK/机器ABI/运行门禁或六closure，§10标准保持。

<a id="msvc-crt-cpp-source-state-20261010"></a>
## MC01-C1 当前源码状态

六组私有库的六CPP/六头和NativeCppException头共13文件/5209行已退休；CMake删除对应六源项，LegacyMouse仅删除旧include和无现消费者的两参数展开适配。全部业务源、两个实际单RET退出回调/guard/登记次数、八真实标准流及tie/unitbuf/log借用/pubsync顺序、真实晚段Init与经典locale保持。源扫描与[前态/精确差异证据](cg2d_msvc_crt_cpp_evidence.json)证明退休导出函数/类型/数据/方法表和native_cpp命名空间无现组外消费者；未知原端间接调用不判不可达。

205个地址具名定义覆盖201入口/200原库存ID，54A577只原共享片段、不造新F；六库不再定义或保存旧库对象、虚表、locale/ref/codecvt、异常payload和捕获元数据。202库职责原ID按实际边界登记195 host/7应用流配置刷新adapted；共享裸字节两入口和30共享/旧进程记录仅退休旧库消费者，现业务/服务提供者保持。232现语义对齐，完整前态以固定Git标签/路径/原索引读取；116已知CRT成员补归属，完整枚举仍false。

语义2029（1981F＋48X）、合同2091保持；已审F1215→1366，软件证据1329→1331。两限定I/S仅当前源/职责与接线；全部完整feature门禁、184叶整项验收0、任务关闭11/60及六closure保持。汇总器仅为13明确退休历史附件核固定Git快照，当前语义source不使用该回退；SDK真实类型/标准流提供者、应用精确差异及原台账/§10守恒通过静态核对。

旧浮点格式/数值、多字节及剩余初始化/错误/退出/堆策略继续MC01-C后续；业务数组/SEH/owner、完整正常结果、SDK生成ABI与当前链接/运行仍待。未构建/测试/运行/算法/查产物/计算摘要，无新依赖。固定引用cg2d-re-msvc-crt-cpp-20261010，MC01全CRT与整体SPEC未完成。

<a id="msvc-crt-numeric-decision-20261010"></a>
## MC01-C2：浮点格式、数值与旧环境整组退休（2026-10-10）

用户CRT直接采用MSVC及既有double/标准数学许可覆盖旧私有浮点文本/96位转换、CPU FDIV探测、控制字抽象、pow错误分派/记录及异常内部核。全当前src/include核到：LegacyFloatFormat/Numeric/Environment/Math组只有Shop命令W32读取和成功多字节映射的双BYTE读取仍有真实组外调用；计数BYTE桥只有旧声明，MC01-A实际计数已直接strnlen。其余函数、类型、槽/描述符与初始化都无当前组外消费者，LegacyRuntimeExit仅有未使用的旧头引用。退休四CPP/三头及该引用，不保私有表、未生产gate、88B/40B数学错误对象或同名空体来模拟MSVC内部。

实际地图/音频CPP仍直接使用MSVC标准数学及各自控制作用域、intrinsic截断，所有常量位样本、业务F32、double中间值、整数读取/写回序与RNG保持；本步不改这些源。流/字符串格式及扫描已用vfprintf/vsprintf/_vsnprintf/vfscanf/vsscanf，数值格式/解析与错误/locale/浮点状态按真实MSVC合同，不全局开启旧选项或补格式参数。旧六浮点callback/trap/null文字槽和无消费者的CountedFormatString退休，原浮点trap及其共享尾片段只保历史身份，不制造当前调用。

Shop读取移到现有LegacyFormat.cpp，声明与定义统一实际const int32_t*；仍由__movsd只复制一个DWORD并按原W32位样本返回，不直接求值未生产整数或补值，P07仍未解决。映射双BYTE读取移到现有LegacyMultibyte.cpp，保先首BYTE再次BYTE的两个__movsb和首值<<8再加次值；调用方、NLS系统返回和结果消费不改，不新增文件或业务流程。计数桥实现/声明一并退休，相关旧ASM注释更新为已存在的strnlen合同。

LegacyFormat中的U64乘法、移位及余数三个helper无现消费者，随旧数值组退休；MainLoop计时仍实际调用的U64除法完整保留原参数/返回、正文与调用点，不借统一库删真实业务依赖。共享内存/errno/格式入口仅退休旧数值消费者；当前MSVC错误槽与其余业务服务不变。未证原端间接调用不据当前无消费者排除，原函数/区间身份和完整原证据/实施前态保可读Git快照。

普通pow的旧gate、数学记录/RaiseException与手工控制状态现在没有业务调用；沿MC01交MSVC，不再要求维持这套库私有逻辑，具名覆盖ACP01-C中永久保旧错误链的阶段条款。当前真实业务控制/状态读取或系统回调若另有消费者仍沿相应业务合同，不把本组覆盖扩大为全部SEH或业务错误处理删除。单线程、不构建/测试/运行/查产物/摘要；多字节和初始化/错误/退出/堆策略余组继续，全部适用总SPEC门禁与运行验收保持。

<a id="msvc-crt-numeric-acceptance-20261010"></a>
## MC01-C2 静态核对

逐项核全体旧组导出/数据/头引用与两真实读取消费者，实际业务数学源、五格式/扫描服务及计时U64除法逐字保持；新读取定义/原声明一致，DWORD和逐BYTE intrinsic的宽度、顺序及参数借用不变。核退休源/CMake/slot/type/trap/helper与全部现语义来源，历史附件仅按固定快照核可读性，不放宽当前source校验。原ID/区间身份、业务字段/参数/返回/原证据与全部任务/完整门禁守恒，限定I/S只证本步源码与接线，不能替代SDK/业务/运行或全CRT完成。

<a id="msvc-crt-numeric-source-state-20261010"></a>
## MC01-C2 当前源码状态

四CPP/三头的旧浮点格式/数值/环境组共7文件/2460行已退休，CMake撤四源项、运行时退出源撤一个未使用头引用。106旧定义退休，含已由strnlen替代的计数BYTE桥、浮点trap/六slots与三个无消费者U64帮助体；MainLoop真实U64除法正文和调用点保持。Shop DWORD读取和成功NLS双BYTE读取移到LegacyFormat/LegacyMultibyte，前者const int32_t*声明定义一致并仍只__movsd取1 DWORD；后者保两个__movsb首后次及首<<8+次。原计数桥声明退休，过时ASM/循环复读注释改实际宿主strnlen合同。

旧库其余导出/数据/类型/初始化没有现组外调用，不保第二套FDIV探测/默认PC/旧gate/96位数值/错误记录/描述符/RaiseException链；实际MSVC库的格式、数学和错误状态内部按MC01，原事实与实施前态可追。实际地图/音频CPP、各自control作用域、常量/F32/发布序、四格式/两扫描提供者与真实计时源逐字保持，未改协议/资源/存档/格式串/RNG、未补Shop未生产命令，P07仍未解决。完整前态和[精确差异/消费者证据](cg2d_msvc_crt_numeric_evidence.json)以固定Git引用读取，未知原调用不作不可达/排除判断。

107现语义对齐：88私有F host、8已有应用数学/控制关联adapted、10共享记录仅移旧消费者，一545038原X尾片段保持身份并只关联真实宿主格式职责边界、不造F/当前尾体。45既有原ID补处置与CRT已知成员：F1366→1411、CRT421→466且枚举false；两限定I/S使软件证据1331→1333。语义2029（1981F＋48X）、合同2091/3680、所有原分母与完整门禁、任务关闭11/60及六closure保持。

退休引用/现真实读取/准确签名、业务数学与服务逐字守恒、SDK实际intrinsic/数学/格式声明、原身份/所有台账和门禁/政策、UTF-8/LF/差异与静态汇总/-Check核对；未构建/测试/运行/算法/查产物/计算摘要，无新依赖。固定引用cg2d-re-msvc-crt-numeric-20261010；剩余多字节、堆策略及初始化/错误/信号/退出设施继续MC01-C，全部业务/owner/SDK及适用运行待。MC01全CRT与整体SPEC未完成。

<a id="msvc-crt-exit-decision-20261010"></a>
## MC01-C3：错误、信号与私有退出设施交MSVC（2026-10-10）

按用户CRT直接采用MSVC要求，退休无现应用调用的旧onexit登记/扩容/容量表、normal/quick/callable调度/终止表和标志、四旧OS历史槽、信号表及raise/filter私有实现、错误消息表/窗口API缓存/banner/process-exit/terminate钩子。现应用构析登记已实际使用std::atexit，业务初始化/退出回调、登记次数与原相对序保持，不建第二套回调表或同名空体。

MSVC本机exe_common.inl的pre_cpp_initialization实际先调用__scrt_set_unhandled_exception_filter，utility_desktop.cpp由库筛选MSVC异常并调terminate。当前LegacyMouse再次手工安装/恢复的旧filter与仅由它持有的前任槽/指针验证退休，直接由MSVC入口/编译器持有异常支撑；不调用SDK私有函数或补本机异常记录。两旧console槽位于匿名namespace、仅初始化为-2并由CloseLegacyConsole读取，没有当前生产/拥有者；与该私有关闭器一并退休，实际stdio/descriptor与控制台资源归MSVC。

FinishHostedLegacyRuntime仅清旧callable标志、刷新stdio、关上述私有console及恢复旧filter，属于纯CRT层；删除它及唯一std::atexit登记、手工filter安装，不留下空收尾函数。stdio最终刷新/关闭与库内部normal/quick/callable清理由MSVC提供。具名覆盖HFI01/HPI01/HSI01在本步filter/退出范围的重复私有初始化/清理要求；不删除真正的业务资源回调、标准流应用配置/刷新、世界/图形/动画/窗口/音频清理或改变SDK实际正常/快速退出合同。

CommandOptions三个分配失败点仍传原9/9/8，堆初始化失败仍传原28；两实际错误适配入口直接std::_Exit(255)，准确标noreturn，保现致命失败与退出码255、不跑普通atexit、不伪造成功或补已失败数据。旧诊断编号只保callsite参数/历史原事实，不再解析19槽R600x消息、调用MessageBox或旧钩子；std::_Exit本身不输出这些旧文字，此库内诊断差异明确接受，不能称消息/终止机制等价。未调用的旧abort入口退休；仍由实际数组二次C++异常消费的TerminateLegacyRuntime直接std::terminate，SDK负责其handler/abort/信号/最终终止，不模拟旧信号子类型或吞错误。

真实三数组构造/析构/展开及代码筛选叶保留原S32数量、U32字节步长/乘积、实际callback地址、正序构造/逆序析构、完成/剩余前缀和SEH传播；只替换其下游terminate库服务，不改本体或三组业务caller。残留LegacyRuntimeExit文件只承载这些实际数组边界，LegacyRuntimeError只剩三真实MSVC适配。所有退休F/区间身份、原数据/原分支和实施前态保持固定Git证据；未知原调用/alias不据当前无消费者排除，完整业务/owner/SDK/运行与总SPEC§10不关闭。

<a id="msvc-crt-exit-acceptance-20261010"></a>
## MC01-C3 静态核对

核全体CRT函数/导出类型/槽的当前消费者与所有实际std::atexit登记；核SDK实际终止声明、静态初始化前filter安装及宿主筛选源码。数组四正文、三组业务caller/真实资源回调、标准流配置/刷新、命令行参数及失败点逐字保持；LegacyMouse仅删除纯CRT回调/登记/手工filter及不再使用的include。核旧private符号/布局/表和当前语义来源，历史信号附件只核固定快照可读性，现来源不得回退。限定I/S只证本步代码/接线，不替代SDK生成ABI、硬件SEH、正常结果、所有owner及实际启动退出运行；继续多字节和堆策略余组，不恢复构建/测试/运行/产物或摘要权限。

<a id="msvc-crt-exit-source-state-20261010"></a>
## MC01-C3 当前源码状态

旧信号CPP/头与CMake源项退休；错误/退出提供者从31定义及纯CRT FinishHosted回调收敛为三实际MSVC终止入口和四数组/筛选正文，25定义退休。无当前生产的两个匿名console哨兵、旧OS四槽、onexit/termination表和标志、19槽R600x消息、API缓存/各钩子与信号数据不再保存；MSVC入口实际在pre_cpp阶段安装默认filter，应用不再覆盖/恢复旧私人filter。LegacyMouse只删纯CRT callback/唯一atexit及安装filter和三未用include，当前heap(0)/失败28、multibyte前置和其余完整业务/SDK流初始化登记逐字保持。

ExitLegacyRuntimeError与FastLegacyRuntimeErrorExit准确noreturn并真实std::_Exit(255)，CommandOptions9/9/8及heap28调用/失败分支不变；旧编号不再进入R600x/MessageBox/banner/hook，_Exit本身不输出旧诊断，此差异获批。TerminateLegacyRuntime真实std::terminate只供实际数组二次C++异常，不用旧terminate槽/abort/raise代码，SDK管理真实handler/信号。三组数组/四正文、callback/S32数量/U32步长和乘积、正逆序/完成剩余前缀/SEH传播与全部实际业务回调保持，不能将其作为无消费CRT删除。

[准确前后态/消费者与SDK证据](cg2d_msvc_crt_exit_evidence.json)保存私有层分类、MSVC public声明/标准转引及exe_common/utility_desktop实际安装/筛选，不调用SDK私有名或接旧x86入口。62现语义同步，21私有F host/3真实终止适配/37共享或业务仅移旧消费者，53DECD原X尾部保原身份/基线并关联真实_Exit职责、不造F/当前尾体。业务原登记机器target/callsite保留，当前明确std::atexit服务，旧完整映射沿标签/原索引可读，不把原地址当x64调用。

F1411→1429（新18）、CRT已知成员466→486（新20）且枚举false；软件证据1333→1335只限定I/S。语义2029（1981F＋48X）、合同2091/3680、所有原分母/完整功能门禁、184叶整项验收0、60任务关闭11及六closure保持。两旧信号附件仅历史路径核固定快照，当前source独立校验无退休提供者；源码/SDK声明与启动filter、致命255/参数、数组/业务精确守恒、所有原身份/台账/政策/门禁及UTF-8/LF/差异和静态汇总/-Check核对。

未构建/链接/测试/运行/算法/查产物/计算摘要，无新依赖。固定引用cg2d-re-msvc-crt-exit-20261010；剩余实际多字节/heap策略继续MC01-C，完整业务owner、正常结果、SDK与正常快速退出/运行仍待。MC01全CRT与整体SPEC未完成。
