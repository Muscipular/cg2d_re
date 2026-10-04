# stdio 指针索引表宿主清零域 SPEC（SCH01）

2026-10-05，按[D02分配域方案](cg2d_crt_runtime_boundary_spec.md)
与[内存合同](cg2d_crt_memory_contract.md)，迁移53E9A5的两个表分配点。
固定快照：`cg2d-re-stdio-table-heap-source-20261005`。

<a id="stdio-table-heap-baseline"></a>
## 原基线与实际域

[证据](cg2d_stdio_table_heap_evidence.json)保存五完整根260条/659B：
初始化53E9A5为59条/165B，calloc53BFF8为72条/177B；
取空闲流53F848、刷新53BBA2及关闭5446AD共129条/317B。
calloc有23条已证直接call：21应用点沿既有宿主裁决保持，
另2为本初始化的53E9C7/53E9E0，不据此排除间接caller。

原索引表1086AC4只有本初始化两次发布，四根共十条已证直接引用。
初始化先将0限制设512、小于20设20；首次空才限制设20、再次分配，
第二次空进入错误26。成功后反复读取当前全局表写入20个实际静态FILE地址，
再检查底层0..2句柄，INVALID/空才置对应descriptor=-1。
当前表项已按原地址拓宽为LegacyFile*，两个size参数为既有sizeof(pointer)，
逻辑count与限制S32、乘积/请求/handler数值继续W32。

原calloc先取低W32乘积；不大于FFFFFFE0时零变1并向16对齐，
尝试失败后读取1086150；非零才向541A28传当时对齐bytes，零/返回0结束，
非零重试、异常传播。模式3私有分桶及mode2/HeapAlloc域由宿主承接。

本域是指针数组块，不是数组中各FILE对象/字节缓冲。
当前已知消费仅索引、发布/借用FILE、取流/刷新/关闭；
5446AD释放的是index>=20的FILE对象，并重新读取当前表归空该项，
没有释放整个表。已知路径无整表扩容/容量查询/释放，原保留存活期不改；
十条直接引用不是全端不存在未知alias/free的证明，R1和完整退出仍待。

<a id="stdio-table-heap-decision"></a>
## 具名开发裁决

1. LegacyStdio.cpp内新增本域私有、返回LegacyFile**的清零提供者，
   使用std::calloc(1,bytes)。两个真实53BFF8调用点改调用本提供者；
   不修改全局CallocateLegacyMemory或21应用calloc、动态FILE的Malloc/Free域。
2. 保U32 count*elementBytes低乘积、既有上界/零变1/16对齐顺序。
   超上界不分配，但原失败后new-mode/handler循环仍运行；
   成功不读new-mode，失败每轮读真实1086150并向541A28传bytes。
   不补乘法保护、扩大数值到size_t再乘、不捕获或返回假成功。
3. SDK分桶/容量/地址、清零已请求对齐跨度、低内存/errno/处理器内部策略
   为D02 adapted；原mode3只清逻辑乘积的私有尾差异不称等价。
   正常表项零初态、逻辑数量、512/20/错误26及失败消费保持。
4. 全表现有已知操作为分配/清零与借用，成组切换这两种操作；
   不新增整表Free/resize/query、自动析构或退出登记，不清旧表/修重初始化泄漏。
   任何以后发现的全表拥有操作必须在该小步核同一SDK域，不能交旧HeapFree/块头查询。
   原未知alias/完整退出仍保留，不把局部无释放观察外推全进程。
5. FILE布局/20静态记录/临时缓冲、表限制/计数、描述符生产与标准句柄、
   取流/读写/刷新/关闭正文和原调用顺序保持。
   不借SDK FILE、不启用C5/旧cinit/原入口，本步只补真实兼容表生产者。

<a id="stdio-table-heap-acceptance"></a>
## 本组验收标准

| 项 | 标准 |
|---|---|
| I | 两初始化调用同一真实SDK清零提供者，LegacyFile**返回/既有pointer宽度，无旧模式3表分配 |
| W | 512/20回退/错误26、逐项20存储/3句柄、原handler/失败消费及消费/表项释放域保持 |
| S | 五完整根260条/659B、两实际call/23入向/十数据引用可追；限定源码重放，原calloc/消费者及其余源头不变 |
| 限制 | 全表已知无释放/query/resize，不造新操作；未知alias/完整owner/STDIO启动及SDK失败/运行仍待 |

C01/C08/C10/C11继续open，任务10/60、叶0/182和完整门禁不提升。
禁止构建/测试/运行/摘要，RPLAN.24和原格式明文保持。

<a id="stdio-table-heap-source-closure"></a>
## 源码小步收口

两个真实初始化callee已接本域std::calloc，helper保持U32乘积、
零变1/16对齐/上界、成功短路及失败new-mode/真实handler次序。
四类限定差扣除后一源全文与前提交一致；其余src/include、
通用原calloc和21应用域、三表消费者/所有FILE与描述符逻辑保持。
未增加整表释放/query/resize、退出登记或活动C5初始化。

五完整根260条/659B、23 calloc直接入向/两个本表点、十表引用可追；
20静态FILE逐次全局表存储与三句柄、512/20/错误26及动态FILE项释放保原消费。
只切全表已知allocate/clear，不把进程期表块当其FILE对象、SDK FILE或旧堆块头；
未知alias/free、完整STDIO/owner/SDK实际失败和运行仍待。

两旧语义定点更新/1205其他与全政策保持；
初始化一新增adapted使150→151/3680，共享calloc仅按本域关联，不全体重复审核。
限定E108→111；语义1207/1204有效ID、18pending/4分类引用、
任务10/60、叶0/182与完整门禁/R保持。
C01/C08/C10/C11继续open，不能把本表提供者代完整CRT初始化/退出或原内核等价。

**SCH01交付检查**：13项严格UTF-8无BOM/LF、731个本地Markdown链接、36项限定证据/决议引用及Git差异格式通过；measure生成与独立-Check均退出0。任务10/60、叶0/182及完整门禁不提升。
