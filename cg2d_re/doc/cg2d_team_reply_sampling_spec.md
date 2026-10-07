# 团队回复对象与目标取样 SPEC（BTR01）

日期：2026-10-07。承接BTP01/BTQ01真实上下游，按AGENTS.md第8条既有委托先记设计再实施；[完整原指令/前态](cg2d_team_reply_sampling_evidence.json)。

<a id="team-reply-baseline"></a>
## 原端基线

指令按每70条归档与检查为四页（70/70/70/18）；原取证请求分别读取首70条及后158条，页数不代表MCP请求次数。

当前cg2d.exe完整[50A3E0,50A6A4)：228条708B/四页一块、19直接调用/13不同目标、26jump/5RET，唯一上游512E40@514C55。入口9A6E0C门禁后52D0D0解析第1项base62掩码；socket参数无消费，现JJQj I/T模板接线正确。原零掩码先槽W16活动检查，非玩家objectIndex一次取样供比较和4462B0查找，同结果NULL后442C70。接着一次取槽action，NULL后同指针+200取X；首次读取后重新取槽action+210读Y。原401EA0内部LegacyCryptInt与已有Read_401EC0承接保留，不是网络加解密。

随后flags W16清0→action清0；五槽活动计数及首个大于被清索引的next。count<=1先FF0D36 W16清0再51B080，余分支next>=0且任一坐标非负才46AFE0，正常S32计数/比较保持。非零掩码原FF0D32清0→FF0D36置1→槽flags置1，按3/5/9/11/21/41掩码顺序消费；value10在0C之前，字符串提取255、旧Unescape2、原长度U32>16才用已有???，不是新增长度保护。最后再一次objectIndex供比较/查找，同非NULL结果先pub槽再C40，index0才BD0；玩家先发布玩家action再A5E0、index0才A5A0。

现重复读取的两objectIndex与NULL后首X指针有差异；Y独立fresh槽、其它绑定/值序与真实门槛正确。原EAX多路偏移/指针/参数索引或残值，唯一上游CALL后ADD栈→XOR EAX0→RET，无业务消费；复用BTP01原窗，不虚构统一返回类型。尾[50A6A4,50A6B0)十二BYTE90/NN_NOP、无入向/后真入口，只对齐用途。

<a id="team-reply-decision"></a>
## 具名裁决及实现

SceneState.cpp既有50A3E0三片段：离组/入组各held objectIndex，同值比较/查找；离组held targetAction供原NULL与首X。在YobiRuntime既有ReadMapActorTarget增加同名重载（YobiAction* action,YobiAction*& slot,x,y），以已取指针读X，随后共享槽fresh读Y；旧签名保留并在入口按旧方式取槽后代理该重载，不改变46AFE0既有调用或观察。头文件对应增加同类型声明/更新注释；不泄漏私有布局/SDK类型，非拥有，不复制槽或新增owner。

三文件五唯一片段、两既有正文与一个同名重载；无新命名helper、影子状态或业务，不能把小范围读取修复扩成统一持有action，也不保留错误重复读取。正确字段格式/掩码/旧文本/名字门槛/全部callee相对序、逐槽清/五槽计数及短路和原native槽保持。父维持void观察合同：原残值在唯一上游清零有据，具名承载不模拟x86寄存器，不声称SDK机器ABI已经验收。网络原格式明文、内部LegacyCryptInt/音频暂停/RPLAN.24保持。

<a id="team-reply-acceptance"></a>
## 本步验收

1. 完整SceneState仅三片段、YobiRuntime仅原helper段替换为同名重载与旧签名代理、头仅该声明段；同索引/NULL首X与fresh Y分别对照，其余源/声明保持。
2. 四页228条708B/19call/26jump/5RET与唯一入向/复用上游清0，正常两分支字段/掩码/value序/名称阈值/绑定/逐槽清有据；十二BYTE真实NN_NOP只用途。
3. 新1659语义/adapted/实际F处置，LOOP71→72、NETWORK56→57，各两局部I-S真实来源；三任务current/引用保持open，全部旧对象/门禁保。
4. 只必要静态与纯文本measure/-Check，终态追加实际结果；不编译测试运行或摘要，不以完整原根/同名重载代全部callee、全部正常owner-alias寿命、SDK-ABI/明文互通或运行R8。

<a id="team-reply-source-state"></a>
## 完成边界

BTR01完整50A3E0团队JJQj I/T回复228条708B/四页一块、19直接callee/13不同目标、26跳转/5RET/唯一512E40入向；实修SceneState三片段两索引同取样供比较/查找、原NULL检查同targetAction供X而Y独立fresh共享槽。既有ReadMapActorTarget增同名原生T*+T&重载一项，旧签名保并按旧取样代理，无新命名helper/owner/布局/保护。三文件五片段/两既有正文与一新增重载；原掩码/字段/value10先0C/名称16字节门槛及旧文本、清槽和五槽计数/选next/坐标短路、全19原callee相对序/非玩家同查找结果发布NULL与两标志/玩家action再两标志保持。原混合EAX偏移/指针/索引残值在唯一上游XOR EAX0，具名void观察适配，不模拟寄存器。12B实际对齐只用途。网络原格式明文/内部LegacyCryptInt/音频暂停/RPLAN.24保；完整原根静态不等于全部callee/正常寿命、SDK-ABI或运行R8验收。

预期语义1660/有效1653、文档1771（48.12%）、处置801/E908、LOOP72/NETWORK57、R7 369余2692。原pending18/pending7、差队列172/缺定位171/待归属43、closed11及完整叶0/globalfalse保。全部正常生产消费/owner-alias寿命、全callee/机器ABI及运行仍按原门禁推进。

## 本步静态终态（2026-10-07）

完整三源头五片段、原根228条708B四归档页全部原MCP字段/19直接call与唯一原模板入向/复用上游清0、十二BYTE NN_NOP用途、完整机器台账两域具名增量及五文档/三任务定向静态通过。纯文本measure/-Check终态0；完整派生快照仅一新语义/两域成员和候选/一X/三任务与六指标，十五文件UTF-8无BOM/LF和四局部I-S各25真实来源通过。百分比依既有汇总器中点舍入为48.12%，已对齐文档，不手改生成结果。

实际语义1660/有效1653、文档关联1771（48.12%）、处置801/E908、LOOP72/NETWORK57、R7 369余2692；完整叶验收0、整体未通过。未编译、测试、运行或摘要；全callee/全部正常生产消费及owner-alias寿命、SDK-ABI/链接/明文互通和运行R8按原门禁继续。
