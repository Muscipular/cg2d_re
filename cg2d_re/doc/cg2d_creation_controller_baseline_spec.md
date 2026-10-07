# 创建控制器完整原新基线 SPEC

编号BVH01；2026-10-08。落实4626A0完整本体与既有批准门禁的可追踪映射，修正两处过期的当前事实。正确源码保留，源码改动0/无新I；功能级pending继续保留。

<a id="creation-controller-baseline"></a>
## 原基线

[原证](cg2d_creation_controller_baseline_evidence.json)保存[4626A0,462870)完整130条464B/两页，18 direct/16目标、零indirect、30跳转/一返回，唯一原调用WorldUpdate@51C505。场景4完整局部[51C4FA,51C523)十条41B：清73ECC0→4596D0输入→创建控制器→Yobi更新→角色绘制→输入绘制→44A410→返回。

原首条PUSH ECX保存入口残值；初始10按该值与1比较。原ECX未知仍保，[D01](cg2d_local_determinism_spec.md)与[P08](cg2d_creation_gate_adaptation_spec.md)已批准并完成唯一World调用显式gate=1，这是适配而非原常量。本步只在该明确批准范围核现完整本体，不重开裁决或推断原寄存器。

| 状态组 | 原与现实现的合同 |
|---|---|
| 0/1/2/3 | 0先清理再fresh状态；1初始化/发布3后同次选择；2重建后fresh状态++；选择返回1进入10，2切type3/state10后重读 |
| 10/11/12 | 10须gate1，10/11初始化并发布12；restore==1先恢复后清BYTE，再编辑；返回1进入30并清理输入，2发布2后直达尾门禁 |
| 30/31 | 重置资料→发布31→同次请求轮询；1增W16计数/切type3/发布W16入口1再读状态，2进入100，其他返回重读状态 |
| 100/101 | 重置消息→fresh状态++；确认时restore BYTE先1、状态10、gate1回462726；等待时fresh状态后检查尾回边 |
| 尾及默认 | restore BYTE等于1只重做后半分派，保当前local状态与gate；否则返回，不改为每帧一次switch |

现ProcessCreationScene_4626A0(std::int32_t)完整本体正确；确认时选择索引先持样、状态30发布后一次51B3B0，再fresh读取弃值供原空叶、520200/SetContext，保持原顺序。18调用的真实源码承载均存在；不据此声称16个callee全部验收或请求往返已运行。

创建/升级共用版本正文已由[BRT01](cg2d_version_payload_zero_prefix_owner_spec.md#version-zero-prefix-source-state)落实唯一const char[1]、原零前缀/两consumer/输入只读和静态寿命；不存在需要补造的版本正文生产者。本步承接该已完成的数据合同，原网络格式/mode5/空格/正文及明文不改。

<a id="creation-controller-decision"></a>
## 实施决定

1. 不改五相关源、完整创建本体、参数、唯一调用、版本owner或请求实现。正确已实现代码只补完整原新，不制造修复、新helper/owner或新I。
2. 新1762/F:000626A0按adapted纳管，显式列D01固定1与原ECX未知；CHARACTER仍23个既有成员，新增一限定S/26真实来源。只覆盖批准门禁范围内的完整controller状态/字段/调用顺序及唯一调度静态，不外推callee/正常alias寿命/完整交互/ABI运行。
3. pending7仍代表完整创建功能未验，18个pending身份及pending_record_policy保持。该ID同时有静态原新和功能级pending，两个层次分别记录；不因映射或批准适配删除待验身份，不降低验收标准。
4. 修正入口SPEC两处现时“无参待接”句；pending7当前要求删除已由BRT完成的正文缺定义描述，改为完整创建交互/请求回复/所有callee及寿命/ABI运行待验。D01/P08、原寄存器未知及历史结果保。
5. U11/V03/R04只current/来源推进仍open；P08及U03原closed保持，不新关闭任务/完整门禁。网络原格式明文/两加解密叶排除、音频暂停/RPLAN.24、单线程、不构建/配置/调试/测试/运行/产物/摘要限制保持。

<a id="creation-controller-acceptance"></a>
## 度量与验收

| 项目 | 可复核要求 / 预期 | 边界 |
|---|---|---|
| 原本体 | 完整130条464B/两页、18调用/16目标、30跳转/一RET；唯一caller十条41B | 不代16个callee/所有生命周期验收 |
| 源及适配 | 五源/完整函数与HEAD一致、唯一显式gate=1/私有S32无默认参数，D01/P08不改 | 0代码改动/无新I，不称原ECX等价 |
| 正文事实 | BRT唯一const[1]及共享消费已完成，入口SPEC两现时句更正 | 不计算摘要/执行请求或客户端 |
| 原新与pending | 新1762 adapted、一CHARACTER限定S各26来源；pending7与18/成员23保 | 映射与完整功能待验分开 |
| 看板 | 语义1763/有效1756、合同1873（50.90%）、F904/E1079；R7已审465余2596，closed11/全叶0/globalfalse保 | 仅具名差集/三任务open |

<a id="creation-controller-source-state"></a>
## 收口

先行SPEC后完整原新、当前事实修正、限定S/七对应文档/三任务与唯一进度同步；集中静态及文本派生后记录实值，精确提交固定cg2d-re-creation-controller-baseline-20261008。完整创建交互/真实请求回复、全部callee与owner-alias寿命、机器ABI链接/运行及R8继续；原ECX未知、pending18及全部闭包保持，整体目标active。

### BVH01 当前创建controller原新限定静态结果

集中静态通过：完整原根130条464B/两页、18直接/16目标/零间接/30跳转/一RET、唯一World场景4十条41B，原入口ECX保存及两后半回边有实际指令；五源/完整本体及私有S32声明、唯一D01 gate1调度与BRT const[1]正文/共享消费全文保持，代码0改动无新I。入口SPEC两现时无参待接句已更正、pending7当前说明承接本体原新和已完成正文事实；pending7/18与原policy保。完整机器/派生仅一原新/F、pending7当前事实、一域S/三任务与三指标差集；十四文件UTF8无BOM/LF、四锚点/一限定S的26真实来源通过。

文本汇总及Check均退出0；实际语义1763/有效1756、合同1873（50.90%）、F904/E1079，R7已审465余2596/CHARACTER23、原分类120/mixed3/完整入口排除113、closed11（P08/U03原closed）/差队列及完整闭包保持。完整controller只在D01批准固定门禁范围静态通过，原ECX仍未知，不称原寄存器等价；完整创建交互/请求回复、16个callee及正常owner-alias寿命、机器ABI链接/运行和R8未验，整体目标继续。
