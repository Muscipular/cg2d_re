# 能力3输入正常加减与提交取样SPEC（BSE01）

本项依工程委托裁决，聚焦能力3登录输入控制器的实际源码差异；[原合同](cg2d_capability3_input_controller.md)、[最新决议](cg2d_closure_decisions_spec.md)与[验收体系](cg2d_traceability_acceptance_spec.md)继续适用。

<a id="capability3-normal-sampling-baseline"></a>
## 基线与缺口

原45FA70／F:0005FA70为完整301条1068B，五页只读取证，23直接调用／12唯一目标、39跳转／一RET；唯一45F190@45F699。原字节、指令与边界见[机器证据](cg2d_capability3_input_normal_sampling_evidence.json)。其中原屏幕键盘分支包含534AB0、strncpy和两404060共四点，仅保原证据及既有排除，不恢复或计已实现。

现私有AddW32／SubtractW32用uint32和bit_cast构造专项回绕模型；按钮与普通Enter成功块先写result1／账号NUL再读取密码长度。原两块45FB34..45FB4A／45FC87..45FC9E各先取账号与密码+12C，随后result1、账号NUL、密码NUL。原反编译cursor名字不能替代字段事实：+12C对应totalByteLength，+128才是caretByteOffset。正常数值已一致，本步不宣称观察到运行故障。

邻接真实X:0005FE9C-0005FEA0仅4B，全90／逐BYTE解码为canonical nop、无入向，前RET后真实入口；只登记用途，不造F或范围排除。

<a id="capability3-normal-sampling-decision"></a>
## 裁决与实现

仅改src/LoginCapability3InputController.cpp四个片段：

1. 两私有工具恢复普通S32 left+right／left-right，形参／返回和noexcept、调用点保持。
2. 按钮成功路径先单读账号totalByteLength，再单读密码totalByteLength；两个S32局部值持有齐后写result1、账号NUL、密码NUL。
3. 普通Enter成功路径执行同样取样与发布顺序；保持原账号／密码strlen与两文本校验、各处短路和失败返回。

不新增辅助接口或共享状态；局部长度只保存本次原字段快照，不建立独立文本owner。保持实际W16坐标及LowW16位型转换、两次当前上下文采样和度量返回后的Y-X-font、IME框／三输入绘制、Tab/Enter先后、密码再账号鼠标命中、私有focus/latch／共享焦点表与SetContext次序、原按钮图号／模式／尾部版本绘制。既有屏幕键盘排除保持。

RPLAN.24下不模拟数学溢出／回绕，不新增钳制、拒绝、补零或长度/字体/焦点保护。网络只原格式明文，音频暂停，不扩展业务。

<a id="capability3-normal-sampling-acceptance"></a>
## 可度量验收

| 项目 | 本步通过条件 |
|---|---|
| 实修 | 单整源仅四授权替换，三既有源函数变更：两私有工具与原入口；其它正文保持 |
| 取样 | 两成功块各账号→密码单读，再result1→账号NUL→密码NUL；长度S32且仍+12C |
| 正常算式 | 两工具普通S32；IME四低W16及既有位型转换保持，不新增异常模型 |
| 原证据 | 完整301条1068B／五页连续、唯一真实caller与两成功原点可追；单X4B仅用途 |
| 追踪 | 一个真实F／INPUT成员、一个X、两局部I-S及U10／R04current/引用、唯一进度同步 |
| 静态 | 集中源码／证据／完整机器与文档增量、measure/-Check、完整派生快照／UTF-8-LF和实际引用通过 |

本步局部I/S不替代完整父45F190、所有命中／输入／绘制依赖、别名和寿命、机器ABI／链接／运行。按现规则不构建或测试运行，完整验收标准保持。

<a id="capability3-normal-sampling-source-state"></a>
## 实现与计量状态

新增映射1587／单adapted F:0005FA70，私有AddW32／SubtractW32／LowW16仅附加来源符号，不新增F身份。INPUT25→26，语义1588／有效1581、文档1699／3680（46.17%）、处置721／E830、R7 293余2768。U10／R04仍open；源码closed11／P05原closed、冻结定位／pending与缺口队列、完整叶0／globalfalse和所有完整门禁保持。

实施命名快照：`cg2d-re-capability3-input-normal-sampling-20261007`。实际静态结果完成后追加。

本步静态结果：单整源四授权片段／两普通S32工具与两提交长度先取齐、原低W16／排除及其它正文保持，完整原根301条1068B／唯一父与单X4B、完整机器／文档增量均通过；measure及-Check完成，完整派生快照／12文件UTF-8-LF及两I-S各15实际来源通过。未构建、测试或运行，完整父／依赖、寿命、机器ABI与整体SPEC验收不提升。
