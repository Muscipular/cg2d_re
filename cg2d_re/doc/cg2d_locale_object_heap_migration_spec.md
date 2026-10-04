# locale 实现对象存储与退出迁移 SPEC

> LCH01；2026-10-05依工程委托采用。上位：[运行库边界](cg2d_crt_runtime_boundary_spec.md) §1/§4、[启动职责](cg2d_crt_startup_lifecycle_spec.md)。只成组迁移既有 LegacyLocaleImplementation 对象块和两处退出登记；原 locale、标准流与 codecvt 的可见合同继续保留。

<a id="locale-object-heap-baseline"></a>
## 1. 原基线与当前差异

[只读证据](cg2d_locale_object_heap_migration_evidence.json)保存十二原体269条唯一指令，三变化体54A404/54A59B/54A3E3共82条，其余九体187条用于界定所有权/清理。复用[NSH01提供者](cg2d_narrow_string_heap_migration_evidence.json)五体85条，总354唯一指令；不把局部统计当全局新增覆盖率。

| 原点 | 业务对象与时点 | 当前问题/实施边界 |
|---|---|---|
| 54A404@54A422→53C9CF | global为空才分配40B实现；非空块构造，原地址拓宽后为既有64B | 唯一当前_Locimp存储生产仍进入未闭合模式3；不迁移其facet向量或SDK对象 |
| 54A404@54C39F→53B4E7 | Ctor异常只释放分配块/重抛，不完整析构未完成对象 | 与正常对象释放成对迁移 |
| 54A59B@54A5AB→53B4E7 | 完整54A5B7后flags低位1才释放storage，返原地址 | 同本组malloc块配对；基类facet的54A58F仍原域 |
| 54A404@54A450→53B406 | 先发布global，再登记54A4BD，返回弃；登记失败不回滚 | 改实际std::atexit承接私有表，原回调/时点保持 |
| 54A3E3@54A3F3→53B406 | 先54A3ED仅RET，再登记54A3FA，直接返完整S32 | 改实际std::atexit；55315C全表caller尚未接，不创建新模块/抢先登记 |

原550154只有54A505安装与54A5B7析构恢复两个直接写者，槽0为54A59B；54A505唯一已证直接caller为54A404。当前源码只有54A404生产实现对象；filebuf/ios持有该实际指针的引用，默认locale在发布_Init返回后重读global递增。三槽1086728/108672C/1086730沿既有虚拟零合同，原引用与类别仍U32，透明/名称prefix仍原U8，布局不变。

这些直接引用和当前源码边界不声称排除了未知间接别名/其余未归属代码。未知facets、codecvt、向量生产继续开放，不能将其块交本组free，也不据当前向量为空删除原向量析构或虚调用。

<a id="locale-object-heap-decision"></a>
## 2. 本组决定

1. 既有LegacyLocale.cpp匿名域添加AllocateLegacyLocaleObject_53C9CF/FreeLegacyLocaleObject_53B4E7私有成对入口，用std::malloc/std::free承接_Locimp对象storage；增加cstdlib，撤去不再使用的LegacyRuntimeExit.hpp。不新增cpp/公共allocator/容量或指针表、CMake项、SDK类型或外依赖。
2. 分配仍U32上界FFFFFFE0、enable1，零请求宿主1；失败逐轮调用现有CallLegacyHeapNewHandler_541A28并传原byteCount，空/零结束、非零重试，处理器异常传播。无预清/新保护/失败补值/强制bad_alloc。本组实际请求仍sizeof既有64B实现，40→64只已有地址拓宽。
3. 仅54A422分配、54C39F失败释放、54A5AB正常释放三个callee改本组私有入口；原构造/默认locale、方法表、引用重读、名称、分类/经典槽发布、循环与异常清理正文不改。Ctor失败不完整析构；正常delete先54A5B7后flags&1，不自动delete对象/清槽。
4. 两实际登记改std::atexit同一原回调：54A450先发布global，弃返回；54A3F3先原RET晚段再直接返回宿主S32。私有表增长/返回失败时点按D02/D03承接，不造原53B406空包装、不双登记、不新增once/回滚或即时清理。两退出按原引用/虚析构和不清槽正文处理。
5. 54A57E基类facet释放及54A5B7向量free仍原域，名称仍NSH01；SDK std::locale、现有宿主标准流、codecvt与其他堆对象不切换。旧heap头/容量/resize不读取本组块。今后若增加本域真实_Locimp生产，须用本组配对；未知原producer未闭合，不外推。
6. 经典54A3E3接口仅提供正确宿主登记，未接通55315C的全应用初始化相对序；不为它增加模块调用/抢先登记。当前Legacy流与宿主流的活动归属、全引用归零/正常及快速退出仍待C03/C04，不能将登记函数存在算活动生命周期完成。
7. 三变化体、九未改边界及五共享提供者共十七既有语义记录同步当前callee/域，保原数字target与历史实作信息。三变化体登记adapted，其余边界不扩审；C03/C04/C08/C11保持open，C05生产不因本组改变。

<a id="locale-object-heap-acceptance"></a>
## 3. 局部门禁

| 门禁 | 本组标准 |
|---|---|
| I | 对象分配、Ctor失败释放、flags正常释放同域；原请求与真实处理器、两实际宿主退出登记已实现 |
| W | 550154唯一当前实现对象域与三槽/共享引用保持，vector/facet/name域不混；Ctor异常不完整析构，两退出不清槽；经典全表caller明确未接 |
| S | 十二原体269/复用85条可追踪；扣除私有pair/cstdlib/include及五callee、域注释后源码全文同前提交；header仅域注释 |
| R | 禁止构建/测试/运行；实际分配失败/处理器异常、完整标准流/退出、ABI/SDK与快速退出未验 |

只归档限定I/W/S，不提升完整FUNC-CRT门禁、叶验收或任务关闭数。不把354局部指令、两个登记接口或已定分配域当成全CRT/启动验收；音频暂停、原格式明文及无构建/测试/运行/摘要保持。

<a id="locale-object-heap-source-closure"></a>
## 4. 本组源码收口

私有AllocateLegacyLocaleObject_53C9CF/FreeLegacyLocaleObject_53B4E7已成对std::malloc/std::free；保原U32上界、零请求宿主1与同一541A28/原byteCount循环。唯一当前实现对象生产54A422、Ctor失败54C39F和正常flags删除54A5AB全部同域；未完成对象不完整析构，正常仍先实际54A5B7，字段/引用/名称/方法表/三槽与清理时序保持。

54A450/54A3F3已分别std::atexit登记原54A4BD/54A3FA，前者发布后登记且弃返回，后者原RET晚段后直接返S32。经典55315C全表caller仍未接，无新模块/once/提前登记或SDK对象互转。54A57E基类facet与54A5B7向量仍原free，名称仍NSH01；分域注释已对齐。

十二原体269/复用85总354及五数字call核对完成，两源码扣除限定差异后全文同前提交；十七既有语义之外/元数据保持，1152/1149有效ID、18pending/4分类引用不变。三变化体新增adapted使87→90/3680，三限定E使36→39；旧已审和证据不覆盖或重复计数。

实现固定于Git命名快照cg2d-re-locale-object-lifecycle-source-20261005。C01/C03/C04/C08/C11继续open，C05真实生产不改变，关闭任务10/60、叶验收0/182、完整门禁与R不提升。未构建/检查产物/新增或执行测试/运行/摘要，完整SDK、活动流/退出相对序及运行仍待。
