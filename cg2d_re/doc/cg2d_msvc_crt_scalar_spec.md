# MSVC标量与字符串宿主合同（MC01-A）

本SPEC整理自既有已提交记录，承接用户“CRT直接使用MSVC”的决议，不新增迁移或验收。[总合同](cg2d_crt_host_migration_spec.md#msvc-crt-direct-decision-20261009)及各入口已具名边界继续有效。

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
