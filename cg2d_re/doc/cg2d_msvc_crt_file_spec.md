# MSVC FILE/stdio/lowio宿主合同（MC01-B）

本SPEC正文整理自既有已提交记录，承接用户“CRT直接使用MSVC”的决议，不新增迁移。[总合同](cg2d_crt_host_migration_spec.md#msvc-crt-direct-decision-20261009)及各入口已具名边界继续有效；后附MCE01只核全量表的错误转换职责关系，不提升完整验收。

## 实施范围

采用已安装UCRT/VCRuntime/STL及同域资源释放；业务协议、字段宽度、RNG、owner与调用顺序不随文档整理改变。旧私有算法及机器ABI差异沿原总合同，不把宿主承接记作运行验收。

| 既有入口 | 来源 | 既有承接 |
|---|---|---|
| `OpenLegacyStream_53B2F1` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `OpenSharedLegacyStream_53B2D1` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `ReadLegacyStreamByte_53C8B3` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `ReadLegacyStreamLine_53B226` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `ReadLegacyStream_53BCCD` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `WriteLegacyStream_53C680` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `WriteLegacyStreamByte_53CFDC` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `WriteLegacyStreamString_53B356` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `TellLegacyStream_53CB78` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `SeekLegacyStream_53BC41` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `RewindLegacyStream_53B27D` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `SetLegacyStreamBuffer_54ACF1` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `UnreadLegacyStreamByte_54463F` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `UnreadLegacyStreamBytes_549147` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `TellLegacyStream64_54B239` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `SeekLegacyStream64_54B3C0` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `GetLegacyStreamPosition_54ACB7` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `SetLegacyStreamPosition_54ACD9` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `WriteLegacyStreamWide_54AD92` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `ReadLegacyStreamWide_54AF70` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `UnreadLegacyStreamWide_54AE87` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `FlushLegacyStream_53BB02` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `CloseLegacyStream_53B1D0` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `FlushAllLegacyStreams_53BB99` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `CloseAllLegacyStreams_5446AD` | `src/platform/LegacyStdio.cpp` | 真实MSVC stdio服务或既有顺序的ungetc回推 |
| `FormatLegacyOutput_53EB28` | `src/platform/LegacyFormat.cpp` | std::vfprintf |
| `FormatLegacyStream_53BC0F` | `src/platform/LegacyFormat.cpp` | std::vfprintf |
| `ScanLegacyInput_53DEF4` | `src/platform/LegacyScan.cpp` | std::vfscanf |
| `ScanLegacyString_53CADC` | `src/platform/LegacyScan.cpp` | std::vsscanf（此前已迁） |
| `OpenLegacyDescriptor_53D435` | `src/platform/LegacyLowIo.cpp` | return ::_open(filename, static_cast<int>(flags), static_cast<int>(permissions)); |
| `GetLegacyDescriptorLength_53D3BD` | `src/platform/LegacyLowIo.cpp` | return static_cast<std::int32_t>(::_filelength(descriptor)); |
| `CloseLegacyDescriptor_53D30A` | `src/platform/LegacyLowIo.cpp` | return ::_close(descriptor); |
| `ReadLegacyFileCreationTime32` | `src/platform/LegacyTime.cpp` | _fileno→_fstat32→成功发布S32 st_ctime |

原登记33个入口，其中32个新增宿主提供者；不因本文重复计数。完整C++对象/codecvt、异常/初始化退出及其余CRT收敛沿MC01-C；业务与SDK实际运行全部仍待。

## 原验证范围

仅只读核对已安装Windows SDK/UCRT声明及已有源码差异，未构建、测试、运行或计算摘要。SDK声明来源：已安装Windows SDK 10.0.26100.0/ucrt；stdio.h:73 fpos_t为__int64、stdio.h:157/205/210/255/278/292实际FILE操作、corecrt_io.h:140/172/392/421实际_close/_filelength/_open、sys/stat.h:154 _fstat32/ctime S32。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_msvc_crt_file_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 将文件与流格式成组迁移到MSVC运行库」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

原验证记录：`build`=false；`tests`=false；`runtime`=false；`hash`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。

<a id="win32-error-current-service-20261012"></a>

## MCE01：542F44系统错误到errno的当前承接（2026-10-12）

全量表该项此前提供者未定。本轮原只读26条／103B及九处入引用确认：先将输入W32系统错误写1085FE4，再按591E80..591FE8的8B表项查对应errno写1085FE0；未命中时按13h..24h映射13、BCh..CAh映射8，其余22。九引用来自旧mkdir／fstat／close／sopen／chdir／lseek／write／read／lseeki64，都是旧CRT文件／目录服务的错误转换，不凭名称或地址猜分类。

当前CreateLegacyDirectory／ChangeLegacyDirectory实际调用::_mkdir／::_chdir，文件时间通过::_fileno→::_fstat32，描述符打开／关闭直接::_open／::_close；错误由同一MSVC服务持有。LegacyErrno_1085FE0／LegacyDosError_1085FE4实际访问当前线程errno／_doserrno。按CRT直接MSVC边界，由这些公共文件／目录／错误服务整体承接，不新增独立dosmaperr包装、不手写旧错误表或维护影子错误槽。

本步源码／头／构建配置保持，表中该项有承接、给出真实提供者，原ID／既有host保留。当前MSVC的错误映射／TLS／参数校验不是原表逐项或原机器ABI等价声明；业务返回／错误消费、全部文件owner／正常失败／寿命及SDK／运行继续待验，全部任务和完整门禁保持。不构建／测试／运行／查产物／摘要或引依赖。
