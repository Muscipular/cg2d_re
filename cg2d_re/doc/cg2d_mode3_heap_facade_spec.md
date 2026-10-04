# 兼容堆模式3成组宿主承接 SPEC（M3H01）

2026-10-05，按[D02](cg2d_crt_runtime_boundary_spec.md)
与[宿主总SPEC](cg2d_crt_host_migration_spec.md)推进C08。
固定快照：`cg2d-re-mode3-heap-facade-source-20261005`。
上位D02以[运行库边界](cg2d_crt_runtime_boundary_spec.md) §1为准。

<a id="mode3-heap-facade-baseline"></a>
## 原事实与域

[机器证据](cg2d_mode3_heap_facade_evidence.json)保五完整根449条/1167B：
53C3EA 46/116、53C0A9 45/105、53BFF8 72/177、
53FA5B 246/672、53FCFB 40/97。
三未实现模式3核心541FB1/541C88/542466共有7个实际call，
全部来自上述分配/清零、释放、扩容根；容量根模式3还读原块前W32再减9。

10869A8的已证直接引用只有五分配族读取和541BB8初始化写入。
当前源码也仅初始化写模式；迁移要求模式在其返回块存活期稳定，
不能在存活SDK块间切到1/2，不能将原Win32/模式2块交给模式3接口。
直接引用不排除未知alias/外部改写，完整生命周期和R1继续待验。

Malloc_53C3AC/Free_53C0A9、六裸字节、21应用calloc和已迁私有域保持。
原模式1/2、原堆/区域元数据构造与历史证据保留，不删除或伪实现三个旧核心。
模式3不再执行旧pool核心或读取其区域/块前缀；原selector=3仅兼容选择，
实际本域为SDK分配器，不把它报告成已还原旧池。

<a id="mode3-heap-facade-decision"></a>
## 成组开发裁决

1. 仅五入口的mode==3分支同时切std::malloc/std::calloc/std::realloc/std::free/
   UCRT _msize；模式1/2及其他正文原文保留。
   malloc零请求转1；calloc保低W32乘积、零变1/16对齐和既有上界。
   SDK失败不能落回Win32 HeapAlloc，避免本域混入两种来源。
2. 保malloc外层上界/enableHandler/原请求重试，calloc每失败轮new-mode/对齐bytes处理器；
   realloc保NULL→既有malloc、非空零→既有free/返回空、上界/失败new-mode/重试，
   SDK扩容失败原块仍可用，新增区域不清零，成功保存已有业务内容。
3. realloc模式3不再按旧区域/块头分组；SDK失败处理器统一收到未改的输入byteCount。
   原部分fallback会先向16对齐而改变handler数值；
   该旧私有分桶/物理请求/失败策略差异依D02具名adapted。
   不能称每次失败回调数值/时机或旧复制尾字节等价；原API业务请求及W32宽度不变。
4. 容量来自真实SDK块_msize并按原U32数量返回，不读memory-4、不造影子容量/压缩指针/
   分配标签表；SDK可用容量/对齐/地址/错误机制按D02。
   free空门禁保持；非空SDK块只std::free，同域配对，不回到旧HeapFree/FindRegion。
5. 不改模式/threshold/区域存储、初始化/异常/成员析构/业务清理顺序；
   旧Win32区域元数据不是SDK块，保其独立原有分配/释放。
   不新增启动、转换外来原池块或处理器/指针保护，不改变已迁私有域。
   后续任何新生产者/未知拥有者必须在该小步核定同一域；不以本组关闭所有caller/alias。
6. 五原身份及七数值call保留映射，三个旧核心不造同名空体。
   这是整族mode3实际提供者替换，不是三个旧内核还原；
   实际链接/SDK ABI、失败/模式稳定、完整CRT与运行未验。

<a id="mode3-heap-facade-acceptance"></a>
## 可度量标准

| 项 | 标准 |
|---|---|
| I | 五mode3操作均有同域真实SDK提供者，不余旧三个核心调用 |
| W | 外层请求/条件、失败旧块/成功业务内容、NULL/零、handler与分域明确；mode1/2及其他函数保持 |
| S | 五完整根449条/1167B、七原call及mode六引用可追；五分支和头注释限定重放 |
| 待验 | 模式存活期、全部拥有者/未知alias/R1、SDK实际失败/ABI/正常快速退出及R |

C01/C08/C11继续open，任务10/60、叶0/182与完整门禁不提升；
不构建/测试/运行/摘要，不把原分桶/块前缀变成SDK私有布局。

<a id="mode3-heap-facade-source-closure"></a>
## 源码小步收口

五mode3分支已分别malloc/calloc/realloc/free/_msize同域承接，
SDK失败不混入Win32fallback，原三个核心七CPP调用均消除，无同名假体。
容量不读块前W32，扩容不访问原区域链；
五分支/一个calloc防混域条件及include外CPP全文保持，
两头仅分域注释变化，mode1/2/原初始化/元数据、外层接口和全部其他源保持。

五完整原根449条/1167B、mode五读一写与七核心call有证；
业务请求/NULL/零/上界与SDK失败旧块可用保留。
realloc SDK失败handler为未改输入数量，旧部分物理对齐请求/分桶/复制尾及错误策略
明确D02 adapted，不能据此称每次失败行为或原私有ABI等价。
模式寿命稳定/SDK与原块不混用是本域合同，未知owner/alias和实际运行未验。

五旧记录定点/1202其他与全部政策保持，五新增adapted使151→156/3680；
限定E111→114；语义1207/1204有效ID、18pending/4分类引用保持。
C01/C08/C11继续open，任务10/60、叶0/182、完整门禁与R不提升；
SDK链接/异常、完整所有权/模式/正常快速退出及R1仍待。

**M3H01交付检查**：14项严格UTF-8无BOM/LF、718个本地Markdown链接、42项限定证据/决议引用及Git差异格式通过；measure生成与独立-Check均退出0。任务10/60、叶0/182及完整门禁不提升。
