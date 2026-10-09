# 异常基类拥有文本域宿主迁移 SPEC

> 2026-10-10当前C++边界：[MC01-C1](cg2d_crt_host_migration_spec.md#msvc-crt-cpp-source-state-20261010)已退休无现组外消费者的六组私有C++库及NativeCppException头。当前采用真实MSVC标准流/Init/locale及库内string/facet/codecvt/异常支撑，现流配置/缓冲借用/刷新、业务构析与退出登记保持。下文旧Legacy对象/方法表/私有登记/异常payload及其源码状态属固定历史，不再要求复刻；完整业务/owner/未知原alias与SDK实际运行仍待，未编译/运行。

> EXH01；2026-10-05按既有工程委托采用。[运行库边界](cg2d_crt_runtime_boundary_spec.md) §1批准按真实分配域成组迁移；接续[NSH01](cg2d_narrow_string_heap_migration_spec.md)，本组只迁移exception基类拥有文本，不迁移异常对象存储、类型信息缓存或本机抛出ABI。

<a id="exception-text-heap-baseline"></a>
## 1. 原体与现有数据闭包

[原证据](cg2d_exception_text_heap_migration_evidence.json)保留54B079/54B0B6/54B100三变化体25/31/7条，54B05D对象删除11条、54B116的what五条，共79条新完整原指令；复用NSH01五提供者85条，总164唯一指令。复用未改的提供者不重复取证，数量不计全局新覆盖率。

| 原点 | 原状态/顺序 | 当前接线 |
|---|---|---|
| 54B079@54B090→53C9CF | 先方法表，strlen(*sourceText)+1 U32，分配后先发布text，非空才重读*sourceText复制，最后ownsText=1；空也保持1 | 现有构造本体，两派生构造调用同一提供者 |
| 54B0B6@54B0D8→53C9CF | 先复制完整U32拥有位型；非零才strlen/分配/发布/重读源text复制，零直接借用源指针 | 现有复制本体、派生复制与捕获复制适配共用 |
| 54B100@54B10F→53B4E7 | 先判拥有非零，再恢复方法表，非零释放text；不清text/ownsText | 现有所有基类清理调用共用 |
| 54B05D@54B06D→53B4E7 | 先54B100，再flags&1释放对象storage，返回原this | 对象块仍原堆，不能交文本宿主free |
| 54B116 | 非空返回text，空返回既有unknown静态文本 | 原what与回退均保持 |

现有源码完整ownsText写者只有上述构造/复制：拥有块都由两原分配点产生，借用分支不释放。增长/容量查询不存在于该文本域。对象中的text指针按x64拓宽，拥有标志仍完整U32；不能截U8或把非零改成精确1。派生窄字符串message已沿NSH01，基类text与message是独立块，不能合并owner或清理。

原53C9CF固定enableHandler=1；53C3BE先拒绝U32请求大于FFFFFFE0，再分配，失败向541A28传原请求，处理器每轮重读108614C，零/空结束、非零重试，异常向外传播。原分桶/模式与物理对齐由D02允许宿主承接；不改业务strlen+1、原失败消费或复制消息。

## 2. 当前差异与迁移边界

两文本分配与拥有文本释放仍直接进入旧原堆，依赖未闭合的模式3核心。删除析构另释放异常对象storage，类型信息缓存、locale facets和filebuf对象也各有原域；一次全改53B4E7所有调用会跨域。本组在真实字符块边界成对切换，不把LegacyExceptionObject或旧ThrowInfo解释成宿主std::exception/SDK对象。

<a id="exception-text-heap-decision"></a>
## 3. 本组实施决定

1. 在既有LegacyException.cpp匿名域添加同域AllocateLegacyExceptionTextBytes_53C9CF和FreeLegacyExceptionTextBytes_53B4E7两个私有入口，用std::malloc/std::free承接字符块；显式增加cstdlib，不新增文件、公共分配API、数据副本或依赖。
2. 私有分配保原U32拒绝上界及enable1实际循环；宿主零请求取1，其他传原byteCount。每轮失败仍调现有541A28并传原byteCount，零返空、非零重试，处理器异常原样传播。不改处理器槽/模式，不另建宿主new-handler状态。
3. 两构造本体只改分配callee，拥有析构只改文本释放callee与域注释。方法表、完整拥有位型、strlen/返回/发布/源指针重读、空失败和原复制门槛不改。尤其分配空时不清拥有标志、不改借用或造新异常；不缓存源指针而省略原重读。
4. 54B05D整份本体保持：基类析构释放EXH01文本后，flags&1仍交原FreeLegacyHeapMemory释放对象。54B116、所有派生构析/消息/捕获适配/ThrowInfo/抛出声明源码保持；不补假抛出、不变SDK或降低C06/C07门禁。
5. 头仅纠正text拥有域及显式生命周期注释，不改对象24B/方法表16B布局或类型。对象存储、借用文本、类型信息缓存、locale/filebuf/原堆域和六裸字节/21应用calloc不迁移。NSH01算法/私有入口也不重写。
6. 两共享53C9CF/53B4E7记录只更新本组实际callee：文本两分配/一释放标EXH01宿主，对象54B06D仍原域；拆开混写的文本/对象source关联，保其他原入向与身份。三变化体、对象删除/what边界及53C3BE/541A28当前责任共九既有语义记录同步，不添加新原F。

<a id="exception-text-heap-acceptance"></a>
## 4. 局部门禁

| 门禁 | 本组标准 |
|---|---|
| I | 文本两分配与一释放同域宿主；保原上界、零请求/真实处理器、拥有位型、空失败和源重读 |
| W | 所有当前拥有text写者来自两构造，同一析构释放；54B05D对象仍原堆，what/回退和派生清理原序保持 |
| S | 三变化体63条、两未改边界16条及复用85条可追踪；源码扣除两个私有入口/cstdlib、三callee及对应注释后同前提交全文一致；头只改注释 |
| R | 禁构建/测试/运行；实际分配失败/处理器异常、完整异常展开与SDK/CRT生命周期仍未验 |

只归档EXH01限定I/W/S，三变化体按本组适配审核。C08/C11与C06/C07、完整CRT和R继续open；不得把文本域关闭当成对象/异常ABI或整体SPEC完成。

<a id="exception-text-heap-source-closure"></a>
## 5. 本组源码收口

两私有文本字节入口已以std::malloc/std::free成对提供，原上界、零请求宿主1、同一541A28/原byteCount和处理器异常传播保持。54B079/54B0B6两原分配点及54B100文本释放改调该入口；方法表、完整拥有位型、strlen+1、先发布text/非空才重读源复制、空失败仍拥有、析构不清槽/标志保持。

54B05D对象释放仍原FreeLegacyHeapMemory，what/静态回退、所有派生构析/捕获适配/抛出声明及类型信息缓存源码不改；头仅两域注释调整，24B对象/16B基类方法表保持。NSH01和其他CRT/六裸字节/21应用calloc域不修改。文本和对象在语义source关联中明确分开，未以宿主文本free释放对象。

三变化体、两未改边界及四共享提供者共九既有语义记录同步，1152/1149有效ID、18pending/4分类引用保持。三变化体按本组adapted已审75→78/3680，两共享入口保原NSH01处置并补EXH01引用；不是整个57/120原入向或异常SDK收口。

三限定E-EXCEPTION-TEXT-HEAP-I/W/S-20261005使累计30→33，原63变化体/16边界/复用85条总164唯一指令只支撑本组。实现固定至Git命名快照cg2d-re-exception-text-heap-source-20261005。C08/C11及C06/C07、完整功能门禁与R继续开放，源码任务关闭10/60、叶验收0/182不提升；禁构建/检查产物/新增或执行测试/运行/摘要、音频暂停及原格式明文保持。
