# 主角色更新 BYTE 取样 SPEC（BTY01）

日期：2026-10-07。承接BTX01状态更新，按AGENTS.md第8条工程委托先记后实施；[本步证据及完整前态](cg2d_actor_update_byte_reads_evidence.json)。原根复用[AAS01冻结证据](cg2d_actor_attachment_sampling_evidence.json)，不重复完整取证或重验正确的441DB0/附件/整数左比。

<a id="actor-update-byte-reads-baseline"></a>
## 原端基线与缺口

既有4417E0身份F:000417E0、语义1297，完整466条1483B/七段、38call/72jump/2RET及441DEA唯一type1真实回调绑定已冻结。已过附件五路径与整数左比只覆盖各自局部；本步推进三处W8读取，不据旧整根存档或本步指令位置外推整回调通过。

| 原点/字段 | 实际指令与字段 | 当前差异/处置 |
| --- | --- | --- |
| 441822 / FDA88E | f6058ea8fd0080，TEST BYTE固定槽80；现flags18C承载[FDA88C,FDA890)，第2字节对应FDA88E | 当前ReadPlayerActionFlags返回W16且实现读W32右移；新增单BYTE边界接口，此处改调用新接口 |
| 441839 / flags120低字节 | f60485e0c55a0080，TEST BYTE低80；现flags120类型uint16，新端已拓地址后实际偏移128，字段身份保持 | 此处从整W16读取改为同字段第0字节，不修改布局或其他消费者 |
| 441879 / flags42高字节 | f68703c55a0002，TEST BYTE高2；现flags42为uint16/偏移42 | 从整W16与200改为第1字节与2，原控制分支保持 |
| 441979 / category | MOV AX读取原W16再AND100 | 当前W16正确，保持 |
| 441A81 / flags114 | MOV EAX完整W32，随后TEST AH10及TEST EAX400380消费同取样 | 当前完整W32正确，保持，不按AH误窄 |
| 441C10、441D29..441D3B / 主flags | MOV EAX完整W32后TEST AH4；读完整W32、AND/OR AL低位后仍MOV完整EAX发布 | 现完整W32测试与读改写正确，保持，不改成BYTE载荷 |

字段类型和offset沿当前WorldObjectState/PlayerRuntimeSlots已有承载；只读取已证字节，不扩大/猜字段或地址，不触碰.flags18A（FDA88A）另一槽。既有ReadPlayerActionFlags的W16接口及实现保持。新的ReadPlayerActionFlagsByte只在SceneState私有owner边界从flags18C[2]取单BYTE，原接口不降宽，不暴露owner/SDK类型。

主角色早退/玩家指针与payload取样、阶段0/1消费、移动/待方向短路、共享倍率/动画、附件descriptor借用/绘制、±16范围删除、末端泡泡顺序以及原数值/RNG保持。完整回调与正常寿命/机器ABI/场景画面未验。

唯一新用途X:[441DAB,441DB0)实际5B逐地址90/NN_NOP、无入向/后真实已纳管441DB0，只对齐用途，不新F或范围排除。

<a id="actor-update-byte-reads-decision"></a>
## 具名裁决与实施

只改三现有源文件五片段：YobiRuntime既有4417E0正文三处读改为实际BYTE；SceneState在既有W16读取前新增std::uint8_t ReadPlayerActionFlagsByte() noexcept；既有WorldUpdateDependencies补单BYTE声明。私有玩家owner需由SceneState承接，故用单一边界getter，不向Yobi暴露内部槽，也不修改全体W16消费者。

不新建源码文件/布局/共享owner，不增加保护、故障门槛、回绕模型、钳制或新业务。旧语义1297追加本步基线/来源/局部元数据，原F、冻结位置、旧AAS01/BSB01信息完整保，不为新getter虚构原地址或另增语义。网络原格式明文、内部LegacyCryptInt、音频暂停/RPLAN.24持续。

<a id="actor-update-byte-reads-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 验收边界 |
| --- | --- | --- |
| 源码 | 两cpp/一既有hpp，五片段；既有正文1、新边界getter1 | 三完整源精确差异；W16 reader/声明、其他W16/W32合同及全部其它正文保 |
| 基线 | 复用466条1483B/七段与唯一绑定，13个宽度锚；新增5B实际尾区用途 | 原字节/文本/原schema逐段比对；不重验其他已过内容 |
| 语义 | 只扩充既有1297/F，新增语义/F处置均0 | 旧全部字段及AAS01/BSB01保，noF/code各7/pending18与ECX pending7保 |
| 功能 | LOOP84/MAP81成员不增；4个局部I/S各25真实来源 | 完整叶/运行门禁保，不伪闭 |
| 派生 | 语义1672/有效1665、文档1783（48.45%）、处置813保持；E936、R7已审381/余2680 | 由汇总器生成，只本步实际指标 |
| 任务 | U10/V03/R04只current/引用更新，继续open | closed11、差队列和完整门禁保 |

完整回调/父/callee、正常owner-alias寿命、数值、机器ABI、链接、画面、明文互通及运行R8继续。遵守不构建、不测试、不运行、不计算摘要，不降低标准。

<a id="actor-update-byte-reads-source-state"></a>
## 当前实现与证据状态

本步已据已有完整原根和当前字段承载确认三处BYTE缺口；先记SPEC后实施，并同步唯一进度/任务/机器台账。静态汇总实值另行追加。最终命名快照：cg2d-re-actor-update-byte-reads-20261007。

### 本步验收实值（2026-10-07）

三处BYTE读宽与单Byte边界接口已实际落实，原Word/其它W16-W32合同保持，原身份/覆盖数量不增。

集中静态通过：YobiRuntime/SceneState/WorldDeps三完整源仅五片段变化，三BYTE取样与一新Byte定义/声明、私有玩家owner边界、既有Word及正确category W16/flags114和主flags W32、所有其它正文保持。复用466条1483B原档七段逐字段与AAS01冻结来源一致，十三宽度锚、唯一callback绑定、实际5B NN_NOP用途、所有证据前态/完整机器/五文档/三任务模型、十五文件UTF8无BOM/LF和四局部I-S各25引用通过。纯文本汇总与-Check沿同一次执行均终0，完整派生快照模型比对通过。实际语义1672/有效1665、文档1783（48.45%）、处置813、LOOP84/MAP81均保持，E936、R7已审381/余2680；既有1297完善，无新语义/F。原pending18/ECX pending7、差队列172/缺定位171/待归属43、closed11/完整叶0/globalfalse保。未编译、测试、运行或计算摘要，整回调/父/callee/正常owner-alias寿命/数值/机器ABI与运行R8继续。
