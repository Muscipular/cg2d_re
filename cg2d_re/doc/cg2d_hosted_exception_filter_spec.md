# 宿主异常筛选与终止接线 SPEC（HFI01）

2026-10-05。按既有D02/D03工程委托完成本项x64记录与宿主初始化/收尾接线。
上位：[CRT生命周期](cg2d_crt_startup_lifecycle_spec.md)、
[本机C++异常](cg2d_native_cpp_exception_spec.md)、[宿主前置](cg2d_hosted_runtime_prefix_spec.md)。
固定快照：`cg2d-re-hosted-exception-filter-source-20261005`。

<a id="hosted-filter-baseline"></a>
## 原基线与实际缺口

[原证据](cg2d_hosted_exception_filter_evidence.json)保四既有根：
54503A筛选24条/70B、545080安装4条/17B、545091恢复3条/13B、54B20C控制台关闭18条/45B，
共49条/145B。原54503A仅对代码E06D7363、参数数3、参数0低W32等于19930520进入原540A5C；
不匹配时验证旧过滤器、重新读取同一槽并转调，或返回0。
545080的真实C5槽是55317C，位于553178多字节之后、C++应用初始化之前。
普通终止固定非空槽顺序是553188的stdio、55318C的console、553198的filter恢复；
quick/callable原路径另由既有DispatchLegacyExit承载。

现状已用真实本机类型throw/throw;，但54503A仍只匹配x86三参数；
当前宿主前置也没有安装它，普通宿主退出只接stdio，console/filter未进入该组收尾。
这三处是本步解决的源码缺口，不能拿“已有函数本体”代替接线。

本机已安装MSVC14.50.35717源码只读证据：
- include/ehdata_values.h:62–66明定_M_X64的EH_EXCEPTION_PARAMETERS为4；
  :28–33说明原生记录使用EH_MAGIC_NUMBER1=19930520。
- include/ehdata_forceinclude.h的_WIN64分支启用相对类型信息；
  crt/src/vcruntime/throw.cpp的_CxxThrowException以魔数/对象/ThrowInfo/镜像基址四项交RaiseException。
- utility_desktop.cpp:86–102提供本机顶层筛选及安装机制背景；
  本线仍使用既有原业务终止/旧过滤器转调，不照搬它的其他魔数或托管识别范围。

SDK源码是当前记录合同的来源，未编译/运行，不宣称生成ABI实测。
5405F5只看异常代码、没有参数数检查，已能筛原生C++异常；本步不改它、数组清理或成员EH。

<a id="hosted-filter-decision"></a>
## 实施裁决

1. 54503A参数数匹配改为当前x64 SDK四参数，保E06D7363、参数0低W32的19930520，
   原540A5C目标、IsBadCodePtr验证、验证后共享槽重读/返回均不改。
   这是D02私有ABI适配；原x86三参数事实保留，不同时新增三参数旁路、其他魔数或托管范围。
2. 在已有LegacyMouse首个前置构造中，原InitializeLegacyMultibyte之后调用545080，
   保C5先多字节再过滤器、随后应用投影顺序；只安装一次，不启用完整旧入口/cinit/90表。
3. 将现有FinishHostedLegacyStdio改名FinishHostedLegacyRuntime，原callable=0→stdio保持，
   后续依次复用54B20C及545091。该同一callback仍在应用登记之前调用std::atexit，
   所以后登记应用先结束、本组再按stdio→console→恢复filter顺序收尾。
   不新增第二次stdio注册、不添加宿主句柄副本、跨表free、HeapDestroy或提前清共享槽。
4. 545080/545091共享原唯一10862E0；console两个独立句柄保原-2初值和-1/-2跳过规则。
   不从只有读xref推断全部间接写入不存在，不删除原控制台逻辑，也不造其未知写者。
5. 原DispatchLegacyExit normal/quick/callable、异常/SEH、70源＋15SDK配置投影和业务正文保持。
   普通宿主固定终止表迁即时登记按D03处理失败/时机差异；源码接线不称全退出行为等价。
   不新增保护、固定成功、RTTI、私有SDK元数据或外部依赖。

<a id="hosted-filter-acceptance"></a>
## 限定验收

- 一处ABI判定加说明；LegacyMouse现有callback改名并加两个真实收尾，前置尾加一次真实安装。
- 扣除限定补丁后两CPP全文保持；stdio/heap/lowio/multibyte、过滤器安装/恢复、
  console、terminate、数组清理、完整旧入口及全部业务provider保持各自已有正文。
- 四既有语义同步当前接线；四R1处置为本组适配，不增加原ID/成员枚举或完整门禁。
- 三限定I/W/S只支撑本步源码、原槽/原序及SDK记录事实。C02/C04/C06/C07/C08/V02仍open；
  旧私有5402AB/5407BC、全部捕获/成员清理、硬件SEH、SDK继承/生成ABI、
  跨TU首次使用/owner/alias及完整normal/quick/callable和运行继续按原门禁推进。
- 只静态核新增差异/引用及台账汇总；不构建/查产物/新增或执行测试/运行/摘要。
  音频暂停与原格式明文保持；本项源码不解除PCP01所需的原端动态许可。

<a id="hosted-filter-source-closure"></a>
## 本组源码收口

以当前x64四参数承接54503A识别，接通宿主C5尾安装以及普通退出stdio→console→filter恢复。
共享owner、原终止/转调与quick/callable分工保留；不再将三参数旧筛选当本机异常的可用接线。
实际初始化、API成功失败、运行/退出及完整SDK/SEH门禁尚未执行。

限定静态核对通过：两CPP五片段精确对照，四语义/四处置/三限定E及登记簿精确转换，四文档限定追加/C04-C06两行；13项UTF-8无BOM/LF、12个新Markdown链接、36个本组E引用及Git差异格式通过。看板已生成，独立Check通过；未复扫历史正文或运行客户端。
