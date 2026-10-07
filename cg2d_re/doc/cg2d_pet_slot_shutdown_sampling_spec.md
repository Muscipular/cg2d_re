# 五槽退组指针与标志取样 SPEC（BTQ01）

日期：2026-10-07。沿BTP01真实父链推进，工程方案依AGENTS.md第8条既有委托先记录后实施；[完整原端与前态](cg2d_pet_slot_shutdown_sampling_evidence.json)。

<a id="pet-slot-shutdown-baseline"></a>
## 原端基线

当前cg2d.exe只读完整[51B080,51B0EE)：37条110B/一页一块、3直接callee/7跳转/1RET、2入向50A4C8与50D043。原FF02E0五槽每次先W16活动非零，再W32索引与玩家索引比较。玩家分支一次取action，NULL判断同值后仅清其+270 W16，调用51A5F0。非玩家分支先一次FDA88A跨字段W32读取、TEST AH1；仅满足才取action及同指针NULL判断/W16清0，之后另一次取槽action调用442C70。不能将该跨字段观察缩为flags18A W16，也不能把两次不同用途的action读取合并。

每槽依次清W16 flags/W32 objectIndex/原地址槽，其余两字节、数据及姓名保持；原步长48至五槽结束，新原生指针使步长56，现结构已落实，不改布局。尾调用51A5B0并透传原地址结果，现原生返回正确。50A4C8父尾透传至512E40清0，50D043后MOVSX EAX方向覆盖，不能称为全部直接弃返；上游清0复用BTP01已取证据。尾后[51B0EE,51B0F0)两BYTE90/NN_NOP无入向、后真入口，仅对齐用途；IDA列表将两BYTE显示为align，逐BYTEdecode才是事实。

<a id="pet-slot-shutdown-decision"></a>
## 具名裁决及实现

仅SceneState.cpp既有Sub_51B080一正文两片段：玩家分支held原生非拥有action供同NULL/清0；非玩家复用既有ReadPlayerFlags18A的memcpy一次跨字段W32后同条件，成立后才held action供NULL/清0。442C70仍另取槽action、原门禁及先后序保。ClearMapActorPathCount现实际field270 W16写0正确，不改；三原callee/五槽逐字段清/尾原生地址返回及其它源码和声明保持。无新helper/owner/影子状态/保护/范围限制/回绕模型。

同时更正BTP01十三语义元数据名称：original_all_actual_callers_discard_return_kept改为original_local_caller_observation_and_upstream_return_clear_kept，值保持true；其原记录误将“最终无业务消费”写成“所有直接调用者弃返”，原证据/正文已经明确BD0透传链。本步只改描述字段，不改BTP01指令、源码、处置、计数或验收结论。

网络原格式明文/内部LegacyCryptInt/音频暂停与RPLAN.24保持，具名方案依既有委托实施。

<a id="pet-slot-shutdown-acceptance"></a>
## 本步验收

1. 完整源仅两具名片段，同NULL指针取样、真实W32和短路、独立再次取槽供442C70、逐槽清序与尾原生返回保；其它源/声明/布局保持。
2. 完整37条110B/3call/7jump/1RET/2调用路径、复用上游清0字段及逐BYTE2B用途有据；列表align不误判为非NOP或新增范围。
3. 新语义1658/实际F处置、LOOP70→71、两局部I-S各真实来源；BTP01只修十三元数据名称、三任务current/引用保持open，旧对象及完整门禁保持。
4. 必要静态及纯文本measure/-Check后记实际指标；不编译、测试、运行或摘要，不以本叶完整原指令和局部接线代全部父链/正常寿命或机器ABI及运行R8。

<a id="pet-slot-shutdown-source-state"></a>
## 完成边界

BTQ01完整51B080五槽退组父37条110B/一页、3直接callee/7跳转/1RET/2入向，实修一正文两片段：两分支同一次原生非拥有action取样作NULL判断与field270 W16清0；非玩家原FDA88A跨字段W32读取复用既有ReadPlayerFlags18A后TEST AH1等值条件，短路后才取action；442C70独立重新取槽action及其门禁保、三字段逐槽清序/五槽及尾51A5B0原生地址透传保。不新增owner/helper/保护/业务；两调用点分别透传到上游清0或覆盖，局部静态不代完整父/全部正常生产消费/寿命或机器ABI/运行；BTP01十三机器描述改为真实局部观察及上游清0，不误称全部直接弃返。

预期语义1659/有效1652、文档1770（48.10%）、处置800/E904、LOOP71、R7 368余2693；原pending18及ECX pending7、差队列172/缺定位171/待归属43、closed11及完整叶0/globalfalse保。实际终态在集中静态与汇总后追加；局部通过不降低整体验收。

## 本步静态终态（2026-10-07）

完整cpp两片段、完整37原MCP字段/两父路径及复用上游清0、2B逐BYTE用途、完整台账具名增量及BTP01十三描述更正、五文档/三任务定向静态通过。纯文本measure/-Check终态0；完整派生快照仅一语义/一X/三任务及六指标，十三文件UTF-8无BOM/LF和两局部I-S各21真实来源通过。

实际语义1659/有效1652、文档关联1770（48.10%）、处置800/E904、LOOP71、R7 368余2693；完整叶验收0、整体未通过。不编译、测试、运行或摘要；全部正常生产消费/寿命、完整父链与机器ABI/运行R8按原门禁继续。
