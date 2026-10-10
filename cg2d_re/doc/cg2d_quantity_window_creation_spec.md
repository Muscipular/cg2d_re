# 数量选择框创建与状态发布 SPEC（QIP01）

2026-10-11。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。沿子AGENTS第8条工程委托裁决，同一cg2d_re分支、不打tag。

<a id="quantity-baseline"></a>
## 原端合同

QIP01仅数量框创建48AC60/动画48ABC0/发布48A830三个完整原根282条873B/五页。创建actionFlags W32先发布→声56→真实new(3,24H)→旧窗bit15/声57→原失败直接发布并返回空；成功prototype持址先于flags低BYTE→原四S32尺寸→低BYTE flags/低BYTE mode，两CryptInt构造复制析构原序及非拥有callback保持。24H既有WindowOpeningPayload结构不改，不补未写半高/模式其它字段。动画描边四坐标低U16→半宽/半高增量，counter按S16>=10才callback快照→mapY后mapX真实CryptInt读取→七参数发布→标死，末无条件W16自增；现void与原残留EAX分界具名，完整机器ABI/所有调度消费待。发布窗口42已存在清bit15/声57否则真实OpenWindow(42)，图号写elem6→maximum先窗口指针取样→value10/value14/callback→该窗口elem2初值→返回窗口取样→sourceWindow/sourceElement，返回该快照；仅补低BYTE及两个指针取样组三处，不新增保护/清零/默认/回滚/字段/公开helper。四入CALL、各四后邻及一个动画data持址仅来源/局部消费，不代两外父完整或动态callback寿命。当前WAV-only/原命令字段LF明文/MSVC CRT及所有完整门禁保持；数量确认/取消/关闭、共享窗口与载荷owner、生成机器读取/ABI、SDK/声音/画面及实际运行待，整体未完成。

原证完整指令与入引用见 [证据](cg2d_quantity_window_creation_spec.md#evidence-archive-20261011)。两外部创建来源为原4886E0/488D30；动画持址来源48AD23为数据引用，不能计作CALL。窗口发布两入CALL来自动画与创建失败路径。

<a id="quantity-decision"></a>
## 方案及取舍

保留现有24H struct与真实Yobi/Window/CryptInt提供者，只将创建windowFlags取样改成unsigned char低字节别名，并在发布函数显式持有状态写入窗口及返回窗口两个非拥有指针。计数和两CryptInt读写正确正文保，不补模式缺省、半高初始化、空窗口保护或销毁回滚。callback继续现非拥有原生指针，不为原残留EAX生成虚假数值返回；完整ABI及动态消费保持未验。

<a id="quantity-acceptance"></a>
## 本步验收

必要静态核全部原根边界/opcode/E8、低BYTE与指针取样序、两CryptInt/末增、四源文件全文精确差异回放，六类型/六提供者及四已有记录保持。登记三新F/三adapted、YOBI/UI各三成员，两限定S零I八open；静态汇总/-Check必须真实终态0，全部其它派生及完整门禁保持。禁止构建、测试、客户端/算法运行、产物检查与摘要。

<a id="quantity-source-state"></a>
## 当前状态

补齐数量框创建、展开与窗口42状态发布三个完整原根282条873B/五页；低BYTE flags与两次窗口指针取样两组三处对齐，动画正确正文保持。callback持址、两CryptInt、S16完成判定/W16末增和原失败直发布有据。三新F三adapted、YOBI/UI各三成员，两限定S零I八open；确认/取消/关闭及完整callback/owner/ABI、画面声音与运行待，整体未完成。

语义预期2315=2267F关联+48有界代码，处置1770/证据1568，文档关联2377/3680=64.59%只关联率。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_quantity_window_creation_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐数量框创建与状态发布合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x48AC60 | 0x48AE9A | 186/570 |
| 0x48ABC0 | 0x48AC5F | 59/159 |
| 0x48A830 | 0x48A8C0 | 37/144 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
