# IME 组合与提交顺序对齐 SPEC

> EIC01；2026-10-05依既有工程委托采用。对应 T01/T02，遵循[总SPEC §2.3](../plan/cg2d_full_reconstruction_plan.md#23-执行逻辑与可观察行为)的 RPLAN.24 与[文本边界](cg2d_text_encoding_spec.md)。本组修正已有源码差异，不新增输入策略或 Unicode 转换。

<a id="ime-composition-baseline"></a>
## 1. 原端与当前差异

[只读证据](cg2d_ime_composition_alignment_evidence.json)保存459580全部97条指令、唯一caller46728D的10条邻窗，以及共享槽直接引用。函数边界[459580,4596C8)，原W32实参只消费低W16，返回低U8；窗口调用后弃返回并返回0。邻窗不代表完整WIN_PROC已审核。

| 部分 | 原行为 | 当前需修正 |
|---|---|---|
| 结果串800H | 256B局部未预清；LONG查询，signed >=256才取255；再次读取后按第一次长度写NUL | 删除局部{}和额外byteCount>=0门禁；保持两次API各自读取共享HIMC |
| 结果串追加 | NUL扫描得到原W32数量，signed >=256取255，S32循环逐字节AppendChar | strlen边界收回S32数量，循环不使用宿主size_t扩宽 |
| 组合串8 / 属性10H | 同样查询/原上限/再读/第一次长度下标置0，两个共享256B数组不额外清零 | 删除各自负长度门禁，终止下标保留有符号LONG |
| 游标80H | 查询一次，完整LONG写73EFE0，再626C0C=20，返回API低U8 | 现有正文保持 |
| 多标志 | 800H→8→10H→80H四个独立if；前分支不短路后分支 | 不合并、不增加上下文快照或失败早返 |

原4595A6/459624/459669与4595E0均为signed jl门槛；没有负值拒绝。原4595D4仅在第二次API后写第一次查询长度对应的一个NUL，局部未预清；第二次API返回被弃用，不能新增实际读取长度覆盖或失败补值。原4595F8唯一AppendChar调用按字节重取局部数据，AppendChar本体仍处理实际当前InputContext。

## 2. 数据生产消费与边界

| 原身份 | 当前真实存储/入口 | 本组所核接线 |
|---|---|---|
| 73EDD8 HIMC | WindowProcedure.cpp的gImeContext，ImeLifecycle.cpp实际创建/发布 | 七处API分别读取共享槽，不新增缓存句柄/空句柄门禁 |
| 73EDE0组合串 / 73EEE0属性 | 同文件gImeComposition[256] / gImeAttributes[256] | API直接写同一存储，再原下标终止；InputCaret与RenderInput等原消费者仍借用 |
| 73EFE0游标 / 626C0C相位 | gImeCompositionCursorPosition S32 / input::gValue626C0C | 游标API完整W32后相位20，不把字符偏移改Unicode索引 |
| WIN_PROC466DC0@46728D | WindowProcedure.cpp WM_IME_COMPOSITION→Sub_459580 | 既有lParam32低W16传入，返回弃；ENDCOMPOSITION只清组合首字节保持 |
| 459580@4595F8→44ADB0 | cg::input::AppendChar_44ADB0 | 循环逐字节调用，既有上下文/过滤/容量/副作用不改 |

直接xref只支撑所列读写和当前源码绑定，不排除间接别名，不证明所有IME绘制/候选/编辑消费者已完整对应。共享存储初值和生命周期沿既有[生命周期合同](cg2d_ime_lifecycle.md)、[窗口合同](cg2d_window_procedure.md)，本组不重建owner或升级这两父的全体审核。

<a id="ime-composition-decision"></a>
## 3. 本组决定

1. 只修改既有ImeComposition.cpp：撤销三处额外负值门禁、结果缓冲预清，结果长度及循环索引改S32，数组终止直接用signed LONG；删除随之无用的cstddef。四分支/七API/AppendChar/游标与返回、ABI/共享定义均保持，无新函数/公共接口/依赖或RTTI。
2. 保留原已证255上限及DWORD API参数转换；不把负LONG改0、不跳读取、不换第二次返回长度、不加补零/检查/重试。API原字节仍A系，未决定新的编码或Unicode映射。
3. 负查询长度、读取失败留下未写局部字节等情况属于RPLAN.24不专项处理域；原事实保留，源码不新增保护，也不模拟原x86故障。**不声明这类访问在x64安全或故障等价，不把它计为确定结果/修复通过。**
4. 将459580完整本体补入语义映射，七原API点维持既有5349B0导入关联；局部函数处置限定适用正常/原上限/多标志路径。保留完整父/消费/SDK与实际运行未验状态，T01/T02继续open。
5. 本组不改已正确的UI记录初始化，不以其最终值/纯局部计算时点制造源码调整；U01/U02邻接引用仍无旧目录/名单生产证据，数组容量不从相邻独立字段推定。

<a id="ime-composition-acceptance"></a>
## 4. 局部验收

| 门禁 | 本组标准 |
|---|---|
| I | 三额外拒绝/局部预清已撤销；S32追加数量与七实际原生API、逐字节AppendChar落在原本体 |
| W | 既有窗口低W16调用、唯一共享256B数组与游标/相位接线保持；不造影子文本或新的全局owner |
| S | 97原指令逐分支对照；仅以上限定源码差异，查询与终止/追加/游标/返回次序可追踪；异常域不纳入故障等价结论 |
| R | 不构建、检查产物、新增/执行测试或运行/摘要；实际IMM32/IME输入、焦点/绘制和SDK未验 |

只归档限定I/W/S，不提升完整FUNC-INPUT/FUNC-TEXT门禁、叶验收/全量闭包或任务关闭数。此处原字节与局部数量不是完整输入验收。

<a id="ime-composition-source-closure"></a>
## 5. 源码收口

459580源码已按原97条实现：三额外负长度拒绝与256B结果局部预清撤销，结果数量/索引为S32，终止直接signed LONG；原四255上限、四独立标志、七API每次重读HIMC、第二次返回弃与第一次查询下标、逐字节AppendChar、游标完整LONG/相位20和低U8返回保持。既有窗口低W16调用/弃返回及共享owner未改。

集中静态核对通过：源码限定变更重放全文一致，唯一源文件/原97条和10条父邻窗共107唯一指令、四signed门槛/七API及实际共享槽核准。语义仅一既有导入关联更新与一完整本体新增，其余1180记录/顶层元数据保持；1182条/1179有效ID、18pending/4分类引用不变。原42证据、90处置、其他功能/任务、执行策略/全局闭包保持，本组限定I/W/S三证据及一equivalent使45/91。

固定实现为Git命名快照cg2d-re-ime-composition-source-20261005；T01/T02保持open，关闭10/60、叶验收0/182、完整功能门禁不提升。负查询/未写局部访问仍RPLAN.24不专项处理域，不记安全/故障等价；完整IME消费者/焦点/生命周期、间接别名、SDK及运行仍未验。未构建/检查产物/新增或执行测试/运行/摘要。
