# 图号探测等级与调色板选择 SPEC（PCR01）

2026-10-11。沿PTR01实际调色板依赖，补45C970图号探测完整根和既有450B70选择根，落实9A7F64的已证生产来源。依子AGENTS第8条委托先记本SPEC再修两处，不从名称猜产品版本、权限服务或资源包业务。

<a id="palette-capability-baseline"></a>
## 原端证据与范围

| 原根 | 完整区间 | 指令/字节 | 原入向 |
|---|---|---:|---|
| 45C970 / ProbePaletteCapability_45C970 | 45C970–45CA1B | 62/171B | 十CALL/九父 |
| 450B70 / SelectPalette | 450B70–450BAD | 16/61B | 四十七CALL/十六父 |

两完整根78条232B/两页，2E8唯一45A8B0、九跳五RET。[原指令与调用后邻窗](cg2d_palette_capability_selection_spec.md#evidence-archive-20261011)保两根及引用。十probe调用中九处将EAX发布9A7F64，一处458113只判负；六指令邻窗仅支撑返回消费来源，不能替代九完整父/全部实参、动态别名或生命周期。

probe在栈上依序写五S32图号99657、245151、22311、22212、22002。本地output原未预置值；每次GetGraphicMapNoIndex实际直接从图号映射表读W32写出参，返回1沿GRP01完整旧根复用，不重复全根。首轮由0查询到第一个非零输出，原TEST后JA只与零比；五项全空返回-1。命中后第二轮从同一个index重新查询，若该次已空才继续后项，末有效项返回4-index，否则-1。现同两轮/重查/返回正确，但output额外初始化为0。

Select先按S32 index>=39拒绝返回index；否则取130B记录的S16 threshold，与当次S32 capability有符号比较，超门槛仍返回index。原没有负索引保护。接受先发布active=index，再取S32 steps，原TEST同局部在steps全局发布之前；非正时全局步数覆盖1，返回仍原steps。现局部判断写在全局发布之后，需明确原判定阶段。

45物理记录的S16 threshold/path与现初表逐项相同，0..38为选择上界内、39..44仍是其它读取可见物理项，不把选择上界改为45。原完整130B取证不等于新端未写path填充冻结，也不宣全表运行不可改。本文只按实际探测将共享槽称为图号探测等级(-1/0..4)，不定义产品版本业务。

PCP01完整UpdatePalette/RebuildPalette32与GRP01映射表查询和生产合同保持，资源内容/所有父/主表owner/其它读取者不据本步验收。当前九原生S32发布点和一判负route沿已存在源码，不新增影子能力或重新获取资源。

<a id="palette-capability-decision"></a>
## 方案及裁决

1. Probe只移除mappedGraphicNo原无的零初始化；同一栈局部交每次真实查询写出参。callee完整旧证已证明必写，不添加查询返回检查、默认值、容量限制或一轮缓存。
2. Select先算局部nonPositiveSteps，再发布steps，持局部决定原可选全局1覆盖；拒绝返回index、接受返回原steps和active发布时点保持，不变成功布尔或钳返回。
3. 两定义、两声明、一个gPaletteCapability说明共五注释，四个源文件精确回放。非地址仍S32/S16、45记录布局/两provider现函数和原初表保持；仅确认已证来源，更新过时待取证说明。
4. 新Probe一F，旧171仅追加合同/两来源，两个新adapted；MAP/RENDER各两成员，两个限定S零I，八任务仍open。
5. 不改PCP01数值、CRT、WAV-only、原命令/字段/ASCII |及LF明文，不增加异常模型/新钳制、空/索引/容量保护、回滚、owner、接口或依赖。

<a id="palette-capability-acceptance"></a>
## 本步静态验收

两根指令与字节连续、E8目标、五图号栈常量/两轮同index重查、原未预清局部、-1/4-index各返回、Select S32上界/S16门槛/steps先判后发和原返回可追。十结果邻窗九发布一判负、45初表逐项对照仅本族，不扩大全部父/动态生命周期。

四整源精确回放仅两组两处/五注释，Lookup及45初表/三个既有完整语义保持；新Probe与旧171追加、两处置/两限定证据/六文档和八任务引用，派生仅六指标/两F/两功能/八任务，其它门禁保持。

语义2340=2292F关联+48有界代码，关联2402/3680=65.27%仅关联率，处置1805/证据1610，MAP193/RENDER120。184叶验收0、60任务关闭11开放49及六closure保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

<a id="palette-capability-source-state"></a>
## 当前状态与剩余

Probe实体在GraphicInfo.cpp，共用映射表写出参；Select在Palette.cpp，共享能力值来自当前实际调用点。旧资源来源对9A7F64只列writer的历史保，本步追加完整探测合同，不回改既有历史。

完整图形资料加载与主表有效内容/owner生存、所有caller/读取者、SDK和机器ABI/生成读取/资源像素及运行仍待。局部源符合不代整条画面或整体完成。

同一cg2d_re分支精确提交、不打tag，保七项外部改动及原暂存删除。不构建、测试、运行、读产物、计算摘要或引依赖；整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_palette_capability_selection_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 对齐图号探测与调色板选择合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x45C970 | 0x45CA1B | 62/171 |
| 0x450B70 | 0x450BAD | 16/61 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
