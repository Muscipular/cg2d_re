# 能力4提交长度取样SPEC（BSG01）

本项沿登录主流程，依工程委托先裁决再实施，只补能力4输入控制器两条普通成功路径的取样先后。沿用[BSC01原基线](cg2d_capability4_input_normal_arithmetic_spec.md)、[能力4合同](cg2d_capability4_login_input.md)、[最新裁决](cg2d_closure_decisions_spec.md)与[验收体系](cg2d_traceability_acceptance_spec.md)。

<a id="capability4-submit-length-baseline"></a>
## 已有基线与当前缺口

真实464E30／F:00064E30已经纳管为语义1582，原完整350条1262B／五页、31直接调用／15唯一目标、42跳转／一RET，唯一464840@464ADF。复用BSC01取证，不重复获取同一原根；两原成功块及当前源前态见[本步证据](cg2d_capability4_submit_length_sampling_evidence.json)。

当前两块先写result1／第一个NUL，再读取第二个长度。原指令先读齐两项+12C totalByteLength，再result1和两个NUL；两条路径具体顺序不同：

| 路径 | 原读取顺序 | 取齐之后的发布顺序 |
|---|---|---|
| 按钮464ED9..464EF0 | 密码9A5678→账号9A5960 | result1→密码NUL→账号NUL |
| 普通Enter465033..46504A | 账号9A5960→密码9A5678 | result1→账号NUL→密码NUL |

+12C对应S32 totalByteLength，+128才是caretByteOffset；原反编译cursor名字不改变字段身份。普通数值本身已经正确，不宣称观察到运行故障。ProcessLoginInput_45CA20已有两值先取齐的实现，不纳入本次修改；屏幕键盘排除路径不恢复。

<a id="capability4-submit-length-decision"></a>
## 实现裁决

仅在src/LoginInputController.cpp的ProcessCapability4Input_464E30替换两个成功片段。各按上表用S32局部值单次取齐，然后result=1并按原相同顺序写两NUL；保现索引边界转换和原字节，不提前窄化或合并两路径顺序。

保持两按钮结果与所有失败分支／返回、密码和账号strlen先后、Tab/Enter短路与文本校验、原上下界单读及鼠标owner/RNG、两私有正常S32工具、真实低W8/W16、焦点/latch/计时、绘制/保存和声音、现屏幕键盘排除及同文件其它函数正文。

局部长度仅本次字段快照，不新增owner、辅助函数或保护。不增加长度/负值限制、拒绝、补零、钳制、越界或数学异常模型；网络原格式明文、音频暂停及现验证限制保持。

<a id="capability4-submit-length-acceptance"></a>
## 本步验收

| 项目 | 通过条件 |
|---|---|
| 源码 | 单整源只有两个授权成功块替换，一个既有函数体变更，其它正文保持 |
| 取样 | 每块两字段单读S32，按钮密码→账号／Enter账号→密码；读齐后才result1和两个NUL |
| 原证据 | 复用BSC01完整原根，逐点证明两块读齐与发布次序，+12C真实字段身份 |
| 台账 | 只更新旧1582与其adapted处置证据，INPUT成员数不变，不新增F、X或私有函数身份 |
| 验收 | 两局部I-S、U10／R04current/引用与唯一进度同步；集中静态／measure/-Check／完整派生快照／编码引用通过 |

局部通过不代完整输入父／依赖、正常alias-owner寿命、SDK-机器ABI／链接／明文互通和运行；不构建或测试运行，完整门禁保持。

<a id="capability4-submit-length-source-state"></a>
## 状态与计量

旧映射1582补本步来源／验证记录，原adapted处置只追加引用和边界。INPUT26／语义1589有效1582／文档1700（46.20%）／处置722／R7 294余2767保持，仅E832→834，两局部I-S不提升完整门禁。无F/code区各7／pending18／缺口队列、源码closed11（P05原closed）／完整叶0/globalfalse保；U10／R04仍open。

实施命名快照：`cg2d-re-capability4-submit-length-sampling-20261007`。实际静态结果在本步完成后追加。

本步静态结果：单整源仅两授权提交块、各自不同原读序／S32先取齐再result1与两NUL，公共登录体和其它正文保持，BSC01完整350条1262B复用及两成功原点／+12C身份、完整机器／文档增量均通过；measure及-Check完成，完整派生快照／12文件UTF-8-LF与两I-S各14实际来源通过。未构建、测试或运行，语义／文档／成员／R7与完整门禁保持，不提升整体SPEC验收。
