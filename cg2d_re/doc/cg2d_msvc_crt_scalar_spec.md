# MSVC标量与字符串宿主合同（MC01-A）

本SPEC正文整理自既有已提交记录，承接用户“CRT直接使用MSVC”的决议，不新增迁移。[总合同](cg2d_crt_host_migration_spec.md#msvc-crt-direct-decision-20261009)及各入口已具名边界继续有效；后附MCT01仅核全量表的五项日历职责关系，不重建旧私有算法或提升完整门禁。

## 实施范围

采用已安装UCRT/VCRuntime/STL及同域资源释放；业务协议、字段宽度、RNG、owner与调用顺序不随文档整理改变。旧私有算法及机器ABI差异沿原总合同，不把宿主承接记作运行验收。

原登记33个入口，其中29个新增宿主提供者。既有入口如下，不因本文重复计数：

`GetEnv_54518B`, `Atol_53C5BE`, `Strtol_53DA45`, `Atoi_53C649`, `Malloc_53C3AC`, `Free_53C0A9`, `Itoa_54ABDD`, `StrTok_53D26E`, `StrNCmp_53BFC0`, `StrCmp_5450F0`, `StrLen_53F9E0`, `StrCpy_544DB0`, `StrCat_544DC0`, `StrNCat_53C790`, `StrNCpy_53BEC0`, `StrChr_53C2F0`, `StrStr_53BE40`, `CreateLegacyDirectory_53C654`, `ChangeLegacyDirectory_53DD2F`, `ReadLegacyTime32_53BA26`, `ConvertLocalTime_541160`, `LocalTime_53B8C6`, `CopyLegacyMemory_541620`, `FillLegacyMemory_5419D0`, `CompareLegacyMemory_54B160`, `AllocateLegacyByteBuffer`, `FreeLegacyByteBuffer`, `CountLegacyBytes_543D0C`, `CompareInsensitive_53CF50`, `CompareInsensitiveN_54AB30`, `FormatLegacyString_53B304`, `FormatLegacyBoundedString_53D21D`, `ScanLegacyString_53CADC`。

此前已宿主化：`GetEnv_54518B`, `Atol_53C5BE`, `Malloc_53C3AC`, `Free_53C0A9`。

原本步退休的私有叶：`StrToXl_53DA5C`, `Xtoa_54AC0A`, `RefreshTz_540B1D`, `TzSet_540B08`, `IsDst_540D7B`, `CvtDate_540F27`, `GmTime_541067`, `LowerCompareValue_53DC64`。

记录当时的后续组：FILE/lowio/流格式及旧内部核成组迁移；C++库对象/初始化退出及剩余CRT兼容入口成组收敛。其中FILE组后续由MC01-B承接，最新状态依总SPEC，不把历史待办重新算作未实现。

## 原验证范围

仅只读核对已安装Windows SDK/UCRT声明及已有源码差异，未构建、测试、运行或计算摘要。SDK声明来源：已安装Windows SDK 10.0.26100.0/ucrt；time.h、errno.h、stdlib.h、string.h、direct.h、stdio.h。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_msvc_crt_scalar_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 将首批CRT入口直接交由MSVC承接」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

原验证记录：`build`=false；`tests`=false；`runtime`=false；`hash`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。

<a id="calendar-private-service-relations-20261012"></a>

## MCT01：五项私有日历入口的实际公共服务边界（2026-10-12）

开发选择只来自全量函数表540B08／540B1D／540D7B／540F27／541067的待确认关系。当前IDA只读取得五项入口／尾／调用引用和CALL摘要，共545条指令的边界范围1624B；13处入引用均在旧localtime／本地日期转时间戳链内，不重导或重建这些私有算法。

| 原入口与实际引用 | 当前提供者及限定关系 |
|---|---|
| 540B08：旧_localtime的53B8D9、本地转时间戳的54119F调用；内部540B11调用540B1D | LocalTime_53B8C6的::_localtime32及ConvertLocalTime_541160的::_mktime32整体承接时区初始化职责 |
| 540B1D：仅540B11调用，原内含getenv、GetTimeZoneInformation、字符转换及字符串库调用 | 同两公共服务使用当前MSVC时区策略；不模拟旧TZ缓存／Win32转换／私有全局布局 |
| 540D7B：旧_localtime两处及541205调用；四处调用540F27 | 同两公共服务整体承接夏令时判断，不提供同名旧私有包装或伪称当前DLL按原调用划分 |
| 540F27：仅540D7B内540E14／540E63／540E92／540EA7调用 | 日历服务内的夏令时转换边界随MSVC承接；旧辅助分解和状态存储退休 |
| 541067：仅旧_localtime内53B8FB／53B92B／53B940调用 | 由当前::_localtime32的完整日历转换服务整体承接原链内UTC分解，不声称当前直接调用::_gmtime32，也不新增独立UTC接口 |

实际本线源码有::_localtime32／::_mktime32调用，S32秒值、七日期输入及九S32借用日历字段沿现有适配保持；本步不重验公共包装正文或改变其输入／输出／失败。已批准的MSVC库内参数校验、时区策略、TLS和算法差异沿总合同，不将五原私有入口归为整项业务排除，不扩大函数分母或伪造独立CPP定义。

全量表五项改为“有承接”，仅明确上述真实服务和边界。五个原ID保留，既有host和CRT成员不变；语义登记记录当前提供者与原调用关系，声明／定义的已有公共入口地址注释保持，不给调用关联或MSVC内部函数伪造本线唯一地址。源码、头文件、构建和运行配置均不改，不改外部已有CRT总SPEC修改。

限定静态关系核定不代旧私有函数逐指令等价、全部业务日期／失败／借用寿命或当前SDK机器ABI与运行通过。C06／U10／V03及全部完整门禁保持，不构建／测试／运行／查产物／摘要或引依赖。
