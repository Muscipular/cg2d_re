# cg2d 输入文本绘制还原

## 函数边界与证据

- Renderer::DrawInputText@[4568A0,456D27)，332条，原栈局部0x11C、Destination[256]；[ITD01](cg2d_input_text_alignment_spec.md#input-text-draw-baseline)保存完整原体及测宽支持33条。
- 原32直接点/25父已有当前接线，其中4A7590/4A7640共用ScrollInputAndDraw一个真实绘制调用，新端31个词法调用不等于少接一个原点；各父完整业务/间接alias与寿命仍另行验收。
- 入口只按signed W32判断文本计数>=4096早退，原没有负计数或空context拒绝。上下文为原参数，当前全局指针仅用于最后锚点资格。
- 多行两复制/两绘制、一分类；单行裁宽一测宽/一分类、一QueueText；最后两锚点各一测宽，共十实际调用。字符串核心/Font/队列接口与唯一数据本步不改。

## 多行路径

maxLine>=2从字节0开始按原读取，不预探测固定缓冲NUL，不预清256B Destination。

- 每个非NUL且非CR字节先取当前maxCharPerLine，再增加scan并比较相等。CR之前复制scan低U32字节、补NUL，推进至CR后一字节/增行数；LF无特例。
- 非CR分段先保存当前S32预算，随后strlen(本轮源)取低U32并unsigned比较；余长小于预算才走尾串。真实StrNCpy按保存预算的低U32复制，调用后另取当前预算作为NUL位置；分类后再次取预算，末单元3删孤立高位字节并少推进一字节。
- 完整段与余尾各按当前颜色/字体/低W8模式/Y/X取样后真实DrawText2；低W16坐标、低W8字体/颜色/value7=0和弃返回保持。每次绘制后取当前field14C增Y，不能跨段固定模式/行宽。
- maxLine不限制实际段数，仅决定caret在边界所属用<或<=。最后未找到所属行时沿原nextStart/Y保存，末尾余串仍绘制一次。
- 不添加容量/起点/零推进拒绝；这些是已撤销的新端差异，异常域依RPLAN.24不专项模拟、不称安全或故障等价。

## 单行路径

先用入口计数索引写记录x/y、低W8模式/颜色/字体和值7=0；不写marker、field108，不预清NUL后旧字节。

- encrypt>=1是原星号遮显字段，原网络仍只传明文。先total−field158的S32结果决定进入；每写一星号后分别重读total/field158，增加索引再比较。非正差只写NUL，不固定整个输出数量或添加上限。
- 普通文本先取原text+field158，扫描含NUL长度低U32；按原逐W32读/写，再复制余W8，源身份与前移次序保持，不另加容量拒绝/尾NUL。
- field154非零才重新扫描已写记录，S32长度>0进入裁宽。每次测宽完整fontType/当前阶段记录，返回后再取当前field154比较；超宽先重新取原field158、减预算，再分类原后缀/必要再减，随后重读队列计数。
- 可容纳分支先重读计数再写该记录预算位置NUL；减至非正路径不补NUL而继续排队，保留“一个字符也放不下反而不截断”和逐字节切开DBCS的旧行为。
- 裁宽后按当前阶段计数清next，再读取完整W32模式取尾；空尾先更新原槽，再读当前模式取fontLayer/QueueText并弃返回。非空尾先连旧尾，再读取当前计数/模式更新尾槽。末尾对共享当前W32计数加一，不写入口值加一。
- 现有一基token、池/六槽/fontLayer及owner保持；完整模式和尾token不加新范围拒绝。DrawText2/QueueText各自旧安全差异仍待后续，不由本组改动宣称全队列已验。

## 当前输入锚点

绘制/排队结束后仅当前输入指针等于原参数时更新x/y；本函数不绘制光标或IME。

- maxCharPerLine>0且maxLine>=2：原caret−所属行首为S32预算，直接真实Font::measure，不以正预算/字体范围/源扫描跳过；X为field130+返回宽度，Y为field134+所属行偏移。
- 其他路径先取caret/field158计算S32差；负数写field158=caret、预算0。随后取完整fontType，再次读取实际field158作为源，真实测宽。Font只有原非正预算返回0，不添加字体门禁。
- 两路径测宽后按原先取X/Y源再回写x/y，W32加减、坐标与字节单位不变。完整焦点/字节索引/输入渲染寿命与运行仍待。

## 新端安全边界

本节原六项新增门禁和局部预清/固定快照已依[ITD01决定](cg2d_input_text_alignment_spec.md#input-text-draw-decision)撤销；历史全文见Git固定快照cg2d-re-input-append-source-20261005。原满队列/分支/宽度/锚点条件及实际padding/NUL保持，不把撤销差异称为强化安全。异常对象/Count、越界或溢出按RPLAN.24不专项模拟、不补保护、不记安全或故障等价。

[局部验收](cg2d_input_text_alignment_spec.md#input-text-draw-source-closure)只覆盖本组根及实际接线；完整父、下游队列/消费、间接alias/焦点、初始化/退出寿命、SDK/库优化及画面运行未验。光标及RenderInput沿各具名后续实现，不使用旧“尚未实现”描述替代当前源码。


## 2026-10-03 多行段限长复制真实接线增量 SPEC

既有多行两memcpy段复制接StrNCpy_53BEC0，对应456914/456955，
原Count分别扫描ESI和+148H低W32的U32值；
现有scan/maxCharBits直接传入，不把复制计数提升到size_t。
同一256B destination、caller末NUL、定长分类/删孤立高位字节/
推进与后续逐段DrawText2/光标所属行/Y顺序保持；
字段138H模式读取的memcpy、单行队列和末锚点正文不改。
仅两标识符/两Count表达式及既有LegacyTime.hpp包含，核心/声明不动。

destination预清、局部门禁及maxCharBits/textLength快照保持，
不将这些既有差异记为原拒绝、原重复取样全等或故障等价。
工程裁决见[字符串合同](cg2d_legacy_string_compare.md)；
332条统计中只读相邻55条，32代码引用/25父仅登记，不重验各父。
集中静态核对限定接线，JSON一个既有父及共享项增量，
两合同/唯一进度仅追加，未构建、测试、模拟输入或校验哈希；
完整输入/渲染/跨板块运行验收仍待，音频继续暂停。

## 2026-10-05：当前状态

上节2026-10-03的预清、局部门禁及快照保留描述属于当时源码；ITD01已按完整原体修正当前状态。完整父/数据生产消费及跨板块运行仍待，不恢复音频或网络加解密。

## 2026-10-05：下游文本生产当前进展

[TQP01](cg2d_text_queue_alignment_spec.md#text-queue-producer-source-closure)已校正真实DrawText2/DrawText及QueueText的额外拒绝、复制阶段和空链layer取样；本输入绘制源码及其三完整模式取样/QueueText写序不改。两函数空链取层顺序不同须各按原保留，不能把下游的“层先读”套入ITD01已证原序。完整上游/图形排序/文本消费、间接alias与输入渲染寿命及SDK/运行继续待验。
