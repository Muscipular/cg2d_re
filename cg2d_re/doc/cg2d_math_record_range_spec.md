# 原数学错误记录生产与范围出口SPEC（BWJ01）

<a id="math-record-range-baseline"></a>
## 缺口与完整基线

BWI01已有完整87except/raise及真实依赖，543390/5433A7错误记录生产和5434D9范围出口仍无本体。本步补三完整入口及真实共享段，不缩成只支持正常幂的包装。

| 原入口 | 完整原体 | 新索引/本体 |
| --- | --- | --- |
| 543390..5433A7 | 九条23B/一页，尾转5433B0 | 1832，Cg2dStartLegacyTwoArgumentErrorX87 |
| 5433A7..5433E3 | 23条60B/一页，含5433B0共享段 | 1833，Cg2dStartLegacyOneArgumentErrorX87 |
| 5434D9..54357C | 54条163B/一页 | 1834，Cg2dCheckLegacyRangeExitX87 |

三完整86条246B/三页、三direct三callee/零indirect、12跳四RET，四CALL入向/一范围尾入向，无DATA入向。共享5433B0另有5433A5尾跳与5433AD顺流；不能把这条内部尾跳漏作缺依赖或把共享体复制计原身份。完整原体复用BWG01已保存证据，本轮当前cg2d会话核边界/指令与字节数/全部引用、共享入口及六常量。

原pow 53D022..53D02F保存控制字窗另列：PUSH完整EAX后仅FSTCW低W16，四字节载体高W16来自原EAX。范围出口三个POP EDX返回完整W32；不能把输入缩成W16或把高W16补零。完整pow尚待开发，这个已证输入/返回合同先落实。

<a id="math-record-range-decision"></a>
## 实施决定

- 沿LegacyFloatEnvironment.cpp/.hpp/.asm三现文件五追加，CPP只加六布局静态断言/HPP只加借用输入快照及ASM合同；ASM三真实本体，不加CPP F64返回或新的C桥、mutable owner、CMake项和依赖。
- LegacyMathErrorInputs由调用父生产并借用：原CW完整W32载体/操作号W32/借用名称指针/两F64，x64偏移0/4/8/16/24、32B。只拓已证名称指针，不改数值宽度，不默认填字段/ padding；这个值快照承接已证输入，不造x86通用栈镜像。
- 三入口只在ASM间以RCX借输入、ST(0)连续结果调用；两个错误入口另以EDX传原S32类型。范围出口返回ST(0)及EDX的原完整CW载体，不以CPP double ABI或补零值代替。
- 双参入口先存类型，再按原低W32/高W32序复制第二F64，然后尾到真正共享段。单参入口只存类型，不预填第二F64。两入口同一88B FRAME/.ALLOCSTACK声明；共享段仅有一份，保原FSTP结果、名称指针/第一F64两W32原样复制、真实87except CALL、FLD结果与027FH条件FLDCW。共享标签以MASM全局标签语法`::`跨两PROC可见，不把它留成局部标签。[Microsoft MASM运算符说明](https://learn.microsoft.com/en-us/cpp/assembler/masm/operators-reference?view=msvc-170)
- 40B数学记录仍沿BWG01仅拓名称指针的布局，本帧偏移32..71，输入借用指针72..79；未写第二F64/指针对齐padding不填。保存CW仅读，87except仍把其低W16零扩到自身局部W32，不写调用父载体；共享段恢复也只FLDCW低W16。
- 范围出口用56B正常展开帧：32B调用区/借用指针/8B FST临时值；FST不弹ST0，按原F64高W32指数分支。原CW027FH跳过恢复、未屏蔽inexact位与等待FSTSW样本保；原类型8/4/3及操作号1DH选择双参、其余单参，三原返回都读完整CW载体至EDX。
- 原上下界处理按实际FSCALE/FXCH/FSTP ST1/FABS/FCOMP/等待状态及FMUL顺序，保ST0原扩展结果、F64分类边界与原比较方向。六原F64常量：54FB24最大有限、54FB2C最小正规、54FB34=-1536、54FB3C=1536、54FB44正无限、54FB4C正零。其中最大有限、正无限、正零位同者复用既有只读常量，其余三新常量按原位声明；正零/无限乘法仍是真FMUL，不替成直接装载/钳制或宿主数学。
- 原正常/异常范围分支是原算法，不新增专项故障样例、额外保护/拒绝或模拟等价门禁，RPLAN.24保持。当前generic pow及共享CRT分派/FSAVE-FRSTOR生命周期和真实调用父继续开发；原格式明文/音频暂停及执行限制保持。

<a id="math-record-range-acceptance"></a>
## 可度量验收

| 项目 | 标准 |
| --- | --- |
| 原证 | 三完整86条246B/三页、三CALL/12跳四RET、四CALL与一尾入向及共享入口/六常量 |
| 开发 | 三现文件五追加，三ASM本体、零CPP本体/新桥或mutable owner |
| 生产 | 同一40B记录/共享段、双参低高W32复制，单参不写第二参数，ST0落存/重载 |
| 承载 | 32B借用输入/W32保存CW载体、原类型/操作号/指针/F64位宽 |
| 接线 | 范围两CALL/共同87except CALL与共享尾都接真实本体 |
| 返回 | 原ST0、范围三W32 EDX返回与027FH/FLDCW恢复，88B一致共享展开帧/56B范围帧 |
| 台账 | 新1832..1834/三adapted F，CRT171→174完整枚举/gates保持open |
| 差集 | 定位155→152、范围内未定位CRT27→24、总队列156→153 |
| 限定证据 | I/S各一，F994→997、E1154→1156 |
| 度量 | 语义1835/有效1828、合同1945/3680（52.85%），以文本汇总核实 |
| 任务 | C01/R01/V02/R04同步，pending18与全pow/机器运行保持 |

只做必要静态及文本汇总/-Check，不编译、测试、运行样例或计算hash。共享帧声明与完整三入口/记录接线不代机器展开/ABI、generic pow及其他调用父、FPU生命周期或运行验收。

<a id="math-record-range-source-state"></a>
## 状态

本SPEC先行，依工程委托落实三完整入口及其共享/记录/返回合同；实际静态结果完成后追加。

### BWJ01本步实际静态结果

三完整543390/5433A7含5433B0共享段与5434D9范围出口已开发，共86条246B/三页、三direct三callee/零indirect、12跳四RET，四CALL与一范围尾入向；共享入口另有一尾/一顺流。原体复用已保存BWG01原证，本轮同cg2d会话核完整边界/指令和字节数/全部引用、共享入口与六原常量，三源完整前后态可追。

三现文件五追加，三ASM真实本体/三原身份；CPP只有六布局断言，HPP只有32B借用输入与ASM合同，零新CPP本体/C桥或mutable owner。两错误入口同88B展开声明、共享全局标签与同一40B记录；双参按原低高W32序生产第二F64再尾共享，单参不写第二值/未生产padding，真实FSTP结果/名称指针/第一F64原样复制、87except CALL/FLD结果与027FH条件恢复完整保持。

原控制字载体由完整PUSH EAX再只FSTCW低W16生产；借用输入保W32原宽度，不补高W16，范围三返回都重读完整载体至EDX。范围56B正常帧保FST不弹ST0、F64指数分类、原CW/等待状态、8/4/3类型及1DH分参、两真实错误CALL和三返回。原六常量位/FSCALE/FXCH/FSTP ST1/比较与真FMUL保持，三新只读常量与三同位已有提供者接通；不把原零/无限乘法换成直接装载或宿主钳制，三ST0接口均只在ASM调用。所有12跳/四RET及原x87操作序与常量位、共享可见性/帧声明集中静态通过，不代机器展开/故障等价。

集中源码静态、完整机器/派生差集与文本汇总/-Check通过。新增1832..1834三映射/F、CRT171→174与两限定I/S：实际语义1835/有效1828、文档1945/3680（52.85%）、F997/E1156。源码定位155→152、范围内未定位CRT27→24、总差队列156→153；暂停25/排除103/元数据1保持。这是完整记录生产/范围本体与内部接线，不把尚待开发的generic pow父、全CRT分派或FPU生命周期计完成。

C01/R01/V02/R04与完整CRT枚举/gates保持open；pending18、商店mixed-pending6、创建pending7、R7已审467余2594、closed11、完整叶0/globalfalse保持。接下来沿generic pow/真实调用父和FSAVE-FRSTOR生命周期推进；机器展开/ABI/链接与真实运行仍沿原门禁，不编译、测试、运行样例或计算hash。无新保护/拒绝/补零/捕获或故障样例，旧源码/CMake、RPLAN.24/原格式明文/音频暂停保持，整体SPEC目标继续。

## BWS01原溢出入口真实共享本体（2026-10-08）

[出口合同](cg2d_math_final_exits_spec.md#math-final-exits-decision)、[完整原证/两源前后态](cg2d_math_final_exits_evidence.json)。新增5434C5对应Cg2dCheckLegacyOverflowExitX87使用相同56B正常帧及+32借用输入/+40 F64/+44高W32槽位，FST不弹ST0、AND原指数后真实JMP LegacyRangeOverflowStart::。该唯一全局标签只插在原5434ED对应CMP前，整个既有范围后缀和正常/错误/CW/完整W32 EDX恢复不改。

溢出入口按原体绕下溢判断，不转调用完整范围入口。另54349B精度出口第一AND AX,20H后原JZ恢复，与范围出口JNZ相反；新完整精度体保差异及等待FSTSW/类型8单参真实调用，没有合并该条件。外层生产/机器验收仍未代验。
