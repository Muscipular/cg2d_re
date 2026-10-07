# 功能追踪、度量与验收 SPEC

> 2026-10-04：按用户“构建一个可度量，可验收的体系”采用本方案。属于现有工程委托的文档/度量建设，不增加业务范围，不解除执行限制。
> 上位：[总SPEC](../plan/cg2d_full_reconstruction_plan.md)、[具名裁决](cg2d_closure_decisions_spec.md)。实际推进只写[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。

## 1. 交付与单一来源

| 文件 | 职责 | 更新方式 |
|---|---|---|
| 本SPEC | 分母、证据等级、门禁、例外及维护规则 | 规则改变时更新 |
| [验收登记簿](../plan/cg2d_acceptance_registry.json) | 稳定功能号、功能域、合同、任务、依赖、原端成员及验收证据 | 人工逐项维护 |
| [完整追踪快照](../plan/cg2d_traceability_snapshot.json) | 全3680函数、全导入槽、全未归属区间，以及现有映射/分类/文档和未知集合 | 汇总器生成，禁止手工改 |
| [度量看板](../plan/cg2d_measurement_dashboard.md) | 数量、分母、功能/门禁状态及待补证入口 | 同源生成 |
| [静态汇总器](../plan/measure_cg2d.ps1) | 校验身份/引用、约束验收声明、生成或核对快照 | 无外部依赖的PowerShell 7脚本 |
| 既有原库存、语义/直接定位及分类JSON | 原身份、原新关系及既有排除 | 继续原文件维护，不复制为第二份手填清单 |
| [60项收尾表](../plan/cg2d_spec_closure_checklist.md) | 已知工作项、阻碍、下一步 | 保留原编号，不当作全功能分母 |

登记簿是验收声明的唯一结构化来源；看板不是新进度日志。
汇总器只读取项目文本与检查文件存在，不访问IDA/网络、不调用构建/客户端/测试，不计算摘要。
初次导入仅建立可追踪关系，不把旧文档“已实现/通过”自动升级为本体系验收通过。

## 2. 三层身份与分母

1. **原端层**：函数沿原库存F:RVA稳定ID；导入槽用I:RVA；未归属区间用X:起点RVA-终点RVA（尾不含）。原始3680/218/3061分母保留。别名、共享尾、宿主承接和排除不删除原ID；未归属范围登记不直接从函数分母扣除。
2. **功能层**：FUNC-*为功能域；UI-M-xxx为现代窗口条目，UI-L-xxx为已登记旧选择条目。域是导航，不算叶功能；父域与子项不相加。现代36/37可有两个用户入口，但函数/元素共享身份仍去重。旧选择条目仅从既有索引导入，不凭99条handler推定已全部覆盖。
3. **证据层**：E-*记录证明对象、门禁、证据位置、对照对象、结果和实际验证范围。一个证据可以被多项引用，但不能重复加到功能或原函数分母。

功能枚举、数据对象/间接目标枚举尚未闭合：登记簿的对应closure必须显式记录。
完整快照的candidate_function_ids仅为同合同引用联结，original_inventory_ids才是人工核定成员；两列分别显示。
“已登记条目数”是可用的局部分母，不冒充全客户端功能分母；未知总数使用null/“未闭合”，不显示0或100%。
引用同一文档只能生成候选成员关系；不能由文件名、关键词、目录归属自动证明原函数属于完整功能。
叶条目的实际原成员由original_inventory_ids明确登记；member_inventory_closed及member_inventory_evidence_refs证明本功能成员枚举闭合，不能只填一个入口就通过B；初次没有充分逐项归属时留空并显示“成员待定”。

## 3. 门禁与证据

每个范围内功能采用独立门禁向量，不用加权积分掩盖短板。

| 门禁 | 可通过条件 | 不能替代它的材料 |
|---|---|---|
| B 基线完整 | 原入口/边界、正常/失败路径、数据容量/宽度、状态/时序、读写/owner、真实调用与批准差异可定位；明确功能原成员，未决事实清零 | 有文档路径、反编译名称或原表地址 |
| I 实现对照 | 上述合同逐项关联真实源码/符号，兼容/宿主提供者和排除边界分别登记 | extern、自动定义定位、同名空体或关联备注 |
| W 接线与生命周期 | 真实入口→生产→消费→回调→清理闭环；共享owner、失败副作用、场景往返与依赖有证据 | 单函数本体或主回调表0缺口 |
| S 静态差异核对 | 按总SPEC适用项逐项核原新对应，无未处理差异；具名批准差异定位到实际对象/调用 | 仅文档链接正确、旧摘要或本汇总器退出0 |
| R 运行对照 | 用户恢复执行后，绑定原/新版本和环境，按相同输入/资源/初值记录可观察输出、状态、调用次序及适用画面/音频结果 | 历史静态库构建、推导、截图相似或设计批准 |

B→I→W→S→R按前置门禁顺序；门禁状态为unassessed/blocked/pass/fail，未登记为unassessed。
运行当前固定blocked，不新增或执行测试；这里只定义验收所需证据，不创建可执行测试用例。
整体功能accepted必须全部适用门禁pass、原成员非空、依赖功能accepted、功能阻碍为空，且引用证据属于当前epoch。
暂停项保持分母且不可accepted；范围排除只记录excluded及具体授权，不计pass或accepted。
必要宿主/DX适配是范围内实现方式，不能按“排除”删除全部业务门禁。混合函数只保留批准的局部排除标签。

证据记录至少填写：id、feature_id、gate、epoch、baseline_reference、implementation_reference、
method（static/runtime）、artifacts、observation、expected、actual、differences、result、recorded_on。
版本字段须为可追溯的Git引用/原基线记录位置，不能用“最新版/HEAD/同上”代替固定证据。
artifacts包含文件与稳定章节/符号定位；行号仅辅助。R证据另须environment_reference/input_reference/output_reference指向实际环境、输入和结果记录。
differences为空表示该观察范围无未批准差异；非空逐项填写description与decision_ref，必须指向具名批准，不允许用自由文本豁免。证据仅覆盖明确observation，不能从局部记录外推整功能。
本机脚本能检查字段与引用存在，不能自动裁定原新语义；人工核对责任仍在本线程。

## 4. 指标定义

| 指标 | 分子 / 分母 | 解释 |
|---|---|---|
| 身份纳管 | 追踪快照原函数行数 / 原库存函数数 | 应为3680/3680，仅证明没有漏掉库存行 |
| 合同关联 | 有明确证据文档关联的不同F ID / 3680 | 语义、pending、具名分类的并集；文档路径存在不代表B通过 |
| 历史定义定位 | 直接定位快照中不同F ID / 3680 | 仅生成时源码索引，不是当前实现覆盖率 |
| 语义登记 | 不同有效F ID及无独立ID记录分别计数 | 1136条记录不能直接充当1136个独立函数 |
| 未建立合同关联 | 既有机器台账中无上述文档关联的F ID集合（不等于没有任何局部文档） | 可下载/搜索完整ID，不称全部未实现 |
| 功能B/I/W/S/R | 各门禁pass数 / 已登记且未排除叶条目数 | 仅当前登记集合，域行不加入；暂停保留 |
| 已验收叶条目 | 全适用门禁pass且依赖满足的叶数 / 同上 | 不宣称整体功能覆盖；枚举未闭合时无总百分比 |
| 排除/混合/暂停/pending | 独立集合/数量 | 不加进已实现或已验收 |
| 未归属区间/数据/间接边 | 原库存区间总数、分类引用和closure分别展示 | 不将4份分类记录直接等同于4个完整区间已闭合 |

不输出“1136/3680完成率”，不按文档数、代码行数、耗时或任务数量折算工程完成。
功能分母变化须登记catalog_changes；原库存变化须更新inventory_expected及依据，不能静默缩小分母。
source文件存在不证明符号或行为正确；候选关联和已审成员分开。
验收epoch变化使旧证据失效；影响分析须列受影响功能/依赖并撤销其pass，不能留下旧绿灯。

## 5. 整体关闭门禁

只有以下全部满足，才能宣布范围内完整完成：

- 全原库存身份有明确处置；有映射不等于处置完成，函数分类closure由R01收口。
- 功能目录、数据/间接目标、未归属区间、排除边界四类闭包都有具体证据；未知数为零有依据。
- 已登记所有范围内叶功能accepted，且域内无尚未拆分的业务路径；暂停未恢复不能豁免。
- V01–V06及总SPEC§10的全部适用门禁闭合；最终报告同时列原体对照、批准替换/适配、范围排除。
- function_dispositions逐F ID登记equivalent/adapted/host/excluded/mixed/merged/metadata_only，附epoch/evidence_refs；
  auxiliary_dispositions逐I/X ID登记mapped/excluded/merged及当前证据。区间分类可以引用共享的整段证明，但原区间ID仍逐一处置。
- 不能仅把closure改成closed：必须引用存在的证据文件并给出具体归并/差集结论；人工仍须检查真实性。

当前初始化时以上均保持未闭合。汇总器“校验通过”只指文档/台账结构正确，不是软件验收通过。

## 6. 日常推进

1. 选现有P/C/U/G/T/R/V任务，定位功能ID；缺功能条目则以已有原事实登记新ID，未知继续标明。
2. 先补B所需合同和明确成员，再实现与接线；逐门禁写具体证据与残余，不能直接把任务整体标完成。
3. 同步原语义记录、60项任务行、验收登记簿与唯一进度；不手改生成快照。
4. 在本线plan目录运行 `pwsh -NoProfile -File ./measure_cg2d.ps1` 更新看板/快照；
   `-Check` 仅比较重新推导内容与已存输出，不写文件。任何结构错误先修正，不输出新“通过”快照。
5. 差异检查后精确提交；保留用户原有修改。出现来源内容变化必须做影响分析，不能只依赖-Check：它检查可推导结构，不证明正文语义未变。

## 7. 本次落地范围

先导入现有功能域、现代窗口和已登记旧选择入口，挂接60任务及其合同；全原库存保留未知行。
全量函数/导入/区间追踪由现有JSON自动联结，不重新扫描原二进制、不从自动名称生成业务设计。
初始证据账为空：表示旧成果尚未按这套门禁归档，并非否认已实现本体。历史状态保留在原来源和快照中。
网络验收继续限定原格式明文，401960/401AB0两算法叶排除；音频暂停；无新依赖、构建、测试、执行或摘要操作。

## 8. 登记格式与交付检查

- 功能：`id/kind/parent_id/title/domain/scope`；合同、明确原成员、成员闭合证据、关联任务、依赖、验收重点、批准差异引用和blockers。
- 单门禁：`gates.B = {"state":"unassessed","evidence_ids":[]}`，其余I/W/S/R同形。初始未登记等同unassessed；R受执行限制时显示blocked。
- 函数处置：`function_dispositions`元素包含inventory_id、disposition、epoch、evidence_refs；I/X处置在`auxiliary_dispositions`使用id。文件关联不自动产生处置。
- 任务验收：`task_acceptance`元素包含task_id、state（open/closed）、epoch、evidence_refs；closed必须有当前证据。它不覆盖收尾表中的历史实现描述。
- 功能拆分：保留原ID作为domain或明确记录替代关系，新叶分配新ID，catalog_revision递增并解释分母增减；不能删除未完成子项制造更高通过率。
- 证据字段按§3逐项填实值；实际原数据对象、间接目标集合可在对应合同中维护，closure引用该完整枚举及差集证明。不能只附总SPEC就宣称集合为空。
- 本轮验收对象是“体系是否能追踪并拒绝无依据的通过”，软件验收另计。交付检查包括全库存身份纳管、原ID/文件引用有效、依赖无环、任务号完整、门禁证据规则以及生成结果可重复；不新增/执行项目测试。

尚无结构化合同关联的函数，只表示**目前机器登记未关联**，不直接断言该函数从未取证或没有文档。后续先检索现有合同补关联，再决定是否需要新取证。


## GQ01 开发差集生成入口（2026-10-05）

[局部工具SPEC](cg2d_development_gap_queue_spec.md#development-gap-queue-source-state)把定位/语义身份的登记缺口纳入同一汇总器。
快照development_gap_queue逐项保留原ID或语义JSON引用、原入口/库存名称、具名范围/已有处置/依据及下一步；
metrics.development_gap_counts分别给出定位缺口、语义身份缺口、总数和需核/暂停/排除数量。
看板§5只展开非暂停/非排除的需核项，完整明细仍在快照，-Check同样核生成内容。
无历史定位不代表当前源码未实现；缺ID的记录即使匹配到库存候选也不自动赋值。
未归属项先核具体范围/原身份/当前代码，不能按名称或地址猜范围、恢复音频或生成默认业务。
这些数量只说明登记差集，不是新函数、工作量或完成率；暂停计数仅限本队列的明确成员。
原分母、处置/门禁、全软件验收及执行限制保持，R1/R7未因视图生成关闭。


## CRI01 非函数代码身份登记规则（2026-10-05）

[具名裁决](cg2d_code_range_identity_spec.md#code-range-identity-decision)允许已证代码片段保持inventory_id为空，使用code_identity绑定原库存X区间。
kind为unassigned_range_fragment，原区间ID有效，start/end严格包含于该区间并分别等于原入口/已证边界；证据和源码关联不得缺失。
汇总器校验绑定，semantic_without_inventory_id仍保存原记录，metrics.semantic_code_range_records另计有效区间身份。
GQ01的semantic_identity_unrecorded只统计没有区间绑定的无函数ID记录；不能将有效非函数身份当缺失或补虚构FID。
本组两X区间mapped仅表示完整字节用途/源码关联，不代完整业务、间接边/ABI或运行；原函数/区间分母及软件门禁保持。


## SER01 已批准排除身份的R1登记（2026-10-05）

[逐ID登记SPEC](cg2d_declared_exclusion_registry_spec.md#declared-exclusion-source-state)将原113个具名完整入口排除同步到function_dispositions，原六条保留，本组补107条。
每条保留原分类JSON记录位置或newest五入口依据；没有修改授权/范围、源码或语义记录，没有按名字/地址扩张集合。
键盘共享509080/53A200、两个混合入口、网络两父/原格式明文职责、8位索引资源/调色板及音频范围保持。
排除身份登记参与R1处置数量，不能当实现/验收通过或降低原分母；原定位差集和全部软件门禁保持。
本组排除身份登记完成，不代表R1全量分类、R7未知差集或R8/整体SPEC已经关闭。


## MUP01 既有32位上传的混合身份（2026-10-05）

[混合上传登记](cg2d_mixed_upload_identity_spec.md#mixed-upload-source-state)以原三根392条/1228B和实际色深表绑定现有CPU32位核心及两个已知caller。
原两混合入口的保留/排除分支不变，父职责保留；新增两mixed/一adapted处置、三语义身份和资源组已知成员，成员集合仍未全闭。
定位差集减少两项只表示已有源段现在可对照；本组不新增实现/软件证据，不升级完整门禁。
原第六参非空和1020EC4源的生产/地址/alias仍未闭，不能按没有直接写xref推为不可达或造空producer。


## MPL01 纯NOP元数据的独立处置（2026-10-05）

[处置合同](cg2d_metadata_padding_disposition_spec.md#metadata-padding-source-state)增加metadata_only，限已纳管边界分类中完整五NOP的原元数据条目。
metadata_kind须为alignment_padding，metadata_reference须指向分类JSON确切records/N且出现在evidence_refs中。
汇总器核原ID/入口/末界/唯一chunk、五个90/相邻位置、无已有源/语义映射及无范围重叠；条件不满足则沿既有错误出口拒绝发布派生文件。
该处置保持原函数库存分母，既不是源码实现/语义映射，也不是用户范围排除或功能accepted。
队列保留同一身份，记metadata_padding/metadata_only并单列metadata_only_receipt_items；普通定位差集与需核定位数分开统计。
本组只登记F:000C48EB；其他RET/库/无xref入口或区间不能套用，全部软件门禁和R1/R7/R8保持。


## MRC01 源码取样约束与共享RET归属（2026-10-05）

[本组合同](cg2d_memory_compare_sampling_spec.md#memory-compare-sampling-source-state)落实原低两位对齐和六W32源码取样约束，父记adapted、共享返回记merged。
原两chunk和独立RET的库存身份保持；普通W32载荷复制前后标准signal_fence只核本机SDK头，实际生成指令/读次数/ABI另验。
两限定I/S证据仅覆盖该源表达/原基线/SDK头，不声明全部CRT消费者或运行通过。
父原几何/源算法、共享RET一字节及两入向可追；不混为metadata_only、独立置零体或新caller。
两CRT已知成员和四开放任务同步，完整成员/软件门禁及R1/R7/R8保持。


## BMR01 既有合并片段与父入口的处置（2026-10-05）

[三身份登记](cg2d_boundary_merged_identity_spec.md#boundary-merged-identity-source-state)补两merged/一adapted，绑定已有真实前缀回调/分配失败路径和typed工厂。
数组item head的xref不能代替具体指针槽，BMR01保留实际第13项575B44及前缀索引读取证据。
没有新客户端业务实现、软件证据或全模块完成声明；原ID/分母/范围及完整门禁保持。
公共窗口机制一成员、Yobi两成员和四任务仍open，原其他未知/生命周期/运行及R1/R7/R8继续推进。


## NAS01 适配器源修正与API处置（2026-10-05）

[本组合同](cg2d_network_adapter_sampling_spec.md#adapter-sampling-source-state)登记534EF0 adapted、54AB16 host及I:0014D388 mapped，已有两真实SDK调用和原参数保持，不新造跳板空体。
每槽13B清零、类型一次读取和六U8原序已落既有源；限定I/S证据各一，完整FUNC-NETWORK门禁、成员闭合和五任务仍open，原业务分母/范围/格式调用计数保持。
API本机声明/源调用及导入关系不代加载、链接、SDK实际成功失败、全父/owner/未知alias或原格式明文互通/运行。


## LSP01 五行同源生产和旧字节取样处置（2026-10-05）

[两原身份与共享数据合同](cg2d_legacy_line_sampling_spec.md#legacy-line-sampling-source-state)登记503AB0/4A0640为adapted。原正文写CC2525五行、D672D8只保存同一行T*、旧55借同一owner；不以共享指针表名称判成第二套正文。
本次实际开发为503AB0首U8缓存和首写后尾读取样表达，五行生产体早已存在。限定I/S各一、公共窗口成员9→11并关联U07，U07当前登记open及其余六任务/完整门禁保持，不新增功能分母或改变原排除范围。
全部100父/未知alias/旧表外层/实际ABI及画面/运行继续，静态归属和五行生产存在不能替完整窗口流程验收。


## DFI01 字段基础依赖的原新身份（2026-10-05）

[四字段解析基线](cg2d_delimited_field_identity_spec.md#delimited-field-identity-source-state)补四adapted处置与既有技能生产三解析边，网络已知成员40→44仍未全闭；只是原新关联/处置，不计四项新增实现、软件证据或已验收功能。
原字节/容量/终止/正常返回、x64借地址及比较域取舍均有对应指令与当前源；原函数间隙、306/550完整业务父/字段布局/owner/alias、实际ABI/链接/互通与运行继续保留。
六任务保持open，软件E179和完整门禁/范围及所有原分母保持，明文原格式、音频暂停与执行限制不变。


## FWA01 测宽正常算术与处置（2026-10-05）

[两原入口合同](cg2d_font_width_arithmetic_spec.md#font-width-arithmetic-source-state)补两adapted及限定I/S各一：实际源工作为撤专项U32回绕、保原正常S32字段/计数/读序。
文本成员20→22仍未全闭，T03补具名open依据，其余五任务/完整门禁及所有原分母/范围保持。数学溢出不专项保护/模拟、不称异常等价/安全；全部父/owner/alias、ABI/字体字形与运行继续未验。


## LUA01 旧 UI 共用正常加法（2026-10-05）

[本组合同](cg2d_legacy_ui_add_arithmetic_spec.md#legacy-ui-add-arithmetic-source-state)只改本线共用AddW32正文；五已有业务语义及adapted处置追加依据，无新原身份/语义项/功能成员。149调用表达式保持、原SHR/窄化保留，局部I/S各一不替完整功能门禁。

公共窗口原11成员及未闭状态保持；U06补具名open依据，原派生状态本来open，六任务不计关闭。完整父/外层路由/生产/寿命/ABI/画面/运行仍待；数学溢出不专项模拟、保护或称异常等价。


## NQA01 队列正常长度与三叶处置（2026-10-05）

[本步合同](cg2d_network_queue_arithmetic_spec.md#network-queue-arithmetic-source-state)登记5081E0/508230/508280三个adapted，403E50原处置补依据；实际源码为五处正常S32长度加减。接收入队补当前语义，其他已有身份保持；网络已知成员44→47，成员仍未全闭，I/S各一只适用本组80条257B及源增量。

U10/R01/R04/V03仍open，P09/P10源码关闭状态保持；原帧/业务/明文、门禁/复制数量和发布序不改。完整父/命令/owner/alias/寿命/ABI/SDK、半包/部分发送/互通与运行未验；数学溢出不专项处理/模拟，不称异常等价或安全。


## TTP01 工具准备与生产证据分开登记（2026-10-05）

[三字段生产取证](cg2d_trade_target_production_spec.md#trade-target-production-source-state)只完成定点定位、采集器与范围冲突修正，未捕写者/证明容量/安装执行。旧51功能补当前依据和阻碍，原成员0、完整门禁和源本体状态不变；不登记客户端软件门禁或原处置通过。

U02补具名open依据，原派生状态本来open，R02/R04/V03保持；整体指标/函数分母/语义数/范围及音频暂停、原格式明文不变。动态许可需按AGENTS第3/12条单独明确恢复，工具存在不解除限制。


## APS01 音频暂停归属与实现/处置分开（2026-10-05）

[23成员范围依据](cg2d_audio_paused_identity_spec.md#audio-paused-identity-source-state)补FUNC-AUDIO的七既定暂停根和16依赖，scope保持paused/成员未闭；不新增equivalent/adapted/host/excluded或客户端门禁。十六已知队列项保留原ID/定位缺口，派生为paused，需范围/映射核定75→59/明确暂停0→16只是队列分类。

P11–P17/V05补当前open依据，原派生状态本来open，R01/R04保持。原处置342/3680、语义/软件证据、全部原分母/其他范围和完整门禁不提高；R1最终处置及完整音乐两链/生命周期/真实执行仍待，不能将已列原成员当已实现/已验收。


## CPH01 编译器host与调色板源码证据（2026-10-05）

[本组合同](cg2d_compiler_helper_spec.md#compiler-helper-source-state)补53B498/53BDC0两个host语义/处置，CRT已知成员145→147，成员仍未全闭。原x86寄存器/栈检查私有机制由本机编译器承担，批准差异不代具体生成指令/ABI或全父/浮点环境验收。

调色板两匿名工具实际撤异常补零/回绕，局部I/S各一只计本组源码及原六转换/计数邻窗；完整CRT/渲染门禁、C05/G04/R01/R04/V04和全357条调色板/资源/画面/运行仍未闭。D02不解除业务正常位宽，RPLAN.24不允许称数学异常/故障等价或x64安全。


## MFI01 已有地图实现的正式追踪（2026-10-05）

[本组合同](cg2d_map_record_identity_spec.md#map-record-identity-source-state)将四生产/两比较从仅历史直接位置补为六有效语义及六adapted处置，FUNC-MAP已知成员17→23，仍未全枚举；原六体492条1730B与33引用身份不代全部父实参或整地图验收。

只补本组S静态一项，E187→188，源码未改，不新增I开发成果、运行证据或提升完整功能门禁。U11/G02/R01/R04/V04继续open，其中G02由原派生默认open补显式open和依据；队列177保持，因为六原身份已有历史直接位置。R1/R7/R8及全范围分母保持。


## ICL01 终行长度源码修正与局部证据（2026-10-05）

[本组合同](cg2d_input_terminal_length_spec.md#input-terminal-length-source-state)实际修复44A7D0终行U32+bit_cast回绕承载，保原正常S32含NUL数量、独立取样/槽写后早退。语义1260/有效ID1256、函数处置350/3680及输入/文本14/22明确成员保持，只追加既有原身份依据。

E188→190仅本次FUNC-INPUT I/S局部；完整两功能门禁、T01/T02/R04/V04、10/60任务关闭数及R1/R7/R8保持未闭。不以三行修正或原137条保存推完整编辑/焦点/IME/owner/alias/机器PC53/SDK/画面运行，也不称数学异常安全或故障等价。


## ICM01 光标坐标相位实际源码修正（2026-10-05）

[本组合同](cg2d_input_caret_arithmetic_spec.md#caret-arithmetic-source-state)实际撤两坐标工具/相位的S32回绕承载；既有语义1260/有效ID1256、处置350/3680及输入/文本14/22成员保持。原身份只加三块依据和两匿名来源，不新增F或改变既有处置类别。

E190→192仅FUNC-INPUT本次I/S局部；T01/T02/R04/V04、全输入/文本门禁、10/60任务关闭数及R1/R7/R8仍未闭。83条完整原体保存不代全部语义/完整父、owner/alias/焦点/IME、机器环境/SDK/像素画面运行；数学异常不专项保护/模拟或称故障等价。


## ITA01 输入绘制正常算术实修（2026-10-05）

[本组合同](cg2d_input_text_arithmetic_spec.md#input-text-arithmetic-source-state)实际撤两个工具S32回绕承载，21源表达式/原阶段和字段/数量/窄化/排队发布保持。语义1260/有效ID1256、处置350/3680及输入/文本14/22明确成员保持，只加旧根依据与两匿名来源。

E192→194仅FUNC-INPUT本次I/S局部；T01/T02/R04/V04、完整两功能门禁、10/60关闭数和R1/R7/R8仍未闭。原332条保存不代全部语义/完整父/owner/alias/焦点/IME、SDK/机器/像素画面运行，数学异常不专项模拟/保护或称故障等价。


## RPA01 未归属区间完整字节分类（2026-10-05）

[64项区间合同](cg2d_unassigned_range_classification_spec.md#range-classification-source-state)依既有辅助mapped规则纳管原全部502B，逐项parts连续覆盖；62段纯填充及两表/对齐/后缀有原字节、实际块和引用依据。原2项保留，reviewed_ranges=66/3061，剩余2995；原区间分母/函数分母不改，R03/R04保持open。

无新增F/源实现/软件证据或范围排除；两表父语义/网络数据依据追加，成员47、E194、350函数处置与其余门禁保持。mapped是字节用途登记，不能作为相邻完整功能/未知间接目标、owner/SDK/ABI或实际运行验收。

## IMA01 输入法显示正常算术实修（2026-10-05）

[本组合同](cg2d_ime_display_arithmetic_spec.md#ime-display-arithmetic-source-state)实际撤三个工具的S32回绕承载，保32处表达式/原窄化/SDK字段与候选布局和取样。语义1260/有效ID1256、处置350/3680及输入/文本14/22明确成员保持，只加旧根依据和三匿名来源。

E194→196只FUNC-INPUT本次I/S局部；T01/T02/R04/V04、完整两功能门禁和10/60关闭数保持。357条保存不代全语义/完整父/alias/候选容量寿命/SDK/画面运行；数学异常不保护或模拟/称故障等价，R1/R7/R8未闭。

## ACB01 动画分配预算实际修正与身份纳管（2026-10-05）

[当前合同](cg2d_animation_capacity_spec.md#animation-capacity-source-state)实际撤P1额外零哨兵/清零，恢复固定20500项，仅原指针步长适配；正常包含上界查询/四分配序保持。新增三查询语义、四处置及RESOURCE25→26/ANIMATION0→4明确成员，候选仍只按同文档引用派生。

语义1263/有效ID1259、处置354/3680；E196→198仅RESOURCE本次I/S局部。G01/U11/R01/R04仍open，完整功能、10/60关闭数、R1/R7/R8未闭；原186条548B/五call一回调保存不代全父/寿命/SDK/画面运行，异常不专项保护或模拟/称等价。

## ATP01 动画具名指针实修与清理纳管（2026-10-05）

[本组合同](cg2d_animation_pointer_spec.md#animation-pointer-source-state)实际落实五具名引用/三malloc结果承接与P4独立子项、两清理类型读取，布局和业务流程保持。两旧生产补依据、两清理新身份及四adapted，使语义1265/有效ID1261、处置358/3680，ANIMATION4→8明确成员；候选仍按同文档引用派生。

E198→201只本次ANIMATION I/W/S局部，W不代所有owner寿命。G01/U09/R01/R04、完整功能/10关闭数和R1/R7/R8仍未闭；565条1786B/七call六父保存不重验全部生产语义/完整父/P4生产/alias/实际机器ABI/SDK/画面，异常不保护或模拟/称安全/故障等价。

## MFA01 效果正常固定点实修与限定数值路径（2026-10-05）

[本组合同](cg2d_map_effect_arithmetic_spec.md#map-effect-arithmetic-source-state)实际将固定点四加法/两相机缩放恢复正常S32，原低S32/位型/四固定点与两像素写、x87和RNG序保持。新增三语义/限定adapted使1268条/1264有效ID、361处置；MAP23→24/CRT147→149明确成员。

E201→203只MAP本次I/S局部；六任务和完整功能门禁保持open，原495条/两块内核及引用类型保存不代完整场景/一般pow/其它调用与所有owner寿命/真实ABI/SDK/像素RNG运行。两原定位差集有源可追不计新增数学实现，R1/R7/R8仍未闭。

## MCA01 六处正常坐标算术实修（2026-10-05）

[本组合同](cg2d_map_coordinate_arithmetic_spec.md#map-coordinate-arithmetic-source-state)实际将五像素运动/一投影增量恢复正常S32，原20读10写、取样/扰动/真实位型与投影低W32/快照序保持，另恢复kind4重设尾计数4→新Y速度原序。六新语义/限定adapted使1274条/1270有效ID、367/3680处置，MAP24→30明确成员；候选继续按同文档引用派生。

E203→205只MAP本次I/S；六任务/完整功能门禁和10关闭数保持。908条2948B/六call三父只局部静态，不代全场景/特殊多轮/所有调度/alias/owner寿命/投影环境/实际SDK/像素RNG运行；R1/R7/R8仍未闭，异常不保护或模拟/称等价。

## MFP01 局部验收增量（2026-10-05）

[效果生产合同](cg2d_map_effect_producer_spec.md#map-effect-producer-acceptance)以原64条193B/三call一父和四原读取点/两缩放写为基线；一个本体/真实声明纳入sources和adapted，MAP30→31。E205→207只本次I/S局部静态，不将完整原体保存或数域推导当整个父、池生命周期、实际ABI/SDK/像素与RNG运行的证据。

G01/G02/U11/R01/R04/V04追加同一具名引用且继续open。所有B/D/W/R、完整功能门禁、成员枚举、R1/R7/R8及整体判定保持；音频暂停、网络原格式明文和现行执行限制不变。

## MEP01 池局部I/S（2026-10-05）

[验收合同](cg2d_map_effect_pool_spec.md#map-effect-pool-acceptance)以四原105条400B/25call十父及原低W16/领取回收读写、已有清理消费为基线。三源片段实修，四身份/真实来源/限定adapted和MAP35成员登记；E207→209仅本次I/S，已有46F160清理本体未修改，不把原体保存或身份数计为新增功能。

G01/G02/U11/R01/R04/V04追加同一具名引用并保持open。完整池/场景/所有父与alias/owner重进退出/借用失效和实际ABI/SDK/画面RNG运行未闭，B/D/W/R及全部完整门禁、R1/R7/R8/整体判定不提升；音频暂停、原格式明文和现行执行限制保持。

## MSS01 共用原证据的MAP/LOOP局部I/S（2026-10-05）

[半字快照合同](cg2d_scene_snapshot_width_spec.md#scene-snapshot-width-acceptance)以31条138B/两call两父及原2B取样/高半发布、复读gate→清计数为基线。一语义/来源/adapted分别关联MAP36与LOOP1，E209→213仅各一组I/S；四条记录共用同一实际改动/原证据，不当四项开发或完整功能门禁。

G01/G02/U11/R01/R04/V04追加具名引用且继续open，V02及其他任务保持。完整父/callee/alias及owner寿命/空链机器返回、真实ABI/SDK/时钟RNG/场景画面运行未闭，所有B/D/W/R/完整功能、R1/R7/R8与整体判定不提升；现行执行限制、音频暂停与网络原格式明文保持。

## MQC01 两域共用观察链局部I/S（2026-10-05）

[查询/清理限定验收](cg2d_map_chara_lookup_spec.md#map-chara-lookup-acceptance)以两体57条134B/七引用六call一tailjump五父为原基线。一查询源块恢复next读取点、已有清理未改，两正式语义/来源及adapted与MAP38/LOOP3成员同步；E213→217只两域各I/S，共用一源码改动/两原证据，不计四函数或新清理功能。

G01/G02/U11/R01/R04/V04继续open。五完整父/全producer/地址逃逸与alias、所有owner重进退出/borrow失效/空链机器EAX/实际ABI/SDK/时钟RNG/场景画面运行未闭；全部B/D/W/R/完整功能和R1/R7/R8/整体判定不提升，现行执行限制/音频暂停与原格式明文保持。

## MCL01 共用链接原证据的局部I/S（2026-10-05）

[追加/摘链与已有清头限定验收](cg2d_map_chara_link_spec.md#map-chara-link-acceptance)以三体39条110B/九call七父为原基线；两源块恢复原next读/复读，已有清头未改，三语义/来源与adapted及MAP41/LOOP6成员同步。E217→221仅两域各局部I/S，共用两源实修/三原证据，不计清头新实现或双份函数。

G01/G02/U11/R01/R04/V04仍open。三生产者/两回调及七完整父、全部producer/间接alias和owner重进退出/borrow失效/实际ABI/SDK/时钟RNG/场景画面运行未闭；全部B/D/W/R/完整功能、R1/R7/R8与整体判定不提升，执行限制/音频暂停/网络原格式明文保持。

## MCP01 原生产复制/取样局部I/S（2026-10-05）

[三个生产者限定验收](cg2d_map_chara_producer_spec.md#map-chara-producer-acceptance)以288条1126B/三call两父及原九组内存数值复制、父载荷与三个子槽复读为基线。六具名源片段（五行为/一注释）实修，三语义/来源/adapted及MAP44/LOOP9成员同步，E221→225只两域各I/S，共用同一批源修正/三原证据，不记双份函数。

G01/G02/U11/R01/R04/V04仍open；全288其他语义/完整父与所有callee/五槽全上游/间接alias及owner寿命/实际ABI/SDK/时钟RNG/场景画面运行未闭。全部B/D/W/R/完整功能、R1/R7/R8/整体判定不提升，现行执行限制、原格式明文及音频暂停保持。

## MOS01 源取样与薄父局部I/S（2026-10-05）

[限定验收](cg2d_map_observation_source_spec.md#map-observation-source-acceptance)用44条147B/两call两父，核440F50一具名源片段、图号/载荷/发布取样及原标量/五槽，440FE0原同序薄父源码保持。两语义/来源/adapted及MAP46/LOOP11成员同步，E225→229仅两域各局部I/S，共用一处实修/两原体，不记双份函数或第二处源修复。

G01/G02/U11/R01/R04/V04仍open；完整父与callee/全上游/间接别名和owner寿命/实际ABI/SDK/时钟RNG/场景画面运行未闭，音频callee仍暂停，全部完整门禁/功能、R1/R7/R8及整体判定不提升。

## MOC01 回调读取序局部I/S（2026-10-05）

[限定验收](cg2d_map_observer_callback_order_spec.md#map-observer-callback-order-acceptance)核一回调五片段/原233条767B/唯一type1地址写者，恢复入口/两轴/排序与子绘图读取序；原数值叶、五槽/降序/S8交换及绘后递增保持。一语义/来源/adapted、MAP47/LOOP12成员，E229→233只两域各局部I/S，共用一实修/一原体，不记双份函数或直接call。

六任务G01/G02/U11/R01/R04/V04仍open，所有callee/完整父和全上游/间接别名及owner寿命/真实PC53/SDK/场景画面运行未闭；完整门禁/功能、R1/R7/R8及整体判定不提升。

## MCS01 子生产/阶段取样局部I/S（2026-10-05）

[限定验收](cg2d_map_child_stage_sampling_spec.md#map-child-stage-sampling-acceptance)核422条1479B/三call两父、七源片段、正常子阶段及图号读取点与分类快照承接；两个已有同序创建者保持，单参数分类形式保其它调用者读取点。三语义/来源/adapted、MAP50/LOOP15成员同步，E233→237仅双域各局部I/S，共用一子更新及分类承接/三原体，不记双份函数或两创建者为实修。

G01/G02/U11/R01/R04/V04仍open；其它422语义/完整父与callee/全上游/间接别名和owner寿命/真实ABI/SDK/场景画面运行未闭，音频继续暂停。完整门禁/功能、R1/R7/R8及整体判定不提升。

## MCE01 退场取样局部I/S（2026-10-05）

[限定验收](cg2d_map_child_exit_sampling_spec.md#map-child-exit-sampling-acceptance)核十具名片段、两完整638条2111B/1call与1type1两父及依赖窗92条250B，只子/父阶段与参数、两偏移各原序及共用排序/绘子项。两语义/来源/adapted、MAP52/LOOP17成员，E237→241仅双域各局部I/S，共用一批源修正/原证据；不计双份函数或全43F520。

MCS01分类调用次数的失实短语已据实际一次修正，不重写历史或降标准。六任务G01/G02/U11/R01/R04/V04仍open，完整父与callee/上游/间接别名及owner寿命/真实SDK/PC53/场景运行未闭，全部完整门禁/功能、R1/R7/R8及整体判定不提升。

## MOB01 主回调分支局部I/S（2026-10-05）

[限定验收](cg2d_map_observation_branch_spec.md#map-observation-branch-acceptance)核五片段/四类取样及完整319条999B/唯一type1写者，新227条与旧92条连续复用及F32位型。原飞行/就绪/失败/死亡后继续、子生产消费和共用逻辑保持；一语义/来源/adapted，MAP53/LOOP18成员，E241→245只双域各局部I/S，不记双份函数或call。

六任务G01/G02/U11/R01/R04/V04仍open，所有其它原语义/完整父与全部callee/全上游/间接别名及owner寿命/真实SDK/PC53/时钟RNG/场景画面运行未闭；全部完整门禁/功能、R1/R7/R8及整体判定不提升。

## AAS01 附件取样局部I/S（2026-10-05）

[限定验收](cg2d_actor_attachment_sampling_spec.md#actor-attachment-sampling-acceptance)为五路径四种读序及附加入口/首附件十片段归档：原715条2252B，完整4417E0和442F70两局部窗/43E490四依赖窗分别计；1type1绑定、1type17call分列。新增一语义/追加一既有语义及两adapted，MAP55/LOOP20成员，E245→249只双域各局部I/S，共用同源证据。

G01/G02/U11/R01/R04/V04仍open；全部完整门禁/功能及R1/R7/R8/整体仍未闭。其它原语义/完整父与callee、全上游/间接别名及owner重进退出和借用失效/真实SDK/PC53/时钟RNG/场景画面运行继续，不以四局部pass替代完整验收。

## ACP01 两生产者局部I/S（2026-10-05）

[限定验收](cg2d_actor_coordinate_producer_spec.md#actor-coordinate-producer-acceptance)核两具名源片段/两完整212条829B、12type17call8父、各四独立构析/三字段先取齐再原序发布和W32/U8/低W16保持。两语义/来源/adapted，MAP57/LOOP22成员、E249→253仅双域各局部I/S共用同一实修/原证据；5原挂件call/4现表达式分列。

六任务仍open，全部完整门禁/功能、R1/R7/R8和整体不提升；完整父实参/其它语义及所有callee/上游和owner重进退出/借用失效/SDK/PC53/时钟RNG/场景画面运行未验。

## YLC01 回收取样局部I/S（2026-10-05）

[限定验收](cg2d_yobi_cleanup_sampling_spec.md#yobi-cleanup-sampling-acceptance)核三完整160条444B/31call10父、三源片段全文件精确重放，两个链接写之间原指针重取/计数单读局部判定完整返回/payload单次判空释放，原四相/回调和玩家八槽/文本/玩家槽尾链保持。新增一语义/追加两既有语义、三adapted、MAP60/LOOP25成员，E253→257仅双域各局部I/S同证据。

六任务open、全部完整门禁/功能和R1/R7/R8/整体不升级；完整父/callback/callee/生产/链别名与owner寿命/ABI/SDK/PC53/时钟RNG/场景画面运行仍未验，完整原指令保存不代这些验收。

## YSS01 初始化取样局部I/S（2026-10-05）

[限定验收](cg2d_yobi_sentinel_sampling_spec.md#yobi-sentinel-sampling-acceptance)核初始化四片段及三完整59条225B/22call13父，初start早于end发布、两链接原读序、priority前result快照/graphic目标复读返回result，原两分配/字段和已有清场终止保持。三既有语义仅追加/三adapted，MAP63/LOOP28成员，E257→261仅双域各局部I/S共用同一实修/原证据；语义条数和有效ID不增加。

六任务open、全部完整功能/门禁和R1/R7/R8/整体保持未闭；全部应用/场景父与callee/callback、资源生产/实际跨门禁清场和链别名/owner重进退出/借用失效/SDK/ABI/PC53/时钟RNG/画面运行仍未验。

## YAS01 分配插链局部I/S（2026-10-05）

[限定验收](cg2d_yobi_allocation_sampling_spec.md#yobi-allocation-sampling-acceptance)核一完整118条354B/113call/86已知父+1未归属调用点、四源片段全文重放，载荷同返回快照/原失败与再次清零、原描述符八字段宽度/写序、首尾及两next取样和新节点链接写后的前驱复读；原unsigned FIFO和未链接返回不变。仅一旧语义追加/一adapted，MAP64/LOOP29成员，E261→265只双域局部I/S共用同一源/原证据，语义条数和有效ID不增加。

六任务及全部完整功能/门禁/R1/R7/R8/整体保持未闭；所有113调用点实参/载荷与全部callee/资源生产/链别名和owner寿命/ABI/SDK/PC53/时钟RNG/场景画面运行仍未验。原正常显式零写不当作新增异常保护或故障等价。

## UPS01 旧页预览局部I/S与R7用途（2026-10-06）

[限定验收](cg2d_ui_preview_sampling_spec.md#ui-preview-sampling-acceptance)核三390条1331B/12直接调用8父与1回调地址发布、一根三源替换和完整X587B三部分覆盖/临时只读解码；结果零判定先于单次图号读取，两写分支共享，原空门禁/六图号族与四动作、未匹配保留及其它正文不变。

E265→269为公共UI/Yobi两域局部I/S共用同源/原证据，不代完整门禁；两F与一代码片段、X字节用途身份分列，语义条数不当库存函数数量。六任务与全门禁/叶/R1/R7/R8/整体保持未闭；完整父/外层路由/数据资源与owner寿命/ABI/SDK/PC53/时钟RNG/画面运行仍待，现行不执行限制保持。

## GRS01 采集结果局部I/S（2026-10-06）

[限定验收](cg2d_gather_result_sampling_spec.md#gather-result-sampling-acceptance)核单根437条1538B/四源片段全文重放、descriptor七原宽度写序/重叠、mapped先判后pub与图片/非空详情行Y计算后取X，全部其它正文/原生owner保持。空入向集合仅原事实，不当不可达或整旧表接线证据。

E269→273为公共UI/旧39局部I/S共用同一源/原基线，一旧F/一adapted不计新函数，语义/有效ID与R7/队列保持；五任务及完整门禁/叶/R1/R7/R8/整体不提升，完整真实路由/数据资源/owner寿命/ABI/SDK/PC53/时钟RNG/采集往返和画面运行仍待。

## WRS01 现代窗口与场景调用局部I/S（2026-10-06）

[限定验收](cg2d_window_routing_sampling_spec.md#window-routing-sampling-acceptance)核两完整126条453B/16普通call+1尾跳与3父、单根三源片段全文精确重放，三判定先于共享发布、原WMB/窗口及共享索引复读，已有转生入口/两场景16表达式/声明保持。原现代路径不替代旧57C368尚待证明的+04来源。

E273→277只两域局部I/S共用同三实修片段/原证据，旧F增证/一新F和跨域成员不多计函数。六任务与全部完整门禁/叶/R1/R7/R8/整体不提升；所有间接目标/场景分支与数据资源、owner寿命/SDK/ABI/本机EFLAGS/PC53/时钟RNG/画面运行仍待，原格式明文/音频暂停与不执行限制保持。

## BCS01 字组复制/残影创建局部I/S（2026-10-06）

[可度量验收](cg2d_battle_copy_sampling_spec.md#battle-copy-sampling-acceptance)核五478条1989B/17call+1绑定/10父、三根五源片段全文重放、20/16B九W32字组各读后发布以及残影/创建三取样差异；原十CryptInt三字段与token/reserved、Clone/浅复制/其它正文保持，不添Read/Ctor/RNG或新失败门禁。

两旧语义仅追加/三个既有本体原身份纳管、五F跨域和E277→281四局部I/S不等五全函数或战斗整体通过。U08原默认open补显式open，其余六任务及全门禁/叶/R1/R7/R8/整体保持未闭；完整父/callback与数据资源/owner浅别名寿命/ABI/SDK/PC53/实际RNG/画面运行仍待。

## BTS01 战斗回调公共尾局部I/S（2026-10-06）

[可度量验收](cg2d_battle_tail_sampling_spec.md#battle-tail-sampling-acceptance)核原完整684条2444B/13地址绑定10父、独立257槽1028B/200目标与源对应，以及一根四片段全文精确重放和scene/count/坐标/附件取样。原完整分发与入口载荷、跨消费重新取第二槽及MarkDeath后清槽保持。

一新F跨两域不多计，E281→285只共用本组源与原证据的四局部I/S；七任务、全部完整门禁/叶/R1/R7/R8/整体保持open。所有200动作/13绑定父的实参、数据资源/owner重进退出和浅别名/SDK/ABI/PC53/时钟RNG/画面运行另验，不据局部表对应放宽整体标准。

## BAS01 附件同步字节门禁/取样局部I/S（2026-10-06）

[可度量验收](cg2d_battle_attachment_sampling_spec.md#battle-attachment-sampling-acceptance)核三完整421条1507B/4call同一父与四表44槽176B，以及全CPP从HEAD十三片段精确重放、五W8门禁和位置/方向/flags缓存/投影及辅助动作/坐标取样。原不同空门禁、主回调1/1/2与字段/播放/owner保持，不据原表字节正确验所有生产及父或运行。

三新F同双域不多计；E285→289只四局部I/S共用本组实修和原证据。七任务及全部完整门禁/叶/R1/R7/R8/整体保持open，完整附件生产/各动作与父实参、资源和owner重进退出/浅别名/SDK/ABI/PC53/时钟RNG/画面运行仍需对应证据。

## BAP01 附件生产/槽位发布与清理局部I/S（2026-10-06）

[可度量验收](cg2d_battle_attachment_producer_spec.md#battle-attachment-producer-acceptance)核八442条1393B/24call+3绑定15父、6槽24B及七根九片段精确重放：BC0独立原正文/直接factory、C20合同、各槽快照与跨调用复读/清槽、type3先判后pub与三F32、OR/W16与共用callback取样、原正确工厂/失败/返回和八既有声明保持。

八新F同双域不多计，E289→293只四局部I/S共用本组实修与原证据。两index0/1调用点只计数/传参/循环尾与当前源对应，不等两完整父验收；七任务与全完整门禁/叶/R1/R7/R8/整体仍open。全部父/动作实参和数据资源、owner重进退出与浅别名/SDK/ABI/PC53/时钟RNG/画面运行仍待。

## BAR01 清槽与恢复限定I/S（2026-10-06）

[可度量验收](cg2d_battle_attachment_recovery_spec.md#battle-attachment-recovery-acceptance)核三根十一片段、七逐槽/三恢复快照与调用后原slot-payload复读及先判400后scale pub，完整状态177条608B/父八窗401条1549B/比较前置16条66B和十普通入向4父可追；三父本体/五清理三恢复及四既有声明保持，不据三父完整元数据验余1385条5495B。

同四F双域不多计，E293→297只四局部I/S共用同源差异和限定原证据；不造CPP helper库存函数。七任务及所有完整门禁/叶/R1/R7/R8/整体open，完整三父/所有动作参数、数据资源与owner重进退出和浅别名/SDK/ABI/PC53/时钟RNG/实际画面仍需对应证据。

## BRA01 动作226完整根的局部I/S（2026-10-06）

[可度量验收](cg2d_battle_ride_exit_sampling_spec.md#battle-ride-exit-sampling-acceptance)核一个完整277条1145B/四页指令原字节和28调用、唯一入向，以及十二源片段精确重放及读取/发布/清槽时点。已有F:000154C0在语义index1322/原adapted及双域追加依据，不增身份/成员/处置或完整门禁。

E297→301四局部I/S共用本根证据；语义1324/有效ID1319、disposition426和全部原成员/任务及门禁保持。BAR01旧108条454B与新增169条691B分列，224/227余1216条4804B及全部数据/owner/SDK/ABI/PC53/时钟RNG/画面运行仍待，七任务和R1/R7/R8/整体open。

## BRB01 动作227完整根的局部I/S（2026-10-06）

[本次可度量验收](cg2d_battle_ride_animation_sampling_spec.md#battle-ride-animation-sampling-acceptance)核完整389条1714B五页原指令字节/28调用与唯一父、十二源片段精确重放和22标量/四W32/五属性各目标/同对象取样、计数发布后offset门禁/两S32轨迹及尾序；仅本根静态，不沿动作226统一分支模板。

已有F:00015940/index1323和adapted及双域只追加依据，身份/成员/处置与完整门禁保持；E301→305四局部I/S共用本根证据。旧100条429B与新增289条1285B分列，仅224余927条3519B、全部数据/owner/SDK/ABI/PC53/时钟RNG/画面仍待；七任务及R1/R7/R8/整体open。

## BRC01 动作224完整根的有限I/S（2026-10-06）

[可度量验收](cg2d_battle_ride_transition_sampling_spec.md#battle-ride-transition-sampling-acceptance)核十五页1136条4251B指令原字节/90调用/唯一父与31源片段，来源/目标/同表行/同对象、预推进一次取样及两不同计数门禁和原失败/早退可核。旧209条732B/新增927条3519B及三父本体有限差集0分列，不外推运行等价。

已有F:00014050/index1321及adapted/双域只追加依据，四局部I/S令E305→309，所有计数/成员与完整门禁保持；七任务、R1/R7/R8/整体open。所有其它动作/数据资源及owner重进退出/浅别名/借用失效、SDK/ABI/PC53/时钟RNG/实际画面继续。

## BEV01 场景事件生产和推进有限I/S（2026-10-06）

[可度量验收](cg2d_battle_event_production_sampling_spec.md#battle-event-production-sampling-acceptance)核五原110条362B的实际字节/边界、439普通call188父和3出向及三根三片段精确重放；各flags OR/pub、动态mask/初始化后slot发布及不同null/计数/哨兵/返回分工可追。

五新F/五adapted与双域同成员同步，E309→313四局部I/S共用同组三片段/原证据，不能据188父引用当父正文或完整流程；全部其它字段/完整门禁及七任务、R1/R7/R8/整体open，所有其它动作/数据资源和owner/SDK/ABI/PC53/时钟RNG/画面继续。

## BQR01 初始化/追加与就绪有限I/S（2026-10-06）

[可度量验收](cg2d_battle_queue_readiness_sampling_spec.md#battle-queue-readiness-sampling-acceptance)核四完整211条751B/五页指令原字节、101普通call7父/8出向与两根五片段精确重放，四W8/普通有序坐标短路、原五/三计数读取及各初始化/恢复/返回可追。

四新F/adapted与双域同成员同步，E313→317只共用五片段和原证据，不验全部父/流程。七任务及R1/R7/R8/整体和完整门禁open，所有其它动作/列表数据资源和owner/SDK/ABI/PC53/浮点状态/时钟RNG/画面继续。

## BQA01 正确既有叶的基线/身份有限I/S（2026-10-06）

[可度量验收](cg2d_battle_reset_append_baseline_spec.md#battle-reset-append-baseline-acceptance)核四原99条439B字节/边界、1206普通call26父/无出向/18引用页和四现有正确源合同/全CPP及头保持，不计源码改动；Reset次序/门禁差异、两next/复读与439400六CA0和16B值语义可追。

四新F/adapted与双域同成员同步，E317→321仅已有实现/有限静态证据，所有完整门禁/七任务/R1/R7/R8/整体open。不据引用全集验26父/完整数据列表和owner/SDK/ABI/PC53/时钟RNG/画面，不宣异常/机器ABI等价。

## BMR01 M读取局部I/S（2026-10-06）

[可度量验收](cg2d_battle_m_read_sampling_spec.md#battle-m-read-sampling-acceptance)要求四根220条548B/77普通call5父/14出向五页完整、两根仅三片段修复与整CPP精确差异、正确预读/查找、旧对象头/Input/公开头保持。原格式明文及旧字节/正常S32/W32合同不扩展。

新增四原身份和adapted、双域共用局部I/S；所有七任务及完整门禁保持open。源码明示顺序修复不证明运行故障/异常等价，全部M生产消费、owner/SDK/ABI/PC53/时钟RNG/画面继续。

## BMS01 M读取/预扫描有限I/S（2026-10-06）

[验收标准](cg2d_battle_m_scan_sampling_spec.md#battle-m-scan-sampling-acceptance)要求五完整328条911B/696call8父/27出向、六指令页十二引用页，四根仅七片段与正确439600/旧依赖/公开头保持；RID失败恢复及成功两W8计数后pub游标、两字符串不同第二字节合同可追。

新增四语义身份/五adapted，旧813历史追加不重复计数；双域共用局部I/S/七任务与所有完整门禁保持open。明示顺序/宽度修复不证明运行故障/异常等价，全部8父/M生产消费、owner/SDK/ABI/PC53/时钟RNG/画面继续。

## BMD01 M分派/生产有限I/S（2026-10-06）

[验收标准](cg2d_battle_m_dispatch_sampling_spec.md#battle-m-dispatch-sampling-acceptance)要求三完整1725条6015B/41call6父/435出向与23页、6逻辑表646B及407输入槽/382技能槽+19表外当前分类、父三片段精确差异/两个正确依赖和共享表/公开头保持，数据生产/原消费次数可追。

三新F/adapted双域共用局部I/S/七任务及所有完整门禁保持open；源码零写/条件序不证明运行故障/编译后指令或异常等价，全部6父/M资源消费、owner/SDK/ABI/PC53/时钟RNG/画面继续。

## BME01 M冲突/事件/初始化局部I/S（2026-10-06）

[验收标准](cg2d_battle_m_event_dispatch_sampling_spec.md#battle-m-event-dispatch-sampling-acceptance)要求三完整302条966B/五页/2普通call与1绑定3父/10出向、十三片段精确差异和真实合并/冲突/工厂失败/初始化合同；正确依赖/初始化/公共头及共享声明头仅具名注释差异保持。

旧814/815仅追加、一新F及三adapted双域共用局部I/S/七任务和完整门禁保持open；未知头原计数不补零/加拒绝，不宣异常或编译指令/运行故障等价，完整所有事件/资源消费及owner/SDK/ABI/PC53/时钟RNG/画面继续。

## BMF01 三域局部实现/静态证据（2026-10-06）

[验收边界](cg2d_battle_event_finish_scene_lifecycle_sampling_spec.md#battle-event-finish-scene-lifecycle-sampling-acceptance)：两完整850条3188B与十八逻辑二十二处源码修复，完成→Init/标死和十状态C/M/Yobi清理/转场/退出请求真实接线纳管；三表22槽88B含原音乐父选择，音频仍暂停。旧803追加/一新场景、两adapted及三域成员：LOOP只场景，YOBI/BATTLE两根；E337→343仅六局部I/S。

七任务仍open、60项已闭10及全部完整门禁不提升，原格式明文不变。关联1463/3680=39.76%仅关联覆盖率；完整427470/五父/资源owner与ABI/PC53/时钟RNG/画面、R1/R7/R8与整体继续。静态文本汇总不替代运行验收。

## BMG01 有界门禁实现与静态证据（2026-10-06）

[验收边界](cg2d_battle_event_execution_gates_spec.md#battle-event-execution-gates-acceptance)：四区间312条1201B七页/25槽100B和真实type1/data绑定、四inline计时去重；两CPP三根四片段恢复S32头与CHT/延迟零/同timer剩余值负条件先pub，其它正文/头保持。旧812追加、不新原身份/分类/完整成员，E343→347仅YOBI/BATTLE四局部I/S。

七任务open、语义/有效ID/关联/成员、R7与60项已闭10和所有完整门禁保持。17298父统计不代全部消息目标/SEH/SDK/ABI/owner/时钟RNG/画面运行，R1/R7/R8和整体继续；获准汇总仅文本，不降标准或解除运行限制。

## BMH01 数据消息与渲染字段承载局部证据（2026-10-06）

[验收边界](cg2d_battle_m_data_sampling_spec.md#battle-m-data-sampling-acceptance)：六数据块及共用Finish尾329条1314B九页/63call、五CopyPayload点与正确9条27B核心；五取样差异、两CPP六片段/唯一头声明、实际S32字段const引用绑定及原DX读取接口/其它消费者保持有限静态。E347→353只YOBI/BATTLE/RENDER六局部I/S，Render仅新字段读取与Chat消费者。

旧812追加无新F/X/adapted/完整成员；七任务open、60项已闭10/R7区间67/3061/队列175、全部计数和完整门禁保持。全部消息目标/SDK/SEH/ABI/资源owner/PC53/时钟RNG/像素/画面仍待，获准汇总不代运行或降低整体验收。

## BMI01 转场/宠物有界实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_transition_sampling_spec.md#battle-m-transition-sampling-acceptance)：九区间448条1743B十二页/60call与六目标原跳转范围闭合，单根七逻辑八处取样/byte门禁和两Rand后不同借用序；其它所有CPP/头、布局/正确依赖及列表/随机消费保持。E353→357仅YOBI/BATTLE四局部I/S，旧812追加无新原身份/分类/完整成员。

七任务open及60已闭10、所有计数/R7和完整门禁保持；完整其余消息目标/列表消费/资源owner与SDK/SEH/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续。获准文本汇总不代运行或降低标准。

## BMJ01 六状态目标局部实现/静态证据（2026-10-06）

[验收标准](cg2d_battle_m_status_sampling_spec.md#battle-m-status-sampling-acceptance)：九区间643条2575B十四页/89call与本组原跳转闭合、四逻辑七处/唯一接线注释，三门禁/技能等级/两flags W8与flags→target取样有限静态。原live mode/旧payload与当前槽、Power1/3/Queued任意非零/列表/RID/58及所有其它源/头/布局核心保持，E357→361仅YOBI/BATTLE四局部I/S。

旧812追加不新原身份/分类/完整成员，七open任务与60项已闭10/R7/全部计数和完整门禁保持。完整其它消息/技能/列表消费与资源owner/SDK/SEH/ABI/PC53/时钟RNG/画面以及R1/R7/R8/整体继续；文本汇总不代运行或降低标准。

## BMK01 两目标局部实现/静态证据（2026-10-06）

[验收标准](cg2d_battle_m_position_monster_sampling_spec.md#battle-m-position-monster-sampling-acceptance)：五区间647条2787B十二页61call、本组原跳转及三个共用尾，两根九处指定取样差异/其它源头核心保持有限静态；原旧槽/重复附件/失败完成/工厂先pub与固定payload/四载体/正常W8-W16-W32及列表/58保持。E361→365仅YOBI/BATTLE四局部I/S。

旧812仅追加、七open任务/60项已闭10/R7及完整门禁保持；完整其余消息/技能/列表消费者、资源owner/SDK/SEH/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，文本汇总不代运行或降低标准。

## BML01 骑乘/群体目标局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_ride_gmn_sampling_spec.md#battle-m-ride-gmn-sampling-acceptance)：五区间1198条4711B十八页170call、本组原跳转与三个共用尾，九组十一补丁定义十二处/原三W8和取样及其它源头正确核心保持有限静态。正确401EA0一条5B尾跳、入口I8零初始化沿既有合同，不混父分母或造差异；E365→369仅YOBI/BATTLE四局部I/S。

旧812仅追加，不新分类/完整成员，七open/60项已闭10/R7与所有完整门禁保持。完整其余消息/技能/全部列表消费、资源owner/SDK/SEH/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续；文本汇总不代运行或降低标准。

## BMM01 道具/连击局部实现/静态证据（2026-10-06）

[验收标准](cg2d_battle_m_item_combo_sampling_spec.md#battle-m-item-combo-sampling-acceptance)：五区间968条3545B十五页119call、三个共用尾和实际四态四W32/16B、原直接与间接分派目标闭合，二十四组二十七处/两W8/两S32索引、调用后live读取/末target寿命及其它源头核心保持有限静态。E369→373仅YOBI/BATTLE四局部I/S。

旧812仅追加、不新分类/完整成员，七open任务/60项已闭10/R7及全部完整门禁保持；24非技能头有界基线不代完整技能0/SEH/全部消费者与资源owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体，文本汇总不代运行或降低标准。

## BMN01 技能入口/等待态局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill_entry_waiting_sampling_spec.md#battle-m-skill-entry-waiting-sampling-acceptance)：五区间490条1894B十页35call、三个Caster尾及五压缩表512B按真实元素/长度与映射核，所有直接/等待态表目标闭合，42AE37明确后续主体分派边界；三根四组七定义八处、唯一private参数/一次58、四W8与两条件pub时序/原消费和其它源头核心保持有限静态。E373→377仅YOBI/BATTLE四局部I/S。

旧812仅追加、不新分类/完整成员；七open任务/60项已闭10/R7及全部完整门禁保持。正常各技能主体/完整17298父与SEH/全部消费者和资源owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体按实际缺口继续，文本汇总不代运行或降低标准。

## BMO01 技能4局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill4_sampling_spec.md#battle-m-skill4-sampling-acceptance)：正常主体256条894B四页26call/所有直接目标、入口低段202B表实际映射case4及八原零写生产者，整CPP单根九组九处/两S32索引/两效果参数与条件pub、原消费和其它源头核心保持有限静态。E377→381仅YOBI/BATTLE四局部I/S。

旧812仅追加，不新分类/完整成员；七open/60项已闭10/R7及所有完整门禁保持。其它各技能主体/全路由、完整17298父与SEH/全部消费者与资源owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，文本汇总不代运行或降低标准。

## BMP01 四技能共用主体局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill61_sampling_spec.md#battle-m-skill61-sampling-acceptance)：566条2220B八页56call/直接目标闭合、202B入口四case真实定位和原十二S32偏移48B，整CPP单根十九处、83中点/字段与参数/累计pub及一W8、原反向响应消费和其它源头核心保持有限静态。E381→385仅YOBI/BATTLE四局部I/S。

旧812仅追加，不新分类/完整成员；七open/60项已闭10/R7及所有完整门禁保持。其它各技能/全部路由/完整父与SEH、所有消费者和资源owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，文本汇总不代运行或降低标准。

## BMQ01 五技能共用主体局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill50_sampling_spec.md#battle-m-skill50-sampling-acceptance)：103条423B两页13call/直接目标闭合、202B表五case真实定位及原48B偏移，整CPP单根五处/空比较pub、两字段与自身效果/图号取样、原三Take/Move-增58无RID及其它源头核心保持有限静态。E385→389仅YOBI/BATTLE四局部I/S。

旧812仅追加，不新分类/完整成员；七open/60项已闭10/R7及所有完整门禁保持。其它各技能/全路由/完整父-SEH、所有消费者与资源owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，文本汇总不代运行或降低标准。

## BMT01 三技能四根局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill12_82_97_sampling_spec.md#battle-m-skill12-82-97-sampling-acceptance)：749条2827B十一页78call/三范围直接目标闭合、两个原循环及唯一共用尾、202B表三case定位/原48B偏移与八W32初值，整CPP四根二十二处及不同图号序/82交错坐标/主辅参数-S32索引/字段-player-kind、原各不同消费和其它源头核心保持有限静态。E389→393仅YOBI/BATTLE四局部I/S。

旧812仅追加，不新分类/完整成员；七open/60项已闭10/R7及所有完整门禁保持。其它各技能/全路由/完整父-SEH、所有消费者与资源owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，文本汇总不代运行或降低标准。

## BMU01 技能93局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill93_sampling_spec.md#battle-m-skill93-sampling-acceptance)：241条1085B四页39call/主体与真实尾直接目标闭合、202B入口case93，整CPP单根五处/比较pub与字段-Album后参数、工厂前后两target/固定created-payload及十整数-29字符串-四内存载体-W16零/坐标live/无RID尾和其它源头核心保持有限静态。E393→397仅YOBI/BATTLE四局部I/S。

旧812仅追加，不新分类/完整成员；七open/60项已闭10/R7及全部完整门禁保持。98及其它各技能/全路由/完整父-SEH、所有消费者与资源owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，文本汇总不代工厂owner或其它运行验收，不降低标准。

## BMV01 技能98准备局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill98_prepare_sampling_spec.md#battle-m-skill98-prepare-sampling-acceptance)：86条362B两页11call/准备范围直接目标闭合，202B入口case98与48B原偏移上下文；整CPP单根五处、同位置caster/字段-图偏移原序、三Take/空Finish false和六动作后true，无目标/RID/58新消费及其它根/头保持有限静态。E397→401仅YOBI/BATTLE四局部I/S。

旧812仅追加，不新分类/完整成员；七open/60项已闭10/R7及全部完整门禁保持。42D0D0起原比较链分派/各模式主体、其它技能/全路由/完整父-SEH、所有消费者和资源owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续；不降低完整98/整体验收标准。

## BMW01 技能98 mode6局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill98_mode6_sampling_spec.md#battle-m-skill98-mode6-sampling-acceptance)：三范围98条405B三页12call/全部直接目标闭合；26条114B原比较链及202B表仅入口。整CPP单根四处、目标比较pub/同位置空门禁-解析目标效果/两字段actor先值，单Take循环-五动作-原常数及255或空Finish均终结-Move-RID-增58，正确尾helper及其它正文/头保持有限静态。E401→405仅YOBI/BATTLE四局部I/S。

旧812仅追加，不新分类/完整成员；七open/60项已闭10/R7及全部完整门禁保持。其它mode/完整98、其它技能-全路由/完整父-SEH/所有消费者-owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，不降低标准。

## BMX01 技能98 mode7/8局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill98_mode7_8_sampling_spec.md#battle-m-skill98-mode7-8-sampling-acceptance)：五范围341条1410B七页23call/全部直接目标闭合，两个选择比较五条22B与202B表仅入口；整CPP两根十八处、目标/位置/八坐标-三附件路径与mode8低W8/同位置Reset、原固定与live/非对称门禁-含null槽交换及255或空Finish后的RID状态有限静态。正确尾helper/其它正文和头保持，BMW两尾22条82B与当前证据一致不称新增；E405→409仅YOBI/BATTLE四局部I/S。

旧812有界追加，七open/60项已闭10/R7及全部完整门禁保持。其它98模式/技能-全路由/完整父-SEH/所有消费者-owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，不降低标准。

## BMY01 技能98 mode12/13/14局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill98_mode12_13_14_sampling_spec.md#battle-m-skill98-mode12-13-14-sampling-acceptance)：五范围681条2625B十二页72call/全部直接目标闭合，六条27B选择比较/202B入口和八W32正常零只作上下文。整CPP三根32处、中点-效果字段-S32选择/player同位置及0/149条件重读、14双金额类型/位置原序，12/13初始直接返回和循环selected终结74、14无空门禁且双哨兵caster终结/原Move-RID-增58有限静态；正确两尾helper/其它根头保持，BMW两尾19条73B非新增。E409→413仅YOBI/BATTLE四局部I/S。

旧812有界追加，七open/60项已闭10/R7及全部完整门禁保持。其它模式/完整98-技能-全路由/完整父-SEH/所有消费者-owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，不降低标准。

## BMZ01 技能98 mode15/16局部实现与静态证据（2026-10-06）

[验收标准](cg2d_battle_m_skill98_mode15_16_sampling_spec.md#battle-m-skill98-mode15-16-sampling-acceptance)：五范围363条1280B九页35call直接目标闭合、两主体与不同参数尾都核；四条18B比较/BMO八W32正常零与202B仅上下文。整CPP单根五处/target-effect-aux-S32选择/player原序，真实6/3与无空门禁、原flags条件重读/动作-金额/图值跨目标及同caster终结Move-RID-增58有限静态；其它根头和正确尾helper保持，BMX尾18条67B非新增。E413→417仅YOBI/BATTLE四局部I/S。

旧812有界追加，七open/60项已闭10/R7与完整门禁保持。状态目标/17-默认及完整98-其它技能-全路由/完整父-SEH/所有消费者-owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，不降低标准。

## BNA01 技能98原区段指令闭合与局部门禁（2026-10-06）

[验收标准](cg2d_battle_m_skill98_mode17_status_sampling_spec.md#battle-m-skill98-mode17-status-sampling-acceptance)：本步306条1163B八页31call/直接目标及最后比较默认EB0A，整CPP两根11处/17 fixed flags与状态caster先pub-S32选择/字段和原动态mode、正确wrapper/默认/全部其它根头保持有限静态。五既有证据与HEAD一致、本步与原捕获一致，原98区段1833条7116B/170call按EA去重连续且重叠字节一致；既有26条97B非新增。E417→421仅YOBI/BATTLE四局部I/S。

区段覆盖不代完整父-SEH/SDK/ABI-所有消费者与资源owner/PC53/时钟RNG/画面运行，七open/60项已闭10/R7及所有完整分类-成员-门禁保持；其它技能真实缺口与R1/R7/R8/整体继续，不降低标准。

## BNB01 技能99族局部I/S（2026-10-06）

[验收合同](cg2d_battle_m_skill99_family_sampling_spec.md#battle-m-skill99-family-sampling-acceptance)限定单根14定义15处落码和[400条实际正文/原表上下文](cg2d_battle_m_skill99_family_sampling_evidence.json)。E421→425只增加YOBI/BATTLE四局部I/S；七任务open和完整门禁/完整成员保持。全部其它指标、分类与原基线不变，不以固定payload/具体主体和获准文本汇总替代完整父/SDK-ABI/owner/运行或R1/R7/R8。

## BNC01 技能107局部I/S（2026-10-06）

[验收合同](cg2d_battle_m_skill107_sampling_spec.md#battle-m-skill107-sampling-acceptance)限定单根五定义五处落码和[93条实际正文/原202B表上下文](cg2d_battle_m_skill107_sampling_evidence.json)。E425→429仅YOBI/BATTLE四局部I/S；七任务open和全部完整门禁/完整成员及其它指标分类保持。完整父SEH/全部消费者-owner/SDK-ABI-PC53-时钟RNG画面与R1/R7/R8不因局部证据或文本汇总退出0变pass。

## BND01 技能108局部I/S（2026-10-06）

[验收合同](cg2d_battle_m_skill108_sampling_spec.md#battle-m-skill108-sampling-acceptance)限定单根十处落码和[239条原正文/真实尾](cg2d_battle_m_skill108_sampling_evidence.json)。E429→433仅YOBI/BATTLE四局部I/S；七任务open和全部完整成员/门禁及其它指标分类保持。两原正常分支/共享图号与真实尾有界证明，不代完整父SEH/全部消费者-owner/SDK-ABI-PC53-时钟RNG画面运行或R1/R7/R8。

## BNE01 技能109局部I/S（2026-10-06）

[验收合同](cg2d_battle_m_skill109_sampling_spec.md#battle-m-skill109-sampling-acceptance)限定单根九处落码和[251条原正文/真实Move尾](cg2d_battle_m_skill109_sampling_evidence.json)。E433→437仅YOBI/BATTLE四局部I/S；七任务open、全部完整成员/门禁和其它指标分类保持。三级原固定W32flags与真实尾有界证明，不代完整父SEH/全部消费者-owner/SDK-ABI-PC53-时钟RNG画面运行或R1/R7/R8。

## BNF01 技能112局部I/S（2026-10-06）

[验收合同](cg2d_battle_m_skill112_sampling_spec.md#battle-m-skill112-sampling-acceptance)限定单根九处落码和[226条原正文/真实Move尾](cg2d_battle_m_skill112_sampling_evidence.json)。E437→441仅YOBI/BATTLE四局部I/S；七任务open、全部完整成员/门禁及其它指标分类保持。原flags发布/一次W32-AH选择与两个位置切换有界证明，不代完整父SEH/全部消费者-owner/SDK-ABI-PC53-时钟RNG画面运行或R1/R7/R8。

## BNG01 技能114局部I/S（2026-10-06）

[验收合同](cg2d_battle_m_skill114_sampling_spec.md#battle-m-skill114-sampling-acceptance)限定单根八处落码和[166条原正文/真实caster终结尾](cg2d_battle_m_skill114_sampling_evidence.json)。E441→445仅YOBI/BATTLE四局部I/S；七任务open、全部完整成员/门禁和其它指标分类保持。具体图号/flags发布-成功字段和真实尾有界证明，不代完整父SEH/所有消费者-owner/SDK-ABI-PC53-时钟RNG画面运行或R1/R7/R8。

## BNH01 技能117局部I/S（2026-10-06）

[验收合同](cg2d_battle_m_skill117_sampling_spec.md#battle-m-skill117-sampling-acceptance)限定单根七处落码和[146条原正文/真实Move后缀](cg2d_battle_m_skill117_sampling_evidence.json)。E445→449仅YOBI/BATTLE四局部I/S；六组过期selected_case字段删除不追加通过证据。七任务open、全部完整成员/门禁和其它指标分类保持。具体flags发布/两结果和真实后缀有界证明，不代完整父SEH/所有消费者-owner/SDK-ABI-PC53-时钟RNG画面运行或R1/R7/R8。

## BNI01 技能73非零局部I/S（2026-10-06）

[验收合同](cg2d_battle_m_skill73_sampling_spec.md#battle-m-skill73-sampling-acceptance)限定非零单根六处落码和[305条原区段/分列上下文](cg2d_battle_m_skill73_sampling_evidence.json)。E449→453仅YOBI/BATTLE四局部I/S；七任务open、全部完整成员/门禁及其它指标分类保持。两Effect具体参数取样、两S32索引与原位置/flags门禁-动作及RID尾有界证明，不代73零模式完整消费者/全case/完整父SEH-所有owner/SDK-ABI-PC53-时钟RNG画面运行或R1/R7/R8。

## BNJ01 技能106族局部I/S（2026-10-06）

[验收合同](cg2d_battle_m_skill106_family_sampling_spec.md#battle-m-skill106-family-sampling-acceptance)限定单根八处落码与[409条原主体/Move-RID尾](cg2d_battle_m_skill106_family_sampling_evidence.json)。E453→457仅YOBI/BATTLE四局部I/S；七任务open、全部完整成员/门禁及其它指标分类保持。两Effect取样/双S32索引和原2002额外取包/29容量/live门禁-原初末动作有界证明，不代完整父SEH-所有consumer-owner/SDK-ABI-PC53-时钟RNG画面运行或R1/R7/R8。

## BNK01 公共攻击三父局部I/W/S（2026-10-06）

[验收合同](cg2d_battle_m_attack_targets_call_sampling_spec.md#battle-m-attack-targets-call-sampling-acceptance)限定两根四处/三参数差异落码和[80条三父/前缀](cg2d_battle_m_attack_targets_call_sampling_evidence.json)、真实core/四输出与value角色Move-RID/两种58后继静态接线。E457→463仅YOBI/BATTLE六局部I/W/S；七open和P01源码closed状态不变、全部完整成员/门禁及其它指标保持。P01已有3089本体基线保持，不代编译链接、全部consumer-owner/SDK-ABI-PC53-时钟RNG画面、完整父或R1/R7/R8。

## BNL01 普通非零局部I/S（2026-10-06）

[验收合同](cg2d_battle_m_attack_nonzero_sampling_spec.md#battle-m-attack-nonzero-sampling-acceptance)限定单根23定义24处落码和[985条完整正常区段/真实RID尾](cg2d_battle_m_attack_nonzero_sampling_evidence.json)。E463→467仅YOBI/BATTLE四局部I/S，七open、P01源码closed及全部完整成员/门禁和其它指标保持。坐标/Effect取样-原固定amount与末W8、四S32索引及原图分工和未指定局部边界有界证明，不代完整父SEH-所有consumer-owner/SDK-ABI-PC53-时钟RNG画面或R1/R7/R8。

## BNM01 技能263、400、401局部开发验收（2026-10-06）

依[具体验收](cg2d_battle_m_skill263_400_401_sampling_spec.md#battle-m-skill263-400-401-sampling-acceptance)登记三根23定义23处正常取样实修：[536条2090B八页66call](cg2d_battle_m_skill263_400_401_sampling_evidence.json)连续实际字节/直接出口与23组EA，本组180B原路由/既有48B图表只作上下文。I核整CPP限定重放/关联声明和分派；S核原连续段及263同参Effect、400Take固定flags/条件Cast0重读与401两字段后live flags，各低/高图及初空-循环两出口/原尾-固定58=1或2无RID保持。

E467→471仅YOBI/BATTLE四局部I/S。旧812五引用/三symbol/本步对象、双域基线/阻碍与七open任务同步；不新F/X/adapted/分类/完整成员。语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed保持。完整父SEH/所有consumer-owner/其它技能-全路由/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续，不以本组三根-536条或静态汇总代整体。不构建/测试/运行/摘要；网络原格式明文，音频暂停。

## BNN01 技能402、405局部开发验收（2026-10-06）

依[具体验收](cg2d_battle_m_skill402_405_sampling_spec.md#battle-m-skill402-405-sampling-acceptance)登记两根20定义20处：[342条1303B五页44call](cg2d_battle_m_skill402_405_sampling_evidence.json)连续实际字节/直接出口与20组EA，既有180B/48B图表具名上下文。I核整CPP限定重放/关联声明分派，S核402第三Take后live caster/图号-遍历效果与比较先pub、405两图不同次序/五效果各次取样-两S32索引、原取包-转换/动作和各初空-循环出口/真实尾-402固定58=2及405固定58=1无RID保持。

E471→475仅双域四局部I/S；旧812五引用/两symbol/本步对象、双域基线阻碍及七open任务同步。语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed及完整成员/门禁保持，不新F/X/adapted/分类。完整父SEH/其它技能-全路由/所有consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续，不以两入口/342条或静态汇总代整体。不构建/测试/运行/摘要，网络原格式明文/音频暂停。

## BNO01 技能404族局部开发验收（2026-10-06）

按[具体验收](cg2d_battle_m_skill404_family_sampling_spec.md#battle-m-skill404-family-sampling-acceptance)登记单根21组23定义23处：[主1286条4898B十七页147call](cg2d_battle_m_skill404_family_sampling_evidence.json)连续实际字节/全部出口与23组原EA，原406三节点18B直跳和404既有表/BMO业务零上下文。I核限定整CPP重放/关联声明分派；S核预读caster缓存、两段各八分支坐标/固定100C-初Effect/rider取样、普通Effect/Aux和四S32索引-比较、最终恢复/尾Y-X及两W8-原W32门禁和原子角色/消费-字段/动作/真实Move-RID固定58=1保持，不补未指定局部。

E475→479仅双域四局部I/S；旧812五引用/一symbol/本步对象、双域基线阻碍与七open任务同步，不新F/X/adapted/分类/完整成员。语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed及全部完整门禁保持。完整父SEH/其它技能-全路由/全部consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续，不以本根/子角色生产/静态汇总代完整功能。网络原格式明文/音频暂停，不构建/测试/运行/摘要。

## BNP01 2028与2005族局部开发验收（2026-10-06）

按[具体验收](cg2d_battle_m_skill2028_2005_sampling_spec.md#battle-m-skill2028-2005-sampling-acceptance)登记两根24定义24处。两根24处恢复同parsed caster/target、payload先skill/actor先level、2028低高图的角色/payload-表各次取样、累计总量先target/固定flags与实际W8响应、反射字段/总量和最终AEL；2005族Effect/Aux-两S32索引/target先比较。原全部取包/字段-list-count、固定响应优先和149后双位置重读、动作/各失败-2028固定58=2与2005增一的真实Move/RID尾保持，其它根头不改。[证据](cg2d_battle_m_skill2028_2005_sampling_evidence.json)：四范围713条2702B十一页72call连续字节/全部出口、24补丁/改前两根与旧812完整前态；BMP79条348B/9call+BMT7条26B/2call共86条374B/11call复用非新增，新627条2328B/61call分列。2028五S32/20B图表、实际56B表和4003顺序入口/2005既有表仅本组定位，不代全路由。I核整CPP限定重放/真实分派和关联声明；S核24组EA、各图/累计与固定响应-W8/反馈尾、Effect/Aux/两S32-比较、各失败与不同状态尾。

E479→483仅双域四局部I/S；旧812五引用/两symbol/本步对象、双域基线/阻碍与七open同步。语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed及所有完整门禁保持，不新F/X/adapted/分类/完整成员。完整父SEH/其它技能-全路由/全部consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续，不以本组/累计反弹或静态汇总代整体。不构建/测试/运行/摘要，网络原格式明文/音频暂停。

## BNQ01 技能32族局部开发验收（2026-10-06）

按[具体验收](cg2d_battle_m_skill32_family_sampling_spec.md#battle-m-skill32-family-sampling-acceptance)登记单根13定义14处。单根13定义14处恢复同parsed caster/target、payload先skill/actor先level、三初图各取样、附加效果target-level-skill/固定flags、基础字段与两成功skill actor先值、一S32分档行和八分档图按表先actor或payload先表。原状态类型/成功门禁-比较、固定payload跨替换动作、正确debuff默认0与未指定statusType、所有消费-字段/动作和各失败/End-Move-RID增58保持，其它根头/图表不改。[证据](cg2d_battle_m_skill32_family_sampling_evidence.json)：1065条4656B十四页76call/全部直接和十二表间接出口，十二表117W32槽468B+93W8索引=561B、169S32/676B图矩阵与旧BMP48B偏移/具名入口上下文，13补丁/改前根和旧812完整前态保存。I核整CPP限定重放/真实分派和关联声明；S核十四页和全部出口、内部561B表/676B图数据、三初图/八分档图/附加效果与字段-一S32行、正确状态比较/载荷跨替换动作及真实尾保持。

E483→487仅双域四局部I/S；旧812五引用/本步对象和既有唯一symbol、双域基线/阻碍及七open任务同步。语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed及所有完整门禁保持，不新F/X/adapted/分类/完整成员。完整父SEH/其它技能-全路由/全部consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续，不以本根-状态生产或静态汇总代整体。不构建/测试/运行/摘要，网络原格式明文/音频暂停。

## BNR01 技能19族局部开发验收（2026-10-06）

按[具体验收](cg2d_battle_m_skill19_family_sampling_spec.md#battle-m-skill19-family-sampling-acceptance)登记26定义27处。单根26定义27处恢复同parsed角色、初低图位置-表-加法、累计先target后发布、普通Effect/Aux与两AEL、半额反馈/反弹字段-copy和末效果取样，四S32业务索引及五处实际W8门禁。原entry0/1比较、业务零/未指定lastAmount、全部图号-金额门禁/字段/动作与真实Move-RID固定58=2保持，其它根头不改。[证据](cg2d_battle_m_skill19_family_sampling_evidence.json)：43412E..434EDF914条3505B十二页67call/全部直接和三个内部表间接出口，三表121B、3037入口36B及原48B偏移/具名低中入口只上下文，26补丁/改前根及旧812完整前态保存。I核整CPP精确重放/header和关联声明保持；S核完整十二页/出口、三表121B-3037入口36B和27处原EA、四S32索引/五W8门禁及真实尾。

E487→491仅YOBI/BATTLE四局部I/S；旧812追加、七open与唯一进度同步。其它指标及全部完整成员/门禁不变，完整父/全部消费者-owner、SDK/ABI/PC53/时钟RNG画面及R1/R7/R8继续，不以本根-静态汇总代整体。不构建/测试/运行/摘要，网络原格式明文/音频暂停。

## BNS01 技能267局部开发验收（2026-10-06）

按[具体SPEC](cg2d_battle_m_skill267_sampling_spec.md#battle-m-skill267-sampling-decision)，单根25定义27处恢复角色字段、effectCount/初图、累计发布-普通与循环/末Effect-Aux参数、图copy/反馈-state取样；四S32业务索引和两W8门禁，原全部图号-字段/金额门禁/动作与真实Move-RID固定58=1保持，其它根头不改。[证据](cg2d_battle_m_skill267_sampling_evidence.json)：434EDF..43573F576条2144B八页45call及全部直接出口，无内部表/复用尾；267/3029原180B/36B入口表和十二S32/48B偏移仅上下文，25补丁/改前根与旧812完整前态保存。循环累计source先清flags pub，末Effect沿固定flags/Aux另live flags；入口两个W32业务零和未指定lastAmount、所有原调用/门禁-状态动作保持。

旧812只五引用/一既有源码symbol登记/本步对象，E491→495仅双域四局部I/S，七open及全部其它指标/完整成员-门禁保持。完整父17298/SEH、其它技能/全路由、全部consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8/运行另验。

## BNT01 独立技能目标局部开发验收（2026-10-06）

依[具体SPEC](cg2d_battle_skill_targets_core_spec.md#battle-skill-targets-core-decision)，单根四处恢复target255比较先发布、普通Effect/Aux的target输出先source输出，以及redirect输出发布后的参数原序；十参/四native非拥有S32输出、原每轮八业务零/最后零values扫描、两图表-固定flags/value及所有门禁/两返回保持。[证据](cg2d_battle_skill_targets_core_evidence.json)：独立426FC0完整369条1185B五页32call/全部直接出口和两ret，十三条31B选择叶/两40B表及唯一4369CC caller26条94B上下文分别计数；四补丁和改前根/独立ID前态保存。初initial/source同GetPos返回依序写；正常255只sourceEnd/74、初/target空只Finish。caller原十实参和返回后重读initial-Move/RID固定1保持，其它根/header不改。

原ID已在库存，新增独立语义记录1350/adapted分类与FUNC-BATTLE显式成员52→53，旧812整记录保持。语义1351/有效ID1346、分类457、文件关联1464/3680=39.78%，E495→497仅BATTLE两局部I/S；175定位队列不变（此前已有历史定义）、七open/60源码closed10及全部完整门禁保持。完整战斗/父SEH、全部consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8/运行仍待。

## BNU01 两独立正确源合同的局部I/S（2026-10-06）

[具体SPEC](cg2d_battle_redirect_aux_baseline_spec.md#battle-redirect-aux-baseline-decision)：两既有正确根保持，新增三方转向与辅助效果的独立原ID/基线、语义映射、adapted分类和战斗成员。[证据](cg2d_battle_redirect_aux_baseline_evidence.json)：229条693B四页31call/两ret和一条件出口：转向180条28call/24Cast-四EffectList-三字段及S32返回，辅助49条三Effect/fixed flags800000-payload/CA0-1跨两Append-旧四字段读取；五转向/37辅助入向与返回消费窗口分列。source/redirect/target的完整序列/每次actor取样保持，不合并别名；三W32字段各写后另取actor，辅助第三记录沿旧槽position4-flags-position3-position，非输入flags替代。既有源码/头和caller不改，BQA/BQR已过未变生产叶不复验。

新增语义1351/1352绑定原F:000242B0/F:0003BC10，战斗成员53→55但完整枚举仍open；语义1353/有效ID1348、分类459、关联1466/3680=39.84%、E497→501仅各函数I/S四条。两ID已有历史定义、不在原175队列，不虚减队列；所有旧语义/其它域成员、60源码closed10/七open和完整门禁保持。完整父/所有consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8/运行另验。

## BNV01 创建取样修正的局部I/S（2026-10-06）

[具体SPEC](cg2d_battle_effect_creation_sampling_spec.md#battle-effect-creation-sampling-decision)和[证据](cg2d_battle_effect_creation_sampling_evidence.json)：两个完整创建函数125条412B三页九call/六分支/四ret。随机与位置flags各恢复一次取样两次发布，位置X取样先image/direction、截断后取Y再发布X，三个补丁/两个根解决；两次RNG/Load与live S16图宽、graphicOffsetY、原失败返回及S32/F32/W32/W16/W8字段保持。其它源/头和caller不改；原__ftol沿CPH01 host裁决。入向80/28父和13/9父仅列身份，不代全部父或owner运行。

新增独立语义1353/F:00019FC0、1354/F:0001E680和adapted分类，FUNC-BATTLE55→57仍未全枚举；语义1355/有效ID1350、分类461、关联1468/3680=39.89%、E505仅各函数两局部I/S。两ID原有历史定义，队列175保持。全部旧语义/其它功能、七open任务与所有完整门禁保持；完整consumer-owner/资源重进退出、SDK/ABI/PC53/实际RNG画面及R1/R7/R8另验。

## BNW01 回调取样的局部I/S（2026-10-06）

[具体SPEC](cg2d_battle_effect_callbacks_sampling_spec.md#battle-effect-callbacks-sampling-decision)与[证据](cg2d_battle_effect_callbacks_sampling_evidence.json)：两callback/三创建入口完整168条576B五页17call/14分支/八ret。位置callback恢复X截断→Y取样→X发布→Y截断发布，严格PlayFrame(1)==1才MarkDeath，标死和未完成都继续投影；MarkDeath仅标死不立即free/摘链。其它四正确根保持：坐标/轨迹两创建绑定position callback、屏幕创建绑定one-shot callback；轨迹RNG仅无设备路径/结果后取flags，mode只1加20000后另live flags，低W8优先级/固定输入坐标保。原__ftol沿CPH01 host，六callback绑定和11直接创建call分别列身份，非所有父已验。

新增语义1355..1359五原ID/分类、BATTLE57→62但枚举open；语义1360/有效ID1355、分类466、关联1473/3680=40.03%、E515仅五函数各I/S。位置回调一块问题已解决，四既有正确源不计新增；原有历史定义/队列175及所有旧语义-其它域/七open-完整门禁保持。全部consumer-owner/资源重进退出、SDK/ABI/PC53-RNG画面及R1/R7/R8继续。

## BNX01 特效精度与RNG合同的局部I/S（2026-10-06）

[具体SPEC](cg2d_battle_rising_emitter_fade_sampling_spec.md#battle-rising-emitter-fade-sampling-decision)和[证据](cg2d_battle_rising_emitter_fade_sampling_evidence.json)：七完整340条1215B八页19call/19普通内部出口-14ret、背景五槽switch20B/两16项S32局部表128B。上升callback一处恢复X截断后Y取样先X发布，原未舍入speed/Y中间值、F32存储/父deathFlag严格1早退与低于phase标死后仍Play(0)保。其它六正确源保持：创建先RNG再分配、无设备可二次RNG；屏幕发射先Y随机再X、创建后另live phase--；背景0贯穿1可两次发布/phase4而本次不标死，2归零贯穿3/4置flags2后标死。原业务0/4/8/255/失败和字段宽度、packed颜色高W8访问器保24位及所有caller/声明保持，全部父/ABI-PC53-RNG画面不外推。

七独立原ID纳入语义1360..1366/分类、BATTLE62→69仍枚举open；语义1367/有效ID1362、分类473、关联1480/3680=40.22%、E529各函数I/S共14E。五callback绑定/七普通入向分列，历史定义已有/队列175保持；其它旧记录/功能域、七open/所有完整门禁与验收0保持。实际取样问题已解决，完整consumer-owner/资源重进退出、SDK-ABI-PC53-RNG画面及R1/R7/R8继续。

## BNY01 上游生成取样局部I/S（2026-10-06）

[具体SPEC](cg2d_battle_rising_emitters_sampling_spec.md#battle-rising-emitters-sampling-decision)与[证据](cg2d_battle_rising_emitters_sampling_evidence.json)：两完整258条1057B四页七call/20内部分支-两ret，四原局部数组64S32项256B对应三源表48项192B，两个Y表33/-33与44/-44差别保。随机emitter恢复先取X偏移/live X并求S32和、再取Y偏移/发布F32 X；其它所有源/caller/声明保持。随机oldPhase<50/新phase40才outer flags2、先RNG再分配/无设备另RNG，索引oldPhase>0且nameX<32/失败也nameX++、三次live signed%16、两个live Y保持。particle父是当前emitter，field270清当前emitter；50/32完成计数门禁后fresh outer parent/1024非零才写2再标死。两个41CD00 callback绑定只列身份，不代父全文/全部owner或运行。

新增语义1367/F:0001A310、1368/F:0001A7D0/分类与BATTLE69→71仍未枚举闭合；语义1369/有效ID1364、分类475、关联1482/3680=40.27%、E533仅各函数I/S共四E。随机取样问题已解决，索引正确源不计开发新增；历史定义已有/队列175、所有旧语义-其它域/七open及完整门禁保持。完整上游/consumer-owner/资源重进退出、SDK-ABI-PC53-RNG画面及R1/R7/R8继续。

## BNZ01 完整创建基线的局部I/S（2026-10-06）

[具体SPEC](cg2d_battle_effect_factory_sampling_spec.md#battle-effect-factory-sampling-decision)及[证据](cg2d_battle_effect_factory_sampling_evidence.json)：共同创建完整1583条5825B20页66call/267普通转移21ret、四switch/分派263B+层级48B+scale4B共315B。17补丁18段含六scope和11取样定义12段：原flags先callback/priority、type13/35同次flags两发布、skill400 flags先新payload，以及35两个effect固定index→flags/position4/position3/position四独立值传16B记录/Append后另live flags。原所有type图号/递归19/RNG/Init与第三输入type>47既有action、共同blend-link-X/Y与两个真实payload判空保持。34/43前置death写和音频二参既有unused第三栈字合同保，不恢复暂停音频或新保护。

新增语义1369/F:0001CD00、分类与BATTLE71→72仍未全枚举；语义1370/有效ID1365、分类476、关联1483/3680=40.30%、E535仅本根两I/S。25入向/20父只分列身份，原历史定义已有/队列175不减；所有旧记录/其它域-七open/完整门禁保持。已发现源码取样差异已解决，全部上游/callee-owner/资源重进退出、SDK-ABI-PC53-RNG画面与R1/R7/R8继续。

## BOA01 三个坐标取样的局部I/S（2026-10-06）

[具体SPEC](cg2d_battle_completion_scale_sampling_spec.md#battle-completion-scale-sampling-decision)/[证据](cg2d_battle_completion_scale_sampling_evidence.json)：三完整97条306B三页11call/十内部出口三ret、step/1/0原常量12B。三末坐标片段各恢复X截断→Y取样→Xpub→Y截断pub。完成严格Play(1)==1/parent非空-1024非零才写2，随后标死仍投影；父关联仅非空parent死亡严格1才标死、之后仍Play(0)/投影；缩放phase0/2原step加减、X存F32但用未舍入double判定，原业务phase1/X-Y1与X-Y0/phase3保持，无MarkDeath。13目标地址引用含EBP加载，不能当13实际callback写或全父-owner已验。

新增语义1370..1372/三ID分类、BATTLE72→75但枚举open；语义1373/有效ID1368、分类479、关联1486/3680=40.38%、E541仅各I/S共六E。三取样问题已解决，所有其它源/声明、旧记录-其它域/175队列/七open与完整门禁保持；全部父/callee-owner/资源重进退出-ABI-SDK-PC53-RNG画面及R1/R7/R8继续。

## BOB01 五初始化助手的取样与记录消费（2026-10-06）

[具体SPEC](cg2d_battle_initializers_sampling_spec.md#battle-initializers-sampling-decision)/[证据](cg2d_battle_initializers_sampling_evidence.json)：五完整608条1940B十页30call/35普通内部分支/五ret，Target switch60B与100/200原常量8B、六入向来自41CD00。五记录显式固定C9C：Target/Lower先position4再flags，另三根先flags再position4；随后position3/position传值，Append后仍live flags/Aux。Center游标先新payload，Offset live游标→targetX→position值次序一并恢复。五根真实action/source空门禁、ScreenY可空side输出、所有条件RNG/固定与fresh目标、原W8值4/W16业务零及DX S16高/对象S32偏移保持。

新增语义1373..1377/五ID分类、BATTLE75→80枚举仍open；语义1378/有效ID1373、分类484、关联1491/3680=40.52%、E551各局部I/S共十E。本步六补丁已解决取样问题；全部其它源/声明、旧记录-其它域/175队列/七open与完整门禁保持，全部父-callee-owner/资源重进退出-ABI-SDK-PC53-RNG画面及R1/R7/R8继续。

## BOC01 两命中回调的HP消费与取样（2026-10-06）

[具体SPEC](cg2d_battle_impact_callbacks_sampling_spec.md#battle-impact-callbacks-sampling-decision)/[证据](cg2d_battle_impact_callbacks_sampling_evidence.json)：两完整732条2647B十页57call/74普通内部分支/两ret，三callback地址引用来自41CD00。各五补丁恢复入口phase先payload、完成position/flags先查target、负HP临时encoded先查copyTarget，以及两数字分支flags→targetY→记录数值→targetX→S32 Y-80。保原负HP零临时/上限三字段复制、目标和载荷固定/live源游标重读、分类10000/40000/20000/default与scene计数、严格完成/标死继续、old58/FD0及偏移投影尾；通用CopyPayload/固定内存CryptInt/音频接口不改。

新增语义1378..1379/两ID分类、BATTLE80→82枚举仍open；语义1380/有效ID1375、分类486、关联1493/3680=40.57%、E555仅各局部I/S共四E。十取样问题本步已解决，所有其它源码/声明、旧记录-其它域/175队列/七open与完整门禁保持；全部父-callee-owner/资源重进退出-ABI-SDK-PC53-RNG画面及R1/R7/R8继续，网络原格式明文/音频暂停。

## BOD01 帧事件HP/视觉消费与消退（2026-10-06）

[具体SPEC](cg2d_battle_frame_event_callback_sampling_spec.md#battle-frame-event-callback-sampling-decision)/[证据](cg2d_battle_frame_event_callback_sampling_evidence.json)：完整471条1723B六页38call/56普通内部分支/一ret，两个callback地址引用来自41CD00。四补丁恢复phase先payload、负HP临时encoded先查copyTarget、style5/6数字targetX先Y-80；本根target先initialFlags/治疗金额先targetY原序保。旧事件>=10000消费，406260后fresh事件<20000且1038不严格1才实际HP/五分类，否则仅视觉/目标phase26；两分支FD0门禁不同，原HP上下限与20直接gate1/alive1保。old58/freshFD0严格零才Play(1)，图号范围标死后仍设备flags/nameY首次增，fresh旧fade可再次增/旧值>=20标死，仍偏移投影；通用复制/固定内存CryptInt/音频接口不改。

新增语义1380/单ID分类、BATTLE82→83枚举仍open；语义1381/有效ID1376、分类487、关联1494/3680=40.60%、E557仅局部I/S两E。本步四取样问题已解决，全部其它源/声明、旧记录-其它域/175队列/七open与完整门禁保持，全部父-callee-owner/资源重进退出-ABI-SDK-PC53-RNG画面及R1/R7/R8继续，网络原格式明文/音频暂停。

## BOE01 移动命中与标死后运动尾（2026-10-06）

[具体SPEC](cg2d_battle_moving_impact_callback_sampling_spec.md#battle-moving-impact-callback-sampling-decision)/[证据](cg2d_battle_moving_impact_callback_sampling_evidence.json)：完整636条2217B八页45call/78普通内部分支/一ret、原F32 50/0.5共8B，单callback地址引用来自41CD00。五补丁恢复phase先payload、首次图号先targetPayload、负HP临时encoded先查copyTarget、治疗Y-金额-X后Y-80、末优先级发布前F32 X取样；default X-金额-Y/三次live DX低S16半高原序保。特殊提前标死仍继续普通命中/HP四分类、延迟和运动；old58严格0启动音效262/RNG0..32次Play，正FD0减一-技能level震幅/fresh零门禁、实时中心F32转向/移动-speed+0.5、旧hit先速度发布/尾迹第三族额外nameX++-共同节拍/最终标死-rising0/原W8优先级+20与偏移投影保。

新增语义1381/单ID分类、BATTLE83→84枚举仍open；语义1382/有效ID1377、分类488、关联1495/3680=40.62%、E559局部I/S两E，百分比沿现Percent两位Round。五取样问题本步已解决，全部其它源/声明-边界、旧记录-其它域/175队列/七open与完整门禁保持，全部父-callee-owner/资源重进退出-ABI-SDK-PC53-RNG画面及R1/R7/R8继续，网络原格式明文/音频暂停。

## BOF01 追踪命中与出界后运动（2026-10-06）

[具体SPEC](cg2d_battle_homing_impact_callback_sampling_spec.md#battle-homing-impact-callback-sampling-decision)/[证据](cg2d_battle_homing_impact_callback_sampling_evidence.json)：完整676条2381B九页51call/83普通内部分支/一ret、原F32 0.1/-200/840/680常量16B，单callback地址引用来自41CD00。四补丁恢复phase先payload、负HP临时encoded先查copyTarget、style5金额-Y-X与style6 Y-金额-X后正常S32 Y-80。特殊flags20000000/frameEvent0-距离<=70全屏偏移/标死继续与fresh事件0/距离<=8普通命中独立，无406260；原HP上下限四分类-scene/default目标转向1-不同命中/启动音效映射保。old58/正FD0-fresh零才Play，未命中F32步长+0.1先发布再fresh目标转向，已命中出界标死仍Move/五族Y先X RNG-F32尾迹/110046额外nameX增与共同节拍/偏移投影保。

新增语义1382/单ID分类、BATTLE84→85枚举仍open；语义1383/有效ID1378、分类489、关联1496/3680=40.65%、E561局部I/S两E。本步四取样问题已解决，全部其它源/声明-复制边界、旧记录-其它域/175队列/七open与完整门禁保持，全部父-callee-owner/资源重进退出-ABI-SDK-PC53-RNG画面及R1/R7/R8继续，网络原格式明文/音频暂停。

## BOG01 真实运动/角度依赖的局部I/S（2026-10-06）

[基线](cg2d_battle_motion_angle_dependency_spec.md#battle-motion-angle-dependency-baseline)、[裁决](cg2d_battle_motion_angle_dependency_spec.md#battle-motion-angle-dependency-decision)、[局部验收](cg2d_battle_motion_angle_dependency_spec.md#battle-motion-angle-dependency-acceptance)。原证据完整九根441条1422B/11页/14call-60普通跳转（406356外尾跳__ftol独列）/17ret与一个八槽switch32B；219条入向引用仅作上下文。80B常量、原可写F64表3400B逐项静态对照，不换sin/atan2/fmod。对象转向恢复负向当前角-step/正向step+当前角/小余量difference+当前角；46F970恢复左表先权重、左乘后右表/右乘。两CPP两处还原，其余七正确根保，原空门禁与无空门禁、Y差先X/F32、角差单圈、S32截断幅度、F32发布后仅方向参数严格1、两次运动live角/速度、原八方向与重复归一、表index/fraction/八象限保。

九独立语义1383..1391/adapted分类，BATTLE85→94枚举仍open；语义1392/有效ID1387、分类498、关联1505/3680=40.90%、E579局部I/S18条。175待处置队列、其它域/历史、60源码closed10/完整验收0/P01及七open/完整门禁保持。表修改者/全部219入向父-callee-owner/生命周期/ABI-PC53-实际画面与R1/R7/R8继续；网络原格式明文/音频暂停，不构建/测试/运行/摘要。 每根单独I/S各一条、method=static，member_inventory_closed=false、gates不变；九根独立纳管不代全部父或共享表生产消费闭包，静态不推断旧画面有差异或PC53完全一致。

## BOH01 图标与附加消费的局部I/S（2026-10-06）

[原基线](cg2d_battle_flags_aux_icon_consumer_spec.md#battle-flags-aux-icon-consumer-baseline)、[裁决](cg2d_battle_flags_aux_icon_consumer_spec.md#battle-flags-aux-icon-consumer-decision)、[局部验收](cg2d_battle_flags_aux_icon_consumer_spec.md#battle-flags-aux-icon-consumer-acceptance)。完整五根286条952B/六页26call-22内部分支/六ret，无switch；175普通入向call与单41F4D8 callback地址绑定分列，F32 0.8/7.2常量8B。三个函数六还原：浮动入口phase一次样本、未舍入速度先比较后存F32、创建/更新X截断后Y先Xpub、audio后骑乘图号先fresh对象/Rider后gear先fresh对象。原Reset的S32载荷差-F32方向链、Aux固定payload后三Advance/四fresh游标输出与别名、Apply入口flags-position/五独立位顺序200000-400000-800000-1000000-2000000及2000对象分流保；Aux后末两块不刷新初记录。浮动parent仅本次坐标借用，创建失败空/Play后取坐标、未舍入速度推进Y、入口phase1达60标死仍投影保，既有二参数音效214/暂停保。

新增语义1392..1396/五adapted分类、BATTLE94→99枚举仍open；语义1397/有效ID1392、分类503、关联1510/3680=41.03%、E589局部I/S10条。其它历史/功能与175队列、R7区间67/3061、60源码closed10/完整验收0/P01、七open和完整门禁保持。全部176入向父-callee/共享表生产消费-owner/资源重进退出-ABI-PC53-实际画面与R1/R7/R8继续；网络原格式明文/音频暂停，不构建/测试/运行/摘要。 每根单独I/S各一条且method=static，原四输出别名与三Advance、fresh对象和图标父坐标借用/标死继续定向登记；不以六还原或完整286条代176入向全部父/运行，不宣静态已证明旧画面差异或宿主PC53一致。

## BOI01 字符创建与整数Y更新局部I/S（2026-10-06）

[基线](cg2d_battle_number_production_update_spec.md#battle-number-production-update-baseline)、[裁决](cg2d_battle_number_production_update_spec.md#battle-number-production-update-decision)、[局部验收](cg2d_battle_number_production_update_spec.md#battle-number-production-update-acceptance)。完整创建191条687B/其中原59格式接线历史保-补132条对象/资源布局差集，完整更新44条144B；共235条831B/四页15call-21普通内分支/两ret无switch，46call+两不同父callback地址分列，十行四列40图号与常量/字符串175B。两函数九还原：临时W8/W32/W32三值先取样再pub、S16graphicX先字体宽度、X/Y-flags-name样本发布序与两个name投影、更新入口phase/oldDelay单样本和未舍入speed先比较后F32pub。原未清buffer/%d-+/-顺序/font0测宽-固定strlen/逐字失败保已建对象-return0、S8两次读与map/XY资源调用不加门禁、图号表/正常F64-F32布局/flags2-OR40000/两重生W8-58=2index保。更新正旧延迟仅减一，零/负才清2-未舍入speed截断加S32 y/峰值后下次计数，50标死仍name投影，原无对象XY投影/PlayFrame或父跟随；不与浮动60帧/F32Y合并。

原语义1071指定续记/完整当前基线和独立历史格式副本，新增1397更新ID；两adapted分类/BATTLE99→101仍未全枚举、E589→593局部I/S四条。语义1398/有效ID1393、分类505、关联1511/3680=41.06%；全部旧格式事实/其它域/175队列-R7区间67/3061-60源码closed10/全验收0/P01及七open/完整门禁保持。第二4206C0数字构造者、23父/46调用点/测宽/图号-XY资源/字载荷-draw consumer与回收重入owner/ABI-PC53-画面及R1/R7/R8继续，网络原格式明文/内存CryptInt保/音频暂停，未构建/测试/运行/摘要。 续原记录不造新重复ID；两根各I/S一条且method=static，member_inventory_closed=false/gates保。原完整根与九还原不代资源/字符消费/所有caller-owner闭包、第二时间数字创建或运行，不宣旧画面差异/PC53完全一致。

## BOJ01 默认测宽冲突修正与旧Number I具名失效（2026-10-06）

[原基线](cg2d_battle_time_default_measure_spec.md#battle-time-default-measure-baseline)、[决议](cg2d_battle_time_default_measure_spec.md#battle-time-default-measure-decision)、[局部验收](cg2d_battle_time_default_measure_spec.md#battle-time-default-measure-acceptance)。原Time156条560B/旧50格式事实保持并补106条，Default22条50B正确体；两完整178条610B/三页11call-16内部分支/两ret无switch，四普通入向及三图号/格式-负号24B分列。发现两个数字父错接通用TextMeasure44B2A0，原41F242/420718均call44B260；四补丁恢复已有MeasureDefaultText44B260的非空入口缓存字体0双宽/单宽，并恢复Time encoded-mask-count三样本后W32/W32/W8 pub、S16 graphicX先S32→font2 live宽度→FILD/F64后宽度乘原index的次序。正确的Time X/Y/name pub/flags先B8、unsupported map0跳过仍计原index-间距/2index延迟/失败return0保原对象、借593850当前前缀及正常S32格式除余保；Default空串先返回0/高位字节前进2加固定双宽/其它进1加固定单宽、不严格解码保，通用44B2A0及正确已过更新体不改。

1072续原时间ID/保独立格式历史，1398 Default新ID，1071仅本次原call接线事实；两新增adapted分类/BATTLE101→103枚举open、六新局部I/S E593→599。旧E-BATTLE-NUMBER-PRODUCTION-UPDATE-0001F1A0-I-20261006保原快照/expected材料但改fail并具名解释，旧S与格式事实不否定、不掩已证错误。语义1399/有效ID1394、分类507、关联1512/3680=41.09%；其它历史/功能/175队列-R7区间67/3061-60源码closed10/完整验收0/P01及七open/完整门禁保持。全部字体宽度/资源/字载荷-draw消费/原父与回收重入owner-ABI-PC53-画面及R1/R7/R8继续；网络原格式明文/内存CryptInt保/音频暂停，未构建/测试/运行/hash。 两完整根与Number仅局部接线分别I/S，不以复用BOI01 call证据重报整根；新I/S均static。旧I失效记录不删，不将相同font0参数等同44B260与44B2A0读取合同，完整gates仍未宣通过。

## BOK01 图形记录与数字查询的分域局部证据（2026-10-06）

[基线](cg2d_graphic_record_production_lookup_spec.md#graphic-record-production-lookup-baseline)、[决议](cg2d_graphic_record_production_lookup_spec.md#graphic-record-production-lookup-decision)、[验收](cg2d_graphic_record_production_lookup_spec.md#graphic-record-production-lookup-acceptance)。完整Init634条1996B与XY21条80B、MapIndex7条25B，共662条2101B/10页/21call-97内部分支/10ret，无switch；入向1/20/29普通call、父1/18/21分列，三个含NUL字符串55B。仅Init13处恢复seqNo→当前主表→processed、11字段各自先主表后字段、有效索引发布分支才重读去重key/越界沿初样本、先end后start查找、追加后主表实际记录T*先于首见树key取样。40B栈记录未清/原EOF短读仍消费、32B字段原宽/reserved不写、索引后见覆盖与首见树借实际表位置、每有效文件S32排序→关Info不清槽→base=processed、原0/1/2/3/7/-1和各自清理保持；两个正确getter原上界/输出顺序/直接读索引保持，公开头不扩。

736/160/165三个已有ID续记，完整历史另存，旧stdio/格式事实保持；语义1399/有效ID1394及关联1512/3680=41.09%保持，分类507→510。RESOURCE26→29三成员，BATTLE103→105只登记两个真实数字查询依赖；共享基线candidate不替代成员，member_inventory_closed=false、完整gates仍空。RESOURCE三根与BATTLE两getter各I/S新增10条局部static证据，E599→609仍只有三个原函数；七open仅加进展/引用，60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0保持。 同一正常根在两个真实功能域分别登记I/S，不把10条证据当10个函数；Init仅RESOURCE正式成员，共享基线在BATTLE命中的candidate不计成员闭合。旧三个语义完整历史保，736先前受限stdio审查不追认容器/内容验收；本步当前局部范围以独立SPEC为准。

沿已批准宿主vector/map/sort承接本处值/引用合同，不宣STL节点/排序过程/分配故障或ABI逐条等价；所有父/callee、主表分配与GraphicRead真实内容/解码-字载荷绘制消费/owner回收及画面运行、R1/R7/R8继续。仅定向文本静态/计量，无构建/调试/测试/运行/hash；网络原格式明文/内存CryptInt保/音频暂停。

## BOL01 原正常别名范围与读盘消费证据（2026-10-06）

[基线](cg2d_graphic_read_decode_sampling_spec.md#graphic-read-decode-sampling-baseline)、[决议](cg2d_graphic_read_decode_sampling_spec.md#graphic-read-decode-sampling-decision)、[局部验收](cg2d_graphic_read_decode_sampling_spec.md#graphic-read-decode-sampling-acceptance)。原读盘75条222B与核心182条461B共257条683B/4页，4call-32内部分支/5ret无switch；读盘4普通call/4父，核心2普通call/同一父45A900分列。读盘二处恢复32B快照后同一实际记录地址读S16 fileIndex、缓存已检查FILE seek→重取FILE→快照length读取，以及可选palette判定在共享解码区指针发布之前取样；严格read返回1、失败不清输出/不回滚、缺省局部palette与两原core目标保持。核心正确CPP整文保持：初始目标/头宽高缓存、raw不读header.length且前向逐字节、RLE固定边界/仅命令间检查/value先长度/20优先10、literal只10及原FFFFF比较、原pal和length→width→height发布序不改。

167/168两个已有原ID完整历史深拷贝续记，旧167 FILE调用事实保；1399语义/1394有效ID/1512关联(41.09%)不变，分类510→512、RESOURCE29→31枚举open、四独立正常局部static I/S E609→613，完整gates空。七open只加当前进展/引用，队列175/R7区间67/3061、60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它功能保持。 两根分别I/S，全为static；既有源码不代表全部内容消费及寿命，撤额外不重叠条件使正常合同按原写序完整表达，不为达标缩范围；历史限定验证与本步当前局部证据分列。

撤旧接口及旧解码SPEC额外的两两不重叠前提，按原实际取样/字节处理/发布顺序保正常别名，不用附加前提缩原范围；旧限定范围下的历史验证保但不追认当前运行。正常W32/native地址注释按RPLAN.24，不增溢出/越界/补零/钳制/短读修复或异常等价。未构建/调试/测试/运行/hash；全部caller、当前所有正常别名/真实资源内容-调色板/draw/主表FILE owner及画面/R1/R7/R8继续，网络原格式明文/内存CryptInt保/音频暂停。

## BOM01 独立调色板共享/32色与native承载的局部证据（2026-10-06）

[原基线](cg2d_independent_palette_production_refresh_spec.md#independent-palette-production-refresh-baseline)、[决议](cg2d_independent_palette_production_refresh_spec.md#independent-palette-production-refresh-decision)、[局部验收](cg2d_independent_palette_production_refresh_spec.md#independent-palette-production-refresh-acceptance)。原生产313条1163B及刷新30条87B，共343条1250B/五页/3call-34内部分支/4ret，无switch；入向生产1/1父、刷新2/1父分列。完整原体含已排除16/24位四区间541B，只共享及32色实施。七生产补丁撤跨调用entry长借，失败/成功表与像素源先当前图号pub，复制后补清新表，固定实际目标/逐项新源record、R-B-G原色取样与R表→record-R/G表-G/B字节→B表取样，完整W32颜色先写、纯黑再W32=1，两W16版本各自新表先值再record写。原loaded不置位/计数/768B和L/3清段保，刷新两正确正文/原+22 S16与U8=0/1/2、仅2清零写回保持。

原5562E8/EC/F0三槽B-G-R/初值628FFC-6290FC-629200的12B与完整数据引用已归档；ColorChannelMaps及唯一位置初始化恢复B-G-R native顺序/布局断言，具名三通道仍各指同一数组，PaletteTone成员指针消费者/GPC01源码保，不重新全面复验其它父。

新1399/1400两原ID独立纳管：语义1399→1401/有效ID1394→1396、分类512→514、合同关联1512→1514/3680=41.14%；刷新两实现体同一原ID，历史直接定义1/2保。RESOURCE31→33及RENDER26→28枚举open，两根各域I/S共八新局部static证据E613→621，仍两原函数/三实现体；member_inventory_closed=false/gates空，七open仅进展/引用，175队列/R7区间67-3061、60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它历史功能保持。 低色深仅完整原库存纳盘，不以整函数排除丢共享/32色范围，也不以全部原条数代实施覆盖；两域八证据不当八函数。只有指针字段拓宽/布局顺序与具名绑定分列，所有者和运行仍独立未闭合。

仅已批准共享及32位正常局部静态，不代全部Tone/数组初始化父、读取/分配-回收/重入owner、上传像素/画面和R1/R7/R8。未编译/调试/测试/运行/hash；RPLAN.24正常原宽/已证地址，不新增异常保护/回绕或故障等价，网络原格式明文/内存CryptInt保/音频暂停。

## BON01 过渡/通道/查色的两域局部证据（2026-10-06）

[基线](cg2d_palette_transition_channel_sampling_spec.md#palette-transition-channel-sampling-baseline)、[决议](cg2d_palette_transition_channel_sampling_spec.md#palette-transition-channel-sampling-decision)、[局部验收](cg2d_palette_transition_channel_sampling_spec.md#palette-transition-channel-sampling-acceptance)。完整过渡357条1499B与查色23条80B，共380条1579B/六页/15call-40内部跳/3ret，无switch；主根6普通引用/5父、查色1/1父分列。两个低色深调用分流区各20B仅纳盘；9B格式/模式与135B ToneTargets共144B，三元表逐项一致保持。12补丁(主11/查色1)恢复初始化颜色及目标差、默认指针先状态pub、Tone三Byte/索引/差/浮点、steps先Framepub、完成Tone目标B/G先Bpub/R先Gpub、版本先两状态清、Tone G/R浮点先B/GBytepub、callee后活动快照及节流counter/steps先pub并同样本判定、查色R/G中间整数先B样本。

723/172两个已有ID及完整旧记录深拷贝续记，FILE及CPH六正常转换/五加法历史保；语义1401/有效ID1396/1514关联(41.14%)不变，分类514→516、RESOURCE33→35/RENDER28→30枚举open、八两域局部static I/S E621→629。历史直接定义1/0保持；两个原函数不当八函数，member_inventory_closed=false/gates空、七open仅进展/引用，队列175/R7区间67-3061/60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它历史功能保持。 旧FILE105条/CPH邻窗按旧范围保持，两个当前完整原根/取样独立记录；原15call含低色深/ftol/FILE分列，不拿源码词法call数相除覆盖率。

原512B未清path/三getc低U8/168黑色、色调helper逐项重取与业务上限255、查色完整W32颜色后纯黑1/透明0和400H返回保持；只共享/32正常局部，不代全部caller/PC53/私有ABI/数组及通道初始化-owner/像素画面和R1/R7/R8运行。未编译/调试/测试/运行/hash；RPLAN.24不增溢出/越界/补零/故障策略或回绕等价，网络原格式明文/内存CryptInt保/音频暂停。

## BOO01 初始化/读取的两域局部I/S（2026-10-06）

[BOO01 SPEC](cg2d_palette_initialization_read_sampling_spec.md#palette-initialization-read-sampling-decision)/[原证据](cg2d_palette_initialization_read_sampling_evidence.json)。完整初始化387条1544B与读取77条234B，共464条1778B/六页/19call-27内跳/5ret、三普通入向。原8/16/24区85条289B按既定范围排除；固定128条写入、128B局部色表与当前逐字节一致，八段原常量100B保留。Init仅1处恢复R→G→中间整数→B查色取样；W32完整色/纯黑1/透明0保持。Read原逐项DWORD头尾复制保持，不改Byte分写；旧FILE链、备份先168置黑、实际独立CPU绑定、中性通道顺序保。

169/170两已有ID完整历史深拷贝续记，语义1401/有效ID1396及1514关联41.14%不变；分类516→518、RESOURCE35→37/RENDER30→32仍枚举open，八两域局部static I/S E629→637。历史定义0/0保持，两个原函数不计八函数；七open及完整门禁保持，不代全部父/owner/正常别名/像素运行。 旧stdio六call/根及局部核对历史完整保，本次完整两根独立取证；低色深区和获批SDK机制分列，不拿源码call词法数量计算原覆盖率。

本步静态与汇总仅验上述范围；未构建/测试/运行/hash。原DX机制沿已有批准承接，不猜HRESULT或补COM模拟，不扩溢出/越界策略。网络原格式明文，音频暂停；继续沿实际初始化/资源消费/owner推进整体SPEC。

## BOP01 取色生产链局部I/S（2026-10-06）

[BOP01 SPEC](cg2d_automap_palette_color_production_spec.md#automap-palette-color-production-decision)/[证据](cg2d_automap_palette_color_production_evidence.json)。45A190→ReadPalette/451D80→ReadGraphicDataByIndex/451E90三原根完整273条768B/五页，5call-27内跳/5ret、四普通入向。Get恢复蓝累计在count之后发布、蓝/绿中间组包先红平均两处阶段；Nearest去通用闭包，首色R²+B²+G²与候选R²+G²+B²及取样各按原顺序表达。现有正常数值保持，不误报取色值错误；Sub45A190正确体保，原1024B局部palette/500000B色表清零及两map区、调用后fresh GraphicInfo不改。

三既有源码根新增机器语义1401/1402/1403，语义1401→1404/有效ID1396→1399，关联1514→1517（41.14%→41.22%），分类518→521；RESOURCE37→40/RENDER32→34仍枚举open，各成员独立I/S共10条static E637→647。历史直接定位各1/旧行号保；三个原函数不当十函数，七open仅进展/引用，60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0保持。 三根/两个域3+2成员，各I/S十条；新增分类与文档关联只度量追踪覆盖，不视为新实现十功能或关闭全枚举。BOO01/BOL01邻provider保，不重新计算原覆盖率。

只全根正常局部文字static；共享解码区单次借用/height入口样本与每像素fresh width、U32除法/低U8、严格<先见/候选16..<count-16保持。auto.dat父/实际图形与地图画面/共享owner运行继续推进，未构建/测试/运行/hash；网络原格式明文、内存CryptInt保、音频暂停，不新增溢出/越界策略。

BOP01当步同时修复functionRows两集合输出形态：baseline_documents/feature_memberships按@(...)捕获Unique流水线输出，单项与空项均保持数组；不改全局Unique、成员、统计与门禁。派生全对象仅增加这两字段的机械形态规范化，重新measure/-Check。

## BOQ01 DBF索引生产局部I/S（2026-10-06）

[BOQ01 SPEC](cg2d_palette_dbf_index_production_spec.md#palette-dbf-index-production-decision)/[原证据](cg2d_palette_dbf_index_production_evidence.json)。522900完整170条553B/三页、16call-11内跳/1ret，唯一普通入向522B8E/父522B30。五段原数据31B；58D360在.data，初值44424600、唯一直接引用5229C5为W32读。新增私有U32承载1、headerMagic→共享比较字和stride→fresh payload→S32偏移→native record地址两正文补丁。原正常值、两清零path/第255Byte判断、短读共享头/魔数不符不关闭、两分配及解码/旧块与键/重复覆盖/count-stride后pub/关闭后迭代入队保。

724已有ID完整历史深拷贝续记，语义1404/1399有效ID及1517关联41.22%不变；分类521→522、RESOURCE40→41枚举open，独立局部I/S E647→649。历史直接定义1/旧FILE属性保持；七open仅引用/进展、60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它门禁保持。 一根两局部证据，不把16原call计作16实现；旧五FILE/格式点及限定历史核对保，本次完整根正常索引生产独立登记。

仅全根正常局部static；W32比较字非地址不扩宽，不据无直接写引用宣只读或所有alias闭合。私有vector/map沿已有批准宿主承接，不以本根代原容器机制/53AF90或SDK-owner运行。未构建/测试/运行/hash；网络原格式明文、音频暂停，不新加异常/越界/故障或关闭策略。

## BOR01 初始化与hidden表局部I/S（2026-10-06）

[BOR01 SPEC](cg2d_independent_palette_initialization_sampling_spec.md#independent-palette-initialization-sampling-decision)/[本步证据](cg2d_independent_palette_initialization_sampling_evidence.json)。1237/522B30完整94条286B/两页6call-7内跳/1ret，两个普通入向；原指令继承PGM01完整归档，本轮核边界/入向及缺口取样。hidden58B7E8..58D348全7008B/1752W32与60源段初值逐项一致，保持可写表。初始化落实queueCount/首分配、clearCount先索引pub/实际取得表清零、recordCount与lock后initialCount先记录pub、逐项records→index→recordpub→fresh index loaded0、尾fresh count先index++比较。正文1/来源注释1，正常值和旧SDK适配/释放根保。

1237完整旧PGM01记录/分类保留续记，既有source2/历史定义1不变，语义1404/有效ID1399及1517关联41.22%、分类522/RESOURCE41保持；两局部static I/S E649→651。没有新增原函数或成员，七open仅进展/引用，60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及全门禁保持。 7008B是原可写数据初值覆盖，不能将1752项计作1752功能/运行样例；两证据仅同一原根正常局部。

仅正常局部static，native index16B仅首T*拓宽、loaded仍U8、记录0xB04无指针/count S32保，字节量在已批native API边界。保两GPTR/GlobalLock后才判初始数量，不新做空指针/失配/溢出保护、旧池回收/回滚。全部父/真实SDK/queue-DBF两表owner-正常别名/画面运行继续推进；未构建/测试/运行/hash，网络原格式明文、音频暂停。

## BOS01 入队/描述符生产局部I/S（2026-10-06）

[BOS01 SPEC](cg2d_graphic_enqueue_descriptor_production_spec.md#graphic-enqueue-descriptor-production-decision)/[原证据](cg2d_graphic_enqueue_descriptor_production_evidence.json)。公共521B10与520970两入队根完整227条704B/四页、4call-24内跳/6ret，422普通引用仅纳盘、父按根181/30分列。两根撤额外负计数拒绝、保S16上限4096/原-2；普通坐标撤S32回绕helper按正常S32+S16。真实record目标在map/XY前保存，描述符每对W32读取/发布四次，空参4W16→颜色W32FFFFFFFF→两U8→末W16；Draw3先实际descriptor pub再X/Y逐W16修正。clip先batch pub、cacheIndex/live计数和队列尾原宽/时点恢复；同文件helper不增原函数ID。

旧7完整历史续记、1404新机器映射，语义1404→1405/有效ID1399→1400、关联1517→1518（41.22%→41.25%）、分类522→524，RENDER34→36枚举open；独立局部I/S共4条E651→655。RESOURCE41保持，历史定义0/1不刷新；七open仅进展/引用、60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及全门禁保持。 四证据仅两原根正常局部；422调用点只引用库存，不代其父正文/编译或运行，私有helper不作为新原入口增加分母。

四文件补丁仅两根正文、同文件描述符helper插入、原S32回绕helper退休；其它根/既有W16减法/公开头/map-XY provider和队列布局保持。完整caller/正常源目标别名及排序/绘制consumer-owner/真实画面运行继续推进；不执行异常样例或扩大拒绝/保护策略，未构建/测试/运行/hash，网络原格式明文、音频暂停。

## BOT01 其它入队根独立I/S（2026-10-06）

[BOT01 SPEC](cg2d_graphic_secondary_enqueue_production_spec.md#graphic-secondary-enqueue-production-decision)/[原证据](cg2d_graphic_secondary_enqueue_production_evidence.json)。521C80/521DD0两完整入队根206条650B/四页、3call-22内跳/5ret，26普通调用点只纳盘、父分根1/18。两正文保存callee前的真实record目标与原字段样本，复用BOS01原四W32逐对消费/空参W16-W32-U8默认发布；C80 clip先batch、DD0 batch先clip各按原序，保各图号门槛/映射与仅XY的不同职责和special范围。

两既有源码根新增机器映射1405/1406，语义1405→1407/有效ID1400→1402、关联1518→1520（41.25%→41.30%）、分类524→526，RENDER36→38枚举open；四独立局部static I/S E655→659。历史定位各1不刷新，RESOURCE41保持、helper不增原函数；七open仅进展引用，60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它门禁保持。 四证据仅两正常原根局部，不当四功能/所有26调用点之父已验；原helper与BOS01邻根按已证范围保持，不累计重算原覆盖率。

本步只整DrawGraphic.cpp两个正文补丁；BOS helper/另外两根、其它正文及布局/头/owner/provider保持。C80低图号直传，不借其它根增加[-1,999]拒绝；DD0图形号直接XY、不加map。正常S32坐标与W16-U8字段/计数保，不新加负拒绝/保护/异常策略。全部caller/正常别名及排序-绘制/owner-像素运行继续推进；未构建/测试/运行/hash，网络原格式明文、音频暂停。

## BOU01 地图/着色生产独立I/S（2026-10-06）

[BOU01 SPEC](cg2d_map_colored_enqueue_production_spec.md#map-colored-enqueue-production-decision)/[原证据](cg2d_map_colored_enqueue_production_evidence.json)。地图521AA0/着色45F090完整58条217B/两页，1call-1内跳/3ret；55普通调用点只纳盘，已知父分根1/8，地图495E47保parent=null，不猜所属函数。两个正文恢复地图实际record/cache3与x/y取样，及着色栈W32/W16/U8重复-重叠发布/八实参-S32返回；原正常着色最终值正确，不记颜色修复或故障等价。

两既有源码根新增机器映射1407/1408；语义1407→1409/有效ID1402→1404、关联1520→1522（41.30%→41.36%）、分类526→528、RENDER38→40枚举open；四独立局部static I/S，E659→663。历史定位各1/旧行号保，RESOURCE41/队列175/R7区间67/3061及其余门禁保持；七open仅进展引用，60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保。
四证据仅两正常原根的局部static，不记四功能或全部55点之父已验；无所属调用点不自动闭R1/R7。

两CPP只五补丁（两个正文、退休一个私有投影及两未用include）；BOS/BOT四入队根、描述符消费helper/队列owner/排序/总绘制/provider及公开头保持。保正常S32/W32/W16/U8/native地址分工、地图batch5及不写字段；不加负拒绝、图号/结果门槛、保护或异常回绕模型。沿真实场景生产、地图/绘制consumer-owner与正常别名/像素继续推进；未构建/测试/运行/hash，网络原格式明文/音频暂停，整体SPEC范围不缩。

## BOV01 描边批次发布独立I/S（2026-10-06）

[BOV01 SPEC](cg2d_hover_batch_unconditional_publication_spec.md#hover-batch-publication-decision)/[原证据](cg2d_hover_batch_unconditional_publication_evidence.json)。描边522180全41条139B/一页、1call6内跳1ret；195普通调用点/115父仅纳盘。原call521B10后无条件U8 batch2写；真实helper额外索引范围拒绝与旧249文字合同不符，本步撤该拒绝/size_t，按native实际record写batch。已证-2前缀+5C写及正确描边包装/四低W16-两W32/step图号标记和六实参保持，不称颜色或坐标错误。

旧249续记并保存完整旧record；源码冻结定位0/旧两文档事实保，不新增语义或helper原ID。语义1409/有效ID1404、关联1522（41.36%）保持；分类528→529、RENDER40→41枚举open；两独立局部static I/S E663→665。RESOURCE41/队列175/R7区间67/3061及其余门禁保，七open仅进展引用；60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保持。
两证据仅一个完整原根和相关单helper的局部static，不当两功能、全部195点115父或异常等价。

只GraphicPendingQueue.cpp一个helper正文；正确DrawGraphicHover/BOS/BOT/BOU入队和描述符helper、排序比较/队列owner/总绘制/公开头保持。原容量门槛和失败后前缀写保，不加负拒绝/保护/钳制或异常回绕模型，不运行异常输入。既有void接口保，不称原EAX五倍索引残值/机器ABI、全部195点/115父、正常别名/描边case2填充-owner/像素运行已验。未构建/测试/运行/hash，网络原格式明文/内存CryptInt保/音频暂停，整体SPEC继续。

## BOW01 节点释放局部I/S与根身份（2026-10-06）

[BOW01 SPEC](cg2d_renderer_node_release_sampling_spec.md#renderer-node-release-decision)/[原证据](cg2d_renderer_node_release_sampling_evidence.json)。真正Destroy根44C3C0完整136条412B/两页18call15内跳1ret、2普通点2父只纳盘；44C408只是20条46B内部节点段，不登记独立原函数。18调用按4 IAT/4 GlobalHandle寄存器/10 vtable+8类型保，不把操作数当绝对callee地址。单节点helper保存一次surface、非空同owner释放/清同槽，fresh count先取再推进index/native node，沿同样本判尾；正确销毁包装及其它阶段保。

新增机器映射1409对应已有真正原根；语义1409→1410/有效ID1404→1405、关联1522→1523（41.36%→41.39%）、分类529→530、RENDER41→42枚举open；两独立节点局部static I/S E665→667。冻结定位1/旧行号保，RESOURCE41/队列175/R7区间67/3061及其它门禁保持；八open包含C09仅进展引用，60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保。
两证据仅正常节点20条46B及包装调用序，不当两个功能、136条全根实现或完整生命周期通过。

仅GraphicDxCache.cpp一个节点释放helper正文；根包装/主线程与32位SDL-DX7承接、Temp/AutoMap/设备/接口/调色板/Back/呈现器/renderer owner及其它源码保持。不清count、next/image/reuse/W16或未知尾、不扫原count<=0失败前缀，不新增拒绝/回滚/保护/异常策略。完整原取证不代整个Destroy实现验收；正常节点段/包装调用序局部static不代全部父-callee/正常别名/逐面owner-双树-C09/重建退出/SDK-机器ABI/像素运行。未构建/测试/运行/hash，网络原格式明文/内存CryptInt保/音频暂停，整体SPEC继续。

## BOX01 IsLost节点与CPU尾局部I/S（2026-10-06）

[BOX01 SPEC](cg2d_renderer_lost_probe_sampling_spec.md#renderer-lost-probe-decision)/[原证据](cg2d_renderer_lost_probe_sampling_evidence.json)。IsLost4536D0完整57条140B/一页4 vtable+60调用10内跳1ret，唯一466E2D/父466DC0只纳盘；60是操作偏移不是callee地址。单IsLost正文恢复count入口先取、每项探测后count先取再推进index/native node并沿同样本判尾；lost置1后仍继续节点，Temp起完整CPU尾保持。不新增AutoMap探测，不把CPU锁定当lost；无介入正常结果一致，不称已有画面/失效结果错误。

新机器1410对应已有真实失效根；语义1410→1411/有效ID1405→1406、关联1523→1524（41.39%→41.41%）、分类530→531、RENDER42→43枚举open；两条独立根内局部static I/S E667→669。冻结定位1/旧行号保，RESOURCE41/队列175/R7区间67/3061及其余门禁保持；八open含C09仅进展引用，60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保。
两条证据只一个原根的正常取样与CPU承接局部，不记两功能、全部模式或完整失效重建运行。

只SdlPresenter.cpp一个IsLost正文的节点循环；既有Temp/Back/呈现器尾、Destroy、节点/面owner、头/实际父与工厂/LegacyTextureState采样分母S16保持（描述符W32不混同）。CPU存活映射沿批准合同，不恢复COM/HRESULT机制，不增负拒绝/容量保护、清槽、释放重建、早停或异常回绕模型。正常别名、唯一父-callee/失效重建-逐面owner/双树-C09/SDK-机器ABI/模式画面运行未验；未构建/测试/运行/hash，网络原格式明文/内存CryptInt保/音频暂停，整体SPEC继续。

## BOY01 音频控制原身份范围纠错（2026-10-06）

[BOY01 SPEC](cg2d_audio_control_paused_scope_spec.md#audio-control-paused-acceptance)/[原证据](cg2d_audio_control_paused_scope_evidence.json)。九根完整305条1481B/11页，仅纠正旧7/旧8专属入口的既定暂停归属；7地址/2尾跳不是普通call入向，共享返回回调不纳入暂停。AUDIO23→32仍paused、成员枚举未闭；九队列项仅范围转paused，队列175/未定位174保持，需核55→46、暂停16→25。语义1411/有效ID1406、pending18、分类531、软件E669、文档关联1524（41.41%）、60源码closed10/完整验收0及全部完整门禁保持。 不新增pending消队列、不修改汇总器，不把具名成员数量当音频完成分母；11项相关任务只进展/引用，保持open，R1/R7/R8与整体SPEC仍未闭。

## BOZ01 两矩形消费者局部I/S（2026-10-06）

[BOZ01 SPEC](cg2d_queued_rectangle_consumption_sampling_spec.md#queued-rectangle-consumption-acceptance)/[原证据](cg2d_queued_rectangle_consumption_sampling_evidence.json)。两原根完整72条188B/两页4call4内跳4ret、两普通点同一父520DD0及选定8条24B case2/3窗口纳盘。单CPP两个正文：522210取消额外RECT整块零写，原两W32样本/四边发布/模式优先与低W16颜色保持；520AD0依mix→color→y高/y低/x高/x低四次实际W16取样，S16实参与完整返回887601AE→共享dirty=1尾保持。无介入旧正常坐标/颜色结果一致，不称像素错误。

新增机器1411/1412对应已有两个原根；语义1411→1413/有效ID1406→1408、关联1524→1526（41.41%→41.47%）、分类531→533、RENDER43→45枚举open，四独立局部static I/S E669→673。冻结定位各1不刷新；RESOURCE41/AUDIO32暂停、pending18、队列175/R7区间67/3061及其它门禁保，七open只进展/引用；60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保持。 四软件证据为两独立根各I/S局部，不当四功能/完整父或全模式通过；原parent片段从实际指令5216E9起，8条另列，不能把最初中部窗口纳入原覆盖。只GraphicPendingQueue.cpp两个完整正文补丁；头/布局/实际父、排序/入队/描边包装、共享Back/调色板/纹理provider与owner保持，不新增helper或状态。原父忽略两EAX/既有void和native低S16-W16-U8接口保，不记机器ABI/EAX等价。完整生产-父-callee/正常别名/逐面owner/dirty重建-场景退出及SDK-机器ABI、全模式像素运行未验；不增加保护/拒绝/钳制、计数写/释放或异常回绕模型。未构建/测试/运行/hash，网络原格式只明文、内存CryptInt保、音频暂停，整体SPEC继续。

## BPA01 真实匹配绘框根局部I/S（2026-10-06）

[BPA01 SPEC](cg2d_matching_hover_record_sampling_spec.md#matching-hover-record-acceptance)/[原证据](cg2d_matching_hover_record_sampling_evidence.json)。真实507090完整59条175B/一页1call5内跳1ret，唯一普通点5076BD/父507140及8条25B窗口仅纳盘。单正文恢复匹配参数/实际DX条目在y/x前取得、四边bottom/top/right/left正常S32生产、count先取再index/主record/平行层native推进；七实参U16边界、CE/DA和step0、绘框追加项参与后续遍历保持。无介入旧正常坐标/颜色/匹配结果一致，不称像素错误。

新增机器1413对应已有原根，语义1413→1414/有效ID1408→1409、关联1526→1527（41.47%→41.49%）、分类533→534、RENDER45→46枚举open，两独立根内局部static I/S E673→675。冻定位1/旧行号、RESOURCE41/AUDIO32暂停、pending18、队列175/需核46/暂停25、R7区间67/3061及其它门禁保持；七open仅进展/引用，60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保。 两证据不是两个功能，完整原根/8条父窗口与独立根内I/S各自分列；不升级整个命中/渲染门禁。只GraphicPendingQueue.cpp一个完整正文补丁；所有其它正文/头/布局/实际父、绘框/排序/provider与owner保持，不新增helper/私有状态/原入口。原S32参数任意、不借父>=3门槛；正count后初始化平行游标，不经cacheIndex重定位、首匹配退出或固定入口长度。完整命中/全部callee与正常alias、队列-表/纹理-Back-owner/场景-失效-退出、SDK-机器ABI/全模式像素运行未验；不加拒绝/补零/保护/钳制或异常回绕模型。未构建/测试/运行/hash，网络原格式只明文、内存CryptInt保、音频暂停，整体SPEC继续。

## BPB01 完整条带与四正确依赖局部I/S（2026-10-06）

[BPB01 SPEC](cg2d_ripple_full_strip_production_spec.md#ripple-full-strip-acceptance)/[原证据](cg2d_ripple_full_strip_production_evidence.json)。五个真实波纹根完整160条537B/六页，9个调用操作含7绝对call/2vtable+1C、9内跳6ret；五普通点分根父1/1/1/1/1共4不同父，只纳盘。521870单正文恢复实际16B矩形初始化/整数相位重复发布、正常S32乘除/推进与完整60次8行条带提交，返回忽略；四正确依赖/64项表和五浮点常量保持。

旧机器6全文档案保留续记，四新1414–1417对应已存在原身份；语义1414→1418/有效ID1409→1413、关联1527→1531（41.49%→41.60%）、分类534→539、RENDER46→51枚举open，十独立正常局部static I/S E675→685。前四冻定位各1/私有差分helper0不刷新；RESOURCE41/AUDIO32暂停、pending18、队列175/需核46/暂停25、R7区间67/3061与其它门禁保持，八open含G04只进展/引用；60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保。 十证据不当十功能，正确helper纳管不记新源码实现；表64S32/256B与五浮点项28B只静态数据对照，不执行样例或声称全部间接写/浮点环境。此裁决只更正旧波纹caller裁剪承接，不扩原生豁免。源码仅一个521870完整正文、algorithm include退休与头单注释，四正确helper/表/其他正文和SurfaceState所有provider、真实父/公开接口/布局/owner保持。无预裁剪/部分条带、continue或新拒绝/补零/保护/钳制，旧BlitTempToBack接受限制不扩大；不做异常回绕/故障等价或浮点环境专项模拟。完整父/所有callee-正常alias/Back-Temp及其它资源-owner/场景-失效-退出、SDK-机器浮点ABI与全模式像素未验，未构建/测试/运行/hash。网络原格式只明文、内存CryptInt保持、音频暂停，整体SPEC继续。

## BPC01 角色生产两域局部I/S（2026-10-06）

[BPC01 SPEC](cg2d_character_enqueue_record_production_spec.md#character-enqueue-production-acceptance)/[原证据](cg2d_character_enqueue_record_production_evidence.json)。角色521F20完整160条595B/两页3绝对call28内跳1ret，18普通调用点3个父仅纳盘。单正文恢复入口/名称后实际order-record游标、独立fresh容量计数和名称-坐标目标阶段；四W32逐对描述符复制、各U8颜色/低flags、mix沿saved clear值两pub、clip先取/image一次取样与原链公共尾恢复。无介入旧正常最终坐标/颜色等正确，不称像素错误。

新机器1418对应已有角色原根；语义1418→1419/有效ID1413→1414、关联1531→1532（41.60%→41.63%）、分类539→540、RENDER51→52/YOBI55→56均枚举open，两域独立正常局部I/S共4条E685→689。冻定位1不刷新；RESOURCE41/AUDIO32暂停、pending18、队列175/需核46/暂停25、R7区间67/3061及其它门禁保持；八open含G04只进展/引用，60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保。 四软件证据为同一角色根的两功能域I/S，不当四功能、两个原根或完整FP/像素运行；源生命周期/正常别名继续独立推进，不因现有最终字段值正确省略原取样/发布证据。只YobiRuntime.cpp一个完整RenderChara正文，所有其它实现/存储/头/公开或私有声明、action-队列布局、实际父/名称及排序-命中-绘制provider与owner保持，不新增helper/影子状态/原入口。原有效内嵌空名也调用、parity只cache=-2、图号-1/death严格1/低flags2和S16容量原门槛保；正常PC53/S64低S32边界保，不做FP环境/异常回绕模拟或新增拒绝/保护/补零/钳制。全部上游-18点之父/callee/正常alias与action-队列-图形-纹理-owner及场景-失效-退出、SDK-机器FP ABI和像素运行未验。未构建/测试/运行/hash，网络原格式只明文、内存CryptInt保、音频暂停，整体SPEC继续。

## BPD01 原数值解析三根局部I/S（2026-10-06）

[BPD01 SPEC](cg2d_delimited_numeric_normal_sampling_spec.md#delimited-numeric-normal-acceptance)/[原证据](cg2d_delimited_numeric_normal_sampling_evidence.json)。三个真实数值解析根完整104条286B/三页4绝对call14内跳6ret，26普通点16不同父分根1/13/12点和1/6/9父，只纳盘。单52D050正文恢复正常S32 value/sign与一次字节样本、下一字节先取再游标推进；52CFF0状态/52D0D0栈field两正确体保持。字符表、先乘62、任意位置负号、非法0/空0与field空-1/status空2不写输出均原样保。

三新1419–1421对应已有原根；语义1419→1422/有效ID1414→1417、关联1532→1535（41.63%→41.71%）、分类540→543、NETWORK47→50枚举open，六独立正常局部static I/S E689→695。三冻定位各1不刷新；RENDER52/YOBI56/RESOURCE41/AUDIO32暂停、pending18、队列175/需核46/暂停25、R7区间67/3061及其它门禁保；U10/R01/R04/V03四open只进展引用，60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保持。 六软件证据不是六功能；两个正确体纳管与单decoder算式/取样实修分列，不以关联计数代完整108回复/owner或互通验收。只NetworkRuntime.cpp单Base62正文，两个正确包装/四DFI工具、其它实现/头/API/共享缓冲及帧-108handler/provider/owner保持，不新增helper或输出默认值，不复用5176B0前缀负号。撤专项U32回绕，正常S32算式/原业务字符-容量/返回保持，无额外拒绝/保护/钳制或异常模型。完整26点之父/字段-108回复-请求与状态/正常alias-缓冲及输出owner/关闭退出/SDK-机器ABI和实际互通未验；未构建/测试/运行/hash，原格式只明文、两加解密叶排除/内存CryptInt保、音频暂停，整体SPEC继续。

## BPE01 两原整数根局部I/S与引用修复（2026-10-06）

[BPE01验收合同](cg2d_protocol_integer_normal_arithmetic_spec.md#protocol-integer-normal-acceptance)/[完整原证据及前态](cg2d_protocol_integer_normal_arithmetic_evidence.json)。只5176B0正常S32算式实修，正确517190包装保持；两根57条132B和158普通点两父、两个既有映射续记，非新功能。四新增I/S只指各根正常局部，完整父/字段/正常alias-owner/寿命、机器ABI/链接与真实明文互通未验。

六BPD具名证据artifacts末项拆回五独立引用，原前五/其它字段与证据数保持；不能把串联项当可定位的多个文件，也不因修引用提升原门禁。语义1422条/1417原ID、合同关联1535/3680（41.71%）、分类543、NETWORK50枚举未闭均保持；E695→699只增加两个既有根各I/S四条正常局部证据。60任务源码closed10/完整验收0、叶验收0/global_accepted=false及完整门禁保持。 U10/R01/R04/V03仍open；仅必要静态与measure/-Check，未构建/测试/运行/摘要。

## BPF01 调色板老化正常局部I/S（2026-10-06）

[BPF01验收合同](cg2d_palette_age_normal_counter_spec.md#palette-age-normal-acceptance)/[原证据及前态](cg2d_palette_age_normal_counter_evidence.json)。原根21条86B及唯一公共尾11条40B，实际计数表达式正常S32恢复/无用bit头退休；两域各I/S四条只验正常局部算术/原回收与取样，不增四个功能，不代完整520DD0/全部上游和正常alias-owner/寿命或全模式像素运行。

既有1204与equivalent分类续记，原字段/接口及成员保持。语义1422条/1417原ID、关联1535/3680（41.71%）、分类543、RENDER52/RESOURCE41枚举未闭保持；E699→703仅两域各I/S四条正常局部证据。60任务源码closed10/完整验收0、叶验收0/global_accepted=false及所有完整门禁保持。 G01/G02/G04/R01/R04/V04增加进展引用且仍open；仅必要静态与measure/-Check，未构建/测试/运行/摘要。

## BPG01 输入扫描预算正常局部I/S（2026-10-06）

[BPG01验收合同](cg2d_input_scan_budget_sampling_spec.md#input-scan-budget-acceptance)/[完整原证据和前态](cg2d_input_scan_budget_sampling_evidence.json)。追加原根182条660B及四普通点四父、旧368两源取样片段实修；两域各I/S四条只验当前正常局部预算/判定与原控制/回写保持，不增四个功能，不代完整四父/文本绘制/焦点和正常alias-owner/寿命或运行。

语义1422条/1417原ID、关联1535/3680（41.71%）、分类543、INPUT14/TEXT22枚举未闭保持；E703→707仅两域各I/S四条正常局部证据。60任务源码closed10/完整验收0、叶验收0/global_accepted=false及全部完整门禁保持。 T01/T02/U04/R01/R04/V04只增进展/引用且仍open；旧表+04调用来源不借现代输入链猜补，机器ABI/SDK/链接与输入画面运行继续待。仅必要静态与measure/-Check，未构建/测试/运行/摘要。

## BPH01 查询索引/预设消费正常局部I/S（2026-10-06）

[BPH01验收合同](cg2d_key_index_preset_source_spec.md#key-index-preset-acceptance)/[原证据及前态](cg2d_key_index_preset_source_evidence.json)。三个实际原根87条282B、两查询新映射/旧373续记与三原分类，完整W32参数和真实caller去收窄；INPUT三个根各I/S、TEXT预设根各I/S共8条，只限定正常键查询/文本消费，不算8个新增功能或完整父/播放/输入/焦点通过。

语义1422→1424条/有效ID1417→1419、关联1535→1537/3680（41.71%→41.77%）、分类543→546，INPUT14→17/TEXT22→23枚举未闭；E707→715仅8条正常局部I/S。60任务源码closed10/完整验收0、叶验收0/global_accepted=false及全部完整门禁保持。 三冻定位各1保持，不代当前编译或链接；音频支持体未计门禁，AUDIO32暂停、pending18/R7及未知差集保。T01/T02/U04/R01/R04/V04只增进展/引用且仍open；未构建/测试/运行/摘要。

## BPI01 输入绘制两源阶段局部I/S（2026-10-07）

[具名验收合同](cg2d_input_text_alignment_spec.md#input-text-source-phase-acceptance)/[原窗口/旧807前态](cg2d_input_text_alignment_evidence.json)。两源片段实修、两窗口29条109B与全文逆换静态；FUNC-INPUT/FUNC-TEXT各I/S，共四条，只计分类判定/预算和多行X/Y发布阶段。旧equivalent/身份及所有完整门禁保持，不据局部判定生成机器代码、并发或画面等价。

语义1424/1419、关联1537/41.77%、分类546保持；登记E715→719，INPUT17/TEXT23枚举仍open。T01/T02/R04/V04只增进展引用；10/60源码任务、0完整验收/叶验收及global_accepted=false保持。未构建/测试/运行/摘要，正常owner/寿命及完整运行按原标准推进。

## BPJ01 专用幂帮助入口局部S（2026-10-07）

[具名合同](cg2d_map_effect_arithmetic_spec.md#pow-twotos-acceptance)/[完整原帮助体及前态](cg2d_map_effect_arithmetic_evidence.json)。11条21B/唯一call父邻窗和既有十条x87内联来源，只增一CRT局部S/一adapted身份；源码未变，I/完整功能门禁不提升，不重复登记整个一般_pow通过。

语义1425/1420、关联1538/41.79%、分类547、CRT150枚举open、登记E720；队列174/待归属45仅移本原ID，暂停25/排除103/NOP1与R7已审67/3061保持。两任务R01/R04仅引用/进展仍open，10/60源码任务/0完整验收/0叶/global_accepted=false保持；SDK/ABI/环境、场景RNG/owner寿命及运行按原标准继续。

## BPK01 私有捕获职责局部S（2026-10-07）

[可验合同](cg2d_native_cpp_exception_spec.md#cpp-catch-private-host-acceptance)/[两原体/旧前态与SDK定位](cg2d_native_cpp_exception_evidence.json)。仅一组局部S：315条875B/两唯一call父邻窗、当前六类/copy和真实SDK职责定位；两旧原ID按D02登记host。旧数据接线false和所有业务源码保持，私有头只注释，不把安装源码或host批准记为生成ABI/实际捕获通过。

语义1425/1420与关联1538/41.79%保持，分类549、CRT152枚举open、登记E721；队列174/待归属45、R7已审67/3061及所有完整门禁保持。C06/C07/R01/R04只增进展引用并对齐C06职责条件，仍open；10/60源码任务/0完整验收/0叶/global_accepted=false不变，实际typed捕获/切片/清理、成员/硬件SEH/owner寿命和ABI/链接/展开运行按原标准继续。

## BPL01 悬停发布与点击关联正常局部I/S（2026-10-07）

[验收合同](cg2d_hover_input_publication_spec.md#hover-input-publication-acceptance)/[17条发布窗口和完整19条查询分列](cg2d_hover_input_publication_evidence.json)。三域各I/S六条只记两源判定阶段实修/关联既有查询基线，RENDER只枚举悬停根，查询仅关联消费；不把六证据算六功能或完整悬停/输入通过。旧823无处置状态保，新query equivalent有完整原基线。

语义1426/1421、关联1539/41.82%、分类550、INPUT19/TEXT25/RENDER53枚举open、登记E727；队列174/待归属45、R7已审67/3061及其他域/完整门禁保持。T01/T02/U04/R01/R04/V04只引用/进展仍open，10/60源码任务/0完整验收/0叶/global_accepted=false不变；完整消息/799悬停/主循环与焦点、间接alias-owner/寿命、SDK/ABI/链接及实际输入画面按原标准继续。

## BPM01 原帮助入口与当前正常阶段的局部S（2026-10-07）

[限定合同](cg2d_map_effect_arithmetic_spec.md#pow-input-control-acceptance)/[两原体及缺口前态](cg2d_map_effect_arithmetic_evidence.json)。完整27条90B、五普通点/两父邻窗与两个应用参数关联窗口分列；仅两个当前正常阶段的来源映射，原默认027F条件绕过和泛用PC/特殊F80未完整实施边界显式记录。源码保持，新增一CRT局部S，不登记新增实现或提升完整I。

语义1428/1423、关联1541/41.88%、分类552、CRT154枚举open、E728；队列172/待归属43只移两个具名ID。暂停25/排除103/NOP1、R7已审67/3061、两open任务及10/60源码关闭/0完整验收/0叶/global_accepted=false保持；运行/SDK/ABI、完整场景RNG和所有owner寿命仍按原标准。

## BPN01 正常指数估算的一源差与局部I/S（2026-10-07）

[限定验收](cg2d_float_decimal_estimate_spec.md#float-decimal-estimate-acceptance)/[原证据及本次前态](cg2d_float_decimal_estimate_evidence.json)。仅原正常15条52B/唯一call父的取参与发布、现一估算表达式与真实字段/调用保持静态；原232全部算法和96位/e/f/g/环境未新验。两局部I/S各九独立来源，不把无介入旧值一致称已修数值/显示错误。

语义1428/1423、文档1541/41.88%、队列172/待归属43、暂停25/排除103/NOP1及R7 67/3061保持；分类553、CRT155枚举open、E730；C12/R01/R04仅追加引用/进展，60任务源码closed10/完整验收0、叶0/global_accepted=false及全部原适用门禁保持。

## BPO01 第二批完整字节用途mapped（2026-10-07）

[限定验收](cg2d_unassigned_range_classification_spec.md#range-second-batch-acceptance)/[64原身份前态及1477B](cg2d_unassigned_range_second_batch_evidence.json)。每段完整字节/逐字节owner和directxref、实际函数块/RET或直接JMP边界核定；完整257槽/200标签原父归属和BTS字节一致，只64辅助mapped及一父数据依据，软件I/W/S/R不提升。

R7用途处置131/3061/余2930；函数3680/导入218、语义1428/1423、文档1541/41.88%、分类553、BATTLE105和E730、队列172/待归属43与暂停/排除保持；两任务R03/R04仍open、10/60源码关闭/0完整验收/0叶/global_accepted=false保持。不把填充或数据称范围排除、功能实现或未知间接目标不可达。

## BPP01 128字节用途与局部字段修复分列（2026-10-07）

[限定验收](cg2d_unassigned_range_classification_spec.md#range-third-batch-acceptance)/[原全字节与六条消费](cg2d_unassigned_range_third_batch_evidence.json)。128X/1272B全部用途辅助mapped；三新增语义只原数据/已有源角色及40F0A0字段片段，未重验682条。两软件I/S各九实来源，仅[1]启用/[0]类型选择恢复，不将X数据分类当软件通过。

R7 259/3061/余2802；语义1431/1426、关联1544/41.96%、BATTLE108枚举open、E732；函数分类553、源差队列172/待归属43、暂停25/排除103/NOP1、10/60源码关闭/0完整验收/0叶/global_accepted=false保持。R03/R04/U08仅进展和引用/原条件不改，所有完整门禁与原运行标准保。

## BPQ01 两动作完整正常本体局部I/S（2026-10-07）

[限定验收](cg2d_battle_action_141_143_spec.md#battle-action-141-143-acceptance)/[完整193条649B](cg2d_battle_action_141_143_evidence.json)。57/136两根/三页、六call、两唯一父邻窗与BPP表分列；图号/参数/default和公共尾静态对应，只有W8读取源差，正常旧判断一致。两局部I/S各九实来源，未把父/所有子依赖、环境/SDK机器ABI、owner寿命或运行记已验。

语义1431/1426、文档1544/41.96%、BATTLE108枚举open、R7 259/3061/余2802及队列172/待归属43保持；分类555、E734、四open任务仅引用/当前；60源码closed10/完整验收0/叶0/global_accepted=false及原运行标准继续。

## BPR01 动作114完整正常源局部I/S（2026-10-07）

[限定验收](cg2d_battle_action_114_spec.md#battle-action-114-acceptance)/[完整七页489条1745B](cg2d_battle_action_114_evidence.json)。34call/57内部jump/六槽switch及一ret、唯一父邻窗、四原W8点和54D4B4常数分列；三源片段恢复真实读取宽度，正常旧掩码相同，原W32消费/业务范围/正确依赖保。两局部I/S各九独立实际来源，未把全父子/环境/owner寿命/生成机器ABI或运行记已验。

分类556、E736；语义1431/1426、关联1544/41.96%、BATTLE108枚举open、R7 259/3061/余2802、差队列172/待归属43及四任务open保持；60源码closed10/完整验收0/叶0/global_accepted=false，原验收标准保。

## BPS01 四动作完整正常源局部I/S（2026-10-07）

[限定验收](cg2d_battle_motion_scale_lifecycle_spec.md#battle-motion-scale-acceptance)/[四完整原根](cg2d_battle_motion_scale_lifecycle_evidence.json)。339条1170B/六页、29call/10被调入口、34内部jump/四ret和四唯一父邻窗、三个原常数分列；155/156回收同阶段一次非拥有指针已恢复，清槽/推进与正确113/154源保。两局部I/S各十一独立实际来源，只正常源阶段，不代生成机器/x87精度/owner寿命或运行。

语义1431→1435/有效1426→1430、关联1544→1548（42.07%）、BATTLE108→112枚举open、分类556→560、E736→738；R7 259/3061/余2802、差队列172/待归属43及四任务open保。60源码closed10/完整验收0/叶0/global_accepted=false，原标准继续。

## BPT01 资源复制三完整正常源局部I/S（2026-10-07）

[限定验收](cg2d_battle_resource_copy_spec.md#battle-resource-copy-acceptance)/[完整540条1709B](cg2d_battle_resource_copy_evidence.json)。九页、63call/11入口、36普通内部jump/15ret与三唯一父九条邻窗，十二原临时复制阶段对应共享helper六取样片段；各字段读齐后按真实次序写，业务上下限、构造/析构及公共尾和162正确体保。两局部I/S各十二独立实际来源，仅正常源阶段，不代全父子/owner寿命或运行。

语义1438/有效1433、关联1551/42.15%、BATTLE115枚举open、分类563/E740；R7 259/3061、差队列172/待归属43、四任务open及60源码closed10/完整验收0/叶0/global_accepted=false保，原标准继续。

## BPU01 两完整图像状态源局部I/S（2026-10-07）

[限定验收](cg2d_battle_rebirth_transition_spec.md#battle-rebirth-transition-acceptance)/[完整570条2302B](cg2d_battle_rebirth_transition_evidence.json)。八页、32call/八被调入口、63内部jump/七ret及两唯一父六条邻窗，七原栈初始化组52值分列。八源片段恢复低W8类型及同阶段借用回收，正常旧值相同，不外推生成机器/并发或故障；正确第三槽/真实重读保持，两局部I/S各十独立实际来源。

语义1440/1435、关联1553/42.20%、BATTLE117枚举open、分类565/E742，R7 259/3061及差队列172/待归属43、四任务open与60源码closed10/完整验收0/叶0/global_accepted=false保持，运行标准保。

## BPV01 六正常动作完整源局部I/S（2026-10-07）

[限定验收](cg2d_battle_count_timer_flags_spec.md#battle-count-timer-flags-acceptance)/[六原根243条815B](cg2d_battle_count_timer_flags_evidence.json)。八页、19call/七入口、21普通内部jump/10ret与六唯一父18条邻窗、四F32原常数及两个实际W8点分列；计时内联正常数学/共享helper和308/309字节发布纳管。165两片段只读取宽度恢复，正常旧值相同，不外推实际x87/生成机器或故障；两局部I/S各十四独立实来源。

语义1446/1441、关联1559/42.36%、BATTLE123枚举open、分类571/E744；R7 259/3061、差队列172/待归属43、四任务open及60源码closed10/完整验收0/叶0/global_accepted=false保持，运行标准保。

## BPW01 七正常状态/同步源局部I/S（2026-10-07）

[限定验收](cg2d_battle_attachment_alpha_background_spec.md#battle-attachment-alpha-background-acceptance)/[完整736条2670B](cg2d_battle_attachment_alpha_background_evidence.json)。13页、43call/69内部jump/九ret、六真实分派点与同步14普通入向/三个实际父参数-弃返邻窗、九槽每操作后fresh/重复对象和两原临时完整取样可追。两源片段不改正确同步共享汇合、原字段与业务上下限，正常旧值一致；两局部I/S各十五独立来源，生成机器/环境及运行未验。

语义1453/1448、关联1566/42.55%、BATTLE130枚举open、分类578/E746，R7 259/3061、差队列172/待归属43、四任务open与60源码closed10/完整验收0/叶0/global_accepted=false保，原标准继续。

## BPX01 九完整转场/坐标/效果局部I/S（2026-10-07）

[限定验收](cg2d_battle_transition_effect_sampling_spec.md#battle-transition-effect-sampling-acceptance)/[完整757条2516B](cg2d_battle_transition_effect_sampling_evidence.json)。16页、48call/24入口/108内部jump/14ret、九入向/27邻窗/32父项、23type参数链/六常数及209六实际W8/两个FDC-FCC读取序/encoded先目标与原交替发布可追。九源码阶段只修读取边界/顺序，正常旧值一致；两局部I/S各十七实来源，完整递归依赖/生产/owner与生成机器/运行未验。

语义1462/1457、关联1575/42.80%、BATTLE139枚举open、分类587/E748。R7 259/3061/余2802、差队列172/待归属43、四任务open及60源码closed10/完整验收0/叶0/global_accepted=false不变，原验收标准继续。

## BPY01 七完整场景/死亡状态局部I/S与同依赖R7（2026-10-07）

[限定验收](cg2d_battle_scene_words_death_sampling_spec.md#battle-scene-words-death-sampling-acceptance)/[完整439条1445B](cg2d_battle_scene_words_death_sampling_evidence.json)。九页、40call/16入口/37普通jump/一四项switch/17ret、七入向21邻窗及四体七读取阶段可追；正常旧值一致，八/十六位原消费、读取发布序和同场景owner不扩大为运行值错误。两局部I/S各十六独立实来源，完整递归/生产/owner与生成机器/运行未验。

X:0001305D-00013070逐字节owner/xref和RET/实际块边界、LEA解码及四W32表/唯一引用证明19B用途mapped，只R7 259→260/余2801。语义1469/1464、关联1582/42.99%、BATTLE146枚举open、分类594/E750，差队列172/待归属43、五任务open及60源码closed10/完整验收0/叶0/global_accepted=false保。

## BPZ01 三完整克隆/基础图还原局部I/S（2026-10-07）

[限定验收](cg2d_battle_clone_restore_sampling_spec.md#battle-clone-restore-sampling-acceptance)/[完整397条1508B](cg2d_battle_clone_restore_sampling_evidence.json)。六页、31call/八入口/42普通jump/八ret、三父九邻窗和源Restore两个使用者对应完整原内联可追；两个先指针后图号/一个末尾低W8三阶段恢复，不称正常数值错误或原返回ABI通过。原同步循环、槽空检查/别名借用和条件回收/业务/状态保。两局部I/S各十实来源，递归依赖/生产/owner与生成机器/运行未验。

语义1472/1467、关联1585/43.07%、BATTLE149枚举open、分类597/E752；R7 260/3061/余2801、差队列172/待归属43、四任务open及60源码closed10/完整验收0/叶0/global_accepted=false保持。

## BQA02 两完整成对效果/距离状态局部I/S（2026-10-07）

[限定验收](cg2d_battle_effect_pair_distance_sampling_spec.md#battle-effect-pair-distance-sampling-acceptance)/[完整346条1169B](cg2d_battle_effect_pair_distance_sampling_evidence.json)。六页、34call/21入口/28普通jump/三ret、两父六邻窗/四常数与四低W8可追，原源HP归零三齐读/原指针寄存器分支及两图标接收者明确。只四读取边界片段，正常旧值一致；两局部I/S各十实来源，不代生成机器/实际x87/运行或完整递归生产/owner寿命。

语义1474/1469、关联1587/43.12%、BATTLE151枚举open、分类599/E754；R7 260/3061/余2801、差队列172/待归属43、四任务open及60源码closed10/完整验收0/叶0/global_accepted=false保持。

## BQB01 两完整宠物快照/骑乘状态局部I/S（2026-10-07）

[限定验收](cg2d_battle_pet_rider_sampling_spec.md#battle-pet-rider-sampling-acceptance)/[完整289条1074B](cg2d_battle_pet_rider_sampling_evidence.json)。五页、25call/14入口/十jump/四ret、两父六邻窗和225九源码阶段可追。正常旧值一致；223正确体、四CopyPayload交替、每阶段fresh槽/已有借指针、真实重复清字段/原坐标还原保持，224既有基线不重复计数。两局部I/S各十实来源，不代生成机器/实际x87/运行或完整递归生产/owner寿命。

语义1476/1471、关联1589/43.18%、BATTLE153枚举open、分类601/E756；R7 260/3061/余2801、差队列172/待归属43、四任务open及60源码closed10/完整验收0/叶0/global_accepted=false保持。

## BQC01 五完整计时/坐标/恢复扣减状态局部I/S（2026-10-07）

[限定验收](cg2d_battle_timing_coordinate_damage_sampling_spec.md#battle-timing-coordinate-damage-sampling-acceptance)/[完整583条2104B](cg2d_battle_timing_coordinate_damage_sampling_evidence.json)。十页、58call/26入口/49jump/六ret、五父15邻窗/52B、228真实内部call229透传返回及两F32常数/五源码阶段可追。正常旧值一致，反伤三齐读/写source和普通扣减encoded先共享目标/后两字段交替分别落实，原HP/图标/Attribute接收者、寄存器W32旗位与独立RNG/共享尾/返回保持。两局部I/S各十三实来源，不代生成机器/实际x87/运行或完整递归生产/owner寿命。

语义1481/1476、关联1594/43.32%、BATTLE158枚举open、分类606/E758；R7 260/3061/余2801、差队列172/待归属43、四任务open与60源码closed10/完整验收0/叶0/global_accepted=false保。

## BQD01 四完整转段/反弹/反伤状态局部I/S（2026-10-07）

[限定验收](cg2d_battle_bounce_reflection_sampling_spec.md#battle-bounce-reflection-sampling-acceptance)/[完整834条2947B](cg2d_battle_bounce_reflection_sampling_evidence.json)。十三页、62call/26入口/86jump/八ret、四父12邻窗/35B、十F32与十一源码阶段可追。四内存W8及W32寄存器消费分清，234下边界两角度读、237两归零独立原序和三数显索引分别落实；正常旧值一致，236只已证父弃返、S16width/共享helper/原业务/接收者/RNG/同步帧循环/返回保。两局部I/S各十四实来源，不代生成机器/实际x87/运行或完整递归生产/owner寿命及图形链。

语义1485/1480、关联1598/43.42%、BATTLE162枚举open、分类610/E760；R7 260/3061/余2801、差队列172/待归属43、四任务open与60源码closed10/完整验收0/叶0/global_accepted=false保。

## BQE01 七完整共用施法/弧射状态局部I/S（2026-10-07）

[限定验收](cg2d_battle_shared_cast_arc_sampling_spec.md#battle-shared-cast-arc-sampling-acceptance)/[完整729条2516B](cg2d_battle_shared_cast_arc_sampling_evidence.json)。十四页、60call/26入口/77jump/十二ret、七父21邻窗/56B、两共用表项/三F32/四原W8和六源码阶段可追。共体归零序/三W8表达式/速度先零比较后phase239和241声事件先X分别落实，原五正确体及W32缓存/接收者/RNG/两播放/真实循环/哨兵后额外推进/返回保。两局部I/S各十五实来源，不代生成机器/实际x87/运行或完整递归生产/owner寿命。

语义1492/1487、关联1605/43.61%、BATTLE169枚举open、分类617/E762；R7 260/3061/余2801、差队列172/待归属43、四任务open与60源码closed10/完整验收0/叶0/global_accepted=false保。

## BQF01 四完整技能显示/移动参数局部I/S（2026-10-07）

[限定验收](cg2d_battle_skill_display_motion_sampling_spec.md#battle-skill-display-motion-sampling-acceptance)/[完整633条2207B](cg2d_battle_skill_display_motion_sampling_evidence.json)。十页、45call/22入口/97jump/四ret、四父12邻窗/32B、一真实系数和三源/四原W8可追。技能图值/模式/级别与六RAND点每路径消费、无扣HP/其它图标后仍Attribute差别、W32缓存/上限copy/两条件播放/退场与三正确移动/正常转换保，正常旧值一致。两局部I/S各十二实来源，不代生成机器/实际x87/运行或完整递归生产/owner寿命。

语义1496/1491、关联1609/43.72%、BATTLE173枚举open、分类621/E764；R7 260/3061/余2801、差队列172/待归属43、四任务open与60源码closed10/完整验收0/叶0/global_accepted=false保。

## BQG01 七完整工厂/批次显示/退出局部I/S（2026-10-07）

[限定验收](cg2d_battle_factory_batch_exit_sampling_spec.md#battle-factory-batch-exit-sampling-acceptance)/[完整704条2305B](cg2d_battle_factory_batch_exit_sampling_evidence.json)。十三页、47call/22入口/120jump/七ret、七父四窗实际20条/69B及三既有表项可追，256父调用后落尾，无额外jmp。只249一次event与255归零/三W8表达式对四原点共五源阶段，正常旧值一致；工厂30类型/31父case及118默认、251两齐读临时/253后取phase与保存/254回收/256两写、255实际counter1DC/固定effect与fresh C9C差别/W32及上限copy/RNG/播放尾保持。两局部I/S各十五实际来源，不代生成机器/运行或完整递归生产/owner寿命。

语义1503/1498、关联1616/43.91%、BATTLE180枚举open、分类628/E766；R7 260/3061/余2801、差队列172/待归属43、四任务open与60源码closed10/完整验收0/叶0/global_accepted=false保持。

## BQH01 六完整参战/朝向/名字与推进局部I/S（2026-10-07）

[限定验收](cg2d_battle_participant_turn_name_sampling_spec.md#battle-participant-turn-name-sampling-acceptance)/[完整171条467B](cg2d_battle_participant_turn_name_sampling_evidence.json)。六页、13call/8callee/19jump/六ret、八真实入向/八窗24条107B及父返回比较/EBP=1局部依据可追。两FDC/两玩家旗位四低W8、两阵营共享投影四S32读序/F32 Y后X发布，共五片段/三体，正常旧值一致。前20槽共享span、先10..19后重读位再0..9、调用后fresh目标、名字测量前Y后X/颜色继承及两推进正确体保；父switch只邻近观察，不新R7或父完整验收。两局部I/S各十七实际来源，不代生成机器/实际x87/运行或完整递归生产/owner寿命。

语义1509/1504、关联1622/44.08%、BATTLE186枚举open、分类634/E768；R7 260/3061/余2801、差队列172/待归属43、四任务open和60源码closed10/完整验收0/叶0/global_accepted=false保持。

## BQI01 单完整命令目标选择局部I/S（2026-10-07）

[限定验收](cg2d_battle_command_target_width_sampling_spec.md#battle-command-target-width-sampling-acceptance)/[完整266条901B](cg2d_battle_command_target_width_sampling_evidence.json)。四页、7call/2callee/63jump/三ret、六UI父八入向/八窗40条141B和20项原栈矩阵有据。四取样片段/六低W8表达式对应七原点，正常旧值一致，原W32参数/全局/OR-AND写、HP先比较再bit2/共享重取、十槽选择与排除、400短路/最后匹配与fresh前排别名、原0/1返回保持。两局部I/S各十七实来源，不代完整参数生产/owner寿命/递归父或生成机器/运行。

语义1510/1505、关联1623/44.10%、BATTLE187枚举open、分类635/E770；R7 260/3061/余2801、差队列172/待归属43、四任务open和60源码closed10/完整验收0/叶0/global_accepted=false保持。

## BQJ01 三完整选择清理/命令入口局部I/S（2026-10-07）

[限定验收](cg2d_battle_selection_cleanup_entry_sampling_spec.md#battle-selection-cleanup-entry-sampling-acceptance)/[完整145条467B](cg2d_battle_selection_cleanup_entry_sampling_evidence.json)。三页、8call/5callee/14jump/四ret、45代码入向45窗135条693B及1数据xref/实际32B prototype/15B旧字节有据。两文件四借用阶段，正常旧值一致；首缓存/后两fresh槽、三阶段标死清槽与空分支、九S32/三个W32关闭、callback原正确图值/优先级/选择链与返回保持。当前窗口26首元素callback原型/发布/消费有据，实际字段56B72C，结构头type1，prototype/runtime两布局不混；两局部I/S各十七实来源，不代完整窗口/全部UI父/owner寿命/生成机器或运行。

语义1513/1508、关联1626/44.18%、BATTLE190枚举open、分类638/E772；R7 260/3061/余2801、差队列172/待归属43、四任务open及60源码closed10/完整验收0/叶0/global_accepted=false保持。

## BQK01 四完整快捷/防御局部I/S（2026-10-07）

[限定验收](cg2d_battle_shortcut_defense_state_sampling_spec.md#battle-shortcut-defense-state-sampling-acceptance)/[完整197条767B](cg2d_battle_shortcut_defense_state_sampling_evidence.json)。四页、20call/7callee/27jump/五ret，快捷九代码入向九窗27条119B及防御1数据xref/第二32B行/15B旧hover/2B原G有据。三个输入状态低W8→S8读、共享W32/原门槛/各专属尾/真实创建结果和回收声返回、防御正确图优先级/原字节/原格式明文G保持；正常旧值一致。两局部I/S各十七实来源，不代完整UI父/窗口26、生产与owner寿命/组包连接发送/生成机器或运行。

语义1517/1512、关联1630/44.29%、BATTLE194枚举open、分类642/E774；R7 260/3061/余2801、差队列172/待归属43、四任务open与60源码closed10/完整验收0/叶0/global_accepted=false保持。

## BQL01 三完整按钮再操作局部I/S（2026-10-07）

[限定验收](cg2d_battle_button_reopen_click_sampling_spec.md#battle-button-reopen-click-sampling-acceptance)/[完整359条1191B](cg2d_battle_button_reopen_click_sampling_evidence.json)。六页、21call/8callee/45jump/十二ret、三数据xref/三32B原型与11-11-21共43B旧hover有据。三个已选分支鼠标掩码2低W8读实修，共享W32与原W8标记、技能重选/重建/关闭返回、物品flag重开与Flush、宠物W32关闭请求、初次点击模式写序/真实创建空结果和图先尾清/返回保持；正常旧值一致。两局部I/S各十五实来源，不代完整UI/生产与owner寿命/生成机器或运行，已过通用链不重验。

语义1520/1515、关联1633/44.38%、BATTLE197枚举open、分类645/E776；R7 260/3061/余2801、差队列172/待归属43、四任务open与60源码closed10/完整验收0/叶0/global_accepted=false保持。

## BQM01 技能窗口与列表局部I/S（2026-10-07）

[限定验收](cg2d_battle_skill_window_click_sampling_spec.md#battle-skill-window-click-sampling-acceptance)/[完整573条1739B](cg2d_battle_skill_window_click_sampling_evidence.json)。十二页22call/13callee/50普通jump/一七项switch/13ret、六外父邻窗36条115B与54B首NUL旧文本有据；47C570两W8索引/四W16坐标取样顺序实修，W16索引/W32坐标与flags、其余七体和原失败/null/RNG/回调/完成槽保持。37B X七W32目标加九NOP用途mapped不等新函数/排除；107包装入向仅登记。两局部I/S各十七真实来源，不代完整生产/父/owner寿命、窗口10-26/运行验收。R7 261/3061余2800、五任务open/60源码closed10/完整验收0/叶0/global=false；关联1641/44.59%、BATTLE205枚举open/E778保持限定口径。

## BQN01 技术选择与命令消费局部I/S（2026-10-07）

[限定验收](cg2d_battle_technique_selection_sampling_spec.md#battle-technique-selection-sampling-acceptance)/[三完整313条1108B](cg2d_battle_technique_selection_sampling_evidence.json)。两体四源片段/第三正确体保，三FP比较、CC4230三写一读/双S32压栈CALL EAX、原提示83B与S格式11B、一个外父六条13B有据；不把寄存器操作数0当函数地址、不重验已过两发布者或合并发送后写序。989历史字段保/正常旧值一致；两局部I/S各十六真实来源，不代完整内容生产/所有模式/父/owner及运行。两新ID加989三处置/BATTLE208枚举open/E780，关联1643/44.65%；四任务open/60源码closed10/完整验收0/叶0/global=false与R7/差队列保持。

## BQO01 窗口收尾与初始化局部I/S（2026-10-07）

[限定验收](cg2d_battle_skill_window_lifecycle_sampling_spec.md#battle-skill-window-lifecycle-sampling-acceptance)/[四完整260条837B](cg2d_battle_skill_window_lifecycle_sampling_evidence.json)。三体九片段取序/阶段借用/hover缓存和两W8实修，原W32关闭、初始化正确体/回退W8减计数/正常S32计算与+8别名/42B提示保，正常旧值一致。真实48/32B原型callback字段54E444/561C14不等IDA头；原七表项/42B用途及十二父60条305B限定静态，83助手入向仅登记。两I/S各十六真实来源不代完整生产/其它父/owner寿命/运行；R7 262/3061余2799、五任务open/60源码closed10/完整验收0/叶0/global=false；关联1647/44.76%、BATTLE212枚举open/E782保限定口径。

## BQP01 列表与回退局部I/S（2026-10-07）

[限定验收](cg2d_battle_skill_lists_rollback_sampling_spec.md#battle-skill-lists-rollback-sampling-acceptance)/[七完整393条1373B](cg2d_battle_skill_lists_rollback_sampling_evidence.json)。九页十call/六callee/37普通jump加一外尾转移/七ret、坐标GetVal真实尾别名、cost-first单右FP读与计数RNG有据；四体六片段/一typed桥/两父/三正确体保。普通值和原FP单读副作用同旧源，原MIS明文/回调/失败/负maximum及原序保持。两I/S各二十真实来源不代全解码/异常/父/owner/运行；14精确文件，1654/44.95%文档关联、BATTLE219枚举open/E784、四任务open/60源码closed10/完整验收0/叶0/global=false和R7/差队列保持。

## BQQ01 关闭片段与区间用途局部I/S（2026-10-07）

[限定验收](cg2d_battle_window_release_range_sampling_spec.md#battle-window-release-range-sampling-acceptance)/[42条145B与整236B用途](cg2d_battle_window_release_range_sampling_evidence.json)。一制造体两片段修复发布/取样序，另一正确体、两48B原型真实release绑定及局部弃返适配保持；六实际CALL/九分支/两RET有据。两个code_identity不增F分母/219成员，两个X mapped只证字节用途；两I/S各15真实来源不代通用回收/全部dispatch/数据生产/父/owner寿命或运行。五任务open/60源码closed10/完整验收0/叶0/global=false，音频暂停及不构建测试运行摘要限制保。

## BQR02 技能与动作状态局部I/S（2026-10-07）

[限定验收](cg2d_battle_skill_state_producer_sampling_spec.md#battle-skill-state-producer-sampling-acceptance)/[十一完整原根](cg2d_battle_skill_state_producer_sampling_evidence.json)，112条361B/十一页、七call五callee/十jump十一ret；两体三片段恢复四原W8/W16点，九正确体/原低S8返回、短路/常量范围、Y-X单Read和计数RNG/fresh socket/明文/发送后发布序保持。两个网络前缀只证明原格式推送，六callback来源/71入向只登记，完整封包/consumer/数据生产/owner寿命/运行未据本批验收。两I/S各24真实来源，四任务open/60源码closed10/完整验收0/叶0/global=false与R7保持；12精确文件，音频及验证限制保。

## BQS01 宠物消费/费用行局部I/S（2026-10-07）

[限定验收](cg2d_battle_pet_skill_selection_sampling_spec.md#battle-pet-skill-selection-sampling-acceptance)/[六完整654条2060B](cg2d_battle_pet_skill_selection_sampling_evidence.json)，十一页39CALL（36直接/13callee和三间接）、40jump/九RET；五体13片段恢复取样/低W8/八W16及原FP callee桥，正确>=叶/每次单读与计数RNG、普通fresh/骑宠held/null收尾和返回/无声差别/格式与图序保。两个旧记录完整续记且保原业务/历史，三数据绑定/十槽store/首NUL及表有据；局部DrawText Word消费不代全renderer/全部父/数据生产/owner/运行。两I/S各23实际来源，四任务open/60源码closed10/完整验收0/叶0/global=false和R7保持；12精确文件，音频及验证限制保。

## BQT01 命令按钮与消费局部I/S（2026-10-07）

[限定验收](cg2d_battle_pet_command_button_sampling_spec.md#battle-pet-command-button-sampling-acceptance)/[三完整304条1152B](cg2d_battle_pet_command_button_sampling_evidence.json)，六页34直接call十callee/24jump六ret；按钮两片段恢复原取样/两个W8和条件发布，两个正确command体/原negative与Check0-非0/target及阈值/映射、原256B/13B packet/明文、重复声/不同计数发布/Close与末phase2保持。两旧格式历史及业务保/当前状态同步，真实元素8/32B字段和八PUSH/30B提示有据；两I/S各21来源不代全keyboard/目标后发送/全部父/数据生产/owner/运行。四任务open/60源码closed10/完整验收0/叶0/global=false和R7保持；12精确文件，音频及验证限制保。

## BQU01 主分发与两叶局部I/S、三表用途（2026-10-07）

[限定验收](cg2d_battle_command_state_dispatch_sampling_spec.md#battle-command-state-dispatch-sampling-acceptance)/[三完整988条3638B](cg2d_battle_command_state_dispatch_sampling_evidence.json)，14页92物理CALL=91direct/24callee+1IAT、137jump/三switch/四RET；两体18位置/15真实W8、两cost-first/一个W32谓词阶段修复，原256B/16B和格式/明文、重复N与fresh socket/finish/MarkDeath清槽、提示12B→取点击→3B/图尾与状态写序保持。两旧业务历史与一正确提示源纳管；62B X实际用途，三处置/242open成员和两I/S各23来源不代上游/全生产/全部owner/运行。五任务open、60源码closed10/完整验收0/叶0/global=false；13精确文件、音频及验证限制保。

## BQV01 目标与高亮局部I/S、四填充用途（2026-10-07）

[限定验收](cg2d_battle_target_hover_sampling_spec.md#battle-target-hover-sampling-acceptance)/[四完整287条872B](cg2d_battle_target_hover_sampling_evidence.json)，六页16直接call两callee/36jump十二RET、46实际代码入向/16父只登记；两consumer四片段/共用BYTE桥一处、同held DX表/三个承载桥与opaque声明落实，原row/bit优先/邻位图序/返回码/四lowW16矩形/严格W16活宠及Reset9正确体保。四40B NOP用途mapped，四处置/246open成员/两局部I/S各22真实来源与五任务不代全生产/其它父/owner/运行；60源码closed10/完整验收0/叶0/global=false，14精确文件与音频/验证限制保。

## BQW01 初始化与消息生产局部I/S（2026-10-07）

[限定验收](cg2d_battle_command_init_packet_producer_sampling_spec.md#battle-command-init-packet-producer-sampling-acceptance)/[四完整164条542B](cg2d_battle_command_init_packet_producer_sampling_evidence.json)，五页七call（六direct四callee+一IAT）/十六jump八RET；两体三片段/原W32/S32 SAR及C-M word-copy-next-tail-copy-CMP-pub、P-A旧格式/24B原字节和真实W32输出/partial失败后续/严格pending1、20槽四1000H槽及两正确体保。四18B NOP用途mapped，四处置/250open成员/两I-S各22真实来源不代全分派/生产/owner/运行；五任务open/60源码closed10/完整验收0/叶0/global=false，13精确文件及音频/执行限制保持。

## BQX01 入口与战果局部I/S（2026-10-07）

[限定验收](cg2d_battle_entry_result_producer_sampling_spec.md#battle-entry-result-producer-sampling-acceptance)/[三完整323条1177B](cg2d_battle_entry_result_producer_sampling_evidence.json)，六页34directcall十九callee/24jump五RET、三真实父局部入向；两主根11片段/四载体与类型谓词/结果field和held判负、一次跨W32桥/布局assert及原4-3-15、W16/W32/RNG/文本门禁/负值零写保。正确W16叶/原caller覆盖EAX及两X7B用途有据；三处置/253open成员/两I-S各21真实来源与五任务不代全分派/owner/CRT-SDK-ABI/运行，60源码closed10/完整验收0/叶0/global=false，12精确文件与音频/执行限制保。

## BRS01 宿主客户端可执行目标（2026-10-07）

[目标SPEC](cg2d_hosted_client_executable_target_spec.md#hosted-client-target-source-state)纳入FUNC-STARTUP基线，新增局部I/S两条E-HOSTED-CLIENT-TARGET，证据800→802。唯一真实WIN32目标/ClientEntry归属与startup-render依赖已接，原三库、WinMain本体及宿主活动初始化保持；没有私有入口、整表重调或占位函数。C02/C04/V02只加本批引用且仍open。函数/语义/处置/成员/R7及完整feature gates不改；源码目标存在不代替链接、运行或整体启动退出验收。源变更和完整前态见[证据](cg2d_hosted_client_executable_target_evidence.json)。

## BRT01 版本正文正常零足迹owner（2026-10-07）

[局部SPEC](cg2d_version_payload_zero_prefix_owner_spec.md#version-zero-prefix-source-state)纳入FUNC-CHARACTER，E802→804仅两局部I/S，各12真实来源。原未装载零值／全部两次只读外传／真实首BYTE零分支对应唯一const char[1]静态owner，新端容量裁决与未知原未使用预留长度分开；不增语义F/处置/成员或R7，不改网络本体和原两个父。U03源码closed，60任务源码closed10→11，完整功能／叶验收0和global=false保持；V03与完整角色门禁未验。见[证据](cg2d_version_payload_zero_prefix_owner_evidence.json)。

## BRU01 登录负分类BYTE owner（2026-10-07）

[局部SPEC](cg2d_login_negative_selection_byte_owner_spec.md#login-negative-byte-source-state)纳入FUNC-LOGIN，E804→806两局部I/S各17来源。已有缺provider唯一定义与固定池只读桥已接，20点直接真实owner／C6加载生产／七坐标BYTE与五原链接身份BYTE承接，原handle、所有父和生产者/布局/RNG保持。V03仅加当前实现引用且仍open，源码closed11、语义1571/有效1564、R7 275、所有原成员/处置/完整feature门禁/global结论保持；不把派生桥造为原F，完整登录/互通/生命周期未验。见[证据](cg2d_login_negative_selection_byte_owner_evidence.json)。

## BRV01 角色列表原语与共享种子局部开发验收（2026-10-07）

[基线](cg2d_character_list_record_production_callees_spec.md#character-list-callees-baseline)、[裁决](cg2d_character_list_record_production_callees_spec.md#character-list-callees-decision)、[验收](cg2d_character_list_record_production_callees_spec.md#character-list-callees-acceptance)及[源码状态](cg2d_character_list_record_production_callees_spec.md#character-list-callees-source-state)关联两完整原根与真实源修复。五私有副本删除，恢复四次现场593850种子前缀复制、result谓词取样及原52D2F0／17个52CFA0／外4×1023和末1×63的52CED0；两组／记录布局／全部写序、原失败／返回／Daj明文格式保持。

两个normal F／两CHARACTER成员、NETWORK仅回复父绑定；两真实X共21B仅原nop_padding用途。新增CHARACTER局部I／S各十九真实来源，E806→808；原功能gates空、member_inventory_closed=false保持。U10／U11／V03只补当前状态与引用、仍open，源码closed仍11，不代完整原语／数据owner／所有人物与网络状态／生命周期／资源／SDK-ABI／互通及运行验收。名义通知尾真实空RET，不制造业务效果；依RPLAN.24不新增故障／回绕模型或保护。

## BRW01 三成功回复局部开发验收（2026-10-07）

[基线](cg2d_login_character_reply_byte_sampling_spec.md#reply-byte-sampling-baseline)、[裁决](cg2d_login_character_reply_byte_sampling_spec.md#reply-byte-sampling-decision)、[验收](cg2d_login_character_reply_byte_sampling_spec.md#reply-byte-sampling-acceptance)及[源码状态](cg2d_login_character_reply_byte_sampling_spec.md#reply-byte-sampling-source-state)关联三完整114条341B与六源码片段。回复U8先读／同值判NUL恢复，原正确正常值、状态／回复／计时及创建字段发布和失败保持。三已有hPUd／DZ／oy入口仅局部绑定，不补协议／SDK或推断寄存器业务返回。

三normal F／六真实功能成员、三个原X27B仅nop_padding用途；LOGIN和CHARACTER各局部I／S共四项，每项十九真实来源，E808→812，完整gates／成员枚举保持open。U10／V03仅当前／引用，仍open；源码closed11与完整验收0不变，不代全部正常owner／数据生产消费／原语／全分派／资源寿命／CRT-SDK-ABI／明文互通／运行。RPLAN.24与音频暂停保持。

## BRX01 角色选择母体局部开发验收（2026-10-07）

[基线](cg2d_character_select_dispatch_sampling_spec.md#character-dispatch-sampling-baseline)、[裁决](cg2d_character_select_dispatch_sampling_spec.md#character-dispatch-sampling-decision)、[验收](cg2d_character_select_dispatch_sampling_spec.md#character-dispatch-sampling-acceptance)及[源码状态](cg2d_character_select_dispatch_sampling_spec.md#character-dispatch-sampling-source-state)关联完整721条2695B／四真实switch及一源六补丁。图号原门禁／mapper取样、正常S32 selector、删除／入地图S16与版本尾一次标志、case作用域落实，原正常有效值／成功失败／状态／返回和共享owner保。末PUK低W8与物理足迹边界明确，不夸称新验所有41callee或ABI。

一个normal F／一个CHARACTER成员、一185B混合X分八块仅原表／填充用途；两局部I-S各二十一实际来源，E812→814。原成员枚举／完整gates空、源码closed11与完整0保持，U10／U11／V03只current／refs仍open；正常alias／资源寿命／其它场景／SDK-ABI／原格式明文互通／画面运行仍逐项推进。音频暂停、RPLAN.24及执行限制保持。

## BRY01 主地图数值初始化局部开发验收（2026-10-07）

[基线](cg2d_main_entry_state_carrier_sampling_spec.md#main-entry-carrier-baseline)、[裁决](cg2d_main_entry_state_carrier_sampling_spec.md#main-entry-carrier-decision)、[验收](cg2d_main_entry_state_carrier_sampling_spec.md#main-entry-carrier-acceptance)及[源码状态](cg2d_main_entry_state_carrier_sampling_spec.md#main-entry-carrier-source-state)关联两完整211条883B、真实三chunk与一体八源片段。七tuple先读齐／C821E8两pub后count／末独立第三对象，原构析RNG与状态／vft-padding／返回保；第二体正确正文保持，六清区／选槽／恢复／数组和数据owner可追。原EH尾仅归属元信息，不扩故障门禁或称SDK-EH运行通过。

两个normal F／MAP两成员，两X7B原填充用途，局部I-S各二十一真实来源，E814→816。完整gates／成员枚举、源码closed11与完整0保持；U09／U10／U11只current／引用仍open，全部正常alias／owner寿命、生产消费／资源／所有callee／主地图流程／SDK-ABI／明文互通／运行继续逐项开发验收，音频暂停和RPLAN.24保。

## BRZ01 登录进度布局局部开发验收（2026-10-07）

[基线](cg2d_login_progress_layout_sampling_spec.md#login-progress-layout-baseline)、[裁决](cg2d_login_progress_layout_sampling_spec.md#login-progress-layout-decision)、[验收](cg2d_login_progress_layout_sampling_spec.md#login-progress-layout-acceptance)与[源码状态](cg2d_login_progress_layout_sampling_spec.md#login-progress-layout-source-state)对应三根176条565B和四源片段。原字节长度与两种原公式、rows／y再长度／columns／x现场消费、非零轮询一次对象／死亡后清槽恢复；W16阶段与父成功失败／原字节消息和现网络格式保持。

新父F与两个已有F补全分开计量，正常处置加三、LOGIN加三；三X27B仅原填充用途，两局部I-S各18个真实来源，E816→818。成员枚举与完整门禁、源码closed11／完整功能叶0保持，U10／V03仅current与引用同步、仍open。局部正常源码验收不代资源与对象寿命、全部callee、真实明文互通或运行／R8；当前执行限制继续适用。

## BSA01 主地图控制器局部开发验收（2026-10-07）

[基线](cg2d_normal_scene_controller_callee_order_spec.md#normal-scene-callee-baseline)、[裁决](cg2d_normal_scene_controller_callee_order_spec.md#normal-scene-callee-decision)、[验收](cg2d_normal_scene_controller_callee_order_spec.md#normal-scene-callee-acceptance)与[源码状态](cg2d_normal_scene_controller_callee_order_spec.md#normal-scene-callee-source-state)关联两个完整924条3782B原根与控制器八片段。六原空叶位置恢复／两个mask-encoded-W8先读齐发布，原状态／调用、2construct-destroy／RNG及目的vft-padding保；直接初始化正确正文保持，实际数据生产消费局部可追。

两个正常F／MAP两成员，两X362B仅原目标表／值表及填充用途，局部I-S各19项真实来源、E818→820。完整gates与枚举、源码closed11／完整功能叶0保持，U09／U11仅current／refs仍open；全部正常alias-owner寿命／资源像素／地图重进与所有传递provider、SDK-ABI及明文互通／运行R8继续逐项推进，音频暂停和RPLAN.24保持。

## BSB01 整数左侧比较的具名开发验收（2026-10-07）

[基线](cg2d_integer_left_compare_dispatch_spec.md#integer-left-compare-baseline)、[裁决](cg2d_integer_left_compare_dispatch_spec.md#integer-left-compare-decision)、[验收](cg2d_integer_left_compare_dispatch_spec.md#integer-left-compare-acceptance)与[源码状态](cg2d_integer_left_compare_dispatch_spec.md#integer-left-compare-source-state)关联完整5条17B原叶、八AL条件和五源12片段。具名体一次右读取、S32 lhs<right覆盖AL／保高24位，八原点显式低W8；既有单读副作用／原鼠标与action对象、短路和原其余正文保持，旧正常条件结果正确。

语义82补原体／1297仅局部两点，不增语义或文档计数；单adapted／MAP及INPUT各一成员，四局部I-S各18来源、E820→824。已审X与R7不重复变更，R01／R04／U11只current／refs、仍open；完整gates／成员枚举、源码closed11与完整功能叶0保持，未知alias／所有数值入口与父流程、CRT-SDK-机器ABI／正常owner寿命／明文互通及运行R8继续原标准。

## BSC01 能力4正常算式开发验收（2026-10-07）

[基线](cg2d_capability4_input_normal_arithmetic_spec.md#capability4-arithmetic-baseline)、[裁决](cg2d_capability4_input_normal_arithmetic_spec.md#capability4-arithmetic-decision)、[验收](cg2d_capability4_input_normal_arithmetic_spec.md#capability4-arithmetic-acceptance)与[源码状态](cg2d_capability4_input_normal_arithmetic_spec.md#capability4-arithmetic-source-state)关联三原根616条2058B与两源九片段。三私有体改普通S32、六上界接现401DB0先算阈值再单次读，原BSB下界／同鼠标owner和短路、组合／焦点框／低W16及保存槽两轮／图号除余保持；比较库体不改，仅AL条件域不代完整EAX／ABI。

三adapted原F／INPUT三实际成员，两X5B只填充用途，两局部I-S各20实际来源、E824→826；全部完整门禁／枚举、源码closed11／完整功能叶0保持，P05旧closed不重开，U10／R04仅current／refs仍open。数学异常不专项处理／不称修复，完整父／资源寿命、所有其它调用和SDK-机器ABI／明文互通及运行R8继续原标准。

## BSD01 登录控件几何与取样开发验收（2026-10-07）

[基线](cg2d_login_control_normal_geometry_spec.md#login-control-geometry-baseline)、[裁决](cg2d_login_control_normal_geometry_spec.md#login-control-geometry-decision)、[验收](cg2d_login_control_normal_geometry_spec.md#login-control-geometry-acceptance)和[源码状态](cg2d_login_control_normal_geometry_spec.md#login-control-geometry-source-state)关联两完整原根176条439B与单源五片段。普通S32矩形／覆盖±2、width延后与drawY→drawX／覆盖取样阶段恢复，最终低W16及鼠标九位／三别名单读、descriptor16与模式／图号保持。

两adapted原F／INPUT两实际成员，两X9B仅填充用途，两局部I-S各14实际来源，E826→828。U10／R04只current／引用仍open，源码closed11／P05既有closed、完整叶0／globalfalse及全部完整门禁保。正常值原正确、不新数学异常／保护或限制；完整父与依赖、所有别名／owner寿命、SDK-机器ABI及明文互通／运行R8继续原标准。

## BSE01 能力3输入正常算式开发验收（2026-10-07）

[基线](cg2d_capability3_input_normal_sampling_spec.md#capability3-normal-sampling-baseline)、[裁决](cg2d_capability3_input_normal_sampling_spec.md#capability3-normal-sampling-decision)、[验收](cg2d_capability3_input_normal_sampling_spec.md#capability3-normal-sampling-acceptance)与[源码状态](cg2d_capability3_input_normal_sampling_spec.md#capability3-normal-sampling-source-state)关联原301条1068B与单源四片段：两私有普通S32工具、两普通提交先读齐账号／密码+12C后发布result1／两NUL，保低W16／键盘短路与原focus-owner/latch、现屏幕键盘排除和其它整源。

单adapted原F／INPUT一成员、单X4B仅用途，两局部I-S各15实际来源，E828→830；U10／R04只current／引用仍open，源码closed11／P05原closed／完整叶0和所有完整门禁保持。正常值原正确、不新异常或保护，完整45F190父／全部输入绘制消费、alias-owner寿命及SDK-机器ABI／链接、原格式明文互通与运行R8继续原标准。

## BSF01 能力3场景运算与取样验收（2026-10-07）

[基线](cg2d_capability3_scene_normal_sampling_spec.md#capability3-scene-normal-baseline)、[裁决](cg2d_capability3_scene_normal_sampling_spec.md#capability3-scene-normal-decision)、[验收](cg2d_capability3_scene_normal_sampling_spec.md#capability3-scene-normal-acceptance)与[源码状态](cg2d_capability3_scene_normal_sampling_spec.md#capability3-scene-normal-source-state)关联原494条1965B和单源四片段：三私有普通S32工具、状态3先左／右owner再读写透明度W8，保原颜色位型和状态／门槛／失败与寿命顺序及其它整源。

单adapted原F／LOGIN一成员、单X51B三实际部分只用途，两局部I-S各17来源，E830→832。U10／R04只current／引用仍open；源码closed11（P05原closed）／完整叶0／globalfalse与所有完整门禁保。正常值原正确，不新异常／保护或业务；完整World::Update父／依赖消费、全部alias-owner寿命、SDK-机器ABI／链接／画面与明文互通R8继续原标准。

## BSG01 能力4提交取样开发验收（2026-10-07）

[基线](cg2d_capability4_submit_length_sampling_spec.md#capability4-submit-length-baseline)、[裁决](cg2d_capability4_submit_length_sampling_spec.md#capability4-submit-length-decision)、[验收](cg2d_capability4_submit_length_sampling_spec.md#capability4-submit-length-acceptance)与[源码状态](cg2d_capability4_submit_length_sampling_spec.md#capability4-submit-length-source-state)关联既有1582及BSC01原350条1262B。单源两成功块恢复先读齐+12C S32、再result1／两NUL；按钮密码→账号／Enter账号→密码照原保持，不统一顺序或新增限制。

仅旧F／adapted处置／INPUT来源更新，成员26与语义／文档／R7不增；两局部I-S各14实际来源、E832→834。U10／R04只current/引用仍open，源码closed11（P05旧closed）／完整叶0/globalfalse和全部门禁保。完整父、所有正常alias-owner寿命、依赖与SDK-机器ABI／链接／明文互通和运行仍待。

## BSH01 能力4场景取样开发验收（2026-10-07）

[基线](cg2d_capability4_scene_sampling_spec.md#capability4-scene-sampling-baseline)、[裁决](cg2d_capability4_scene_sampling_spec.md#capability4-scene-sampling-decision)、[验收](cg2d_capability4_scene_sampling_spec.md#capability4-scene-sampling-acceptance)和[源码状态](cg2d_capability4_scene_sampling_spec.md#capability4-scene-sampling-source-state)关联原385条1437B与单源八片段：普通S32、一次入口状态／默认返回、计数-phase原序及四fade现场alpha，保原颜色位型和所有其它整源。

单adapted原F／LOGIN一成员、单X83B四真实部分只用途，两局部I-S各17来源，E834→836；U10／R04仅current/引用仍open，源码closed11（P05原closed）／完整叶0/globalfalse及全部门禁保。正常值原正确、不新异常／保护或业务；完整World::Update父／24依赖、输入保存与图像消费、所有正常alias-owner寿命／SDK-机器ABI和链接、明文互通／运行R8继续原标准。

## BSI01 登录模式消息分支开发验收（2026-10-07）

[基线](cg2d_login_mode_normal_message_branch_spec.md#login-mode-message-baseline)、[裁决](cg2d_login_mode_normal_message_branch_spec.md#login-mode-message-decision)、[验收](cg2d_login_mode_normal_message_branch_spec.md#login-mode-message-acceptance)与[源码状态](cg2d_login_mode_normal_message_branch_spec.md#login-mode-message-source-state)关联原161条544B与单源五片段：三现源普通S32++映两原INC、负返回短路直接重置／非负100和随后101独立取样，保一次输入／返回流程、原提示和全部其它正文。

单adapted原F／LOGIN一成员，两局部I-S各14来源，E836→838；无新X和内部块F，R7保持。U10／R04只current/引用仍open，源码closed11（P05原closed）／完整叶0/globalfalse及全部完整门禁保。原正常值正确、不新异常／保护或业务；完整World::Update父／12依赖、消息输入与持久／图像消费、正常alias-owner寿命、SDK-机器ABI／链接、明文互通／运行继续原标准。

## BSJ01 选服取样与图号阶段开发验收（2026-10-07）

[基线](cg2d_server_category_click_sampling_spec.md#server-category-click-baseline)、[裁决](cg2d_server_category_click_sampling_spec.md#server-category-click-decision)、[验收](cg2d_server_category_click_sampling_spec.md#server-category-click-acceptance)和[源码状态](cg2d_server_category_click_sampling_spec.md#server-category-click-source-state)关联原1135条3576B与单源三片段：普通S32状态递增；分类每轮先取样、成功后刷新；图号字段5→4和差值／递增阶段；点击后单页号复用于available／索引／两标题。真实位型、原映像／字节、原门禁及声音摘要账号密码先后与其它完整正文保持。

旧1080补全，不增语义身份或文档覆盖数；新增单adapted／LOGIN一真实成员，两局部I-S各17实际来源，E838→840。单104B原X只登记23目标表和12B真实NOP用途，R7 296；U10／R04只current／引用仍open，P05原closed、源码closed11／完整叶0／globalfalse和全部完整门禁保持。完整World::Update父／20依赖、窗口输入消息和图像消费、全部正常alias-owner寿命、SDK-机器ABI／链接、原格式明文互通与运行继续原标准；没有运行摘要或任何测试。

## BSK01 连接阶段与布局开发验收（2026-10-07）

[基线](cg2d_server_connection_phase_layout_sampling_spec.md#server-connection-phase-layout-baseline)、[裁决](cg2d_server_connection_phase_layout_sampling_spec.md#server-connection-phase-layout-decision)、[验收](cg2d_server_connection_phase_layout_sampling_spec.md#server-connection-phase-layout-acceptance)和[源码状态](cg2d_server_connection_phase_layout_sampling_spec.md#server-connection-phase-layout-source-state)关联原165条607B与单源三片段：失败或未进入2时现场读phase，两互斥源位置对应一个原45DB0E；创建用测宽计算样本与已计算参数，绘制明确row-y-column-x取样及正常几何阶段。原两次列数发布／最小2、成功2／3直达、newest旁路、原清理／返回／字节和其它整源保持。

旧191补全不增语义或文档覆盖数；新增单adapted／LOGIN一成员，两局部I-S各17真实来源，E840→842，单1B区间只原对齐用途。U10／V03／R04只current／证据仍open，P05原closed／closed11／完整叶0／globalfalse和全部完整门禁保持。完整父及范围内12依赖、网络明文往返／人物／显示链、正常alias-owner寿命、CRT-SDK／机器ABI和运行仍按原标准；本步不恢复既定排除或执行测试。

## BSL01 人物列表进度长度与字段阶段开发验收（2026-10-07）

[基线](cg2d_character_list_progress_length_sampling_spec.md#character-list-progress-length-baseline)、[裁决](cg2d_character_list_progress_length_sampling_spec.md#character-list-progress-length-decision)、[验收](cg2d_character_list_progress_length_sampling_spec.md#character-list-progress-length-acceptance)与[源码状态](cg2d_character_list_progress_length_sampling_spec.md#character-list-progress-length-source-state)关联原118条377B和单源四片段：两当前缓冲扫描、初次U32×8右移6＋2在stage发布前算好、原S32创建字段与本地参数；非零轮询原非空才MarkDeath／清槽；绘制rows-y-columns-length-x取样及原算式阶段，两S16保。原普通值正确，不新异常／保护／限制。

单adapted／CHARACTER一真实成员，两局部I-S各17真实来源，E842→844；单7B真实NOP仅对齐用途，不造未变Reset或布局F。U11／V03／R04只current／引用仍open，P05原closed、closed11／完整叶0／globalfalse及完整门禁保持。完整人物父／六依赖、真实请求回复／创建-主地图、输入图像生产消费、正常alias-owner寿命、CRT-SDK／机器ABI与链接、原格式明文往返／显示与运行仍待，局部不外推。

## BSM01 删除进度动态坐标开发验收（2026-10-07）

[基线](cg2d_character_deletion_progress_geometry_spec.md#character-deletion-progress-geometry-baseline)、[裁决](cg2d_character_deletion_progress_geometry_spec.md#character-deletion-progress-geometry-decision)、[验收](cg2d_character_deletion_progress_geometry_spec.md#character-deletion-progress-geometry-acceptance)与[源码状态](cg2d_character_deletion_progress_geometry_spec.md#character-deletion-progress-geometry-source-state)关联原102条325B和单源六片段：两只读文本扫描、U32×9后stage发布再逻辑右移／原S32创建，以本地参数承接；绘制rows与y先取、strlen后columns、最后x，两S16保；轮询非空才MarkDeath／清槽。标准cstring声明和常量移除仅支持原算法，原字节／静态寿命、阶段请求／返回及其它源码保，不造可变缓冲或新异常门槛。

单adapted／CHARACTER一真实成员，两局部I-S各17实际来源，E844→846；单11B真实NOP仅对齐用途。U11／V03／R04只current／引用仍open，P05原closed／closed11／完整叶0／globalfalse及全部完整门禁保持。完整人物父／六依赖、真实删除回复／人物创建-主地图、输入图像消费／正常alias-owner寿命、CRT-SDK／机器ABI与链接、原格式明文往返／显示和运行仍待，局部不外推。

## BSN01 认证失败命中与绘制取样开发验收（2026-10-07）

[基线](cg2d_authentication_failure_sampling_spec.md#authentication-failure-sampling-baseline)、[裁决](cg2d_authentication_failure_sampling_spec.md#authentication-failure-sampling-decision)、[验收](cg2d_authentication_failure_sampling_spec.md#authentication-failure-sampling-acceptance)与[源码状态](cg2d_authentication_failure_sampling_spec.md#authentication-failure-sampling-source-state)关联原113条413B和单源六片段：创建原常量；矩形width-x-减66-y单取-left-bottom-top-right；Play51后action单取／非空死亡清槽；文字y-x／两S16，文字后图像width-y-图号-x／SAR和坐标阶段。原15B提示字节／静态寿命、16B descriptor、hover／DF／两图号与其它正文保，编码名保持未证。

单adapted／CHARACTER一真实成员，两局部I-S各17实际来源，E846→848；单3B真实NOP只对齐用途。U11／V03／R04只current／引用仍open，P05原closed／closed11／完整叶0／globalfalse和完整门禁保持。完整人物父／七依赖、事件输入／渲染生产消费、正常alias-owner寿命、CRT-SDK／机器ABI／链接、人物创建-主地图与明文往返／实际画面／运行仍待；不把字段取样算依赖或整体验收。

## BSO01 两弹窗创建与点击清理对象开发验收（2026-10-07）

[基线](cg2d_confirmation_modal_action_sampling_spec.md#confirmation-modal-action-baseline)、[裁决](cg2d_confirmation_modal_action_sampling_spec.md#confirmation-modal-action-decision)、[验收](cg2d_confirmation_modal_action_sampling_spec.md#confirmation-modal-action-acceptance)与[源码状态](cg2d_confirmation_modal_action_sampling_spec.md#confirmation-modal-action-source-state)关联两原根433条1194B和单源四片段／两体：各创建／失败／LTE借同modalAction与返回值、保共享槽发布；各Play79后独立clickedAction单读供非空判断／死亡清槽。原门槛／命中／hover、警告短路、局部记录／各绘制序号、四文字同缓冲与测宽／绘制、返回及其余正文保持。

两单adapted／CHARACTER两真实成员，两局部I-S各18真实来源，E848→850；双7／15B只原对齐用途，不造wrapper或记录F。U11／V03／R04只current／引用仍open，P05原closed／closed11／完整叶0／globalfalse与完整门禁保持。完整两父／合计十依赖、全部共享弹窗正常alias-owner寿命、事件输入／文本图像消费、CRT-SDK／机器ABI／链接及人物创建-主地图／明文互通／实际画面／运行仍待，局部不外推。

## BSP01 公共按钮几何与字段取样开发验收（2026-10-07）

[基线](cg2d_modal_button_geometry_sampling_spec.md#modal-button-geometry-baseline)、[裁决](cg2d_modal_button_geometry_sampling_spec.md#modal-button-geometry-decision)、[验收](cg2d_modal_button_geometry_sampling_spec.md#modal-button-geometry-acceptance)及[原证据](cg2d_modal_button_geometry_sampling_evidence.json)对应两个真F、五片段整源增量、两个局部I-S／各16实际来源、双13B用途分类及U11／V03／R04引用仍open。核四集中静态、汇总器／-Check终态、完整派生快照与编码／引用；完整门禁、P05原closed和其它对象保。

116条275B／两页、4直接／三目标、520CALL／七引用页不等于全部父验收。命中与绘制取样次序分别恢复，hover后重新取记录／text-Y-X，普通S32与实际窄边界、三鼠标BYTE及结果位保持。完整父／三依赖、真实事件／文字图像生产消费、全部owner寿命、CRT-SDK／机器ABI／链接、人物创建-主地图／明文互通／实际画面／运行R8仍待。

## BSQ01 人物模态共享对象与原LTE0开发验收（2026-10-07）

[基线](cg2d_character_modal_action_gate_spec.md#character-modal-action-gate-baseline)、[裁决](cg2d_character_modal_action_gate_spec.md#character-modal-action-gate-decision)、[验收](cg2d_character_modal_action_gate_spec.md#character-modal-action-gate-acceptance)及[原证据](cg2d_character_modal_action_gate_evidence.json)对应单真F、两片段整源增量、两局部I-S／各18实际来源、九字节用途分类和U11／V03／R04引用仍open。四集中静态、measure／-Check终态、完整派生快照／编码／引用／差异记录，P05原closed与全部完整门禁保持。

238条663B／四页、14直接／八目标、唯一实际caller、五字符串原字节及创建返回／LTE0／声音后独立对象／非空标死清槽点为局部证据；Ready>=1普通真假虽等价，恢复原401DA0，不外推比较异常。完整事件／文字图像消费、人物父／八依赖／全部正常owner寿命、CRT-SDK／机器ABI／链接、创建-主地图／原格式明文互通／画面／运行R8仍待。

## BSR01 创建请求进度三个布局阶段开发验收（2026-10-07）

[基线](cg2d_creation_progress_layout_sampling_spec.md#creation-progress-layout-baseline)、[裁决](cg2d_creation_progress_layout_sampling_spec.md#creation-progress-layout-decision)、[验收](cg2d_creation_progress_layout_sampling_spec.md#creation-progress-layout-acceptance)和[原证据](cg2d_creation_progress_layout_sampling_evidence.json)对应单真F／三片段整源增量、两局部I-S各18来源、单字节用途和U11／V03／R04引用仍open。四集中静态、measure／-Check终态、完整派生快照／编码／引用／差异后记结果，P05原closed与所有完整门禁保。

创建几何先算后发布，显示rows／算高／y与strlen／columns／units／算宽／x按原阶段；正确的GE1、五S32、11B文案与动态长度、请求轮询／正常分支保持。原123条399B／两页／六callee及唯一实际caller不代完整controller、正常owner寿命、网络／CRT-SDK／机器ABI／显示／运行R8。

## BST01 创建初始化对象与BYTE发布开发验收（2026-10-07）

[基线](cg2d_creation_initialization_sampling_spec.md#creation-initialization-baseline)、[裁决](cg2d_creation_initialization_sampling_spec.md#creation-initialization-decision)、[验收](cg2d_creation_initialization_sampling_spec.md#creation-initialization-acceptance)及[原证据](cg2d_creation_initialization_sampling_evidence.json)对应两个真F、四片段整源增量、两局部I-S各19实际来源、双14B用途及U11／V03／R04引用仍open。四集中静态、measure／-Check终态、完整派生快照／编码／引用／差异后记结果，P05／P08原closed、pending7整个原记录及全部完整门禁保。

两根41条178B／两页与三个实际入向；controller130条464B／两页仅上下文不混计。清理后++、Puk样本先0／条件1、创建返回判空／+30与原后两字段各共享槽重读分别关联原点。两个slots／原三W8和S32字段、失败／输入尾保持；完整controller／三callee、全部owner寿命与输入／图像消费、CRT-SDK／机器ABI／链接、创建-主地图／明文往返／画面／运行R8仍待。

## BSX01 旧分隔符扫描与人物实际消费开发验收（2026-10-07）

[基线](cg2d_legacy_separator_byte_sampling_spec.md#legacy-separator-byte-baseline)、[裁决](cg2d_legacy_separator_byte_sampling_spec.md#legacy-separator-byte-decision)、[验收](cg2d_legacy_separator_byte_sampling_spec.md#legacy-separator-byte-acceptance)及[原证据](cg2d_legacy_separator_byte_sampling_evidence.json)对应单真F／单片段整个Input源增量、INPUT及CHARACTER各真实成员／各局部I-S共四项、每项15实际来源、7B用途及U11／V03／R04引用仍open。四集中静态、measure／-Check终态、完整派生快照／编码／引用／差异记录；P05／P08原closed及全部完整门禁保持。

25条57B／一页、十CALL七父为原语及消费证据；同U8供NUL与类别、下一BYTE先取后推进，原规则／文本和返回保持。原姓名常量2000／814000只证保持项，不伪报字符／编码修复；完整所有父／输入与输出数据／全部owner寿命、CRT-SDK／机器ABI／链接、人物创建-主地图／原格式明文／画面／运行R8未验。

## BSY01 人物预览共享槽开发验收（2026-10-07）

[基线](cg2d_character_preview_slot_sampling_spec.md#character-preview-slot-baseline)、[裁决](cg2d_character_preview_slot_sampling_spec.md#character-preview-slot-decision)、[验收](cg2d_character_preview_slot_sampling_spec.md#character-preview-slot-acceptance)及[原证据](cg2d_character_preview_slot_sampling_evidence.json)对应单真F／一既有体／两片段、CHARACTER单成员、两局部I-S各23实际来源、单24B原用途及U11／V03／R04引用仍open。原P05／P08 closed及全部完整门禁保持。

变体余数／共享槽／商／图号次序及五字段独立读槽恢复，优先级写后计算x；原局部New返回仍供初次图号，不把全部action一律改为读槽。187条完整728B、四图号表和三个实际CALL只证本步生产消费；所有父／依赖、正常alias-owner寿命、机器ABI／链接、原格式明文互通与运行R8仍待。集中静态及派生终态结果随唯一进度追加。
