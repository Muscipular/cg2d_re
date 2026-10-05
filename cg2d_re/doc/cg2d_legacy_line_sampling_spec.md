# LSP01 旧行提取取样与技能正文同源 SPEC

<a id="legacy-line-sampling-baseline"></a>
## 原端与当前生产链

[只读证据](cg2d_legacy_line_sampling_evidence.json)固定503AB0..503B6D的83条/189B及4A0640..4A0700的62条/192B。仅沿U07五行生产缺口和共同叶推进，不全面复核旧页面或其外层路由。

503ADD第一次取源U8到DL，503B55在继续处重新取值；NUL判定、bit7/CR/LF/01分流及目标首字节写入复用该取值。双字节仅目标行才在503AFD写首字节，然后503AFF读尾字节、503B02写尾。未选行不读尾，超列分支不推进源而重新取样。现源码的循环判断/分流/写目标重复读取首字节，需要恢复这个取样合同。

参数失败条件为line<0、width<=0或capacity<=width，返回0且不写目标；正常路径不论该行有无正文都写一个尾NUL并返回1。bit7字节列宽2，CR与LF各自换行，01转到下一8行边界；达到width、指定行停点及所有推进顺序保持。不用新代码页规则替原bit7判断，不为越界/溢出增加保护、补零或回绕模拟。

4A0640先解析图号字段1，标题字段2/容量21写CC24D4并反转义；字段3/容量255写未清256B局部并反转义。随后以width20/capacity81/行0..4调用503AB0，目标从CC2525逐81B推进至CC26BA；**每次写完旧共享行后把其地址发布到D672D8对应槽**。62条本体已在WindowRuntime/FillChoiceDialogBody落位，不补第二套生产者。
旧55的4D692D/695D/6961/6967同样借CC2525、步长81、终址CC26BA绘五行；当前BorrowNpcDialogLinesCC24D4借同一数组，D672D8只是五个非拥有行指针。旧文档“4A0640只生产现代正文”不符合原指令和当前源码，更新该当前结论。

<a id="legacy-line-sampling-decision"></a>
## 工程裁决与源码修正

沿[工程委托](../AGENTS.md)，只改既有Input.cpp的503AB0本体：

- 每轮建立当前source的只读volatile char视图，取一个char值；先判NUL，以U8位型判bit7，其余分支及ASCII/双字节首字节写入使用保存值。此局部视图表达原读取次数和顺序，不改变公开参数、数据owner或引入线程同步。
- 目标行的双字节先写保存的首字节，再经同一只读视图读source+1并写尾字节。保留可能重叠的源/目标处理序，不预读尾、不缓存下一轮。
- 其他分行/返回/参数门禁/容量及调用者不变。现代与旧页面继续借既有同源行，不新增镜像、空正文或推测外层路由。

<a id="legacy-line-sampling-acceptance"></a>
## 本步验收与边界

必要静态证据：两完整原根共145条/381B，503AB0全部100条直接引用仅分类保留、其中4A06B2为已核真实生产边；4A0640唯一49D53F入向及case16源接线；旧55绘制邻窗的同起点/步长/终址；当前共享数组/指针发布、生产/消费源段和Input.cpp精确补丁。

归档本组I/S限定证据，仍不构建、不新增/执行测试、不运行客户端或文本/API/算法样例、不计算摘要。取样源码表达和当前读取约束不代生成指令/ABI、100完整父、反转义内部、所有同源消费者/owner/alias、旧99表外层及画面/运行。U07五行生产缺口已据证据收窄，U07/U04/T01/T02/R01/R04/V04及完整FUNC-UIRUNTIME保持open，音频暂停、明文原格式、单线程和RPLAN.24保持。

<a id="legacy-line-sampling-source-state"></a>
## 当前交付

[行提取源码](../src/Input.cpp)已恢复取样表达；[公开声明](../include/cg/Input.hpp)与[正文生产](../src/ui/WindowRuntime.cpp)、[共享行定义](../src/ui/WindowRuntimeState.hpp)、[旧55消费](../src/LegacyUiRecord.cpp)保持。503AB0/4A0640分别登记adapted及已知同源成员；只补基线和必要取样源码，原库存与业务分母不变。

固定命名快照：`cg2d-re-legacy-line-sampling-20261005`。完整功能/运行及[总SPEC §10](../plan/cg2d_full_reconstruction_plan.md#10-完成门禁)仍未验收，不能因五行生产已证就宣称旧55整个流程完成。
