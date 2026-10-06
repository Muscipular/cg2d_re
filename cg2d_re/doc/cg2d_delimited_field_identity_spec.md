# DFI01 原字节字段解析的四身份基线

<a id="delimited-field-identity-baseline"></a>
## 范围与原证据

[现有cg2d会话只读证据](cg2d_delimited_field_identity_evidence.json)固定四原身份共132条/311字节：52CEA0为21条/43B、52CED0为40条/87B、52CF30为47条/104B、52CFA0为24条/77B。旧[分段解析合同](cg2d_network_runtime.md)已有正文说明，当前四实现/声明也已存在；缺口是显式语义/当前处置与功能成员登记，不新增业务代码。

- **定位52CEA0**：一次取源U8，低于80h且非NUL才与delimiter低U8比较；高位首字节跳过后继，后继NUL即返回空，否则继续。找到低位分隔符返回其后一字节地址；不按新代码页规则判断双字节。
- **复制52CF30**：原S32 capacity<=0仅写目标首NUL/返1；低位分隔符在当前目标写NUL/返0，源NUL复制后返1，触顶在目标+capacity写NUL/返1。高位首字节先写，容量仍允许才读后继并写；后继为分隔符也复制，为NUL则复制后返1。别名下仍保首写后尾读，不预读第二字节。
- **提取52CED0**：正常ordinal<=1选首段，其余逐次调用定位；源/段缺失只清目标首字节并返1，有段直接透传复制结果。不加“序号必须正数”或额外容量门槛；ordinal减一等数学溢出按RPLAN.24不专项模拟/保护。
- **整数52CFA0**：未清128B局部，以cap127提取并弃返回，首字节NUL返完整W32 -1，否则调用实际53C649并透传。保留已有Atoi_53C649接线，不加严格数字/编码校验或默认正文。

当前只核四完整本体和既有相应源段；直接引用统计分别1/306/1/550，其中选定4A0655/4A066B/4A0689技能生产边和52CFBF内部边留明细。引用点统计不代全部业务父或传递调用验收。原函数间隙不混入本组代码字节或未归属区间关闭数。

<a id="delimited-field-identity-decision"></a>
## 适配与登记裁决

依[本线工程委托](../AGENTS.md)，四项登记adapted，沿现有NetworkRuntime.cpp/公开声明承载。定位结果是非拥有const char*，x64只自然拓宽地址；ordinal/capacity/返回保持S32，delimiter只以原字节参与，不创建句柄或私有正文副本。

复制原在低位分支把delimiter按S8扩展，而源码将其转U8。实际比较的源值仅0..127：delimiter高位为零时两者同值，高位为一时原负值和新128..255值均不匹配；高位源分支不做分隔比较。只在该已证比较域确认当前表达式，不外推任意整数/代码页/API语义。

没有需改写的正文差异；补当前基线、语义、处置和已知成员。未登记身份不能当未实现，既有源与五行生产接线保留，旧[技能正文同源合同](cg2d_legacy_line_sampling_spec.md#legacy-line-sampling-source-state)继续有效。

<a id="delimited-field-identity-acceptance"></a>
## 本步限定验收

核四体132条/311B连续覆盖，各已证call/参数、逐字节/容量/NUL/返回、选定父边、源段及声明；登记/四身份/派生度量、增量文档链接编码和精确提交。

本步不新增软件I/S或功能已验收数。原引用计数不代完整306/550调用父、字段布局/owner/全部alias、Atoi内部、旧表外路由、原格式明文往返、实际ABI/链接/运行。U07/U10/T01/R01/R04/V03保持open，FUNC-NETWORK成员未全闭及[总SPEC §10](../plan/cg2d_full_reconstruction_plan.md#10-完成门禁)保持；音频暂停、两加解密叶排除、单线程和RPLAN.24不变，不构建/新增或运行测试/客户端/算法样例/摘要。

<a id="delimited-field-identity-source-state"></a>
## 当前交付

四身份原新关系绑定[已有实现](../src/network/NetworkRuntime.cpp)及[原声明](../include/cg/network/NetworkRuntime.hpp)，同一[技能生产源](../src/ui/WindowRuntime.cpp)不变。原库存/范围及协议格式保持，四adapted是处置登记，不是四项新增功能实现或完整软件验收。

固定命名快照：`cg2d-re-delimited-field-identity-20261005`。

## BPD01 邻接三数值消费者独立纳管（2026-10-06）

[BPD01 SPEC](cg2d_delimited_numeric_normal_sampling_spec.md#delimited-numeric-normal-baseline)/[原证据](cg2d_delimited_numeric_normal_sampling_evidence.json)。本组52CFF0/52D050/52D0D0另有104条286B，不计入DFI01四根132条311B原分母；两个实际栈field包装仍消费同一52CED0/cap127，不改四提取工具或其分类/源码。三个真实数值解析根完整104条286B/三页4绝对call14内跳6ret，26普通点16不同父分根1/13/12点和1/6/9父，只纳盘。单52D050正文恢复正常S32 value/sign与一次字节样本、下一字节先取再游标推进；52CFF0状态/52D0D0栈field两正确体保持。字符表、先乘62、任意位置负号、非法0/空0与field空-1/status空2不写输出均原样保。

三新1419–1421对应已有原根；语义1419→1422/有效ID1414→1417、关联1532→1535（41.63%→41.71%）、分类540→543、NETWORK47→50枚举open，六独立正常局部static I/S E689→695。三冻定位各1不刷新；RENDER52/YOBI56/RESOURCE41/AUDIO32暂停、pending18、队列175/需核46/暂停25、R7区间67/3061及其它门禁保；U10/R01/R04/V03四open只进展引用，60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保持。 正确两个包装补身份不冒充新实现，全部字段业务来源/borrow与原26点之父仍独立推进；DFI旧记录和工具接口保持。
