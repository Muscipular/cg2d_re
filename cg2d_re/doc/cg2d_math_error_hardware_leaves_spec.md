# 数学错误链的硬件依赖叶实现SPEC（BWG01）

<a id="math-hardware-baseline"></a>
## 当前真实依赖缺口

范围出口5434D9经543390/5433A7共享记录生产进入5460EB __87except；其硬件/errno/原零值叶5478B2/D8/DB/E9/F8均无本体/映射。本步先补这五项，使上层依赖有真实来源，不用抽象控制字或宿主errno替代原硬件/共享槽。

| 原入口 | 完整原体 | 新索引/本体 |
| --- | --- | --- |
| 5478B2..5478D8 | 10条38B | 1821，SetLegacyMathErrno_5478B2 |
| 5478D8..5478DB | 2条3B | 1822，DefaultLegacyMathError_5478D8 |
| 5478DB..5478E9 | 7条14B | 1823，ReadLegacyRawX87Status_5478DB |
| 5478E9..5478F8 | 8条15B | 1824，ReadAndClearLegacyRawX87Status_5478E9 |
| 5478F8..54791B | 15条35B | 1825，ControlLegacyRawX87_5478F8 |

五完整42条105B/五页，零direct/indirect CALL、三跳六RET、五普通CALL入向，无尾/DATA入向。三原入向属5460EB、两属5473E8；父链取证独列，不计五叶或全父完成。5478DB原实质为等待式__statfp，5478E9才是读后清除__clrfp，按指令和调用目标辨认。

<a id="math-hardware-decision"></a>
## 实施决定

- 沿LegacyFloatEnvironment.cpp/.hpp/.asm，子CMake已含CPP/ASM源。三文件七处追加、五CPP/三ASM共八本体/三个私有C桥；只有五原身份，不给桥另加原F。旧环境/分类/整数/通用帮助体及其它源码保持。
- 5478B2原signed比较保持：类型1写既有gLegacyErrno_1085FE0=33，2/3写34，其它不写，EAX原S32输入照返。引用现有LegacyTime声明/单owner，不接现代errno、不预清或另建错误槽。
- 5478D8完整原体仅XOR EAX,EAX/RET，按原返回S32零，完全不访问传入的借用记录，不触发其它处理或填值。
- 两错误记录生产/87except已证原32B记录：+0 S32类型、+4名称指针、+8/+16/+24三个F64。声明LegacyMathErrorRecord，只扩已证名称指针，x64自然布局40B/+0/+8/+16/+24/+32，六个静态断言固定当前承载；不默认初始化，单参生产未写的第二参数不预填。对象由调用方持有，当前零值叶只借用，真实记录生产/完整父随后实施。
- 5478DB等待FSTSW后MOVSX完整S16为S32；5478E9先FNSTSW记旧W16再FNCLEX，返回旧S16符号扩展。保高位和等待区别，不抽象/只取低位或改为清后样本；仅原x87清除，MXCSR不操作。
- 5478F8原FSTCW记旧W16，value/mask保持W32，以原NOT/AND/OR与旧W32读取足迹合并、FLDCW低W16，返旧S16符号扩展。未消费额外高W16不补零、不当业务字段；不借既有抽象ControlLegacyX87，它返回/表示合同不同。
- 三ASM叶借Win64 32B暂存槽，不改RSP/非易失寄存器。控制字两槽保存/合并分离，仅原FSTSW/FNCLEX/FLDCW顺序，原机器栈/故障等价不声明。不增finite/输入保护、故障模拟或宿主策略，RPLAN.24、音频暂停、原格式明文与执行限制保持。

<a id="math-hardware-acceptance"></a>
## 可度量验收

| 项目 | 标准 |
| --- | --- |
| 原基线 | 五完整42条105B/五页、五CALL入向；记录生产/87except/范围出口取证独列 |
| 开发 | 三现文件七追加、五CPP/三ASM/三桥，五原身份与一已证逻辑记录类型 |
| 合同 | 旧errno真实owner/选择性写/原输入返回，原零值叶，完整S16状态/等待/清除/旧CW返回保持 |
| 接线 | 三CPP状态/控制接口→真实ASM，errno复用共享槽；上层87except/范围出口仍须真实接入 |
| 台账 | 新1821..1825、五adapted F；CRT160→165，完整枚举/gates保持open |
| 缺口 | 定位166→161、范围内未定位CRT38→33，总队列167→162 |
| 限定证据 | I/S各一；F983→988、E1148→1150 |
| 度量 | 语义1826/有效1819、合同1936/3680（52.61%），以汇总核实 |
| 任务 | C01/R01/V02/R04同步，pending18及完整父/提供者和运行门禁保持 |

只执行必要静态、完整机器/派生差集与文本汇总/-Check，不编译、测试、运行样例或计算hash。本步不代完整87except/raise/handle、两pow父/范围出口/FPU生命周期、机器ABI/链接或真实运行。

<a id="math-hardware-source-state"></a>
## 状态

依现有工程委托补齐原错误链的真实硬件依赖，不增加异常保护或降低门禁；静态结果完成后追加。

### BWG01本步实际静态结果

五数学错误/硬件依赖已开发：SetLegacyMathErrno_5478B2、DefaultLegacyMathError_5478D8、ReadLegacyRawX87Status_5478DB、ReadAndClearLegacyRawX87Status_5478E9、ControlLegacyRawX87_5478F8。三现文件七追加，五CPP/三ASM共八本体/三个真实C桥，只计五原身份。五完整42条105B/五页、零direct/indirect CALL、三跳六RET及五普通CALL入向有据，原两记录生产/87except/范围出口链及原字节名称独列；三份完整源前后态可追。

数学errno复用原1085FE0声明/单owner：原signed类型1写33、2/3写34、其它不写，完整原S32输入照返。原5478D8完整XOR EAX,EAX/RET零值合同保持，不访问/持有传入记录。已证原32B五字段记录只扩名称指针为x64自然40B，六个静态布局断言固定+0/+8/+16/+24/+32；类型S32/三个F64不扩、无默认字段填充，真实生产仍归两父待接。

硬件三个CPP接口已接真实ASM：5478DB等待FSTSW后完整旧S16符号扩展；5478E9先FNSTSW记旧样本、再FNCLEX，返回清除前S16；5478F8保原W32 value/mask与旧CW读取足迹合并、FLDCW低W16及旧S16返回，未消费额外高W16不填值/业务消费。三叶只借Win64调用暂存槽，不改RSP/非易失寄存器；没有额外MXCSR、环境保存恢复、宿主errno或保护策略。原共享错误槽、其它环境/分类/整数/通用帮助体/地图源与CMake保持，不宣机器栈/故障等价。

集中源码静态、完整机器/派生差集及文本汇总/-Check通过。新1821..1825/五adapted F、CRT160→165与两限定I/S登记，实际语义1826/有效1819、文档1936/3680（52.61%）、F988/E1150。定位166→161、范围内未定位CRT38→33、总差队列167→162；暂停25/排除103/元数据1保持。这是五项真实依赖新增开发与三硬件桥/旧errno同源接线，不把记录类型或父取证算生产闭合。

C01/R01/V02/R04、CRT完整枚举/gates仍open；pending18、商店mixed-pending6、创建pending7、R7已审467余2594、closed11、完整叶0/globalfalse保持。完整87except/raise/handle、两记录生产与范围出口/两pow父/FPU生命周期、机器ABI/链接和真实运行沿原依赖继续；未编译、测试、运行样例或计算hash。RPLAN.24不增故障模拟/专项保护，原格式明文/音频暂停保持，整体SPEC目标继续。
