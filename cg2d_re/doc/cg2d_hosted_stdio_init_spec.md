# 宿主兼容 FILE 表初始化 SPEC（HSI01）

> 2026-10-09当前FILE边界：[MC01-B](cg2d_crt_host_migration_spec.md#msvc-crt-file-source-state-20261009)已将全部现FILE/描述符/流格式与状态/位置/关闭改真实MSVC服务，旧表/缓冲/私有入口及C表退出当前链；下文旧内部布局/发布机制只属固定历史。业务字段/字节、真实结果/owner与全部适用门禁保持，未编译/运行；完整C++余组继续。

2026-10-05，聚焦已发现的FILE表生产缺口，沿既有工程委托实施。
上位：[宿主前置](cg2d_hosted_runtime_prefix_spec.md)、[表块分配域](cg2d_stdio_table_heap_spec.md)。
固定快照：`cg2d-re-hosted-stdio-init-source-20261005`。

<a id="hosted-stdio-baseline"></a>
## 原基线与当前缺口

当前LegacyMouse宿主前置已生产兼容堆和低层描述符，但没有调用
InitializeLegacyStdio_53E9A5。表指针初值null、上限0，
AcquireLegacyStream_53F848因此不遍历/取流，FlushLegacyStreams_53BBA2也没有表项可刷新。
WinMain的CGstart分支实际经OpenLegacyStream读argument.ini；此处不改其业务逻辑。

[本组证据](cg2d_hosted_stdio_init_evidence.json)复用SCH01已核完整初始化根59条/165B；
新取原C5的五槽/20B及初始化唯一553174数据引用，不重验已过初始化/分配/取流/刷新正文。
原553174→53E9A5在553178→546CAE之前，且原入口ioinit在cinit之前。
原根保持512默认、最小20、第一次失败回退20、第二次失败错误26，
20个真实静态FILE地址逐次发布及底层0/1/2句柄检查。

<a id="hosted-stdio-decision"></a>
## 实施裁决

只在LegacyMouse既有LegacyMultibyteStaticInitializer中，低层描述符初始化之后、
普通stdio退出登记与多字节初始化之前，加入InitializeLegacyStdio_53E9A5()。
顺序成为heap(0)/失败28→lowio→stdio→原普通stdio即时登记→multibyte→既有应用投影。

复用已有唯一LegacyFile表/静态记录及SCH01 SDK表块提供者，不新建SDK FILE副本；
不固定上限、不添加惰性重复初始化、成功标记、保护、拒绝或回滚，
不改provider原正常/失败分支、返回/字段发布/重读、表块与动态FILE项的分配域。
错误26沿原provider/退出钩子，不改为异常捕获、继续成功或宿主新错误码。
普通退出登记和callable槽发布沿HPI01保持；不增加全表释放、HeapDestroy或额外关闭。

原C5私有退出表/异常filter等其余角色继续分别处理，不自动执行整套C5/cinit/90表或旧entry。
SDK标准流配置/pubSync、WinMain与应用投影保持；原70源＋15SDK/85对应计数不增。
同TU应用首次使用的stdio前置已接；跨TU首次使用、未知alias和完整SDK/继承ABI仍待。
旧C5时点移入宿主静态适配为D02/D03已授权差异，不声称旧SEH/完整退出等价。

<a id="hosted-stdio-acceptance"></a>
## 限定验收

- 一CPP三片段：stdio头文件、前置注释和一个真实调用；其余src/include不变。
- 原lowio在前，stdio在登记/multibyte及同TU应用投影之前；无新增重复调用/owner。
- provider、Acquire/Flush、错误26路径和WinMain正文保持，不用静态接通宣称实际文件成功。
- 一条既有语义补接线，登记簿/唯一进度/新增引用及派生看板静态一致。
- 未构建/测试/运行/摘要；C01/C02/C04/C08/C10/C11/V02仍open，
  全FILE/配置流程、全部owner/稳定模式、跨TU/unknown alias、
  normal/quick/callable全序、SDK运行和R1/R7/最终验收不提前关闭。

<a id="hosted-stdio-source-closure"></a>
## 源码小步收口

接入已有stdio表生产者，使同TU应用初始化前的兼容取流/刷新使用真实表，
原512/20/错误26及20静态记录/三句柄规则均由原provider继续承载。
只核本缺口、必要C槽/原序背景与新增差异；原根/业务消费者不全面复检。

语义1226/1223有效原ID、195/3680函数处置、15/218导入保持；
限定E150→153。七关联任务保持open，关闭10/60、叶0/182与完整门禁不提升。

限定检查通过：一CPP三片段/一个stdio调用，原provider、取流/刷新、退出适配与WinMain保持；
一语义补充/登记簿精确转换，11项UTF-8无BOM/LF、10个新增Markdown链接、
33个本组E引用及Git差异格式通过。看板生成及独立Check通过；不扫描历史Markdown，不提升完整门禁。
