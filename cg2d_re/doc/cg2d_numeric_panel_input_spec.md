# 数值面板输入与绘制取样 SPEC（NPI01）

2026-10-11。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。沿子AGENTS第8条工程委托裁决，同一cg2d_re分支，不打tag。

<a id="numeric-input-baseline"></a>
## 原端合同

NPI01仅数值面板474550完整540条1675B/九页，26个E8五目标、64跳三RET。原window.elements→windowY→windowX→elemX→elemY→两坐标，16B descriptor原清零/color+8/mix+13=4；四绘制prepare顺序分两类：九宫格与右侧操作先descriptor地址后image表，0/确认先image表后descriptor地址；现只两逻辑组五处明确持样，不新增绘制或默认字段。九宫格按1..9/三行三列原顺序绘制，0/确认/退一位-最大-清零三操作继续；每个hit独立执行left<=mouseX、mouseX<right、top<=mouseY、mouseY<bottom四次真实CryptInt短路读，不共用坐标快照或减少RNG消费。hover文本沿既有GBK现静态文字来源保，hints指针/42个S32图号表不变；hover/pressed及flags bit1许可零值确认、原非正值声55和确认禁态图号，hit及text读取后重新数量读保持。flags bit2才0..9主数字/小键盘按短路先主键后小键读取并首匹配退出；其后Backspace键8无条件优先退一位/声51/AL1，覆盖鼠标或数字选择，且不受flags bit2限制。没有Enter/Esc读取，不能把本根AL3确认解释键盘取消，也不添这些分支。选0..9元素状态先*10写回→fresh元素再+digit写回→fresh元素S32与传入maximum比并保原上限钳回；选10退一位、11写maximum、12清0，原13只声51/AL3不额外改数量，无选择AL0不声，有操作AL1/声51。四入CALL来自三个父，数量48A950只AL归一/两上限钳回/业务callback消费已由QVC01完整合同复用；另4ABB20两个及4B1260一个仅入CALL/四后邻来源，不据本步验全部父/ABI。四真实提供者/三类型/数值图表及三已有语义记录保持；一新F一adapted，INPUT/UI/RENDER各一成员，三限定S零I八open。全部输入事件生产/共享鼠标与状态、命中和数值目标alias-owner寿命、生成机器读取/完整AL与ABI、画面声效及运行待；正常整数数学/原业务钳回保，不新增溢出回绕模型、下限、空检查、容量/索引保护、补零/回滚、字段/helper/API或owner。WAV-only/MSVC/明文协议与全部完整门禁保持，整体未完成。

完整指令/来源和图表见[证据](cg2d_numeric_panel_input_spec.md#evidence-archive-20261011)。数量父48A950已由[QVC01](cg2d_quantity_window_callbacks_spec.md)完整取证，不重计；另两父本步只核四CALL和有限后邻，不假定全部消费。

<a id="numeric-input-decision"></a>
## 方案及取舍

在既有Sub_474550内持elemX/elemY再形成坐标；四绘制调用分别持descriptor和image，九宫格/右侧先descriptor，0/确认先image。现hit lambda保持四个真正CryptInt短路，三次fresh数量取样和原上限钳回不合并。真实数字扫描、Backspace高优先级、鼠标13号确认及AL返回保持，不引入Enter/Esc、其它退出或下限/溢出保护。只两逻辑组五处/两注释，无新函数、API、字段或owner。

<a id="numeric-input-acceptance"></a>
## 本步验收

完整540条1675B/九页、26E8五目标/64跳三RET、四CALL三父各四后邻、168B十四组四十二S32图号核对。必要静态核坐标/descriptor prepare、每hit短路、键读取顺序/Backspace、鲜数量发布和AL返回；两整源精确回放，三类型四提供者/数值图表及三已有记录保持。登记一新F一adapted、三限定S零I八open，静态汇总/-Check须终态0/其它派生和完整门禁保；未构建/测试/运行/产物/摘要。

<a id="numeric-input-source-state"></a>
## 当前状态

补齐数值面板474550完整540条1675B/九页，逐键四次CryptInt短路、数字/Backspace优先级、四鼠标操作与AL0/1/3返回有据。两逻辑组五处坐标及绘制取样对齐；42图号和既有业务正文/原上限钳回保持。本根没有Enter/Esc处理，不添加退出键行为。一新F一adapted、三限定S零I八open，完整输入生产消费/owner、ABI及运行待。

语义预期2323=2275F关联+48有界代码，处置1779/证据1577；文档关联2385/3680=64.81%仅关联率。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

## 2026-10-11 父入口说明勘误

四个调用地址及原指令保持。4ABBDA/4ABC46实际属于4ABB20；此前父入口文字4ABC20更正为4ABB20，未将CALL点或函数内部地址当入口，不宣完整该父验收。原代码及统计不变。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_numeric_panel_input_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 更正数值面板父函数入口说明」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x474550 | 0x474BDB | 540/1675 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
