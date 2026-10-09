# 五类本机 C++ 异常接线 SPEC

> 2026-10-10当前错误/退出边界：[MC01-C3](cg2d_crt_host_migration_spec.md#msvc-crt-exit-source-state-20261010)已将CRT私有退出/信号/消息与钩子、console及重复filter层交MSVC，删除纯CRT收尾回调/唯一登记；真实业务std::atexit与数组构析保持。两致命入口真实_Exit(255)不输出旧R600x/MessageBox，数组二次异常真实terminate；旧库字段/表与原filter生命周期只作固定历史，不再要求复刻。heap/multibyte前置与所有适用业务/owner/SDK/正常快速退出运行门禁仍待，未编译/运行。

> 2026-10-10当前C++边界：[MC01-C1](cg2d_crt_host_migration_spec.md#msvc-crt-cpp-source-state-20261010)已退休无现组外消费者的六组私有C++库及NativeCppException头。当前采用真实MSVC标准流/Init/locale及库内string/facet/codecvt/异常支撑，现流配置/缓冲借用/刷新、业务构析与退出登记保持。下文旧Legacy对象/方法表/私有登记/异常payload及其源码状态属固定历史，不再要求复刻；完整业务/owner/未知原alias与SDK实际运行仍待，未编译/运行。

> NCE01；2026-10-05按D02与工程委托实施。[运行库边界](cg2d_crt_runtime_boundary_spec.md) §2批准以真实本机抛出/捕获承接私有54B123职责；本组迁移所有当前九原点，不复造旧核心、ThrowInfo/RVA或FS链。C06、硬件SEH及完整SDK/运行仍开放。

<a id="native-cpp-exception-baseline"></a>
## 1. 原体、类型及当前缺口

[只读证据](cg2d_native_cpp_exception_evidence.json)保留54B123二十四条、五Raise各十条、54A67A/54A899各35条、549F62六十条；九根204条计入重复共享尾，唯一198条。54A30D属三个原根，不拆新F。5501E8八W32种子为E06D7363/1/0/0/3/19930520/0/0；54B123最终RaiseException，原非继续标志为1。九直接入向与当前源码九调用对应。

五原ThrowInfo的真实unwind及捕获数组如下，属性均0，基础exception原名为全局.?AVexception@@，不能未经映射当成宿主std::exception：

| 原ThrowInfo | 派生→基类 | 原copy / unwind | 本机类型 |
|---|---|---|---|
| 551880 | logic_error→exception | 54A7AE / 54A755 | LogicError→Exception |
| 551810 | length_error→logic_error→exception | 54A881 / 54A80B | LengthError→LogicError→Exception |
| 551938 | out_of_range→logic_error→exception | 54A969 / 54A8F3 | RangeError→LogicError→Exception |
| 551658 | runtime_error→exception | 54A0E5 / 54A08C | RuntimeError→Exception |
| 5515E8 | failure→runtime_error→exception | 54A1B8 / 54A142 | StreamFailure→RuntimeError→Exception |

当前五Raise已先本族copy到临时28B原对象视图，再依opaque ThrowInfo调未实现54B123；两个字符串异常及ios clear已构造临时异常，另有clear null/null重抛。旧数据可追踪，不构成可用x64 SDK对象。所有本族新端字段视图都是同一已证48B LegacyLogicErrorObject别名；基类为24B，message为24B。NSH01/EXH01已分别承接message/text块，非删除析构负责字段，不释放对象storage。

## 2. 单一原生owner与复制策略

真实compiler异常对象持有一个48B业务payload，不把Legacy对象本身传作SDK异常。普通派生tag都经同一持有者；只有最内层owner按实际类型调用一次原非删除析构，禁止基类/派生重复清理。payload字段/宽度保持，构造前不补零或解释未写prefix/padding；新本机虚表、类型名/布局/记录属于D02私有ABI差异。

原catchable数组证明不同捕获层级使用各自copy。基础Exception按值复制只走54B0B6/54B100，不读message；按值截成LogicError/RuntimeError分别走对应copy/析构；具体派生仍各自callback。不得默认浅复制拥有块或删除赋值操作后偷偷生成浅复制；本组不提供未使用的赋值/移动操作。

<a id="native-cpp-exception-decision"></a>
## 3. 具名实施决定

1. 新增本模块私有NativeCppException.hpp，声明cg::platform::native_cpp六种真实C++类型及同一payload owner；实现落在既有LegacyException.cpp，不改CMake/新增cpp或依赖。Root Exception不继承std::exception，避免加原未证明的标准库捕获层级；旧全局exception/五族映射到本机具名tag。
2. owner的kind只控制本机类型复制/清理；原methods、text、完整ownsText、message/prefix/共享标记与逻辑容量仍在同一payload，不造消息副本。what()基础用既有54B116，五派生用共享54A062，保原空回退与消息字节，不使用std::ios_base::failure后缀。
3. 五Raise直接throw对应本机prvalue；本机构造在同一表达式调用一次原本族copy。删原手工临时副本再传54B123的路径，避免复制两次。项目既有C++20提供同类型prvalue直接构造异常对象；不额外调用原构造/RNG或预清。
4. 54A67A/54A899和549F62已经构造异常payload，改throw对应类型的AdoptPayload入口。入口只以trivially-copyable字节转移现48B payload，既有message/text指针和标记不增引用，不清原临时字段、不追加原临时析构。该POD临时无自动析构；只有compiler持有的异常对象最终按原ThrowInfo unwind回调清理，避免双重owner。原临时message catch/Tidy/throw原序保持。
5. clear低U8重抛分支改直接throw;，传播当前异常；不新造failure或成功返回。没有活动异常时由本机语言运行时处理非法重抛，不加备用异常/保护或专项模拟。状态/掩码/最高U8前缀、优先消息及单state临时串清理源码保持。
6. 明确定义基础和所有五派生copy构造，对应原捕获层级/切片；删除复制赋值，防止默认浅赋值。非删除析构按kind仅调用一次原本族析构，异常继续原传播；what不使用RTTI。禁止dynamic_cast/typeid、手工x64 ThrowInfo、把原地址token当原生类型或C++虚表。
7. 同小步撤销旧54B123可调用声明及五不再传出的opaque ThrowInfo声明，保原JSON原始数据/身份和完整旧捕获数组/描述符证据，不填空SDK对象。其他Legacy复制/析构/what/方法表/捕获数据源码不改，原SEH与C06未实现私有职责不由本组自动替换。
8. 九原根及原copy/析构/what成员的当前source关联成组同步，原9数字call点保为基线，当前提供者注明throw/throw;及原生类，不记54B123原体等价。C07推进实际源码，SDK编译/链接/展开未验，不提前关闭C06/C07或完整CRT。

<a id="native-cpp-exception-acceptance"></a>
## 4. 局部门禁与限制

| 门禁 | 本组观察/标准 |
|---|---|
| I | 六类实际C++ tag/同一owner、五copy与三转移、对应清理和what；基础/派生切片分别走原callback |
| W | 五Raise、两个字符串父及clear两个点共八throw/一throw;接通；旧54B123/opaque SDK数据不再用于当前路径，无额外临时原析构 |
| S | 九根204/198唯一及五unwind/十三类型层级可追踪；扣除NCE01新增与限定接线后旧正文一致；无原元数据手造/RTTI/跨对象释放 |
| R | 禁构建/测试/运行；实际C++20生成ABI、按值捕获/清理/非法重抛和SDK/硬件SEH/完整生命周期未验 |

本组仅归档限定I/W/S；编译器生成ABI是采用的实现机制，不能以未编译源码宣称SDK验收。C06/C07、完整功能门禁和R8保持开放。

<a id="native-cpp-exception-source-closure"></a>
## 5. 本组源码收口

NativeCppException.hpp六实际C++类型和LegacyException.cpp同一payload owner已落位；明确基础/派生copy、五原copy及六原非删除析构、两个what提供者，复制赋值删除。五Raise改throw本机prvalue，一次原本族copy；三已准备payload改AdoptPayload字节转移，不增共享引用、不加原临时析构；clear低U8重抛为throw;。

旧54B123可调用声明/五opaque ThrowInfo声明撤销，五Raise按真实noreturn声明；原JSON/捕获数组/描述符和数字call基线保留，当前compiler不消费旧SDK数据。复制/析构/方法表/消息/原临时串清理正文保持；源中仅新增本机类型实现、九接线及必要include/注释，旧头仅限定声明/注释。原基本字段布局48B payload/24B基础/24B字符串不改，SDK对象布局由本机compiler持有。

九原根与十四原copy/清理/what成员共23既有语义同步；1152/1149有效ID、18pending/4分类引用保持。九原根按本组adapted使已审78→87/3680，三限定E-NATIVE-CPP-EXCEPTION-I/W/S-20261005使累计33→36；204重复归属/198唯一只支撑本组，不加原F或以源码数推总覆盖。实现固定至Git命名快照cg2d-re-native-cpp-exception-source-20261005。

C06/C07保持open：本机类型、九原点和当前正常C++清理源码已接，实际C++20生成ABI、按值捕获/非法重抛、SDK/硬件SEH/完整CRT与运行未验。源码任务关闭10/60、叶验收0/182、完整功能门禁及R8不提升；禁构建/检查产物/新增或执行测试/运行/摘要、音频暂停及原格式明文保持。


## HFI01 本机抛出后的顶层筛选（2026-10-05）

[宿主筛选SPEC](cg2d_hosted_exception_filter_spec.md#hosted-filter-decision)补本机throw的下游：54503A原三参数条件改接当前MSVC x64四参数，保持原代码/19930520低W32识别、原terminate及旧filter转调。
LegacyMouse按真实C5尾安装，普通宿主收尾按stdio→console→恢复filter接同一owner；未改六本机异常类型、八throw/一重抛、消息/复制/清理或原5405F5。
当前记录合同来自已安装SDK源码，只完成本组源码接线；实际生成ABI、按值捕获/清理、非法重抛、硬件SEH/SDK/完整退出运行及C06/C07仍须原门禁证据，不记整体验收。

<a id="cpp-catch-private-host-baseline"></a>
## BPK01 私有查找与捕获复制职责承接（2026-10-07）

旧509/F:001402AB与517/F:001407BC仍将原完整内核/帧列作“待实现”，且保留“CRT宿主提案仍待确认”；当前D02已采用，NCE01的八throw/一throw;、六本机类/层级及显式copy已有真实提供者。当前源码没有两个旧入口的可调用声明或调用，旧数据不是本机异常元数据。这个缺口是职责映射和当前状态，不是新增手工x64捕获循环的理由。

[原证据的bpk01_private_catch_host](cg2d_native_cpp_exception_evidence.json)追加两个完整原体：5402AB..540452的146条423B/两页，5407BC..540980的169条452B/三页，合315条875B/五页/27实际call、51内部跳转/两外部终止tail、三ret。各一普通入向54029C/父540210与540625/父54060B，父只保存真实邻窗，不重验全帧链。

| 原职责 | 当前承接 |
|---|---|
| FindHandler：状态范围、try/handler/catchable循环、类型匹配、当前重抛及外来异常入口/终止 | 本机compiler生成帧与VCRuntime查找服务；当前六类层级和throw/throw;提供类型与当前异常，业务不执行旧x86查找体 |
| BuildCatchObject：引用/标量/对象路径、PMD地址调整、memcpy/成员copy及私有SEH/终止 | 本机compiler的捕获对象构造；当前六类copy按捕获层级委托原业务copy，单owner仍按原非删除析构清理 |

当前已安装MSVC14.50.35717的crt/src/vcruntime/frame.cpp实际含FindHandler(522)、捕获时T::BuildCatchObject(1318)、BuildCatchObjectInternal(1964)及FrameHandler3/4入口(2035/2046)。这些源码仅证明职责提供者存在；当前项目选用哪种生成帧/链接库和实际展开未编译验证，不以安装源码代产物证据。

<a id="cpp-catch-private-host-decision"></a>
### 具名裁决

依[D02§2](cg2d_crt_runtime_boundary_spec.md)与工程委托，本轮对两私有入口具名登记host，取代此前“未实现旧私有内核”的工程待办；NCE01§3.7此前不自动替换C06的边界在这里仅对两入口明确承接，不能外推全硬件SEH/成员或全部异常流程关闭。

1. 旧x86 FS帧、验证API、try/PMD/SDK私有布局与库内错误机制归本机compiler/VCRuntime；不补同名假体、包装、私有RVA/ThrowInfo或手工查找器，也不让旧数据冒充本机元数据。
2. 旧五捕获数组/六记录、描述符/PMD/原copy回调与原业务构析/what完整保留。它们的sdk_metadata_compatible/handler_parent_connected等原接线字段继续false，只表达旧数据未接本机引擎，不能反写true来称宿主ABI已验。
3. 当前六类继承、copy/单payload owner、已有八throw/一重抛及cleanup源码保持；私有头只补职责注释，不增加typed捕获业务或更换消息/异常种类/释放、RTTI及依赖。没有证据表明这些已有源码需重写。
4. 两旧语义全文分别存previous_mapping_before_bpk01，再更新当前状态/来源与host职责。旧原名字/计数/入向、数据绑定不变；安装SDK原始源码只保存定位/职责，不复制完整文件。
5. 当前C06工程要求更新为“两个旧内核由本机职责承接；实际typed捕获/清理、成员与硬件SEH、生成ABI/运行待验”。不关闭C06/C07或降低运行标准；RPLAN.24不专项模拟异常/溢出、钳制、补零、故障等价。

<a id="cpp-catch-private-host-acceptance"></a>
### 限定静态及计量

两原体315条875B及各唯一call/父邻窗、当前SDK职责定位；私有头仅一条说明变化，所有源/其他头保持。六类/显式copy与既有正常业务payload保持，只复核职责对应，不重新全面验NCE九根或所有旧构析/抛出/cleanup。

语义1425条/1420原ID、关联1538/3680（41.79%）保持；分类547→549，两原ID新增host；CRT150→152枚举open。E720→721仅一组局部S，未登记新实现I或全捕获通过；缺口队列174/待归属45、R7已审67/3061、60任务源码closed10/完整验收0、叶验收0/global_accepted=false保持。C06/C07/R01/R04只进展/引用与C06上述职责条件对齐，全部仍open。

<a id="cpp-catch-private-host-source-state"></a>
### 当前交付

交付两旧私有内核的具名宿主职责映射、完整原基线及当前待办纠正；没有新增捕获业务或手工异常引擎。固定命名快照：`cg2d-re-cpp-catch-private-host-20261007`。

实际typed捕获/按值切片/引用及清理、全部成员/硬件SEH、全应用owner寿命、SDK生成ABI/链接/展开与正常/快速退出运行仍按原门禁推进。仅定向静态与measure/-Check，未编译、测试、运行或计算摘要；网络原格式明文、音频暂停及其他范围保持。

<a id="cpp-continuation-host-source-state-20261009"></a>
## CAC20：私有53B4F2续行/帧恢复的宿主职责（2026-10-09）

按[新具名合同](cg2d_unassigned_range_classification_spec.md#range-eighth-batch-host-decision)与既有工程委托，只将完整原45B/17条53B4F2登记host。原54067C/父54060B在CallCatchBlock返回非空时传续行地址/当前EH节点，旧叶解除FS注册、恢复ESP/EBP并间接跳转；本机MSVC frame.cpp有对应捕获后续行职责。当前六类/真实throw与重抛、copy/清理源语法已由compiler/VCRuntime处理机器帧，不补旧上下文或同名包装。

旧原映射保留历史，当前职责/已知CRT成员与一host处置补记；原53B51F七字节只保用途及未决关系，无独立现源码对应，不新增语义代码身份或宣真实跳入/独立当前函数。三个旧catch元数据接线标志及全部payload/业务源码保持，不外推其余私有叶、typed捕获、成员或硬件SEH、正常owner寿命/生成ABI与运行；C04/C06/C07/V02及完整门禁仍开放。全部客户端源/头无本步改动，无软件I/W/S/R新增通过，原证见[完整第八批](cg2d_unassigned_range_eighth_batch_evidence.json)，固定引用cg2d-re-unassigned-range-eighth-batch-20261009。
