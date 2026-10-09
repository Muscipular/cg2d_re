# 通用幂依赖帮助体开发SPEC（BWF01）

> 2026-10-10当前数值库边界：[MC01-C2](cg2d_crt_host_migration_spec.md#msvc-crt-numeric-source-state-20261010)已退休无现业务调用的旧浮点格式/96位转换/环境与pow错误私有组，数值格式/数学及库内状态交MSVC。实际地图/音频数学源与控制作用域保持，两Shop/NLS低宽读取移回既有模块，计时U64除法保留；旧gate/数学记录/CPU探测和其余内部CPP/ASM源码状态属固定历史，不再要求复刻。全部适用业务/owner/SDK正常结果与运行门禁保持，未编译/运行。

> 2026-10-09最新数值/源合同：[ACP01-C](cg2d_asm_to_cpp_migration_spec.md#asm-cpp-crt-source-state)已取消本组ASM/私有ST0机器ABI，使用CPP/double/标准数学与宿主浮点环境承接；旧x87/PC53/T80/FLAGS数值保证由用户最新许可覆盖。下文原指令、ASM实现与机器状态证据属固定历史；原业务字段/整数读取与错误记录/errno/系统回调保持，当前未编译/链接/运行，完整验收未闭。

<a id="power-helpers-baseline"></a>
## 父函数中的实际依赖缺口

完整核原父后确认：_usepowhlp@543185的IDA区间还包括CRT分派/错误入口，不能把前20条保存恢复片段当整个原身份完成。53D022通用pow父也依赖5433F0/543405/543435；既有1424/1426/1427只纳管地图两个1.25底数活动域，明确未实现通用帮助体。

本步依真实依赖补这三项完整通用本体，复用BPJ01/BPM01已归档原证，不重复给旧身份计数或改现地图路径。原三个帮助体共38条111B/三页、零direct/indirect CALL、一跳四RET；六普通CALL入向。53D01D真实CALL到543435，另有到53D022的顺序FLOW；按目标分列，不将父的FLOW混为帮助体入向。

| 原入口 | 完整原体 | 既有身份/本步通用本体 |
| --- | --- | --- |
| 5433F0..543405 | 11条21B/一页 | 1424，Cg2dLegacyTwoToTOSX87 |
| 543405..54341C | 6条23B/一页 | 1426，LoadLegacyPowerControlWord_543405及ASM |
| 543435..543478 | 21条67B/一页 | 1427，Cg2dLoadLegacyPowerOperandX87 |

完整原体来源为既有cg2d_map_effect_arithmetic_evidence.json的bpj01_twotos/bpm01_input_control。当前父取证另外归档，不作为三帮助体实现或全pow完成证据。

<a id="power-helpers-decision"></a>
## 实施决定

- 沿既有LegacyFloatEnvironment.cpp/.hpp/.asm追加1 CPP控制字值入口和3 ASM帮助体，不新模块、类型、依赖或owner。3原F复用既有语义索引，当前通用本体与过去地图专用适配分开记录，MapEffectMath/CMake及其它已实现源码保持。
- 5433F0私有ASM间以ST(0)输入/输出，保FLD ST/FRNDINT/FSUBR/FXCH/FCHS/F2XM1/FLD1/FADDP/FSCALE/FSTP ST1/RET。沿当前控制字，结果连续留x87，不经CPP double返回或F64落存。
- 543405以保存控制字W16值承接原已消费PC两位，按0300与007F构造临时控制字，借Win64暂存槽FLDCW。原DW载体高W16非消费值，不复制成业务状态。C桥用CX传值/EDX计算；原父保存/027F条件绕过/退出恢复及自身寄存器样本仍由调用父落实，加载叶不擅加恢复或固定PC。
- 543435私有ASM合同为RDX借用F64输入、ST(0)/EAX/FLAGS返回：普通分支EAX为原指数掩码，保CMP标志再FLD F64，ECX保持。特殊分支原高DW OR7FFF0000先写10B槽的+6，再SHLD11/SHL11写+4与+0，保重叠顺序及全部10字节覆盖后FLD F80；TEST EAX,0后MOV原高DW，保原ZF=1及EAX/ECX样本，不改成统一布尔加载接口。
- 两个ST0入口仅在ASM内部，无CPP声明。三ASM叶借32B调用暂存区，不改RSP/非易失寄存器；F80槽只写原三个W32足迹，不预填、补零或扩大数值字段。输入指针只借用，不持有。CW C桥/CPP入口完成真实调用，其它两叶的通用pow调用父继续实施。
- 只有原控制字叶的FLDCW写，另两叶不增控制字/MXCSR、清异常或环境保存恢复；不借宿主exp2/pow、加finite/保护策略或故障模拟。CRT分派/两pow父/环境生命周期未被本步替代，RPLAN.24、音频暂停、原格式明文和执行限制保持。

<a id="power-helpers-acceptance"></a>
## 可度量验收

| 项目 | 标准 |
| --- | --- |
| 原基线 | 复用三个完整38条111B/三页，六CALL入向、另列53D01D至父的FLOW，父取证独列 |
| 开发 | 三现文件五追加，1 CPP/3 ASM共四新本体/一个C参数桥 |
| 合同 | ST0连续结果、PC位构造/FLDCW、普通CMP与特殊重叠F80写/返回标志保持 |
| 状态 | 复用1424/1426/1427、三个adapted F及CRT160成员，不重复登记新身份 |
| 实际度量 | 语义1821/有效1814、合同1931（52.47%）、F983、定位166/CRT38及队列167保持 |
| 限定证据 | I限控制字入口→真实ASM调用，S覆盖三帮助体；E1146→1148，不代全父接线 |
| 保持 | 原地图/环境及已实现分类/整数/特殊辅助/CMake完整保持 |
| 任务 | C01/R01/V02/R04同步，完整枚举/gates与pending18保持open |

只做必要静态、机器/派生差集与文本汇总/-Check，不编译、测试、运行样例或计算hash。实质开发进度以三个通用帮助体新增记录，不以已有身份数量或历史队列变化替代。

<a id="power-helpers-source-state"></a>
## 状态

依现有工程委托补齐通用依赖，保持旧地图活动域；本步实际静态结果随后追加。

### BWF01本步实际静态结果

三个此前只有地图固定域对应的幂依赖已补完整通用本体：Cg2dLegacyTwoToTOSX87、LoadLegacyPowerControlWord_543405/ASM C桥、Cg2dLoadLegacyPowerOperandX87。三现文件五处追加，新增1 CPP/3 ASM共四原生本体，仅扩展原1424/1426/1427及三个F的当前实现边界，不另计原身份。三个原体复用既有BPJ01/BPM01完整38条111B/三页、零direct/indirect CALL、一跳四RET原证，未重复取根体；完整源与三语义/F前后态可追。

原连续十条x87运算/ST0结果及当前CW保持；控制字叶按原PC0300/007F生成临时W16并FLDCW，恢复归真实调用父。普通加载保原指数EAX/CMP标志/ECX，特殊加载保+6/+4/+0三重叠W32写满10B F80、FLD TBYTE及TEST EAX,0后MOV原高W32，返回ST0/EAX/FLAGS并按原改ECX。两个ST0接口仅ASM内部，没有CPP double声明/F64落存；Win64叶只借32B暂存区，无RSP/非易失变化，未消费CW载体高W16不作为业务值/补零。没有新增owner、宿主策略/finite保护、MXCSR写、清异常或环境保存恢复；原CW叶之外不增控制字写，旧地图固定路径、环境/分类/整数/特殊辅助及CMake保持。

静态核对中的53D01D目标混淆已按原指令及两实际引用修正：其CALL到543435，另有FLOW到53D022，三个帮助体共六CALL入向，父FLOW另列；原历史归档保持。当前父取证中的共享分派/错误入口完整归档为后续实施依据，不据此宣完整父已实现。

集中源码静态、完整机器/派生差集和文本汇总/-Check通过。限定I仅控制字CPP入口→真实ASM桥，S覆盖三通用帮助体，E1146→1148；三旧身份扩展后，实际语义1821/有效1814、文档1931/3680（52.47%）、F983/CRT160、定位166/范围内未定位CRT38与总队列167均保持。四新本体是真实开发进度，不用旧身份重复计数或虚减定位队列。

C01/R01/V02/R04及CRT完整枚举/gates保持open；pending18、商店mixed-pending6、创建pending7、R7已审467余2594、closed11、完整叶0/globalfalse、暂停25/排除103/元数据1保持。另两ST0叶通用pow调用父、完整CRT分派/错误出口与FPU环境生命周期、机器ABI/链接及真实运行仍沿原依赖实施；未编译、测试、运行样例或计算hash。RPLAN.24/原格式明文/音频暂停保持，整体SPEC目标继续。
