# 直接使用MSVC CRT调用SPEC

<a id="direct-format-calls"></a>
## 格式化与扫描包装退休（DCF01）

2026-10-14用户要求FormatLegacyString_53B304及同类接口直接使用标准库；有界部分已明确选择保持现有截断合同、直接调用::_snprintf。

| 原入口／旧接口 | 当前直接调用 | 实际CPP调用数 |
| --- | --- | --- |
| 0x0053B304／FormatLegacyString_53B304 | std::sprintf | 800 |
| 0x0053D21D／FormatLegacyBoundedString_53D21D | ::_snprintf | 9 |
| 0x0053BC0F／FormatLegacyStream_53BC0F | std::fprintf | 11 |
| 0x0053EB28／FormatLegacyOutput_53EB28 | MSVC std::vfprintf，无项目消费者 | 0 |
| 0x0053DEF4／ScanLegacyInput_53DEF4 | MSVC std::vfscanf，无项目消费者 | 0 |
| 0x0053CADC／ScanLegacyString_53CADC | std::sscanf | 21 |

只替换841处callee；原缓冲地址、格式串字节、容量、变参和返回值消费逐字保持，不增加格式校验、预清、容量推断或参数转换。九处容量实参只为28／100／256，直接::_snprintf保原MSVC负返回／满长不另补NUL的合同，不改为std::snprintf。
退休六个声明／定义，删除只有两扫描包装的LegacyScan.cpp及单一CMake源项；LegacyFormat保留店铺命令W32取样intrinsic和普通U64除法，不扩展两者的业务／异常合同。
消费者显式引入cstdio；原六个地址引用注释放在实际CRT引用旁，无当前消费者的vfprintf／vfscanf地址留在现有依赖头文件的CRT导入说明，不能冒充新增业务调用。
旧CPP包装／源码接口及其指向删除文件的绑定存具名历史，当前表与语义指向真实标准库／MSVC直接提供者及消费者；原入口身份、原端事实、阶段历史和完整门禁保持，不重新取证或新增已验收结论。
本步只迁移格式化／扫描；字符串和时间按下一单项推进，保持32位时间与真实字段／出参适配，不泛改文件、网络、堆或业务算法。
按第8条工程委托先记录范围，再实施必要静态核对与文本汇总；不编译、测试、运行、查产物、计算摘要、引依赖或委派，按同一cg2d_re分支精确提交，不打tag。

### DCF01源码与静态结果

34个消费者的841处callee已迁移：sprintf800／_snprintf9／fprintf11／sscanf21；两无消费者va_list包装退休，六个旧接口在src／include无残留。
逐文件静态核对仅callee、cstdio导入和原地址引用注释变化，原格式串、缓冲／容量／变参、返回消费及其它源码逐字保持。九容量仅28／100／256，项目未开启标准名安全模板重载，本机UCRT默认0；有界仍沿用户选择的::_snprintf合同。
LegacyScan.cpp和CMake一行源项退休，LegacyFormat保留原店铺W32取样与U64除法正文；本机cstdio真实导入sprintf／sscanf／vfprintf／vfscanf，不把无调用库提供者记成新增业务。
197条既有语义和306个具名历史绑定同步，扫描内存填充旧证据完整转历史；26条既有证据、六处置、CRT功能域及C04／U04／U11开放引用同步。
当前全量表更新直接调用／CRT导入和受影响源行号，3680入口身份／状态、所有原端事实、阶段历史和完整门禁保持，无活动绑定指向删除扫描单元。
UTF-8无BOM／LF、841调用／九容量、必要适配、SDK导入、历史／门禁、原身份及Git差异静态核对通过；文本汇总器及最终-Check通过3680函数／218导入／3061区间／184叶功能／60任务。
仅静态，未编译、测试或运行，不把调用改写计为完整格式／扫描流程或互通通过；精确提交本步并保留其它用户修改，不打tag。

<a id="direct-string-time-calls"></a>
## 字符串／时间通用包装退休（DCS01）

用户继续要求LegacyStringCompare与LegacyTime使用MSVC CRT；两模块底层已是CRT，本步只消除25个纯转发接口，173处实际调用直接使用原提供者；未使用包装退休，不新增消费者。

| 旧接口 | 直接提供者 | CPP调用数 |
| --- | --- | --- |
| ClassifyLegacySpace_53BDEF／0x0053BDEF | std::isspace | 1 |
| ClassifyLegacyControl_53BE17／0x0053BE17 | std::iscntrl | 1 |
| CompareInsensitive_53CF50／0x0053CF50 | ::_stricmp | 2 |
| CompareInsensitiveN_54AB30／0x0054AB30 | ::_strnicmp | 1 |
| LegacyErrno_1085FE0／0x01085FE0 | errno | 1 |
| LegacyDosError_1085FE4／0x01085FE4 | _doserrno | 0 |
| GetEnv_54518B／0x0054518B | std::getenv | 0 |
| Atol_53C5BE／0x0053C5BE | std::atol | 0 |
| Strtol_53DA45／0x0053DA45 | std::strtol | 0 |
| Atoi_53C649／0x0053C649 | std::atoi | 45 |
| Malloc_53C3AC／0x0053C3AC | std::malloc | 1 |
| Free_53C0A9／0x0053C0A9 | std::free | 1 |
| Itoa_54ABDD／0x0054ABDD | ::_itoa | 1 |
| StrTok_53D26E／0x0053D26E | std::strtok | 2 |
| StrNCmp_53BFC0／0x0053BFC0 | std::strncmp | 9 |
| StrCmp_5450F0／0x005450F0 | std::strcmp | 0 |
| StrLen_53F9E0／0x0053F9E0 | std::strlen | 0 |
| StrCpy_544DB0／0x00544DB0 | std::strcpy | 0 |
| StrCat_544DC0／0x00544DC0 | std::strcat | 0 |
| StrNCat_53C790／0x0053C790 | std::strncat | 1 |
| StrNCpy_53BEC0／0x0053BEC0 | std::strncpy | 35 |
| StrChr_53C2F0／0x0053C2F0 | std::strchr | 16 |
| StrStr_53BE40／0x0053BE40 | std::strstr | 48 |
| CreateLegacyDirectory_53C654／0x0053C654 | ::_mkdir | 6 |
| ChangeLegacyDirectory_53DD2F／0x0053DD2F | ::_chdir | 2 |

旧count／malloc请求的U32参数转换保持，调用点在该参数显式static_cast<std::uint32_t>；strlen的U32返回与atol／strtol的S32返回按旧接口保持。strchr／strstr沿原返回char*合同在调用点const_cast，不扩大修改权限或改业务类型。errno直接访问当前CRT线程槽，保原读写，不新增缓存／影子状态；_doserrno无消费者不补调用。
四个真正的时间适配继续保留：ReadLegacyTime32写S32可空出参、七参数ConvertLocalTime组std::tm／_mktime32、LocalTime转九S32字段并借用当前线程对象、文件创建时间_fileno／_fstat32成功才发布；保持_time32／_localtime32／_mktime32，不换64位time_t，不改结构或FILE域。
LegacyStringCompare.cpp／.hpp及CMake项在四接口退休后删除；LegacyTime保留适配与相关声明，LegacyMemory只退休本批malloc／free声明，不扩其他分配／内存包装。原25个参考地址保留到实际标准库导入说明，数据槽地址明确是原全局，不冒充函数入口；无调用的接口只记CRT提供者。
原参数求值、宽度、返回消费、失败及所有原地址／历史／验收门禁保持，更新实际源定位与唯一进度；只做必要静态和文本汇总，不编译、测试、运行、查产物、计算摘要、引依赖或委派，同分支精确提交，不打tag。

### DCS01源码与静态结果

25个纯转发接口已退休，173处实际CPP调用直接使用原CRT；malloc／free各1处，数量不把头文件声明计入调用。四比较接口及其.cpp／.hpp、单一CMake源项退休，旧名在src／include无残留。
调用参数按原U32边界转换，原const char*查询后char*借用由同一const_cast承接，类型／返回消费与errno线程访问保持；四个真实时间适配正文逐字相同，不改32位时间或LegacyTm字段。
LegacyTime仅保留时间适配和布局断言，删除不再使用的头文件依赖；LegacyMemory仅移除本批malloc／free声明，其它分配／内存接口保持。
历史定位快照不重写；汇总器只允许退休比较文件的历史附件读取具名“格式化与扫描直接使用MSVC CRT”提交中的前态，当前语义source仍须实际存在。
原参考地址注释保留在真实CRT导入及调用依赖旁；无消费者的八个服务不造新CPP调用，原全局errno／_doserrno地址明确标作数据槽。
当前源定位／标准库及内部支撑关系、既有语义／证据／处置与CRT及C04／U04／U11开放引用同步，旧绑定具名存历史；3680身份／状态及全部原端事实／历史／门禁保持。
UTF-8无BOM／LF、173参数／类型转换、四适配正文、旧接口与比较文件残留、原身份／历史门禁及Git差异静态核对通过；文本汇总器及最终-Check通过3680函数／218导入／3061区间／184叶功能／60任务。
仅静态，未编译、测试或运行；不把宿主承接计为完整locale／时间／硬件ABI运行等价，精确提交并保留其它用户修改，不打tag。

<a id="direct-lowio-calls"></a>
## 描述符纯转发包装退休（DLI01，2026-10-15）

用户要求检查并去除LegacyLowIo中的冗余包装；三个函数各有一个实际调用，全部位于Sub_4C9060。底层已经直接使用MSVC CRT，本次只撤转发层，不迁移文件格式／编码或引入文件系统新接口。

| 原入口 | 直接调用 | 保持的合同 |
| --- | --- | --- |
| 0x0053D435 | ::_open | 同一ASCII路径、创建／读写标志、读写权限及-1失败判断 |
| 0x0053D3BD | ::_filelength | 返回显式转std::int32_t，继续发布gWindowSavedLengthCC4238 |
| 0x0053D30A | ::_close | 同一描述符、相同关闭位置及返回值丢弃 |

本机既有UCRT fcntl.h／sys/stat.h确认_O_RDWR=0x0002、_O_CREAT=0x0100、_S_IREAD=0x0100、_S_IWRITE=0x0080；将0x102／0x180写成对应组合，数值与原int参数保持。WindowRuntime已直接包含io.h、fcntl.h和sys/stat.h，不增加外部依赖。

删除LegacyLowIo.cpp／.hpp、唯一include及单一CMake源项；原三地址保留在实际调用旁。其余流的打开／读取／关闭、ferror判断、版本校验、清理和发布次序保持，不改成_filelengthi64或改变失败处理。

同步实际CRT消费者及旧库内部支撑关系；删除文件的旧source绑定具名保留历史。历史证据附件只允许从“简化商店命令读取并修正取消项范围”提交读取原文件，当前语义source必须指向现有文件。原函数身份／状态和完整验收门禁保持。

只做必要源码／差异／编码与台账静态核对；不构建、测试、运行或计算摘要。

<a id="direct-terminate-call"></a>
## 无调用的终止包装退休（DRT01，2026-10-15）

用户追加要求LegacyRuntimeError同样处理。TerminateLegacyRuntime_540A5C只转发std::terminate，当前src/include没有调用者；原数组手工展开消费者已退休。本次删除.cpp／.hpp和CMake源项，不增加terminate调用、不改handler／abort／信号机制。原0x00540A5C注释放在SdlPresenter现有exception标准库导入旁，仅表明同一宿主服务，不把现有渲染析构调用冒充原数组消费者。已有SdlPresenter析构的std::terminate语句和条件保持，旧CRT内部支撑继续由现有客户端宿主入口承接；旧绑定保历史，当前绑定不指向删除文件。

DLI01／DRT01静态结果：三处描述符调用、唯一终止包装零调用、现有终止正文及原身份／状态核对通过；旧源码／构建引用清零，35条当前来源与函数定位同步。台账汇总因工作区另行暂存删除的外部键盘历史附件缺失而失败，未生成派生文件；本轮不宣完整台账、编译或运行通过。

<a id="direct-stdio-calls"></a>
## 标准文件流包装退休（DSI01，2026-10-15）

用户随后指定LegacyStdio同类处理。24个接口中12个有当前消费者，共250处调用分布19个源码；另12个无调用，包含两个fpos_t拆分／拼接适配及LegacyFilePosition。直接迁到原MSVC标准库函数，不改路径／模式／流对象／分支、错误处理和关闭顺序。

- fread／fwrite保持原elementSize／elementCount的U32转换，以及返回值的U32转换；ftell返回保持S32，fseek保持S32偏移和int起点。其余活动入口原参数类型与MSVC在当前Windows目标上的类型一致，直接替换callee。
- 保留LegacyFile=std::FILE别名及既有头文件引用；删除24个声明、仅供无消费者接口使用的LegacyFilePosition及布局断言，退休LegacyStdio.cpp和单一CMake源项。无调用接口不新增业务调用，原地址在实际标准库导入说明中保留。
- 当前语义与全量表指向实际CRT消费者／导入，旧绑定存历史；旧附件从“删除描述符和运行时终止的冗余包装”提交读取，当前source不回退。保原身份／状态及完整门禁。
- 本步只做必要静态核对，不编译、测试、运行或计算摘要。消息框宽接口及光标等Window包装按下一单项继续。

DSI01静态结果：250调用／19文件仅callee、所需数值转换和CRT地址导入注释变化；词法括号、退休引用、身份／门禁、编码／JSON和差异检查通过。常量实参值已落在目标范围时省略冗余cast。39条语义／38条退休实现定位已同步；完整台账仍仅因另行暂存删除的两个外部键盘历史附件失败，未生成派生文件。未构建、测试或运行。
