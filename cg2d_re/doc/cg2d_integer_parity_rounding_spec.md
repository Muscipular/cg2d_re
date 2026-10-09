# 整数奇偶判定与舍入依赖实现SPEC（BWD01）

> 2026-10-09最新数值/源合同：[ACP01-C](cg2d_asm_to_cpp_migration_spec.md#asm-cpp-crt-source-state)已取消本组ASM/私有ST0机器ABI，使用CPP/double/标准数学与宿主浮点环境承接；旧x87/PC53/T80/FLAGS数值保证由用户最新许可覆盖。下文原指令、ASM实现与机器状态证据属固定历史；原业务字段/整数读取与错误记录/errno/系统回调保持，当前未编译/链接/运行，完整验收未闭。

<a id="integer-parity-baseline"></a>
## 当前源码缺口

接续BWC01两个分类叶，41个范围内未定位CRT项中的5436AC __d_inttype和5462FF __frnd仍无本体/映射。本步开发整数判定及其真实舍入依赖；__powhlp父与其余39项继续推进。

| 原入口 | 完整原体 | 新映射/本体 |
| --- | --- | --- |
| 5436AC..543711 | 49条101B/一页 | 1818，ClassifyLegacyInteger_5436AC与私有ASM |
| 5462FF..546311 | 10条18B/一页 | 1819，Cg2dRoundLegacyFloatX87私有ASM |

合计59条119B/两页，三direct、两个callee（既有546311及本步5462FF）、零indirect、四跳三RET、三普通CALL入向，无尾/DATA入向。原父54357C的54364F CALL邻窗另列，不计完整父。54FB58原8B为0000000000000040，ASM保原F64常量位。

<a id="integer-parity-decision"></a>
## 实施决定

- 复用LegacyFloatEnvironment.cpp/.hpp/.asm和已纳管546311分类实现。子CMake已有CPP/ASM源，保持工程归属和环境初始化、既有分类与地图1.25幂活动域。
- 5436AC对F64原值先FLD/FSTP落存给546311，TEST AL,90H非零返回S32零；否则再次真实落存给5462FF。第一次舍入返回后FSTP/FLD，再FCOMP原值，FNSTSW/SAHF后不等返回零。
- 上述比较相等时，原值FDIV54FB58，FSTP覆本调用参数槽；再FLD/FSTP给第二5462FF，返回ST(0)直接FCOMP商值，原ZF分支相等返回2、不等返回1。没有新增有限值判定或重新定义无序比较，不合并两次调用/落存。
- 5462FF完整合同为FLD输入F64→FRNDINT→FSTP F64→FLD F64→ST(0)返回。使用当前x87舍入模式，没有控制字修改/恢复。新舍入本体只在ASM间调用，采用RCX借用F64/ST(0)返回，不声明成Win64 CPP double返回，也不借宿主round/trunc/floor或整数强转。
- 判定入口以F64值/S32返回对CPP公开，通过仅存活于本调用的const double*进入ASM。正常Win64 56B FRAME/ALLOCSTACK承载32B调用区与三个F64槽，舍入叶借调用暂存区8B落存；原数据宽度保持，不镜像x86参数栈。新增1 CPP、2 ASM本体/一个私有参数桥，只计两个原身份，没有共享owner。
- 沿原分支、控制状态和调用顺序实现；不写MXCSR/控制字或清异常，不增保护/补零/输入限制，不设故障模拟门禁或宣故障/机器ABI等价。RPLAN.24、音频暂停、原协议明文与当前执行限制保持。

<a id="integer-parity-acceptance"></a>
## 可度量验收

| 项目 | 标准 |
| --- | --- |
| 原基线 | 两完整59条119B/两页；三真实CALL与54FB58常量可追，父邻窗另列 |
| 源码 | 三既有文件六处追加；1 CPP/2 ASM本体、一私有参数桥，旧源码及CMake保持 |
| 接线 | 判定→既有546311一次、→真实5462FF两次；后者只在ASM间返回ST(0) |
| 合同 | TEST AL90H、两次舍入及原F64落存/商覆写/状态比较保；不写FPU环境 |
| 台账 | 新1818/1819、两adapted F；CRT157→159，完整门禁/枚举保持open |
| 缺口 | 源码定位169→167、范围内未定位CRT41→39，总差队列170→168 |
| 限定证据 | I/S各一；F980→982、E1142→1144 |
| 度量 | 语义1820/有效1813，合同1930/3680（52.45%），以生成汇总核实 |
| 任务 | C01/R01/V02/R04同步；pending18及完整提供者/运行门禁保持 |

只执行必要静态、机器/派生差集与文本汇总/-Check；不编译、测试、运行算法样例或计算hash。本步实现两原端职责及内部依赖，不代完整__powhlp/pow、链接、机器ABI与真实FPU运行验收。

<a id="integer-parity-source-state"></a>
## 状态

依已有工程委托决定保持原x87舍入合同并落实两个真实缺失本体；静态结果完成后追加。

### BWD01本步实际静态结果

5436AC整数奇偶判定与5462FF舍入叶已开发并接线：原分类CALL一次、舍入CALL两次保持真实依赖。沿既有CPP/HPP/ASM六处追加一个CPP入口、两个ASM本体及一个私有参数桥；只纳管两个原身份，三份完整源前后态可追。原59条119B/两页、三direct两callee/零indirect、四跳三RET与三普通CALL入向完整，__powhlp父54364F邻窗和54FB58原8B常量独列。

整数判定保TEST AL90H、第一次舍入后F64落存重载比较、FDIV后商覆写本调用F64参数、第二次舍入返回ST(0)直接比较及FNSTSW/SAHF的0/1/2分支。舍入叶保FLD/FRNDINT/FSTP F64/FLD，只在ASM间返回ST(0)，不伪声明为Win64 CPP double返回；使用当前控制字，不增固定舍入/有限值策略或FPU环境写。56B正常展开帧、32B调用区与三个F64槽承载原值；无共享owner、额外保护、补零或机器栈/故障等价声明。既有分类、环境初始化、地图专用幂与CMake保持。

集中源码静态、完整机器/派生差集及文本汇总/-Check通过。新1818/1819及两adapted F/CRT157→159、限定I/S登记：实际语义1820/有效1813、合同1930/3680（52.45%）、F982/E1144。源码定位169→167、范围内未定位CRT41→39、总差队列170→168；暂停25/排除103/元数据1保持。这两个真实开发缺口已消减，不外推完整CRT/pow完成。

C01/R01/V02/R04仍open；pending18、商店mixed-pending6、创建pending7、R7已审467余2594、closed11、完整叶0/globalfalse保持。__powhlp/pow父及其它提供者、机器ABI/链接与真实FPU运行按原门禁继续；未编译、测试、运行样例或计算hash。RPLAN.24不增故障模拟/专项处理，音频暂停/原格式明文保持，整体SPEC目标继续。
