# LUA01 旧 UI 共用加法正常算术

<a id="legacy-ui-add-arithmetic-baseline"></a>
## 缺口与基线

[LegacyUiRecord.cpp](../src/LegacyUiRecord.cpp)的匿名工具AddW32接受、返回S32，却先把两个参数bit_cast为U32再相加并转回S32，额外模拟数学溢出回绕。它是本线共用工具，没有独立原函数身份；不新造F编号、无ID语义项或客户端功能。

[限定证据](cg2d_legacy_ui_add_arithmetic_spec.md#evidence-archive-20261011)记录源码149处调用表达式：公共绘制8、日志加载4、日志页面52、交出/接收确认13、宠物状态选择14、技能详情58。嵌套调用各算一个源码表达式；这些数量不是原call点、运行次数或功能完成数。依据分别沿[公共面板](ui_windows/shared_legacy_panels.md)、[日志](ui_windows/legacy_log_viewer.md)、[确认](ui_windows/legacy_give_receive_confirmation.md)、[宠物](ui_windows/legacy_pet_list.md)、[详情](ui_windows/legacy_skill_panels.md)既有合同，不重新核完整页面。

参数用途为S32坐标/矩形边、日期、行数/页号及分页计数。既有参数转换、字段读取、乘除和调用顺序保持。特例4D62A0正文中心计算原邻窗4D64D2..4D6539共35条/103B：46列提取之后，strlen结果乘9，416减该值，再逻辑右移1，最后与面板X和17相加。当前U32 centered及位型传递用于保原SHR；不能因为本工具撤回绕就改成算术右移。正常提取输出最多46字节，无须为本组增加长度保护或更改原提取逻辑。

<a id="legacy-ui-add-arithmetic-decision"></a>
## 工程裁决

按[工程委托](../AGENTS.md)和[RPLAN.24](cg2d_closure_decisions_spec.md)，将AddW32正文改为正常S32的left + right。保留函数签名和149处调用表达式，不替换业务调用、不改参数求值、不扩宽非地址字段。

Low16、调用后的S16/U16窄化、原有位型访问和4D62A0的U32逻辑右移保持。日期day==32、15行跳页/16行页数、详情行底图Y借panel.x等已证原行为保持。P02/P03/P04/P05/P06已批准适配、共享owner、状态/返回、文字和请求格式保持。

不新增数学溢出专项保护、拒绝、钳制、补零、输入限制或回绕模拟；异常域不声明等价、安全或已修复。不把这项正常算术修正推广到未知原字段/其他工具，不改变音频暂停、原格式明文与执行限制。

<a id="legacy-ui-add-arithmetic-acceptance"></a>
## 本步验收

仅做必要静态：唯一工具正文精确修改；149调用表达式及六组归属与改前一致；其他源码、Low16、原SHR/窄化和取样不变；35条/103B邻窗连续及字节/所列指令对应；五个已有业务原身份追加本步依据，原状态和处置不改。登记限定I/S证据、相关open任务与唯一进度，生成看板并独立Check。

本步没有构建、测试、客户端/算法样例、生成ABI/像素/运行验证。公共绘制两既有包装只是当前源码使用关系，未据名称新增或关闭原身份；完整父、外层旧表路由、数据生产/共享寿命、未知alias、完整模块及最终运行门禁仍待。本步不推进R1处置数量、原成员数或任务关闭数。

<a id="legacy-ui-add-arithmetic-source-state"></a>
## 当前交付

AddW32已恢复正常S32加法；本组149调用表达式保持。五已有业务语义/处置追加局部依据，不增加语义项或原函数分母；FUNC-UIRUNTIME保持成员未闭和完整门禁现状。U04/U06/U10/T01/R04/V04继续open；U06补当前open依据，原派生状态本来就是open，不计状态转换。

固定命名快照：`cg2d-re-legacy-ui-add-arithmetic-20261005`。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_legacy_ui_add_arithmetic_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 恢复旧UI共用正常加法并撤回绕模拟」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

原验证范围：限定静态及正常域；数学溢出不专项处理/模拟，不称异常等价、安全、完整模块或运行通过。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
