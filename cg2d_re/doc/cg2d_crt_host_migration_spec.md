# CRT 宿主承接总 SPEC

> 2026-10-10压缩：保留当前决议、原地址到宿主服务的映射、未决项和验收缺口。压缩前全文可从既有 Git 快照 `cg2d-re-msvc-crt-sort-consistent-20261010` 的本路径读取；逐轮源码差异、SDK依据和完整原身份见各组证据 JSON。
> **当前状态：A/B/C1–C6已落实相应源码迁移，C7仅两个一致排序域已迁；剩余七组/八调用的排序关系待选择。全CRT及总SPEC未完成。**本页不改变实现、功能分母、登记或验收状态；最新计数见[看板](../plan/cg2d_measurement_dashboard.md)和[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。

<a id="msvc-crt-direct-decision-20261009"></a>
## 当前总决议与业务边界（MC01）

用户2026-10-09要求“CRT 直接用msvc代替”：已确认的C/C++运行库职责统一交已安装MSVC UCRT、VCRuntime、STL。薄适配只保必要类型/宽度与业务消费，不重建旧私有算法、CPU/SEH/堆/FILE/locale表、机器ABI或同名假体。原身份、指令、数据和历史提供者继续可追溯；地址是原cg2d.exe VA，不能作为x64跳板。第三方、业务和来源不明项不按名称、地址段或IDA库标志批量归CRT。

库内参数校验、errno/TLS、locale、缓冲、格式/数学结果、分配与正常/快速退出机制采用MSVC合同，记获批替代而非原体等价。接受的主要差异包括：FILE/描述符表与收尾范围、格式精度和宽串失败计数、默认禁用%n、转换状态、STL布局/共享及异常支撑、浮点启动/文本链、库内堆策略。业务数学沿[ACP01](cg2d_asm_to_cpp_migration_spec.md)使用double/标准数学，业务F32字段仍float；旧PC53/T80/私有ST0保证已由具名许可覆盖。

仍保业务输入与非地址字段宽度、格式串、协议/资源/存档字节、读取/发布/调用序、真实返回消费、RNG次数和资源所有权。FILE/分配域整组迁移，禁止旧布局强转、混用分配释放器、影子错误槽、固定成功或补未写参数；MSVC rand不替代业务RNG。不借库迁移改变编码、引入默认值/保护/重试或删除实际业务构析。D01六补值、`4626A0` gate及`5202E0`差异仅沿[决议总SPEC](cg2d_closure_decisions_spec.md)具名范围；网络为原格式明文，音频沿当前WAV-only合同。异常/越界/溢出不专项模拟，依RPLAN.24执行。

## 原地址到MSVC服务的当前映射

表中普通函数使用`std::`，下划线项为MSVC扩展；只表达实际服务关系。库内退休函数没有逐项同名MSVC公开入口，完整地址/区间及宿主职责以[语义登记](cg2d_semantic_function_mappings.json)的`inventory_id/current_provider`和各组证据为准，不伪造一对一函数映射。

<a id="msvc-crt-scalar-acceptance-20261009"></a>
<a id="msvc-crt-scalar-source-state-20261009"></a>
### A：字符串、内存、时间与字符串格式

当前提供者：`LegacyTime.cpp`、`LegacyMemory.cpp`、`LegacyStringCompare.cpp`、`LegacyFormat.cpp`、`LegacyScan.cpp`，均位于`src/platform/`。详见[MC01-A证据](cg2d_msvc_crt_scalar_spec.md#evidence-archive-20261011)。

| 原地址 | 当前 MSVC 服务 | 必要适配 |
|---|---|---|
| `54518B` | `std::getenv` | 参数/真实返回按既有业务接口消费 |
| `53C5BE` | `std::atol` | 参数/真实返回按既有业务接口消费 |
| `53DA45` | `std::strtol` | 参数/真实返回按既有业务接口消费 |
| `53C649` | `std::atoi` | 参数/真实返回按既有业务接口消费 |
| `54ABDD` | `::_itoa` | 参数/真实返回按既有业务接口消费 |
| `53D26E` | `std::strtok` | 参数/真实返回按既有业务接口消费 |
| `53BFC0` | `std::strncmp` | 参数/真实返回按既有业务接口消费 |
| `5450F0` | `std::strcmp` | 参数/真实返回按既有业务接口消费 |
| `53F9E0` | `std::strlen` | 参数/真实返回按既有业务接口消费 |
| `544DB0` | `std::strcpy` | 参数/真实返回按既有业务接口消费 |
| `544DC0` | `std::strcat` | 参数/真实返回按既有业务接口消费 |
| `53C790` | `std::strncat` | 参数/真实返回按既有业务接口消费 |
| `53BEC0` | `std::strncpy` | 参数/真实返回按既有业务接口消费 |
| `53C2F0` | `std::strchr` | 参数/真实返回按既有业务接口消费 |
| `53BE40` | `std::strstr` | 参数/真实返回按既有业务接口消费 |
| `53C654` | `::_mkdir` | 参数/真实返回按既有业务接口消费 |
| `53DD2F` | `::_chdir` | 参数/真实返回按既有业务接口消费 |
| `53BA26` | `::_time32` | 保持S32时间及输出 |
| `541160` | `::_mktime32` | 日历参数逐项适配后调用_mktime32 |
| `53B8C6` | `::_localtime32` | 线程局部九S32日历逐字段复制；失败空返 |
| `541620` | `std::memmove` | memmove保重叠复制 |
| `5419D0` | `std::memset` | 参数/真实返回按既有业务接口消费 |
| `54B160` | `std::memcmp` | 参数/真实返回按既有业务接口消费 |
| `53CF50` | `::_stricmp` | 参数/真实返回按既有业务接口消费 |
| `54AB30` | `::_strnicmp` | 参数/真实返回按既有业务接口消费 |
| `53B304` | `std::vsprintf` | 参数/真实返回按既有业务接口消费 |
| `53D21D` | `::_vsnprintf` | 使用_vsnprintf容量耗尽/返回合同 |
| `53CADC` | `std::vsscanf` | 不补扫描未写目标 |

原数据槽`1085FE0/1085FE4`的访问每次查询当前CRT线程`errno/_doserrno`，不缓存TLS地址。现字节计数已直接采用MSVC `strnlen`，旧`543D0C`桥最终沿C2/C5退休。无独立原F的裸字节AllocateLegacyByteBuffer/FreeLegacyByteBuffer由malloc/free配对承担，不新增原函数身份。

<a id="msvc-crt-file-decision-20261009"></a>
<a id="msvc-crt-file-acceptance-20261009"></a>
<a id="msvc-crt-file-source-state-20261009"></a>
### B：FILE、描述符、流格式与文件状态

`LegacyFile`为公开platform头的真实`std::FILE`别名，所有现读写/格式/状态/关闭同属MSVC域；不读取其内部布局。当前提供者：`LegacyStdio/LowIo/Format/Scan/Time`，详见[MC01-B证据](cg2d_msvc_crt_file_spec.md#evidence-archive-20261011)。

| 原地址 | 当前 MSVC 服务 | 必要适配 |
|---|---|---|
| `53D435` | `::_open` | 保flags/permissions及原返回消费 |
| `53D3BD` | `::_filelength` | 参数/真实返回按既有业务接口消费 |
| `53D30A` | `::_close` | 参数/真实返回按既有业务接口消费 |
| `53B2F1` | `std::fopen` | 参数/真实返回按既有业务接口消费 |
| `53B2D1` | `::_fsopen` | 参数/真实返回按既有业务接口消费 |
| `53C8B3` | `std::fgetc` | 参数/真实返回按既有业务接口消费 |
| `53B226` | `std::fgets` | 参数/真实返回按既有业务接口消费 |
| `53BCCD` | `std::fread` | 参数/真实返回按既有业务接口消费 |
| `53C680` | `std::fwrite` | 参数/真实返回按既有业务接口消费 |
| `53CFDC` | `std::fputc` | 参数/真实返回按既有业务接口消费 |
| `53B356` | `std::fputs` | 参数/真实返回按既有业务接口消费 |
| `53CB78` | `std::ftell` | 参数/真实返回按既有业务接口消费 |
| `53BC41` | `std::fseek` | 参数/真实返回按既有业务接口消费 |
| `53B27D` | `std::rewind` | 参数/真实返回按既有业务接口消费 |
| `54ACF1` | `std::setvbuf` | 参数/真实返回按既有业务接口消费 |
| `54463F` | `std::ungetc` | 参数/真实返回按既有业务接口消费 |
| `54B239` | `::_ftelli64` | 参数/真实返回按既有业务接口消费 |
| `54B3C0` | `::_fseeki64` | 参数/真实返回按既有业务接口消费 |
| `54ACB7` | `std::fgetpos` | 8B fpos_t↔两个W32；成功才发布 |
| `54ACD9` | `std::fsetpos` | 两个W32合成真实8B fpos_t |
| `54AD92` | `std::fputwc` | 返回沿原U16消费 |
| `54AF70` | `std::fgetwc` | 返回沿原U16消费 |
| `54AE87` | `std::ungetwc` | 参数/返回沿原U16消费 |
| `53BB02` | `std::fflush` | 参数/真实返回按既有业务接口消费 |
| `53B1D0` | `std::fclose` | 关闭失败也使FILE引用失效 |
| `53BB99` | `::_flushall` | MSVC全体刷新范围/返回 |
| `5446AD` | `::_fcloseall` | MSVC全体关闭范围/返回 |
| `53EB28` | `std::vfprintf` | 参数/真实返回按既有业务接口消费 |
| `53BC0F` | `std::vfprintf` | 参数/真实返回按既有业务接口消费 |
| `53DEF4` | `std::vfscanf` | 参数/真实返回按既有业务接口消费 |

原EOF/错误字段读取改为`feof/ferror`，条件与查询时点保持；auto.dat创建时间通过同一FILE的`_fileno→_fstat32`，成功才发布S32 ctime（无独立原F）。标准流直接使用stdin/stdout/stderr。日志日期切换、动画按标记收尾在关闭后清空已证owner槽，避免重复fclose；旧FILE游标、缓冲/锁、20/512索引表和描述符表由MSVC拥有。

39旧定义/重复lowio和stdio初始化、旧进程入口/C表退休；`53DE8C`只保原入口片段，不造F。MC01-B阶段的旧filebuf过渡类型已沿C1退出，不再作为当前待迁移项。全部业务字节、元素宽度、偏移、失败返回消费与RNG保持。

<a id="msvc-crt-cpp-decision-20261010"></a>
<a id="msvc-crt-cpp-acceptance-20261010"></a>
<a id="msvc-crt-cpp-source-state-20261010"></a>
### C1：STL对象与私有C++异常

LegacyNarrowString/StreamBuffer/FileBuffer/Ios/Locale/Exception六库及NativeCppException头已退休（13文件、205具名定义/201入口/200原库存ID）。完整地址清单和195 host/7应用配置刷新adapted职责见[MC01-C1证据](cg2d_msvc_crt_cpp_evidence.json)；`54A577`只保共享片段身份。旧string、游标/虚表、locale引用、facet/codecvt、六异常payload/捕获表由MSVC STL/VCRuntime承担，无当前同名私有对象。

八真实标准流、`ios_base::Init`、`locale::classic`以及应用tie、error unitbuf、log借error缓冲、无sentry的pubsync刷新序保持；晚段Init、业务静态初始化与std::atexit登记不重复执行。`54843B`实际单RET退出回调及登记仍保留。未知原间接调用不因“当前无消费者”判不可达；历史附件只从固定快照解析，当前提供者不能回退至已退休文件。

<a id="msvc-crt-numeric-decision-20261010"></a>
<a id="msvc-crt-numeric-acceptance-20261010"></a>
<a id="msvc-crt-numeric-source-state-20261010"></a>
### C2：数值、浮点格式与旧环境

旧FloatFormat/Numeric/Environment/Math四CPP/三头、106定义及六浮点callback/trap槽、96位转换、FDIV探测、未生产gate、40B/88B错误记录和RaiseException私有链已退休。完整88 host/8应用数学控制关联adapted和共享消费者清单见[MC01-C2证据](cg2d_msvc_crt_numeric_evidence.json)；`545038`保持原X尾片段身份，不新造F。

| 原身份 / 现业务点 | 当前提供者与保留边界 |
|---|---|
| `53D000/53D022`，`46F1D0`两个专用活动点 | MapEffectMath的Cg2dMapEffectDivisor使用`std::pow(1.25, exponent)`，指数为signedLevel−1，幂返回后再读取借用幅值 |
| `5433F0/543405/543435`专用帮助体 | 原身份关联上述应用标准数学适配；旧ST0/控制字体已退出，非同名直接调用或一般内核等价 |
| Shop `4D8F5B`单W32读取 | LegacyFormat的Cg2dReadLegacyShopCommandW32仍用一次`__movsd`，不直接求值未生产命令；P07未解决 |
| 成功NLS双BYTE读取 | C2曾迁入LegacyMultibyte；该私有NLS组已沿C5退休，保原证据，不列当前提供者 |
| `53CB10`计时U64除法 | MainLoop真实消费的DivideLegacyUnsigned64保原参数/返回与正文，不作为无消费者CRT删除 |

实际地图/音频CPP数学、控制作用域、常量位样本、F32落存/读取发布序、整数转换与RNG保持；格式/扫描使用表中五类真实宿主服务。旧无消费者U64乘/移/余数帮助体退出；库内pow错误/errno/locale及浮点状态按MSVC，不重建旧私有环境。

### MFA01 保留专用幂路径兼容边界（2026-10-05）

历史专用1.25路径与原端事实保留；其“不换std::pow/保持PC53连续x87”实施要求已由ACP01/MC01-C2覆盖。原`53D019→53D022`是flow；`53D1F5`负底数与`54348E`共享尾只保证据，未知原入向仍待核，不列旧内核复刻债务。

### BPJ01 实际专用幂帮助体的独立基线（2026-10-07）

原`5433F0`完整11条21B及唯一`53D071`调用证据继续可追；当前职责见C2，不将旧x87源当活动提供者或一般数学等价证明。

### BPM01 专用幂输入及控制字身份补齐（2026-10-07）

原`543405/543435`完整27条90B、五call/两父及`53D04C`特定活动域证据保留；旧027F/PC53/F80机器保证不再作为当前验收要求。三历史小步详见[地图数学SPEC](cg2d_map_effect_arithmetic_spec.md)，业务正常数值/RNG和运行结果仍待。

<a id="msvc-crt-exit-decision-20261010"></a>
<a id="msvc-crt-exit-acceptance-20261010"></a>
<a id="msvc-crt-exit-source-state-20261010"></a>
### C3：退出、错误、信号与异常支撑

旧onexit/termination表、OS/console哨兵、信号/raise/filter、19槽R600x消息/MessageBox缓存/钩子和纯CRT FinishHosted回调/唯一登记已退休。实际std::atexit及业务资源构析、登记次数/相对序、标准流应用配置保持；MSVC exe入口持有默认异常filter和库内normal/quick/callable收尾，应用不再次安装/恢复旧filter，也不调用SDK私有名。

| 原地址 | 当前状态 / 宿主关系 |
|---|---|
| `540A5C` TerminateLegacyRuntime | 实际`std::terminate()`；供数组展开的二次C++异常 |
| `53DEAB` ExitLegacyRuntimeError | C3阶段为`std::_Exit(255)`，无R600x/MessageBox；其私有argv辅助组和9/9/8失败随C5退休，当前入口已删除 |
| `53DED0` FastLegacyRuntimeErrorExit | C3阶段为`std::_Exit(255)`；唯一旧heap28失败随C4退休，当前入口已删除 |
| `53DECD` | 原X尾片段及退出职责历史关联保留，不新造F/当前尾体 |

实际三数组构造/析构/展开及`5405F5`代码筛选仍保S32数量、U32步长/乘积、真实callback、正构逆析、完成/剩余前缀和SEH传播，不因CRT迁移删除。详见[MC01-C3证据](cg2d_msvc_crt_exit_evidence.json)；C3阶段“仍保heap/argv致命入口”已分别由C4/C5覆盖。

<a id="msvc-crt-heap-decision-20261010"></a>
<a id="msvc-crt-heap-acceptance-20261010"></a>
<a id="msvc-crt-heap-source-state-20261010"></a>
### C4：通用堆直接采用MSVC

| 原地址 | 当前 MSVC 服务 | 必要适配 |
|---|---|---|
| `53C3AC` | `std::malloc` | U32请求透传；同域free |
| `53C0A9` | `std::free` | 同域malloc/calloc/realloc块释放 |
| `53FCFB` | `::_msize` | 真实CRT容量按现U32接口消费 |
| `53BFF8` | `std::calloc` | U32 count/size透传，乘积校验归MSVC |
| `53FA5B` | `std::realloc` | U32字节数透传；空/零/失败旧块策略归MSVC |

旧LegacyHeap CPP/头、初始化/借用句柄、模式1/2/3、handler四槽、环境选择及失败28入口已退休；上述三容量/扩容/清零公开薄适配在C4核无实际调用方。零请求转1、16字节对齐、FFFFFFE0上界、手工乘积清零及new-handler重试等旧库策略不再模拟。Malloc/Free的旧私有allocator别名共享原ID，不新增F或删真实公共提供者。请求字段仍U32，仅SDK调用按size_t承载；同域owner和真实返回消费保持。详见[MC01-C4证据](cg2d_msvc_crt_heap_evidence.json)；[HMU01](cg2d_heap_malloc_unification_spec.md)旧请求预处理仅作历史。

<a id="msvc-crt-multibyte-decision-20261010"></a>
<a id="msvc-crt-multibyte-acceptance-20261010"></a>
<a id="msvc-crt-multibyte-source-state-20261010"></a>
### C5：ctype、多字节、locale及argv/环境

| 原地址 | 当前 MSVC 服务 | 必要适配 |
|---|---|---|
| `53BDEF` | `std::isspace` | Input实际ASCII 0..127，消费非零结果 |
| `53BE17` | `std::iscntrl` | Input实际ASCII 0..127，消费非零结果 |

旧LegacyMultibyte与StringCompareState、37定义/22私有数据/4布局类型、代码页表、NLS分类映射/五转换、计数/双BYTE桥及私有argv/环境七函数无现组外消费并退休；`543D0C`及上述退出辅助职责由MSVC UCRT入口/多字节/locale/TLS整体承担，没有当前同名包装。

实际WinMain直接消费MSVC传入命令尾；ReadBgmFrequencySelectionByte/BgmRuntimeOrderWords/ParseCommandOptions/ParseIpOptions及其发布槽、业务字节/容量/分支、数组/terminate与实际资源构析保持。不设置全局_setmbcp/setlocale，不借库转换推断协议/资源/存档代码页或转Unicode。未知原alias/邮件重叠保留为缺口。详见[MC01-C5证据](cg2d_msvc_crt_multibyte_evidence.json)。

<a id="msvc-crt-residual-decision-20261010"></a>
<a id="msvc-crt-residual-acceptance-20261010"></a>
<a id="msvc-crt-residual-source-state-20261010"></a>
### C6：退休多BYTE回退与残留边界

`549147` UnreadLegacyStreamBytes的多BYTE反向unget/失败回读函数和声明已退休，唯一旧filebuf消费者沿C1退出；`53C8B3→std::fgetc`、`54463F→std::ungetc`及真正业务仍在。它们是现单BYTE服务/库职责关联，不能称一次ungetc/fgetc实现原549147多BYTE合同。现时间九S32、位置双W32、errno/TLS、数组/SEH、Shop W32和计时U64除法属于必要业务适配。详见[MC01-C6证据](cg2d_msvc_crt_residual_evidence.json)。

<a id="msvc-crt-sort-consistent-decision-20261010"></a>
<a id="msvc-crt-sort-consistent-acceptance-20261010"></a>
<a id="msvc-crt-sort-consistent-source-state-20261010"></a>
### C7：已迁两个一致排序域

原`53C112` _qsort及`53C266/53C2B4` shortsort/swap职责，在以下两域使用真实`std::qsort`；原十直接caller、其余八域现算法保留，整体仍为mixed adapted，非全部host。

| 原caller / 原调用点 | 当前比较器 / 对象宽度 | 保留规则 |
|---|---|---|
| `45BB20 / 45BECC` AnimeInit | `45B420`；sizeof(AnimateRecord)=56B | U32 animeNo升序、同键0；原gAnimateCount发布/数量表达式 |
| `520790 / 5208DC` SortTextures | `5222A0`；sizeof(GraphicCache3)=4B | U8 layer升序/S16 cacheIndex降序、同键0；原容量/保留项及S16转U32数量 |

记录均有可平凡复制/布局依据，完整对象字节（含真实x64指针、未写字段/填充）随排序移动，不按原动画36B裁切，不构析/新增owner。MSVC pivot、短排序、比较/交换次数/次序、参数校验及同键排列采用宿主合同；不保证稳定同键序。LegacySort仍供公会使用。详见[MC01-C7证据](cg2d_msvc_crt_sort_consistent_evidence.json)。

<a id="msvc-crt-sort-pending-decision-20261010"></a>
## 待决议：七组、八处排序

这些比较器同键仍返±1，部分空槽/状态分支不对称。当前保原算法/比较器，不能直接换MSVC并宣称原业务顺序保持；C7两已迁域不替代此选择。

| 业务组 | 原caller | 比较器 |
|---|---|---|
| 地图 | `46C420` | `46C440` |
| 技能 | `518600` | `518660` |
| 称号 | `5198D0` | `5198F0` |
| 联系人 | `4EC1A0` | `4EC030` |
| 邮件两个域 | `4A2C60 / 4B5760` | `4A2CC0 / 4B57B0` |
| 战斗绘制 | `405C30` | `405BD0` |
| 公会 | `52FCC0` | `52FB60` |

1. **全部MSVC**：允许逐项修正非一致比较器，保已证主键/字段方向和正常类别优先；同键/同类无业务次键返回0，空槽/类别两侧统一比较，不新增地址、随机数或字段次键。接受具名排序关系/同键排列及库比较交换次序变化，再验业务画面/列表和owner。
2. **兼容例外**：七组/八调用保现业务比较器和原排序，作为直接MSVC要求的具名例外；不扩展其它CRT，也不能宣称旧排序内核全部退休。

**尚未收到选择，未实施这八处替换或比较器修正，不将预选/未回复视为批准。**依据为完整现比较器、原十caller合同、本机UCRT源码与[MSVC qsort文档](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/qsort?view=msvc-170)。

## 待处理内容与验收缺口

| 任务 | 仍须完成/验证 |
|---|---|
| C01 CRT总策略 | 上述排序取舍及实施；全体实际CRT提供者/业务边界和接线审计 |
| C02 C++90初始化表 | CAB07的72有序源投影+15 SDK提供=87；另1空槽/2键盘排除分列，两音频初始化已接；完整入口/消费者/alias/SDK运行 |
| C03 四Init与登记 | 所有业务消费者/owner、首次使用与初始化清理顺序、真实SDK退出；旧私有容量/registry不再复刻 |
| C04 构析与入口 | 数组/成员EH/硬件SEH、跨TU首次使用、完整正常/快速退出与资源owner |
| C05 codecvt | 业务字节/参数/真实结果、未知原alias/当前owner与SDK完整生命周期；旧私有虚表/容量不再复刻 |
| C06 异常查找捕获 | 保原数组/成员SEH；实际SDK生成ABI、传播/二次异常终止与全业务寿命，不新增专项故障模拟 |
| C07 抛出与SDK | 业务传播/清理、SDK生成ABI及完整寿命；不新增业务throw/catch或消息副本 |
| C08 原堆模式3 | 实际U32输入/返回、未知alias、MSVC真实容量/失败及启动退出；旧池/模式/handler不再复刻 |
| C09 图形双树 | 完整应用入口、逐节点/surface及调色板、普通/快速退出清理与SDK运行 |
| C10 FILE/描述符 | 当前源码编译/链接、全部业务文件结果、owner/alias、成功/失败及正常/快速退出 |
| C11 应用分配域 | 每个实际producer/owner成对分配释放、未知alias、容量/失败/最终清理，保持同域 |
| C12 字符串/格式 | 排序待决、原格式/P07未生产参数、全部字节/格式结果、owner和SDK运行 |

上述C01–C12均开放，适用V02/V03及六closure不因静态迁移关闭。P07未决事实是4D8DA0模式/第二命令原值与生产，单W32读取适配不补值或证明原栈等价。未知原间接调用、共享别名和邮件重叠继续取证；“当前无消费者”不代原端不可达、范围排除或完整寿命闭合。完整状态见[收尾表](../plan/cg2d_spec_closure_checklist.md)、[启动/生命周期SPEC](cg2d_crt_startup_lifecycle_spec.md)和[运行库边界SPEC](cg2d_crt_runtime_boundary_spec.md)。

## 验证级别与证据使用

A/B/C1–C7只完成对应当前源码、真实SDK声明/职责、消费者与差异的限定静态核对（I/S）；C6未给待决排序软件通过，C7仅两个一致域接线。不得据旧编译/链接、库替代许可、原身份数量或局部静态通过宣布全CRT/业务/正常结果完成。当前不构建、测试、运行客户端/算法、检查产物或计算摘要；需要恢复相应权限后按总SPEC§10验收，不引新依赖。

原函数/区间/原caller与机器数据由各组证据和[原库存](cg2d_ida_inventory.json)保留；`53DE8C/53DECD/545038/54A577`等共享/尾片段不得变造独立F。历史源码附件只从各JSON的before_snapshot读取，当前source必须指向真实服务/业务，禁止回退到退休源。旧阶段实施次数、递增计数与重复日志已从本页收敛至对应JSON及固定Git快照；本次压缩仅编辑本文件。
