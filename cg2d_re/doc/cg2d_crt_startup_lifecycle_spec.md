# CRT 启动与生命周期迁移子 SPEC

> **SSO01最新状态（2026-10-05）**：[标准流提供者](cg2d_sdk_standard_stream_owners_spec.md#sdk-standard-owners-source-closure)已完成15槽与完整SDK对象对应；70有序源投影＋15宿主预先提供＝85项对应，另null1/暂停音频2/键盘排除2。85不是85个同TU调用。下文各组数字为各固定快照；完整容量/消费者/alias和SDK运行仍待。

> D03；2026-10-04采用，C02–C04/C09的实施方案。依据[宿主总SPEC](cg2d_crt_host_migration_spec.md)和[决议授权](cg2d_closure_decisions_spec.md)，上位实施方案；局部实现见下方各组合同，全应用启动/退出仍未闭。
> 决定：由宿主MSVC/UCRT完成自身初始化/终止；原应用可见初始化和退出显式承接，不能同时启用两套owner。

## 1. 原表是对应账本，不是要求复造宿主CRT的启动表

保留C5、C++90各原槽/顺序/目标及现有provider证据，但逐槽区分：

| 归属 | 活动路径选择 | 验收 |
|---|---|---|
| 纯宿主CRT私有设施 | 由实际宿主提供者承接；不再执行原53DDB5/cinit整条私有启动来重复建堆/流/locale | 原槽仍保留映射/获批差异，不登记为原体等价或已运行 |
| 业务可见全局/原应用构析 | 在宿主已就绪、原业务入口需要之前沿原相对顺序调用已证显式provider | 保留RNG、发布、即时退出登记、失败前缀与逆序；不提前/合并成无序批次 |
| 混合/归属未证 | 先完成最小边界拆分和消费者核对 | 不用空回调占槽，不按地址段/名称自动划为宿主 |

未来完整客户端采用宿主可执行入口承接WinMain；原53DDB5已实现源码和证据保留作映射，不作为绕过宿主CRT的自定义入口。原OS状态、参数/envp/配置等被业务或保留兼容层读取时仍须在原逻辑时点生产，不能认为宿主启动会自动填旧槽。

## 2. 一对象一活动构析

每组迁移先列“唯一存储、旧宿主构造/atexit、原显式构造/退出、别名、分配域”。在同一小步切换生产与退出，不让新旧路径同时作用同一对象；不能只增加原入口再留旧自动构造。

已有14标准流被动存储、SI01四Init实际地址对象、登记数据和62析构归属按实际消费者分类：宿主对象使用宿主完整类型/生命周期，不能把它强转为旧Legacy*布局；仍被现有Legacy*消费者读取的对象保留其真实存储/显式合同，补实际Init/登记，直到整组迁移完成。缺布局/容量仍是事实缺口。

应用退出按原登记时点与逆序，真实失败清理与数组成员前缀/整体析构分层保留；正常/快速退出分别映射原业务可见路径。宿主运行库内部清理差异按D02接受，不能用它跳过应用退出。

## 3. 图形双树的选择

保留现有std::map业务缓存，由宿主容器负责私有红黑树节点、共享空节点等内部机制；不再要求复造62B0AC/62B0B0及原私有树节点字节布局。44CC30/44CD40等原槽登记“宿主容器职责承接”，不是写几个空函数消缺口。

应用可见TextureSurface*身份、完整键/查找、插入时点、两树选择、清理时点和逐surface释放仍遵循[DX SPEC](cg2d_graphic_dx_cache_x64_spec.md)。清理纹理与析构容器分别核；容器销毁不能擅自拥有/释放借用的surface，业务清理不能被默认析构漏掉。

若发现应用直接依赖旧树私有节点/计数，此项须保留真实兼容访问后才迁移；不得伪造永零哨兵或复制第二套树。原证据不删除。

## 4. 实施小步与关闭条件

| 步骤 | 产出 | 不得提前记完成 |
|---|---|---|
| S1 原表分类 | C5/C++90逐槽列原目标、实际提供者、业务顺序和缺项 | 分类不等于新启动接通 |
| S2 单组切换 | 每个owner同时切换构造/退出/登记，已有函数复用 | 不能残留双执行或跨堆释放 |
| S3 关联状态 | OS/命令行/环境/流/locale等真实消费者接同一owner | 无私有旧槽固定成功或默认数据 |
| S4 启动/退出源码 | 宿主入口→应用显式序→WinMain业务；退出路径对应 | 暂停音频、未知资源/状态仍可阻断完整入口 |
| S5 验证 | 当前仅静态检查原表归属、调用/写序、唯一owner和残留符号 | 构建/链接/SDK/实际启动退出在用户允许后验证，V02不提前关闭 |

本SPEC不要求现在改CMake/运行客户端，不允许删除既有还原成果来制造“依赖为零”。旧私有辅助可保留非活动历史实现；所有被活动路径引用的符号必须是真实提供者。

## 5. C09 本组实施合同

[图形双树构析与宿主登记子SPEC](cg2d_graphic_transform_lifecycle_spec.md)细化两固定owner的被动存储、具名构造/即时登记/退出以及业务清理分工。当前同模块启动适配保留本组前置条件，整表接入时必须同时移除适配，不能双执行；全应用相对顺序和运行仍待。

## 6. C02/C04 图形索引与DBF容器提供者

[资源容器子SPEC](cg2d_resource_container_lifecycle_spec.md#resource-container-source-closure)已按四真实槽实施被动存储/固定构析/即时宿主登记；两个模块位置的适配只保证本组配对。DBF按实际指令修正map→vector构造及载荷释放、集合归空、清树、成员逆序析构。全应用原序接通时必须同小步移除模块适配；四Init/其他owner、全资源/快速退出与运行继续待验，C02/C04/G01不提前关闭。

## 7. C03 / SI01 四Init地址承载

[SI01子SPEC](cg2d_standard_stream_init_identity_spec.md#standard-init-identity-source-closure)基于原311条及八固定引用的地址-only契约，已补四独立平凡原生对象；控制器/计数/14流及登记/退出本体保持。原完整尺寸未知保留，native sizeof不是原容量证据；仅本组无Init字段消费路径适用D02表示承接，其他布局/间接消费仍待。原55310C/553110归既有屏幕键盘排除，不恢复专用构造。C03整项/全表活动序、登记数据/容量、Legacy与宿主双套流归属和运行继续未闭。

## 8. C03 / SR01登记数据已证前缀

[SR01子SPEC](cg2d_standard_stream_registry_prefix_spec.md#standard-registry-source-closure)已补0～8登记指针/U8计数的唯一同名被动存储，PE填零尾证明载入初值；只拓宽实际地址元素，登记/析构及其所有父函数原文保持。9仅已证消费前缀，原完整数组容量、正常较大索引与间接别名仍未知，不能把>8默认为异常/不可达或据此缩减验收。无新增保护/清槽/拥有、自动构析或宿主流互转；C03整项、数据闭包与全应用原序/活动归属/运行仍未闭。

## 9. C02/C04 启动保护四项活动构析

[本组子SPEC](cg2d_startup_protection_lifecycle_spec.md#startup-protection-source-closure)已将553004～553010四项切为完成构造后即时宿主登记，并取消四自动适配析构；唯一无析构模块适配正序调用真实Init，五实际值owner与原构析/RNG/保护算法保持。宿主只承接本组私有退出表职责，全应用原序接通时须同小步移除模块适配，不可双执行；其他组、快速退出/跨模块顺序和运行未闭，C02/C04仍open。

## 10. C02/C04 世界状态17项规范活动路径

[本组合同](cg2d_world_state_lifecycle_spec.md#world-lifecycle-source-closure)接17Init的真实宿主即时登记；四世界/场景/玩家/主宠物模块适配取消自动析构，四坐标/移动/Ripple包装转调13Init，取消重复构造/lambda主体。数组/成员、值/RNG与owner原文保持；17槽及玩家所属四条登记块分别核定，不拆5183E0合并身份。接全应用原序时必须同小步移除八当前模块调用；全应用/快速退出、完整成员EH/数组SEH、其他owner与SDK运行未闭，C02/C04继续open。

## 11. C02/C04 UI/输入五项

[本组合同](cg2d_ui_input_lifecycle_spec.md#ui-input-lifecycle-source-closure)已将两选择器/浏览宠物/鼠标接为同一Init→即时宿主登记→固定退出，选择器包装取消重复构造/可改preview槽lambda，浏览/鼠标取消自动适配析构。数字步进只接具名477160，无新增退出。原固定this+78目标恢复为活动退出，值/RNG/数组/成员源码保持；接全应用原序须同小步移除四模块调用。完整成员EH/数组SEH/跨模块/快速退出/SDK运行与C02/C04继续开放。

## 12. AIS01当前应用投影的同一TU原序

[AIS01收口](cg2d_active_initializer_order_spec.md#active-initializer-order-source-closure)已将两图形树/两索引容器/独立队列与DBF六provider从三个旧模块移到LegacyMouse原相对位置，旧模块调用同小步删除；C5多字节已有调用移到首项应用适配前。当前66应用+2 SDK流配置槽按同一TU声明序和既有wrapper内部原序；owner、RNG、即时退出登记、失败消费和业务正文不改。

这里更新前三组“跨模块相对序未保证”的局部状态，未接完整90：2音频暂停、2键盘排除、17流或经典locale待接及原null单列。旧cinit/CRT根不启用、原90数组不造占位；活动流/全部OS状态、快速退出与全owner/面清理、SDK和运行仍待，C02/C04/C09不整项关闭。

## SSX01 标准流即时退出登记域（2026-10-05）

[具名SPEC](cg2d_standard_stream_exit_migration_spec.md#standard-stream-exit-source-closure)
已将LegacyIos的18真实点（16晚段、2早段）直接接std::atexit，同原callback；
构造先/即时登记/宿主S32传出、异常传播及登记失败不回滚保持。
18原数据槽与分离登记尾有证，原构造/回调、14被动流/缓冲和四Init原文保持；
宿主表扩容/失败时机/非零返回按D03接受，不称原0/-1或表内核等价。

当前66应用+2 SDK配置投影保持，两早段Legacy控制器未新增活动，
16晚段及经典locale共17仍待。完整registry容量/较大正常索引和facet生产，
全应用normal/quick/callable/异常终止、跨组逆序及SDK运行未闭；
没有新模块/双owner或迁移其余53B406 caller，C02/C03/C04保持open。

## SCG01 标准流控制器原构造门禁（2026-10-05）

[具名SPEC](cg2d_standard_stream_controller_gates_spec.md#standard-stream-gates-source-closure)
已恢复5482DD/549397各七固定目标test/jz，现沿既有currentObject非空判定
只调用当次原构造。目标/state先写、原参数/计数、窄宽不同构造序、
实际FILE/共享错误缓冲、动态tie/format、候选展开与SSX01十八登记保持。

固定原/当前对象非零，本组不宣称正常输出修复或新增空安全；
无新活动Init/模块/owner，当前66+2投影及17待接保持。
原计数-1/两七prev/四父及RET有证，但全registry容量、真实facet/codecvt、
全应用normal/quick/callable退出和SDK运行仍待，C02/C03/C04继续open。

## 经典 locale 的宿主活动接线（SCL01）

[本组合同](cg2d_sdk_classic_locale_spec.md#sdk-classic-locale-source-closure)
将原55315C职责映射到同TU真实std::locale::classic调用，
位于553158/553160之间；SDK持有完整经典对象及私有退出。
不重新执行旧仅RET/登记入口，不构造SDK副本、不写三Legacy指针槽。
原四配对登记和两个流适配/应用清理保持；SDK具体清理时点按D02/D03差异承接。

最新当前投影69，16标准流仍待；此1项不解决Legacy消费者归属、
registry全容量/facet/计算alias或全应用normal/quick/callable退出与运行。
C02/C03/C04/C09保持open，不能把宿主query称原体/原登记失败数值等价。

## 晚段窄 Init 的完整宿主生命周期（SLN01）

[本组合同](cg2d_sdk_late_narrow_init_spec.md#sdk-late-narrow-init-source-closure)
将553134接为独立完整std::ios_base::Init对象，同TU位于553130后/553158前；
SDK真实构造/自动析构、私有计数和末次刷新成组承接。
SDK窄flush顺序/指针门禁/现代sentry及注册时点差异按D02/D03 adapted，
既有应用侧原pubsync、全部业务即时登记/清理保持。

旧5491D6/E0/F6接口和592894/1086539/registry/14被动对象不调用或镜像；
当前70项/15待接（宽Init1/流缓冲14），宽_Winit受_CRTBLD隐藏。
不改宏/复制声明，C02/C03/C04/C09/fullCRT及SDK运行仍待。

## 标准流整组完整SDK提供者与兼容有型接口（SSO01）

[本组合同](cg2d_sdk_standard_stream_owners_spec.md#sdk-standard-owners-source-closure)
完成15表角色对应：SDK八标准流与实际stdin/out/err缓冲、
宽流自然生命周期承担私有构析/登记。
应用侧共享error缓冲/tie/unitbuf及原pubsync退出保持；
不重新构造SDK、不调用隐藏_Winit或镜像Legacy槽。

六兼容Init接口改已知LegacyStandardStreamInitializer*参数/返回，
四真实身份对象/所有算法与计数/逐项门禁/EH/登记保持。
源投影仍70，另15为宿主预先提供，合85对应；
此前“15项待接”仅属于原固定快照，不把新增关系当15新增活动调用。
原null/音频暂停/键盘排除和完整消费者/registry容量/facet/alias/R1、
正常快速callable退出与SDK运行仍待，C02/C03/C04/C09不关闭。

## HPI01 宿主前置与普通stdio收尾（2026-10-05）

[局部合同](cg2d_hosted_runtime_prefix_spec.md#hosted-prefix-source-closure)及
[原证据](cg2d_hosted_runtime_prefix_evidence.json)定位旧entry未启用后的heap/lowio生产缺口。
仅在LegacyMouse同TU首个C表投影前接heap(0)/失败28、lowio和普通stdio即时宿主登记，
再继续原multibyte与70源投影。另15宿主角色、85对应总量及SDK配置/pubSync保持。
普通退出callback发布既有callable=0再调原53EA4A；不造清表、HeapDestroy或额外关闭。
原四provider/WinMain正文不改；固定C-term迁宿主登记按D02/D03具名适配。
本组静态接线不代跨TU首次使用、SDK继承ABI/owner/alias、
console/filter与normal/quick/callable完整退出及运行；六关联任务保持open。

## HSI01 兼容stdio活动前置（2026-10-05）

[本组合同](cg2d_hosted_stdio_init_spec.md#hosted-stdio-source-closure)及
[证据](cg2d_hosted_stdio_init_evidence.json)补HPI01之后的FILE表生产缺口。
当前同TU前置为heap/失败28→lowio→stdio→普通stdio登记→multibyte→既有应用投影；
stdio先于multibyte沿原C5的553174/553178相邻序。复用SCH01表块及唯一LegacyFile域，
512/20/错误26、20静态记录/三底层句柄、取流/刷新和WinMain正文保持。
不执行整套C5/旧entry/cinit/90表，不增加全表释放或SDK FILE副本；
70源＋15SDK/85对应计数不变。跨TU首次使用、全部FILE/owner/alias、SDK继承ABI
和normal/quick/callable全序及运行仍待，七关联任务保持open。

## ACI01 数组清理数量裁决（2026-10-05）

[本组合同](cg2d_array_counter_spec.md#array-counter-source-closure)与
[证据](cg2d_array_counter_evidence.json)记录两已有根77条/222B。
析构/展开remaining改直接S32递减、判负及finally传递，撤数量专项回绕。
原U32步长/尺寸偏移、逆序/失败前缀、SEH/筛选、构造和三组实际父保持；
数学溢出/越界沿RPLAN.24不专项处理，不记故障等价/x64安全。
两adapted只记本组源码/批准差异，完整成员EH/硬件SEH、SDK及完整退出运行继续待验。


## HFI01 x64异常筛选与宿主固定终止接线（2026-10-05）

[专项源码收口](cg2d_hosted_exception_filter_spec.md#hosted-filter-source-closure)已修正54503A对本机x64记录的参数数识别，并在原553178多字节之后、应用投影之前接545080安装。
既有普通宿主callback统一为FinishHostedLegacyRuntime，保callable=0/stdio，再按原槽序执行console关闭和filter恢复；同一即时登记的位置不变，应用退出在本组之前。
本机SDK源码证明原生四参数/19930520，原四根49条/145B与真实C5末槽/三个终止槽可追；原5405F5/数组EH及normal/quick/callable/完整旧入口不改。
四语义/四adapted补当前接线，语义1226/1223、导入15/218保持，处置197→201/3680，限定E156→159。C02/C04/C06/C07/C08/V02和完整门禁继续open；SDK生成ABI、API实际失败/恢复、全部捕获/清理、跨TU/owner/alias及退出运行未验。
