# 通用堆接口统一标准分配族 SPEC（HMU01）

> 2026-10-10当前堆边界：[MC01-C4](cg2d_crt_host_migration_spec.md#msvc-crt-heap-source-state-20261010)已将通用数量/零长度/容量/校验与new-mode策略直接交MSVC，三公开calloc/realloc/_msize适配移既有LegacyMemory；旧模式/handler四槽、环境/OS/链接器选择、重复heap初始化及失败28/唯一Fast入口退休。下文旧HMU01预处理/handler/借用查询及更早私有池要求只保历史，不再追加开发债务；当前Malloc/Free与业务U32参数/owner、真实命令失败9/9/8及数组/全部业务构析保持。实际多字节继续迁移，完整业务/未知alias/SDK容量、初始化退出和运行仍待，未编译/运行。

<a id="heap-malloc-baseline"></a>
## 1. 当前事实与实施依据

用户2026-10-09提出“LegacyHeap.hpp 使用malloc不行吗？”。公开`Malloc_53C3AC/Free_53C0A9`在`LegacyTime.cpp`已直接使用std::malloc/std::free，已有明确批准；`LegacyHeap.cpp`的通用清零、扩容、容量及删除释放仍按旧模式1/2进入独立HeapCreate堆或页池，只有模式3已用标准分配族。源码在NT版本查询成功且版本>=5时优先选择模式1；这说明存在两套提供者，不据此猜实际运行值或宣称已复现错误。

现源检索确认：旧区域/组/页类型、10个池状态槽及12个旧池函数只在LegacyHeap.hpp/cpp使用，没有其它CPP调用/取址/字段消费；堆初始化有LegacyMouse和LegacyProcessEntry两个现有调用点。通用删除接口仍由异常、locale、ios、streambuf、filebuf等消费；完整对象生产/未知alias并未因本检索闭合。

依据用户的malloc方向、AGENTS第8条工程委托及D02宿主承接，本线程裁决整族统一到本机CRT。具名覆盖M3H01/M3P01及运行库子SPEC中“模式1/2及池元数据保持”的旧阶段约束，仅限此通用分配域和无当前消费者的CRT私有池；不扩大到字符串/文件/协议等业务兼容算法，不借此宣布完整CRT或原私有体等价。

原堆源、头和当前登记固定于迁移前Git命名快照`cg2d-re-heap-history-20261009`。旧逐指令与静态证据保持原快照，不改写成当前标准库验收。

<a id="heap-malloc-decision"></a>
## 2. 成组接口与保留合同

| 现接口 | 新提供者 | 保留边界 |
|---|---|---|
| AllocateLegacyHeapBlock/WithHandler/Memory | std::malloc | U32请求、零请求最小存储、既有上界、enableHandler/真实处理器重试 |
| FreeLegacyHeapMemory | std::free | 空指针无操作、原caller顺序/不额外清槽，只释放本CRT域块 |
| CallocateLegacyMemory | std::calloc | 原低W32乘积、零变1再16对齐、上界及每轮new-mode/处理器读取 |
| ReallocateLegacyMemory | std::realloc | NULL走既有分配；非空零先释放再返回空；成功保已有内容、失败原块继续有效 |
| GetLegacyAllocationSize | UCRT _msize | 真实CRT块容量按U32返回，不读块前缀或造容量表；不新增NULL保护 |
| CallLegacyHeapNewHandler | 原函数指针 | 原U32参数、每次读取真实处理器，异常继续向外传播 |
| InitializeLegacyHeap | UCRT _get_heap_handle | 实际查询并发布借用句柄，空返回0；成功后保原模式选择与发布；不固定返回成功 |

原通用malloc模式1/2的零/16对齐请求保持，模式3保既有最小1B请求；realloc模式1/2的物理16对齐和handler参数仍保留，模式3继续原未对齐输入。模式只表达已证兼容数量策略，所有模式都使用同一标准CRT块，修改模式不会改变释放域；不伪报已恢复原池算法。公开Malloc/Free两已批准直转调保持原文。

初始化借用CRT已建立的堆，不再HeapCreate/Destroy或构造原区域管理区；serialized参数由宿主同步策略承接，不改变两个caller的错误28消费与其余初始化序。不增加探测分配、不新增owner或进程退出动作，借用句柄不得由本模块HeapDestroy。

删除8个旧私有布局类型、10个池状态槽及12个旧池接口/实现；原函数/数据身份和历史原证保留，按host登记职责承接。九个存活提供者具有真实CPP算法/SDK操作，不保同名假体。原区域/页寿命、失败时机、分桶/物理地址、内部容量/对齐与SDK错误策略作为D02差异，不能记原体等价。

现有裸字节域AllocateLegacyByteBuffer/FreeLegacyByteBuffer继续成对使用HeapAlloc/HeapFree；其它独立NSH/EXH/LCH/ETH等域按既有提供者，不转换存活外来块，不将裸HeapAlloc、VirtualAlloc、new对象或原池块交std::free/realloc/_msize。无当前合法外来块生产者证明不等于全部原owner/间接alias已闭合。

<a id="heap-malloc-host-reference"></a>
## 3. 宿主能力与差异

[_msize](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/msize?view=msvc-170)查询malloc/calloc/realloc块，Debug CRT会映射对应调试实现；[_get_heap_handle](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/get-heap-handle?view=msvc-170)返回CRT使用的堆句柄；[realloc](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/realloc)由同一CRT块扩容。这些只证明宿主接口职责，不证明原程序调用或当前链接/运行。

不以查询到CRT句柄推导裸HeapAlloc块可交调试CRT free/_msize。请求宽度和业务内容保持，实际可用容量与宿主参数验证/errno/new-handler机制按D02，旧共享errno/错误槽不补默认或伪同步。数学溢出/异常不新增防护或样例，既有上界保留。

<a id="heap-malloc-acceptance"></a>
## 4. 本次验收与未闭边界

只核当前接口/调用、数量与条件、同域配对、SDK提供者、已无消费者的私有池、初始化真实返回、头/源/登记/历史引用及静态台账。限定I/W/S证据按实际核查范围登记；完整B/I/W/S/R、C01/C08/C11/V02、原成员/未知owner/生命周期仍开放。

单线程，不编译/测试/运行/产物/摘要，不引依赖。完整SDK ABI、实际失败/扩容/容量、业务画面/资源与正常快速退出待授权；整体SPEC未完成。

<a id="heap-malloc-source-state"></a>
## 5. 源码状态

成组方案已按工程委托确定并实施：九个真实CPP/SDK提供者保留，全部通用模式统一标准分配族，公开两直转调及两个初始化caller保持原文；头/源删除12旧池接口、8布局类型及10状态槽。LegacyHeap.hpp由154行收敛为28行，CPP由821行收敛为141行。数量策略、handler门禁及借用堆查询按上述合同保留；原体仅历史，当前未编译/链接/运行。
