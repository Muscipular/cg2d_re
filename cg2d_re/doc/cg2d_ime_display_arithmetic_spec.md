# IMA01 输入法显示正常S32算术

<a id="ime-display-arithmetic-baseline"></a>
## 缺口与原基线

[RIN01既有显示合同](cg2d_input_render_alignment_spec.md)已落实说明、组合文字、属性扫描和候选绘制。[RenderInput.cpp](../src/world/RenderInput.cpp)的AddW32/SubW32/MulW32仍把S32实参转U32，运算后bit_cast回S32，专项承载回绕；本步依RPLAN.24恢复正常S32算术。23加/4减/5乘共32个源调用表达式保持原位、操作数及嵌套，不将该数量当原机器ALU次数或覆盖率。

[原证据](cg2d_ime_display_arithmetic_evidence.json)保存4590E0..45957E完整357条1182B与21直接call引用身份，分属43CBB0十处、51C430三处、51C7D0八处。本次仅复核正常算术局部和三个工具精确差异；完整体保存不代整父语义、21完整调用点、三个父或所有显示链验收。

正常局部依据包括组合框45926E/45927A和边界45927C/45927F；属性宽度459306/459336；高亮45937B/45937D/459384；候选序号459402/45940E、正文459420/45946A/459470、长度与units45949D/4594AB/4594B3；选择框459532..45954C。保原正常阶段与字段取样，不强配一个工具表达式到一条机器指令。

<a id="ime-display-arithmetic-decision"></a>
## 委托裁决与实现

依[工程委托/RPLAN.24](../AGENTS.md)，仅改三个既有匿名body：AddW32返回left+right，SubW32返回left-right，MulW32返回left*right；签名和实参仍S32。它们属于既有F:000590E0来源，不新增原身份、公共接口或独立算术文件。

保留以下现有合同：

- 组合框坐标、属性扫描的单/双字宽、高亮框、候选序号/正文/选中框均保32处表达式、原取样点、调用和写序。
- units在正文阶段先加1，strlen后再加length/2；原strlen低W32解释为S32、signed<2取2、向零除2保持。原下限2是布局规则，本步不删除。
- 低S16文字坐标、低U16框坐标、低S8模式/字体及完整字段位型仍按原窄化；相关bit_cast用途保持。
- CANDIDATELIST的DWORD count/selection/pageStart/pageSize/offset与+08/+0C/+10/+14/+18布局不改。索引、页大小读取→递增→无符号比较、有效项绘框后重读/skip沿旧表保持；实际API owner和原生指针承接不改。
- 两说明S16存储/初值与Y→X读取、两12B说明、键态/序号局部、Itoa实际返回、真实测宽/绘制/队列和父声明保持。

不加保护、拒绝、钳制、补零、预清或异常模拟，不改字体、候选生产/清理和网络协议。RPLAN.24数学溢出/越界不评安全或故障等价。

<a id="ime-display-arithmetic-acceptance"></a>
## 限定验收

1. 357条1182B连续原范围/库存身份及21call/三父身份可追；仅上述正常算术局部限定核对。
2. 源码仅三个body变化，精确重放后全文一致；32处调用表达式、原取样/字段/窄化/候选流程及其他正文保持。
3. F:000590E0既有语义/equivalent追加依据和三匿名来源；语义1260/有效ID1256、处置350/3680及输入/文本14/22明确成员保持。E194→196只记FUNC-INPUT本次I/S局部。
4. T01/T02/R04/V04和完整功能门禁保持open；未知坐标writer/间接alias、候选分配容量/完整寿命、完整父/焦点/IME、实际机器/ABI/SDK/像素画面运行未验。R1/R7/R8及整体SPEC继续。

<a id="ime-display-arithmetic-source-state"></a>
## 当前交付

输入法显示链三个工具恢复正常S32加减乘，32处既有表达式和RIN01显示/取样合同保持。同步当前SPEC、语义台账、验收登记簿、看板与唯一进度，不将本次局部实修称全部输入法/文本功能验收。

固定命名快照：`cg2d-re-ime-display-arithmetic-20261005`。
