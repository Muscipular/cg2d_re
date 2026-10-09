# 流缓冲方法表指针类型 SPEC（STP01）

> 2026-10-10当前C++边界：[MC01-C1](cg2d_crt_host_migration_spec.md#msvc-crt-cpp-source-state-20261010)已退休无现组外消费者的六组私有C++库及NativeCppException头。当前采用真实MSVC标准流/Init/locale及库内string/facet/codecvt/异常支撑，现流配置/缓冲借用/刷新、业务构析与退出登记保持。下文旧Legacy对象/方法表/私有登记/异常payload及其源码状态属固定历史，不再要求复刻；完整业务/owner/未知原alias与SDK实际运行仍待，未编译/运行。

> 2026-10-09当前FILE边界：[MC01-B](cg2d_crt_host_migration_spec.md#msvc-crt-file-source-state-20261009)已将全部现FILE/描述符/流格式与状态/位置/关闭改真实MSVC服务，旧表/缓冲/私有入口及C表退出当前链；下文旧内部布局/发布机制只属固定历史。业务字段/字节、真实结果/owner与全部适用门禁保持，未编译/运行；完整C++余组继续。

2026-10-05，仅推进已知方法表地址的类型缺口。
上位：[原stdio合同](cg2d_stdio_contract.md)、[运行库边界](cg2d_crt_runtime_boundary_spec.md)、
[既有输出数量适配](cg2d_file_buffer_output_alignment_spec.md)。
固定快照：`cg2d-re-stream-table-pointer-source-20261005`。

<a id="stream-table-pointer-baseline"></a>
## 具体缺口与已知提供者

[原表证据](cg2d_stream_table_pointer_evidence.json)保存四张已加载13槽表、52项/208B与12个原赋表引用：
窄基类55000C/派生54FFD4，宽基类5500A8/派生550070。
四张当前实际const表及各自13字段接口均已存在，STP01不重写表或旧方法本体。
窄表对应LegacyNarrowStreamBufferMethods，宽表对应LegacyWideStreamBufferMethods。

当前LegacyStreamBufferStorage<Character>::virtualTable仍擦除为const void*；
三个CPP的十个读取点随后转回已知窄/宽类型，分别是streambuf六处、filebuf两处、ios同步两处。
同一字段只承载这两类已知实际表，不需要类型擦除。

<a id="stream-table-pointer-decision"></a>
## 实施裁决

按既有工程委托，在原内部头前置两方法表声明，以标准type_traits的conditional_t
建立Character到既有窄/宽表类型的别名；storage只支持已实现的char/uint16_t两种字符类型。
字段改为对应const方法表指针，保持同一个非拥有地址槽，不建立新表、owner或SDK对象。
编译期字符类型约束只表达该内部模板既有范围，不增加业务/输入/容量门槛。

三个CPP十处去掉转回表类型的cast，直接读取同一字段；
快照位置、underflow跨uflow/pbackfail缓存同一表、批量循环的逐次重读和ios同步消费均保持。
原四表定义/槽顺序、十二赋表、locale/FILE/游标别名、分阶段清理及数量/返回/异常规则不变。
仅类型地址槽调整；96B缓冲/104B基类、152B filebuf及104B方法表的既有布局断言保持。
标准头不属外部依赖；不引入RTTI或业务框架。

本步不是codecvt转换器方法表；非空facet实际生产/完整方法表仍缺事实，C05继续open。
不把filebuf区间只见清零的观察扩张为全程序无生产，不造空转换器或排除转换分支。
C03全活动流/owner/SDK、R1/R7与实际ABI/链接/运行继续待；现行执行限制及RPLAN.24保持。

<a id="stream-table-pointer-acceptance"></a>
## 限定验收

- 四源码文件：内部头四块类型调整、三CPP十处同字段表读取；业务语句与表定义保持。
- 四实际原表52项/208B、十二赋表引用与已知窄/宽提供者可追，不验全部方法父/运行。
- 本步差异/新增引用/四既有语义补充与登记簿静态核，不扫描历史文档或全面复检。
- 不构建/测试/运行/摘要，不声称新增原函数/完整facet生产、SDK ABI或功能门禁通过。

四既有构析语义仅补类型合同，其他1220记录和政策保持；
语义1224/1221有效原ID、18pending/4分类/无ID三条、193/3680函数处置及15/218导入不变。
限定E141→144；任务10/60、叶0/182及完整门禁不提升。

<a id="stream-table-pointer-source-closure"></a>
## 源码小步收口

virtualTable已为对应窄/宽实际const方法表指针，三CPP十个读取点直接承接该字段。
内部头四块与十处读取外源码保持；表定义/槽顺序/十二赋表、读取快照和全部业务语句不变。
四原表52项/208B及12赋表引用归档，只证明本步已知地址类型，不代完整方法/父/owner或运行。
四既有语义补类型合同，其他1220记录及政策保持；语义1224/1221有效原ID、193函数处置和15导入不变。
限定E144；C03/C05/R02继续open，任务10/60、叶0/182和完整门禁不提升。
真实codecvt/facet生产与完整表、全部活动流/alias/owner、SDK/实际ABI/链接/退出/运行仍待。

**STP01交付检查**：本步14项严格UTF-8无BOM/LF、12个新增Markdown链接、36项限定证据/决议引用及Git差异格式通过；四源码限定重放、内部头四块/十读取同型、原四表52槽/208B和十二赋表引用及源码赋表保持、四既有语义/登记簿限定重放、measure生成与独立-Check通过。未扫描历史文档或全面复检，旧表/方法与业务正文保持；函数处置193/3680、任务10/60、叶0/182及完整门禁不提升，真实codecvt生产/完整表和实际ABI/运行仍未验。
