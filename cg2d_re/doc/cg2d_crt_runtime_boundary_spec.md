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

## M3H01 通用模式3五操作同域宿主承接（2026-10-05）

[本组源码](cg2d_mode3_heap_facade_spec.md#mode3-heap-facade-source-closure)
已将通用mode3的分配/清零/扩容/释放/容量查询同时接SDK，
消除541FB1/541C88/542466七当前CPP调用及旧块前缀读取，
不造16B双链/压缩指针/影子容量或同名空体。
mode1/2、raw区域元数据/初始化与既有独立迁移域原文保持。

API数量W32、外层条件/NULL零/new-mode和失败旧块有效保持；
SDK分桶/容量/私有复制尾/错误策略为D02 adapted，
realloc失败handler统一原输入数量，旧某fallback物理对齐参数不同。
原selector3只兼容选择，实际mode3为SDK域；模式须在块存活期稳定，原Win32块不混入。
全部owner/未知alias/R1和SDK实际ABI/失败/正常快速退出运行仍待，
C01/C08/C11不完整关闭。

## M3P01 模式3私有核心接口退休（2026-10-05）

[本组合同](cg2d_mode3_private_core_spec.md#mode3-private-core-source-closure)
撤下541FB1/541C88/542466三个无本体、已无源码引用的声明；
原身份/799条/2344B和七原call保留。三原ID按D02登记host，
分别由M3H01的实际SDK分配/清零/扩容/释放角色承担。
原realloc迁移后的两free由SDK内部管理，不再重复执行旧free或区域参数。

不再把这三个旧私有内核列成工程待实现；不称原体等价/排除，
五外层adapted、模式1/2和原兼容元数据保持。
完整owner/模式寿命/alias/R1/R7与实际SDK/运行继续待，C01/C08/C11仍open。

## BPK01 两私有捕获职责具名移交（2026-10-07）

[本次合同](cg2d_native_cpp_exception_spec.md#cpp-catch-private-host-decision)/[完整原基线及SDK定位](cg2d_native_cpp_exception_evidence.json)将5402AB查找/5407BC捕获对象构造明确登记host：当前throw/throw;与六本机类型/显式copy供compiler/VCRuntime，两个旧x86内核不再是工程待补体。原315条875B、两唯一call/父邻窗及全部旧捕获记录保留；旧SDK/handler接线false不改，不用旧布局冒充本机元数据。

NCE01已接业务copy/单owner、消息/析构及所有旧构析正文保持，私有头仅加职责说明。当前安装SDK的frame.cpp能定位真实查找/复制服务，但本项目生成帧/链接和实际捕获展开仍未验。C06/C07/完整CRT继续open，实际typed捕获/清理、全成员/硬件SEH、全部owner寿命及SDK/正常快速退出运行标准不变；不新造捕获业务、异常模型或私有ABI。

## BWC01两个浮点分类叶实现（2026-10-08）

[基线](cg2d_float_classification_leaf_spec.md#float-class-baseline)、[决定](cg2d_float_classification_leaf_spec.md#float-class-decision)、[验收](cg2d_float_classification_leaf_spec.md#float-class-acceptance)、[状态](cg2d_float_classification_leaf_spec.md#float-class-source-state)及[原证/完整源前后态](cg2d_float_classification_leaf_evidence.json)。

546311/5461E4两完整100条236B/两页、一direct/零indirect、21跳七RET与两CALL有据。沿既有浮点环境CPP/HPP/ASM实现两原入口与两私有桥，共四原生本体；特殊位分类、真实x87落存后callee、正常零/正规比较和状态读取、次正规纯位分支与原S32掩码操作保。Win64私有值承载不镜像原参数栈，未消费W16不补零或作为输入；控制字/MXCSR与异常清理不增加，原环境/初始化/CMake和地图专用幂保持。

新原新1816/1817、两adapted F/CRT155→157及限定I/S登记；预期语义1818/有效1811、合同1928（52.39%）、F980/E1142，源码定位缺口171→169/CRT未知43→41。仅两个真实叶，全部pow父/机器ABI与正常运行继续原门禁，RPLAN.24不专项故障模拟；音频暂停、原格式明文与执行限制保持，C01/R01/V02/R04仍open。

## BWD01整数奇偶判定与舍入叶开发（2026-10-08）

[基线](cg2d_integer_parity_rounding_spec.md#integer-parity-baseline)、[决定](cg2d_integer_parity_rounding_spec.md#integer-parity-decision)、[验收](cg2d_integer_parity_rounding_spec.md#integer-parity-acceptance)与[原证/完整源前后态](cg2d_integer_parity_rounding_evidence.json)。

5436AC/5462FF两完整59条119B、三direct/两callee已落为真实本体。前者保TEST AL90H与两次舍入/F64落存、商覆写、状态比较及0/1/2分支；后者保FLD/FRNDINT/FSTP F64/FLD，沿当前控制字在ASM间返回ST(0)。不用宿主舍入、不增有限值门禁或环境写；既有分类、初始化、地图专用幂与构建归属保持。

两新映射1818/1819及两adapted F，CRT157→159；限定I/S不代完整提供者、__powhlp/pow父、机器ABI及运行验收。定位缺口169→167、范围内未定位CRT41→39，按生成台账核实；C01/R01/V02/R04仍open，音频暂停/原格式明文/RPLAN.24保持。

## BWE01完整幂特殊值辅助开发（2026-10-08）

[基线](cg2d_power_special_helper_spec.md#power-special-baseline)、[决定](cg2d_power_special_helper_spec.md#power-special-decision)、[验收](cg2d_power_special_helper_spec.md#power-special-acceptance)及[原证/完整源前后态](cg2d_power_special_helper_evidence.json)。

54357C完整114条304B/两页、唯一callee5436AC已开发并接真实依赖。三现文件六追加，一CPP入口/一ASM本体/一个参数桥；保原重复比较、条件FCHS、指数F64覆写重载及五常量。原S32零同时包含写出与不写出，不改是否处理布尔或补默认结果。

新1820/F:0014357C与一adapted F将CRT159→160，限定I/S不代两pow父的FSAVE/FRSTOR生命周期、完整提供者及机器运行。定位167→166/未定位CRT39→38，按汇总核实；原初始化、分类/整数舍入、地图幂及CMake保持，音频暂停/原格式明文/RPLAN.24与四open任务保持。

## BWF01三个通用幂依赖帮助体开发（2026-10-08）

[基线](cg2d_power_general_helpers_spec.md#power-helpers-baseline)、[决定](cg2d_power_general_helpers_spec.md#power-helpers-decision)、[验收](cg2d_power_general_helpers_spec.md#power-helpers-acceptance)与[原证/完整源前后态](cg2d_power_general_helpers_evidence.json)。

已补5433F0/543405/543435完整通用本体，复用BPJ01/BPM01三个38条111B原体，六CALL入向按实际目标统计；53D01D的CALL到543435及FLOW到53D022分列。三现文件五追加，一CPP入口/三ASM本体，保ST0连续结果、PC0300/007F临时CW与普通CMP/特殊10B F80重叠写、ST0/EAX/FLAGS合同。

三旧语义1424/1426/1427及adapted F扩展当前实现边界，CRT160/语义/F/定位队列数量保持。限定I仅控制字CPP→ASM真实桥，S覆盖三帮助体，E1146→1148；另外两ST0叶通用pow父与CRT分派/环境生命周期继续，原地图固定域/其它源码及CMake保持。

## BWG01五项数学错误链硬件依赖开发（2026-10-08）

[基线](cg2d_math_error_hardware_leaves_spec.md#math-hardware-baseline)、[决定](cg2d_math_error_hardware_leaves_spec.md#math-hardware-decision)、[验收](cg2d_math_error_hardware_leaves_spec.md#math-hardware-acceptance)及[原证/完整源前后态](cg2d_math_error_hardware_leaves_evidence.json)。

5478B2/D8/DB/E9/F8五完整42条105B/五页已开发，三现文件七追加、五CPP/三ASM与三私有桥。数学errno复用1085FE0单owner，1写33/2或3写34/其它不写且原输入照返；原零值叶不访问记录。保等待FSTSW、FNSTSW后FNCLEX与完整旧S16有符号返回、原W32掩码合并/低W16控制写和旧CW返回。

原32B错误记录只扩已证名称指针为x64自然40B，S32/三个F64及未写字段不预填，当前只是类型承载，真实记录生产父继续。新1821..1825/五adapted F、CRT160→165限定I/S，定位166→161/未定位CRT38→33；完整87except/raise/handle及范围出口/两pow父和运行仍按原门禁推进。

## BWH01完整数学掩码处理与三依赖开发（2026-10-08）

[基线](cg2d_math_masked_handler_spec.md#math-masked-baseline)、[决定](cg2d_math_masked_handler_spec.md#math-masked-decision)、[验收](cg2d_math_masked_handler_spec.md#math-masked-acceptance)与[原证/完整源前后态](cg2d_math_masked_handler_evidence.json)。

54769B处理器及54791B/54623E/5461BB三依赖已整组开发，四完整313条855B/七页、八direct三callee与九CALL入向。三现文件六追加、两CPP/四ASM六本体/两C桥，处理器五状态CALL/一分解CALL、分解两指数CALL都接真实本体。

原优先/RC结果、F64边界、S32指数和双W32移位/丢失位/符号恢复/选择性写出、剩余位0/1返回及原状态生产/等待保持；两个ST0接口只在ASM间，无宿主数学/新保护或额外故障样例。四新映射/F、CRT165→169限定I/S，定位161→157/未定位CRT33→29；完整87except/raise/范围出口与pow父/FPU生命周期/机器运行继续原门禁。

## BWI01原88B异常记录生产与完整分派父（2026-10-08）

[完整基线](cg2d_math_exception_dispatch_spec.md#math-dispatch-baseline)、[决定](cg2d_math_exception_dispatch_spec.md#math-dispatch-decision)、[验收](cg2d_math_exception_dispatch_spec.md#math-dispatch-acceptance)与[原证/源码前后态](cg2d_math_exception_dispatch_evidence.json)。

两完整5473E8/5460EB共352条899B/六页、七direct一IAT/八原调用位点已开发，现三文件七追加/七本体与五私有桥。原88B记录/24B单元/条件第二F64、位映射及未生产字节保；真实系统API与原硬件、ctrlfp/matherr/errno同源接线。

原共享5925B8实值2694H，不假设零或折叠原分支；保存CW仅读W16到局部W32，系统返回重读/原FFFFF3FFH及CH掩码/x87写回与S32尾返保。两新原新/F、CRT169→171限定I/S，定位157→155/CRT未定位29→27；上游记录/范围出口与pow父/FPU生命周期/机器运行继续原门禁。

## BWJ01数学错误记录生产与范围出口（2026-10-08）

[基线](cg2d_math_record_range_spec.md#math-record-range-baseline)、[决定](cg2d_math_record_range_spec.md#math-record-range-decision)、[验收](cg2d_math_record_range_spec.md#math-record-range-acceptance)与[原体/共享与载体窗/完整前后态](cg2d_math_record_range_evidence.json)。

543390/5433A7及5433B0真实共享段、5434D9三完整86条246B/三页已开发，三现文件五追加/三ASM本体，CPP仅六布局断言。单参不生产第二F64、双参原低高W32序/同一40B记录与88B展开帧，全局共享标签/真实87except和范围两CALL接通。

保原FST连续结果、六常量/缩放比较/真FMUL、原CW状态/类型与分参和027FH恢复。原上游PUSH W32再只覆盖低W16，32B借用输入与范围三EDX原载体返回不补零高W16；三原新/F、CRT171→174限定I/S，定位155→152/CRT未定位27→24。完整pow/分派与FPU生命周期/机器运行沿原门禁继续。

## BWK01完整通用幂四入口及FPU保存恢复（2026-10-08）

[基线](cg2d_power_generic_body_spec.md#power-generic-baseline)、[决定](cg2d_power_generic_body_spec.md#power-generic-decision)、[验收](cg2d_power_generic_body_spec.md#power-generic-acceptance)与[完整原体/源前后态](cg2d_power_generic_body_evidence.json)。

53D000/53D019/53D022两块与53D1F5四完整159条554B/五页已整组开发，一CPP/四ASM五本体、一个外部C值桥及原未初始化静态W32槽owner；三现文件七追加。CI/普通入口到core及全部依赖真接，13ASM调用、真实指数ST0消费/CL分类、原CW/加载样本/所有分支保。

原两实参F64落存/FSAVE108/真实powhlp/FRSTOR/结果读取与S32分派保，保存区不解释、未生产结果不填，只有外部值桥转换F64；原范围尾适为真CALL后释放正常帧。复用1266/1267、两新身份，CRT174→176/F999限定I/S，定位152→150/CRT未定位24→22；旧活动地图原点及共享分派/全应用和机器环境/运行继续原门禁。

## BWL01完整CI接入两个地图应用原点（2026-10-08）

[基线](cg2d_power_application_wiring_spec.md#power-app-baseline)、[决定](cg2d_power_application_wiring_spec.md#power-app-decision)、[验收](cg2d_power_application_wiring_spec.md#power-app-acceptance)与[窗口/源和六映射前态](cg2d_power_application_wiring_evidence.json)。

46F38D/46F43A两原CI点由现spawn/除数接口接完整CI/core/真实依赖，撤固定域FYL2X/TwoToTOS内联；两现文件五改动、一已有本体替换/零新身份。72B正常帧保原参数顺序/连续ST0消费与幅值指针跨CALL重读，不以EDX CW载体当地址；PC53/CW/截断、Cpp/RNG及提供者/CMake保持。

六旧映射/F当前绑定纠正，原历史/前态可追，MAP129/CRT176不增；两域四限定I/S、E1158→1162，定位150/CRT未定位22和全部数量保持。共享CRT分派/未知父和全应用/FPU生命周期及机器运行仍沿门禁。

## BWM01共享数学身份全部多入口开发（2026-10-08）

[完整基线](cg2d_math_shared_dispatch_spec.md#math-dispatch-shared-baseline)、[决定](cg2d_math_shared_dispatch_spec.md#math-dispatch-shared-decision)、[验收](cg2d_math_shared_dispatch_spec.md#math-dispatch-shared-acceptance)与[原体/原表/完整前后态](cg2d_math_shared_dispatch_evidence.json)。

543185整身份146条501B/三页已开发，包含幂帮助/单参与双参FXAM分派及全部特殊值/共享错误目标；三现文件五追加/十二ASM本体、13布局断言，只有一原身份。原16B分类表/4及16借用代码指针只拓地址，原数值样本/指数/索引/CW/T80/error0-1-7及末标志保。

两原间接尾改真实调用后LEA释放，幂实参/FSAVE108/真helper/FRSTOR与实际ECX数值保持，未生产结果不填。不因无已证直入或初始化指针而排除；原描述符生产/未知父与共享余链仍待。CRT176→177/F1000两限定I-S，定位150→149/CRT未定位22→21；整体门禁保持。

## BWN01共享指数链完整除法依赖（2026-10-08）

[基线](cg2d_math_divide_shared_spec.md#math-divide-shared-baseline)、[决定](cg2d_math_divide_shared_spec.md#math-divide-shared-decision)、[验收](cg2d_math_divide_shared_spec.md#math-divide-shared-acceptance)与[全部原体/原数据/源前后态](cg2d_math_divide_shared_evidence.json)。

沿542FEF真实依赖实现545360/545B51，两完整身份91条300B/三页；两现源四追加/两ASM本体，零CPP/C桥/新owner。原44B片段/T80/未生产padding、16B表/三F32、CW及非规格化循环和EAX/ECX/EDX真实合同保，核心实际调用。CRT177→179/F1002仅限定I/S，源码定位149→147/CRT余21→19。完整共享目标/原描述符生产及其它原调用父、机器运行仍待开发验收。

## BWO01完整共享幂、指数与对数目标（2026-10-08）

[基线](cg2d_math_shared_targets_spec.md#math-targets-baseline)、[决定](cg2d_math_shared_targets_spec.md#math-targets-decision)、[验收](cg2d_math_shared_targets_spec.md#math-targets-acceptance)及[整段原体/三R7/源前后态](cg2d_math_shared_targets_evidence.json)。

542FB0..543185全159条469B已开发，七原身份和三R7合并完整覆盖，26ASM/一个唯一已有FDIV owner只读桥、三源六追加。原幂/指数/对数和零无穷目标、保ST0的整数EAX0-1-2及三个共同RET真实接线；原越界丢内层返回以续行/JRCXZ/LEA保持控制流及末标志。CRT179→186/F1009仅限定I-S，定位147→140/CRT余19→12。三R7 mapped，已审470/余2591；原描述符生产/其它父与机器生命周期继续。

## BWP01完整64项除法分派及三包装（2026-10-08）

[基线](cg2d_math_division_dispatch_spec.md#math-division-dispatch-baseline)、[决定](cg2d_math_division_dispatch_spec.md#math-division-dispatch-decision)、[验收](cg2d_math_division_dispatch_spec.md#math-division-dispatch-acceptance)及[四原体/64表项/源前后态](cg2d_math_division_dispatch_evidence.json)。

545477全412条及545916/545929/545B3C三完整包装已开发，合432条1242B/九页、45CALL一callee/一表JMP、51RET/16原INT 6；四ASM、两现源四追加，零CPP/C桥/owner。64项原表只拓代码地址，126个T80实际偏移、原全部FXCH/弹栈重载/FDIV操作数和真实核心调用保。CRT186→190/F1013限定I-S，源码定位140→136/CRT未定位12→8；未知分派/内存加载父与机器生命周期继续。

## BWQ01八个内存除法加载父完整开发（2026-10-08）

[基线](cg2d_math_memory_parents_spec.md#math-memory-parents-baseline)、[决定](cg2d_math_memory_parents_spec.md#math-memory-parents-decision)、[验收](cg2d_math_memory_parents_spec.md#math-memory-parents-acceptance)及[全部原父/一F两R7/源前后态](cg2d_math_memory_parents_evidence.json)。

54593C..545B3C八个F32/F64/S16/S32正逆加载父已开发，全172条512B/16CALL两callee、12跳20RET。545A65九条27B是逆向F32父的内部TOP=0分支，与两个R7整体补齐，不造八个原F。两现源三追加/八ASM，原指数位直接运算、非等待TOP样本/额外T80交换保存恢复、输入位宽和原EAX保持，实际包装→核心接线。

CRT190→191/F1014仅限定I-S，定位136→135/CRT余8→7；两R7 mapped，已审472/余2589。外层原调用/未知描述符生产及机器生命周期继续。

## BWR01原余数双链与四共享入口完整开发（2026-10-08）

[基线](cg2d_math_remainder_chain_spec.md#math-remainder-chain-baseline)、[决定](cg2d_math_remainder_chain_spec.md#math-remainder-chain-decision)、[验收](cg2d_math_remainder_chain_spec.md#math-remainder-chain-acceptance)及[全部原链/帧常量/源前后态](cg2d_math_remainder_chain_evidence.json)。

545B66..5460EB全400条1413B/九页已开发，四原F与一个18B R7整组覆盖，六CALL四callee/42跳12RET；两现源四追加/八ASM，零CPP/C桥/owner。原48B片段/交叠W32、余数循环/缩放/模式/CW/状态及28B FNSTENV-FLDENV位规则、EBX2/中间及直接FPREM差异和FPATAN-FPTAN栈形状保，真实双父/入口接线。

CRT191→195/F1018仅限定I-S，定位135→131/CRT余7→3；R7已审473/余2588。外层原调用/未知描述符及全应用/机器生命周期继续。

## BWS01已列CRT余三身份与两共享区间开发（2026-10-08）

[基线](cg2d_math_final_exits_spec.md#math-final-exits-baseline)、[决定](cg2d_math_final_exits_spec.md#math-final-exits-decision)、[验收](cg2d_math_final_exits_spec.md#math-final-exits-acceptance)及[完整原体/共享后缀/源前后态](cg2d_math_final_exits_evidence.json)。

原53DEF3真实RET、543489连同完整543478指数父、5434C5完整溢出前缀及54349B精度出口已开发；三F九条26B加两R7二十条59B，共29条85B/一CALL五跳五RET。两现源四改/四ASM，既有范围体只增唯一全局入口，零CPP/C桥/owner。指数特殊重读、精度独有20位JZ/等待状态/类型8真实单参调用与CW完整W32恢复、溢出FST不弹ST0并直达共享后缀绕下溢均保。

CRT195→198/F1021两限定I-S，已列CRT未定位3→0/全源码定位128；R7已审475/余2586。三个身份具备源码位置不代完整CRT成员库存或门禁，外层实际调用/描述符、全应用生命周期及机器验收继续。

## BWV01原通用U64乘法/左移及输入接线开发（2026-10-08）

[完整基线](cg2d_u64_multiply_shift_spec.md#u64-multiply-shift-baseline)、[决定](cg2d_u64_multiply_shift_spec.md#u64-multiply-shift-decision)、[验收](cg2d_u64_multiply_shift_spec.md#u64-multiply-shift-acceptance)及[两全根/三调用/两CC与三源前后态](cg2d_u64_multiply_shift_evidence.json)。

542F10/544620两完整34条83B已开发为两个CPP本体及私有数值声明，原输入53DEF4的53E6EC/53E68B/53E6DB按10/4/3真实接线。原乘法高半零快路/两交叉W32乘加/低乘合成低64结果、原CL单BYTE和>=64零/32..63低半搬高/0..31分支保；原已有64位值不扩大字段或改地址，不加保护/默认计数、宿主CRT或故障样例。

完善既有462/463、两F adapted/CRT198→200/F1023两限定I-S；两个完整CC区间仅用途mapped，R7已审477/余2584。完整成员库存/输入核心和更多间接别名、正常寿命及机器ABI/链接/运行继续。

## BWW01剩余浮点指令片段实际开发（2026-10-08）

[原基线](cg2d_math_residual_leaves_spec.md#math-residual-leaves-baseline)、[决定](cg2d_math_residual_leaves_spec.md#math-residual-leaves-decision)、[标准](cg2d_math_residual_leaves_spec.md#math-residual-leaves-acceptance)及[两完整片段/原常量、五R7与源前后态](cg2d_math_residual_leaves_evidence.json)。

原543383四条7B和54341C七条25B现有两个完整ASM叶；CL单BYTE条件变号/原OR整数标志、EAX原W32位样本TEST 80000H与类型7或真实FADD后类型1、连续ST0保持，原54FB10常量复用现有同位只读F64存储。两现源三改，零CPP/C桥/帧/owner/输入验证或故障样例。

五R7共66B（32B代码、34B CC）完整正文或对齐用途映射，R7已审482余2579。两段无已知外部入向，只有限定S/E1183，不造F/成员或外部调用，不给I通过；F1023/CRT200与其它计数和完整门禁保持。完整原父/入向及描述符生产、成员库存/正常寿命和机器ABI/链接/运行继续。
