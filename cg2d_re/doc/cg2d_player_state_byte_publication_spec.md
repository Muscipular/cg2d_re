# 玩家状态 BYTE 发布 SPEC（BTK01）

日期：2026-10-07。沿文本清理/玩家撤除后的真实状态叶推进，先裁决再实施；原格式明文、音频暂停、单线程/执行限制与RPLAN.24保持。

<a id="player-state-byte-baseline"></a>
## 原基线与差异

| 原叶 | 完整指令/范围 | 合同 |
|---|---|---|
| 51A5A0 | 3条15B@[51A5A0,51A5AF) | flags18A高BYTE OR1，然后flags18C低BYTE写1，void |
| 51A5B0 | 8条35B@[51A5B0,51A5D3) | action一次取样→flags18C低BYTE写0→NULL/field2E4门禁→条件真实W16 AND FEFF；返回同action |
| 51A5E0 | 2条8B@[51A5E0,51A5E8) | flags18A高BYTE OR2，void |
| 51A5F0 | 2条10B@[51A5F0,51A5FA) | flags18A真实W16 AND FDFF，无门禁、void，现正文正确保持 |

合15条68B/四页四chunk，无CALL、两jump四RET，11直接入向六父。四F只有冻结源码定位，新语义1616..1619补完整基线，不造新原函数。

A0原两W8写入分别51A5A0/51A5A7；现以W16 OR0100和W32合成赋值承接，正常位值一样，但读写宽度扩展。恢复仅高BYTE OR1→18C低BYTE写1，无action/模式门禁，顺序保持。

E0原51A5E0只高BYTE OR2；现W16 OR0200，恢复高BYTE。F0原明确66前缀W16 AND FDFF，现同W16正确，不按置位叶的BYTE模式统一窄化。

B0原51A5B0先取action、51A5B5直接低BYTE写0，随后才NULL判断/field2E4完整W32==2判断。NULL或不等2才51A5C9真实W16 AND FEFF；等2保0100。现action先取及条件/返回均正确，只有18C用W32 AND清低8，应改为低BYTE直接写0。返回同借用action的原生T*，不截W32或另取当前全局；真实字段getter和三父现返回消费保持。

父实际直接消费：A0@50A694/50B84B/51AA83，B0@519E2C/51AA8C/51B0E6，E0@50A684/50B83E/50D01C，F0@50D035/51B0A9，共11点六父。只这些调用点和状态叶，不代所有父/网络字段或交互。现测宽/4462E0单撤除/4464D0批清/519D50玩家撤除的T**逐槽与free、清理后重读parent等序正确，保正文，不为纳管造改动或新增其语义/处置。

flags18A是既有W16、flags18C既有W32，唯一gPlayerRuntime owner与跨字段观察保持。只恢复实际访问BYTE，不能将18C改为独立影子状态/字段宽度或假定高24恒零；不能把B0/F0真实W16取消改为高BYTE，保原低BYTE同时读写的合同。

四相邻NOP区分别1/13/8/6B，逐BYTE90/NN_NOP、无入向、前RET后真入口；28B仅对齐用途，不造F/helper/范围排除。

<a id="player-state-byte-decision"></a>
## 实施裁决

SceneState.cpp三个已有正文三唯一片段：A0两个BYTE发布、B0低BYTE清除、E0高BYTE置位。F0、B0条件W16 AND/同action返回/所有接口与其余源码保持；使用调用期字段BYTE视图，不造helper/状态或更改布局。

四根补语义1616-1619与LOOP38→42、三adapted/F0equivalent、两局部I-S与四NOP区28B用途、三open任务current/引用。按实际世界/玩家状态分派依赖归LOOP，不以成员纳管代完整功能、父/provider或正常生命周期/ABI/链接/画面/明文互通/运行。

<a id="player-state-byte-acceptance"></a>
## 可度量验收

| 项 | 标准 |
|---|---|
| 原根 | 四完整叶15条68B/四页四块，无CALL两jump四RET，11点六父 |
| 实修 | A0高BYTE OR1→18C低BYTE1；B0同action取样后低BYTE0；E0高BYTE OR2 |
| 保持 | B0 NULL/完整field2E4!=2才真实W16 AND FEFF并返回同原生action；F0无门禁真实W16 AND FDFF，owner/布局/跨字段观察保 |
| 源差异 | 整SceneState.cpp=HEAD+三唯一片段/三个已有业务体，其余源头/接口/父调用保，无helper |
| 登记 | 四原F新语义/LOOP42/三adapted一equivalent/两I-S、四NOP区28B仅用途，原任务与完整门禁保 |
| 派生 | measure/-Check终态0、完整快照增量/UTF-8无BOM/LF/真实引用/文本差异 |
| 完整门禁 | 全六父/资料生产消费与正常owner-alias寿命、SDK-ABI/链接/画面/明文互通/运行R8不代验，原P05-P08 closed/pending7保 |

<a id="player-state-byte-source-state"></a>
## 实施状态

依工程委托裁决后实施，集中静态/派生结果随本节和唯一进度追加。不构建/测试/运行或计算摘要，不加异常回绕/保护/拒绝/钳制/补零/限长或新编码，不手改派生看板/快照。

## 本步完成结果（2026-10-07）

BTK01已完成SceneState.cpp三个既有正文三处修正，并保留F0正确正文：0100/0200设置仅改flags18A高BYTE，18C状态仅依原顺序写低BYTE1/0；两取消仍按原W16 AND，B0仍返回同一次取样的原生action。未添加新的状态、保护或业务分支。

四组集中静态全部通过：完整源文件仅三个片段变化、四完整原根15条68B及11点六父/四NOP区28B证据、完整机器台账增量、四文档与三任务六列。measure_cg2d.ps1及-Check同次执行均结束为0；完整派生快照逐对象对照通过，十二文件UTF-8无BOM/LF、四锚点和两I-S各21真实来源及文本差异检查通过。

实际汇总为语义1620/有效1613、文档1731（47.04%）、处置761/E888、LOOP42、R7已复核331/未处置2730。四新语义1616-1619对应四既有原F，三adapted一equivalent；其余旧对象及完整门禁保持。该结果完成本步开发和静态验收，全父/资料生产消费与正常寿命、SDK-ABI/链接/画面/明文互通及运行R8仍按整体SPEC推进；遵守现行禁编译、禁测试、禁运行，不把静态结果写成运行通过。
