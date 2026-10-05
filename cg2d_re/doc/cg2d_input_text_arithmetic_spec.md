# ITA01 输入绘制正常S32算术

<a id="input-text-arithmetic-baseline"></a>
## 本次缺口与原基线

[ITD01既有取样/复制/排队合同](cg2d_input_text_alignment_spec.md)已落实[DrawInputText真实源](../src/render/InputText.cpp)。该文件AddW32/SubW32仍先将S32实参转U32，运算后bit_cast回S32，专项承载回绕。依RPLAN.24改正常S32加减，保该绘制链的13加/8减源调用表达式及其他正文；21个表达式不是原机器ALU次数或覆盖率分母。

[本次原证据](cg2d_input_text_arithmetic_evidence.json)保存4568A0..456D27完整332条1159B和32直接引用身份，分属25个父。完整体作为基线保存，本步只限定正常算术、字段/接口和两工具精确增量，不重新验完整332条所有语义、32完整调用点/25父或所有文本/画面。

原正常算术点包括CR推进456925和行计数456929；折行456981/456990/45699E；显示Y/行高456A14/456A2B/456A62；星号数量与索引456AF3/456B10/456B11；裁宽456BA3/456BB4；多行锚点差/宽/坐标456C7D/456C98/456CAA及单行456CCA/456D09。源码保原阶段求值与正常S32结果，不把一个工具表达式自动对应一条指令。

<a id="input-text-arithmetic-decision"></a>
## 委托裁决与实际开发

依[工程委托/RPLAN.24](../AGENTS.md)，只改两个匿名工具：AddW32返回left+right，SubW32返回left-right，接口仍S32。它们属于既有F:000568A0的实现来源，没有独立原函数ID；不新增帮助体或另建算术文件。

13加/8减表达式保持原位和操作数：

- 多行CR推进、折行终止/下一起点、行数/行高与显示Y按现有S32顺序。
- 星号输出的total−visible比较及每字写后重取total/visible、索引递增保持。
- 裁宽按原顺序减1/分类、严格返回3再减1，并保各阶段队列计数重取与原不补NUL分支。
- 多行/单行测宽预算及最终光标X/Y按原字段取样、真实measure和写序；负差回写visible及原count=0分支保持。

[输入字段](../include/cg/Input.hpp)及[文本队列](../include/cg/render/TextPendingQueue.hpp)既有共享owner/容量、S32比较、真实低W32数量、低S16/S8/W8参数、原一基token和队列发布保持。保位型/窄化用途的bit_cast、unsigned余串/复制数量和原令牌，不顺带调整类型或复制/链接算法。

不新增对象/字体/数量/NUL/长度/索引保护、拒绝、钳制、补零、清记录或异常模拟；不改字体/真实字符串/分类/绘制/QueueText、父和声明。RPLAN.24数学溢出/越界不评异常安全或故障等价。

<a id="input-text-arithmetic-acceptance"></a>
## 本步限定静态

1. 原完整332条1159B、连续范围/库存身份及32引用/25父身份可追；仅上述正常算术点与局部接口限定复核，不重验全部原体语义或完整父。
2. InputText.cpp只两个工具body变化，逆换后全文一致，13加/8减源表达式、源阶段/原量/窄化/计数/令牌、所有其他正文/父/声明保持。
3. 既有F:000568A0语义/equivalent追加依据及两个匿名来源，不新增ID/成员/处置类别；FUNC-INPUT/FUNC-TEXT明确成员14/22保持。I/S各一只记本次实修/限定静态。
4. T01/T02/R04/V04与两完整功能门禁保持open；所有父实参、字体/队列消费/焦点/IME/间接alias/所有owner借用与清理、机器PC53/库优化/SDK/链接/像素运行仍未验。R1/R7/R8和整体SPEC继续。

<a id="input-text-arithmetic-source-state"></a>
## 当前交付

输入绘制的多行推进、显示偏移、星号/裁宽及锚点预算/坐标所用工具恢复正常S32算术，21处表达式和ITD01既有原阶段保持。同步源码、当前合同/语义及台账、看板/唯一进度，不把两工具修正称全部输入或文本功能验收。

固定命名快照：`cg2d-re-input-text-arithmetic-20261005`。
