# ICM01 光标坐标与相位正常算术

<a id="caret-arithmetic-baseline"></a>
## 具名缺口与原证据

[光标原合同](cg2d_input_caret.md)和[ICA01取样/接线合同](cg2d_input_caret_alignment_spec.md)已落实真实44A410入口、IME测宽前后取样、静态原字节和相位计算值发布。本步在同一InputCaret.cpp发现两匿名坐标工具及相位递增仍用U32算术后bit_cast回S32，按RPLAN.24撤这三处专项回绕承载，不重做已有取样或移除真实原窄化。

[完整原体证据](cg2d_input_caret_arithmetic_evidence.json)保存44A410..44A535的83条293B和16直接引用身份；16处归于43CBB0八、51C430三、51C7D0五，不称完整三父重新验收。本步只限定坐标/相位及相连接口静态，完整83条作为基线保存，不重验全部原语义、字体/排队或显示运行。

原基础路径44A47A取S32 doubleWidth，经44A482 cdq、44A483 sub及44A485 sar作向零除2，44A487从S32 X减半宽；组合路径44A4E1测宽后44A4E6重取当前对象，44A500..50A另取本次字体半宽，44A50C减、44A50E加测宽结果。末尾44A51A独立读取S32相位，44A51F递增、44A520比较40、44A523先写本次值，44A528 signed jl不命中才在44A52A写0。

<a id="caret-arithmetic-decision"></a>
## 委托裁决和开发边界

依[工程委托/RPLAN.24](../AGENTS.md)，仅改三块：

- AddW32返回left+right，保S32实参/结果和原组合坐标求值关系。
- SubW32返回left-right，保S32实参/结果；基础及组合两次调用原位不动。
- AdvanceCaretPhase直接取当前S32相位+1；保先根据本次值保存resetPhase、发布新值、满足原>=40条件才清0。不得写回后再读共享槽或把增量并入之前的门禁读。

[InputContext](../include/cg/Input.hpp)的S32坐标/字体/颜色及原相位唯一槽、[Font度量](../include/cg/render/Font.hpp)正常宽度保持。/2向零、低S16/S8实参、原模式inc DL低W8、静态A2 78 00及其全局借用寿命不变。保有这些真实字段窄化的bit_cast和IncrementModeW8，不把它们当本次S32回绕工具删除。

不加字体/上下文/长度/算术保护、默认值/拒绝/钳制/补零或异常模拟；不改IME门禁/前后取样、真实measure/DrawText2调用、资源owner和任何父/声明。两个匿名工具无独立原身份，不造新F条目或重复帮助体。

<a id="caret-arithmetic-acceptance"></a>
## 本步限定静态标准

1. 原83条293B连续范围/库存身份及16直接引用、S32坐标两减一加、向零半宽和相位read/inc/compare/store/条件清零可追。
2. InputCaret.cpp仅三块变化，逆换后全文一致；AddW32一次、SubW32两次调用表达式和原位序、低S16/S8/模式W8/静态字节、测宽前后样本及其余正文保持；父与声明、字体源和共享状态不改。
3. F:0004A410已有语义及equivalent处置追加本步依据，实际来源补两个匿名工具，不新增身份或改变处置类别；输入/文本14/22明确成员保持。I/S各一只计本步源码与限定静态，不提升完整功能门禁。
4. T01/T02/R04/V04、完整三父/16实参消费、焦点/IME/间接alias/所有owner与寿命、机器PC53/库优化/SDK/链接/画面运行未闭；R1/R7/R8和整体SPEC继续。数学溢出/越界不专项模拟，也不称异常安全或故障等价。

<a id="caret-arithmetic-source-state"></a>
## 当前交付

光标坐标加减和相位递增恢复正常S32算术；ICA01既有取样/真实调用、原字段/窄化和相位发布序保持。源码、当前合同/语义依据/验收台账、看板及唯一进度同步，不将三处修正称完整输入验收。

固定命名快照：`cg2d-re-input-caret-arithmetic-20261005`。
