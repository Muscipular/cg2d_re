# CRT 堆、异常与文件边界子 SPEC

> D02；2026-10-04采用，细化C05–C12。上位：[宿主承接SPEC](cg2d_crt_host_migration_spec.md)、[收尾决议](cg2d_closure_decisions_spec.md)。
> 选择宿主职责承接来解决尚未闭合的CRT私有实现，不为“统一标准库”回退已有业务兼容逻辑；具体活动调用迁移必须逐组证明。

## 1. 分配域：停止复造旧模式3私有链

采用宿主原生内存提供者承接待迁移CRT私有域，不把两个x64指针硬塞16B空闲块；不改原业务请求数值来凑布局，不造压缩指针/影子链，也不补541FB1/541C88/542466假体。

迁移单位为同一域的分配、清零、扩容、容量查询和释放**整组**，从上层真实域边界切换，消除活动分支对旧模式3链头/块前缀的读取。不能只让Allocate返回宿主块，再交旧Free/Resize或用原-4字节读容量；不得将宿主块与已有Win32原堆/模式2块混用。

| 必须保留 | 可接受的私有差异 |
|---|---|
| 应用传入位宽/调用序、实际空/失败消费、清零的业务可见范围、成功保留内容、扩容失败原块仍可用、owner配对 | CRT内部分桶、节点/地址、分配失败时机、可用容量和库内错误策略按宿主 |
| 已有Malloc/Free窄合同、21应用calloc逐点来源/配对、业务直接可见状态 | 不再复造模式3区域/组/16B链；原模式选择不作为宿主真实分配算法的虚假报告 |

已有原堆/模式2实现和原证据保留，未迁移域继续其完整合同。若容量/模式/错误槽被业务读取并影响输出，须保留兼容访问或证明迁移结果；无法对齐则该域暂不切换，不能对业务返回伪容量/固定模式。本决议解除“必须解决16B双链物理布局”的工程选择，不把分配域迁移记为已完成。

## 2. 异常：采用本机编译器的抛出、捕获与展开

不手工制造x64 ThrowInfo/RVA，不模拟原x86帧/FS/SEH表，不把现有LegacyException存储或TypeDescriptor直接当作宿主异常ABI。保留既有原对象构析、复制、消息和元数据证据。

54B123及5402AB/5407BC私有职责由实际本机异常设施承接；在真实调用边界建立明确的逻辑异常种类/消息/复制与析构映射，由编译器生成其ABI，父caller同步改为该实际提供者。重抛必须传播当前异常，不能新造固定异常；捕获/cleanup/终止顺序与业务可见返回分别核，不吞异常或返回成功。

禁止仅定义同名空包装来称原内核已还原。原生SEH与C++异常路径分别记录，不用统一catch替代所有硬件异常；原合法失败和清理仍须映射。禁用RTTI保持，不能引入dynamic_cast/typeid作为适配。SDK兼容性及实际展开待后续验证，不由设计宣称通过。

## 3. FILE、格式与locale：已有兼容路径优先

已接LegacyFile/格式化/扫描/字符串/多字节本体继续服务现有业务；不批量换fopen/sprintf或把宿主FILE*塞入LegacyFile布局。宿主可接未闭合的纯CRT设施，整组迁移时必须同步类型/数据/消费者/释放。

%n、精度/浮点、原编码、部分读取/EOF/错误、重复关闭/不清槽、格式输出和业务解析等已有可见合同保留；宿主默认行为若改变实际封包/文件/布局，保留相应兼容层。宿主内部locale不反写旧原字节状态，不全局开旧格式选项掩盖差异。

codecvt的工程方向是宿主内部转换器由宿主完整流类型持有；留在LegacyFileBuffer上的消费者仍须真实facet/方法/状态生产，不能桥接半张表或永久空指针。来源编码未知时继续原字节，不猜CP950/UTF-8或新转换策略。

## 4. 实施顺序与验收

1. 给每组列原入口/数据、当前活动caller、host提供者、保留兼容部分、批准差异；原身份不可从清单删除。
2. 优先处理未实现私有依赖；在同一小步替换整组活动调用、状态和释放，静态检查无跨域、旧布局误读、悬空声明或双重owner。
3. 已有函数只因确有业务冲突或接口衔接需要修改；不为删除旧实现而批量重构，不用旧测试数证明新边界。
4. C06/C07以真实异常映射和调用接通关闭，C08以分配域迁移关闭，不再要求补原私有内核全体；C10–C12仍逐原调用分类/接线，既有989真实格式等数量不变。
5. 运行库内部差异标“获批宿主承接”，不得标“原体等价”。实际初始化、失败清理、文件/封包结果、链接/运行依旧待验；不构建/测试/运行/增加依赖。

## 5. NSH01窄字符串字节域成组实施

[局部SPEC与源码收口](cg2d_narrow_string_heap_migration_spec.md#narrow-string-heap-source-closure)已将现有LegacyNarrowString的两私有字节入口成对改std::malloc/std::free；保原上界、零请求与真实541A28处理器循环。九字符串本体/逻辑容量/共享U8及分配→复制→释放不变，不查询宿主可用容量或旧堆块头。

该字节域从原对象存储/异常基类文本/locale facets/filebuf对象等其他CRT域分开；其他域继续原堆，现有六裸字节和21应用calloc不改。两个共享入口只按本域审核，非原全部57/120入向闭合。C08/C11仍open，C06/C07实际抛出/SDK及运行仍待；静态局部证据不代整个CRT迁移。

## 6. EXH01异常基类拥有文本域实施

[EXH01局部源码](cg2d_exception_text_heap_migration_spec.md#exception-text-heap-source-closure)已将两文本分配和一拥有析构释放成对接私有std::malloc/std::free，保原请求/上界/真实处理器、完整ownsText位型、源指针重读、分配空仍原拥有状态及不清槽的析构行为。NSH01的共享标记字符串块与该基类文本是独立owner，不能混作一个对象。

对象54B06D仍原堆释放，what/回退/派生构析/捕获适配/抛出声明及其他域源码保持；无宿主异常ABI转换。九既有语义、三变化体处置和三限定证据只收口EXH01，C08/C11与C06/C07、完整CRT/SDK及运行仍未闭，不把原共享53B4E7全部释放都改成宿主。

## 7. NCE01九抛出点与五类本机异常

[NCE01源码收口](cg2d_native_cpp_exception_spec.md#native-cpp-exception-source-closure)已将当前九原54B123入向成组迁移：五Raise在本机prvalue中一次本族copy，两字符串异常/ios failure已准备payload无增引用转移，clear当前异常用throw;。六独立native_cpp类型保原五层级/全局exception，单一compiler owner按原copy/非删除析构与what处理业务payload。

旧可调用54B123/五opaque ThrowInfo声明撤销；原数据/数字call保留，不手造SDK/FS或将旧对象直接当本机异常。所有旧copy/析构/what/方法表和临时串清理正文保持。C06私有查找/硬件SEH、实际compiler ABI/捕获/SDK及运行仍未闭，C06/C07保持open，不把原198指令或新增类型源码计为完整门禁。

## 8. LCH01实现对象域及两退出登记

[LCH01源码收口](cg2d_locale_object_heap_migration_spec.md#locale-object-heap-source-closure)已将当前唯一_Locimp对象64B存储的分配、Ctor失败释放、正常flags删除成对宿主承接，保U32请求/原handler；两实际退出登记改std::atexit同一原回调，原发布/返回消费、引用与清理保持。base facet/向量仍旧域、名称仍NSH01，未迁移SDK/其余对象或删除原虚清理。

经典55315C全表caller仍未接，不新增模块或提前登记；原活动流与宿主流归属、全应用序/快速退出、codecvt与完整C03/C04/C08/C11及运行未闭。局部域闭合不代全CRT或原私有内核等价。

## ETH01 兼容退出表块成组宿主承接（2026-10-05）

按[ETH01分域与源码收口](cg2d_exit_table_heap_migration_spec.md#exit-table-heap-source-closure)，
仅LegacyRuntimeExit的32槽初分配、两实际块容量查询及4原生槽增长扩容同时切换
std::malloc/std::realloc/std::free/UCRT _msize。容量来自实际块，原U32/new-mode/
541A28、共享首末/发布及失败消费保持，不读mode3块头/原HeapSize，不存影子容量。

兼容callback登记和正常/快速退出本体仍原算法，不改为std::atexit，
不提前执行C5/17标准流项，不新增整表free/模块析构或旧表覆盖时清理。
两变化根为adapted，分桶/内部对齐/容量/地址/低内存及宿主错误机制按D02，
其它环境/对象/业务原堆、已迁NSH01/EXH01/LCH01与宿主登记组保持。
本步仅局部静态，不关闭C03/C04/C08/C11或完整CRT/启动/SDK/运行。

## SSX01 标准流登记提供者当前边界（2026-10-05）

[SSX01](cg2d_standard_stream_exit_migration_spec.md#standard-stream-exit-decision)
只将18个实际标准流即时登记由兼容53B406改宿主std::atexit，
与LCH01两locale登记共同准备后续活动序接入。
原构造完成后登记同一callback、构造异常传播、失败不回滚和完整宿主S32保持；
原0/-1与宿主非零返回的具体位型不称相同，只有原表调用不消费EAX已有证。

退出表内部增长/错误机制按D03，原14存储/四Init、引用/计数/FILE/locale/
虚调用/flush和所有构析body及分配域保持，不强转SDK对象/新增保护或活动调用。
17活动项/真实facet与完整registry、正常/快速/callable退出和SDK运行仍待，
C02/C03/C04与整个CRT不闭；ETH01兼容退出表及其他原caller保持。

## SCH01 stdio 指针索引表清零域（2026-10-05）

[本组源码](cg2d_stdio_table_heap_spec.md#stdio-table-heap-source-closure)
将53E9C7/53E9E0的表块分配/清零成组切私有std::calloc，
保低W32乘积/原对齐上界、新模式/真实handler与512/20/错误26。
SDK内部尾零/容量/分桶/失败策略按D02 adapted，21应用calloc和通用旧堆不改。

进程期表数组与其FILE对象/字节缓冲分域；已知无整表free/query/resize，
不造新生命周期动作或C5调用，原借用/动态FILE项std释放保持。
未知alias/free、完整STDIO/容量/owner及正常快速退出/SDK运行未闭，
C01/C08/C10/C11不关闭，其他域继续逐组迁移。
