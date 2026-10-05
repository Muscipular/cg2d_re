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
