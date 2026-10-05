# FWA01 字体测宽正常算术与 RPLAN.24

<a id="font-width-arithmetic-baseline"></a>
## 实际差异与原基线

[两原测宽体](cg2d_font_width_arithmetic_evidence.json)为44B2A0..44B2DC的24条/60B及44B2E0..44B32A的33条/74B，共57条/134B；原直接call分别147和6，只保集合/统计，不重验全部父。依据原读取、分支和W32字段核本组正常算术，不重做已证字体创建/释放或GDI消费。

现Font.cpp两入口宽度使用U32累加后bit_cast回S32，三参数消费计数亦先U32相加再按S32比较；旧[字体合同](cg2d_font_measure.md)明确把它解释为回绕以避免宿主溢出。该专项回绕承载与最新RPLAN.24“不专项处理/模拟数学溢出”冲突，需要撤销，不能把异常域列为已修复/等价通过。

原两参数：先读源首U8/NUL，再读字体索引；每轮bit7决定1/2推进和对应宽度，推进后先读下一边界U8，再累加宽度并判断NUL。三参数：maxBytes S32<=0先返回0，正预算才读源/度量；高位分支先消费计数加1后读宽度，共尾加宽度后再加1；低位仅共尾消费加1，随后S32比较预算。预算剩1也完整计入双字节，不读尾字节。

<a id="font-width-arithmetic-decision"></a>
## 工程裁决与源码

沿[工程委托](../AGENTS.md)及RPLAN.24，在既有Font.cpp作必要修正：

- 两入口totalWidth改正常S32，直接加原S32宽度并直接返回；三参数consumed改S32，直接和maxBytes比较，撤U32累加/bit_cast回绕。
- 三参数计数明确按原高位分支一次、加宽度后共尾一次；低位仅共尾一次。保留字段宽度、原空串/maxBytes早退、bit7分流/盲推进、宽度读取和两参数“先下一边界再加宽度”顺序。
- 不新增溢出/越界保护、拒绝、钳制、补零、输入上限或故障模拟；正常字段和预算行为保持，异常域不作等价承诺。不改默认测宽44B260、字体初始化/释放/表/句柄/GDI与所有调用父，<bit>仍供原字段位型比较使用。

<a id="font-width-arithmetic-acceptance"></a>
## 本步验收

必要限定静态：57条/134B连续覆盖、两组直接引用、三参数signed早退/尾比较与分支计数/宽度读序、两参数下一首U8先于ADD及不读尾；源码两块精确增量、S32声明/更新/返回、旧当前回绕说明与台账/任务/派生度量同步。

本组I/S仅源码及限定静态，完整FUNC-TEXT及T01/T02/T03/R01/R04/V04保持open。实际生成ABI/指令、147/6完整父/共享owner/未知alias、目标字体/字形、原字节与画面/运行未验；不新增数学溢出专项门禁或称x64安全/故障等价。执行限制、音频暂停、原格式明文及[总SPEC §10](../plan/cg2d_full_reconstruction_plan.md#10-完成门禁)保持。

<a id="font-width-arithmetic-source-state"></a>
## 当前交付

[既有两入口](../src/render/Font.cpp)恢复正常S32算术；[声明/字段](../include/cg/render/Font.hpp)及[TextMeasure原owner声明](../src/LoginStateDependencies.hpp)不变。更新原两语义记录，两个adapted处置/两限定I/S证据纳管；不新造测宽本体或扩展到全字体初始化、消费者与生命周期。

固定命名快照：`cg2d-re-font-width-arithmetic-20261005`。
