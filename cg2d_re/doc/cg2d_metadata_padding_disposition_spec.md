# NOP 元数据身份处置 SPEC（MPL01）

2026-10-05，为已证纯填充的原函数库存条目建立独立处置与度量。
上位：[追踪体系](cg2d_traceability_acceptance_spec.md)、[边界分类](cg2d_function_boundary_classifications.md)、[开发差集](cg2d_development_gap_queue_spec.md)。
固定快照：`cg2d-re-metadata-padding-disposition-20261005`。

<a id="metadata-padding-baseline"></a>
## 原事实与工具缺口

[原基线证据](cg2d_metadata_padding_disposition_evidence.json)及现有分类JSON的records/2确认：
F:000C48EB对应4C48EB..4C48F0，唯一chunk恰为五个90/NOP；前4C48E9为C3/RET，后4C48F0为下一真实入口。
原库存条目保留，不修改IDA函数元数据、字节或原3680分母。
直接xref为空只记录当前引用事实，不作为不可达证明；相邻入口和未知alias不由这五字节验收。

当前汇总器仅支持equivalent/adapted/host/excluded/mixed/merged。
本条既无独立业务指令/返回，也不应生成空函数、借宿主体或套用用户范围排除。
现队列把它列为普通缺定位项；已经证实的元数据用途缺少适当处置和独立统计。

<a id="metadata-padding-decision"></a>
## 实施裁决

增加`metadata_only`函数库存处置，只用于现有权威边界分类中已证的完整纯NOP元数据范围。
处置携带metadata_kind=`alignment_padding`及metadata_reference=`doc/cg2d_function_boundary_classifications.json#/records/N`，该指针必须也在evidence_refs中。
汇总器核：

1. 引用为已纳管的该分类JSON记录、索引有效、库存ID一致；原状态明确为五字节NOP分类。
2. 原入口/末界和唯一chunk与库存一致；每个范围字节均90，长度覆盖完整原条目；前RET位置小于入口，下一入口等于末界。
3. 不与具名排除/混合范围重叠，不存在已登记的直接定义、语义/pending身份或源码映射。

不满足条件时静态汇总拒绝发布派生文件。
仅此已证范围获该处置；不将其他RET片段、库入口、无xref函数、相邻入口或未归属区间套成metadata_only。

队列仍保留库存ID及依据，类别记`metadata_padding`、范围记`metadata_only`。
独立统计metadata_only_receipt_items；定位缺口和需核定位数不再包含这项已证纯填充。
本项不计源码实现、语义映射、功能accepted或用户范围排除；原总队列数和库存分母不减。
明细表只列仍需范围/定位核定的项，元数据处置数另列，快照保留完整记录。

<a id="metadata-padding-acceptance"></a>
## 限定静态验收

核已开IDA只读五NOP/前RET/下一入口及原库存/既有分类JSON相符。
核汇总器限定补丁、metadata_only校验条件/失败出口、队列类别/范围/计数和看板对应。
补一当前函数处置，原323处置和全部语义/软件证据/门禁/范围保持；不新增NOP空体、业务源码或语义函数。
唯一进度及R01/R04同步，文本汇总生成/独立Check和增量引用/UTF-8无BOM/LF有效。
不构建、测试、运行或计算摘要，单线程、原格式明文、音频暂停与RPLAN.24保持。

<a id="metadata-padding-source-state"></a>
## 本组边界

这是追踪汇总工具的开发，不是新增一个客户端业务实现。
汇总器已实现上述校验及独立计数，F:000C48EB已登记metadata_only；原元数据分母、同一条队列身份与范围证据保持。
其他原边界/共享RET归属、全部函数/区间与未知alias、软件和运行及R1/R7/R8/整体SPEC继续推进。
