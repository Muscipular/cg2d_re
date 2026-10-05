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
