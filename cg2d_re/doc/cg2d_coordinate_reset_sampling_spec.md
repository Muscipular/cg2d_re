# 坐标重置载荷取样与快照发布 SPEC（BUG01）

日期：2026-10-08。承接BUF01双投影，沿真实参考坐标生产入口468330推进。按AGENTS.md第8条先记后实施。[完整原指令/四局部窗口/源前态](cg2d_coordinate_reset_sampling_evidence.json)。

<a id="coordinate-reset-sampling-baseline"></a>
## 原端与具体差异

| 原入口 | 完整范围 | 原端量 | 本步处理 |
| --- | --- | --- | --- |
| 46CEC0 | [46CEC0,46CED6) | 6条22B，一页一块，两直接call/两入向 | 先Y GetVal再X Add，现正文正确保持 |
| 46CEE0 | [46CEE0,46CEFB) | 7条27B，一页一块，两直接call/三入向 | 同序读取，将完整W32和写AF10C0并返回，现正文正确保持 |
| 468330 | [468330,468511) | 100条481B，两页一块，16直接call/四入向 | 六临时三字段预取和玩家快照读写交错实修 |

合113条530B/四页三块，20直接call、0jump、三RET、9入向来自6父。四468330父调用后窗口为467F30@467F5B、4681B0@4681C6、5093F0@5094A3、519EC0@519ECA。前三只局部后窗；最后ADD ESP8后RET沿原完整W32返回，不能宣称全部上游弃返或完整机器ABI。

原401EA0经[既有证据](cg2d_cryptint_arithmetic_evidence.json)单条五BYTE尾跳401EC0。两个helper当前复用Read_401EC0正确，仍更新W8 readCount/必要时换mask并消费唯一RNG；不新造包装，不误把GetVal当成无副作用取值。两helper用既有Read/Add宿主接口承接，保正常S32观察；按adapted登记，不以局部读序正确声称原thiscall/全机器ABI等价。

## 六临时载荷

| 目标 | 原读取顺序 | 原发布顺序 |
| --- | --- | --- |
| 玩家X | encoded→mask→count | encoded→mask→count |
| 玩家Y | count→encoded→mask | count→encoded→mask |
| 9EE8D0 | count→encoded→mask | count→encoded→mask |
| 9EE8E0 | count→encoded→mask | count→encoded→mask |
| C27FF8 | encoded→mask→count | encoded→mask→count |
| C27FE8 | count→encoded→mask | count→encoded→mask |

每组三读均先于目标首写；count为W8，encoded/mask为W32。现逐字段读取即发布的源码改为各自三局部预取后原序发布。六次ConstructFromInt/Destroy及各自作用对象保持，不缓存跨构造/析构或合并临时，原玩家X/Y实参和四个-1保持。

## 快照交错与参考坐标生产

原Y浮点读取返回后：读X count→读X encoded→Y乘64→读X mask→写X snapshot count→读Y count→写X encoded→读Y encoded→写X mask→读Y mask→写Y snapshot count→写Y F32当前位槽→写Y encoded→写Y mask。当前先计算Y倍64、随后逐字段取即写，调整至上述顺序；不把两快照或两F32发布合批。

旧/新helper及浮点转换各轴各三次读取，可能更新计数/RNG，保持次数和序。54DD30实际已加载四BYTE F32位值42800000，即64；只证明位值，现double乘法/F32收窄表达式保持，不声明原x87舍入全等。

末尾六析构完成后先取两个当前F32位槽，再按X→Y发布到投影参考W32槽并返回finalYBits完整W32。现正文正确保持，519EC0的收栈/RET透传事实保，不误改为void/固定返回。三个尾区10+5+15B逐BYTE实际NN_NOP/90，无入向/后真入口，仅用途，不新F或排除范围。

<a id="coordinate-reset-sampling-decision"></a>
## 具名裁决与实施

只改Scene一个既有468330正文七片段：六临时各三局部预取，以及一快照读写交错片段。46CEC0/46CEE0正确正文保持；全部数值表达式、六构造/析构、参数、调用者及其它正文保持。无新helper/lambda/声明/共享owner/依赖，也不改内部LegacyCryptInt或网络算法。

新增1686/F:0006CEC0、1687/F:0006CEE0和1688/F:00068330均为adapted。网络原格式仅明文、内部LegacyCryptInt、音频暂停/RPLAN.24保持；不以三身份新增代完整功能验收。

<a id="coordinate-reset-sampling-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 验收边界 |
| --- | --- | --- |
| 源码 | 一cpp、三既有正文，只有一正文七片段变化 | 全文精确模型；六预取/快照交错、原W8/W32/返回/其它语句保持 |
| 原端 | 113条530B/四页、四局部窗口、4B F32与30B用途 | 原字节/取样/写序事实，不代全父/数值/x87/机器ABI |
| 身份 | 新1686/1687/1688 adapted | 旧1686条/冻结定位/pending18与ECX pending7保持 |
| 功能 | LOOP98→101 / MAP95→98，各两局部I/S，共4E各25来源 | 完整成员、生产消费和寿命/运行门禁不伪闭 |
| 派生 | 语义1689/有效1682、合同1800（48.91%）、处置830/E968、R7已审398/余2663 | 汇总器生成，静态后记录实值 |
| 任务 | U10/V03/R04仅current/引用仍open | closed11/差队列及完整门禁保持 |

仅必要静态/纯文本汇总，不构建、测试、运行或计算摘要。完整正常生产消费/全部owner-alias寿命、数值/x87/全父/机器ABI/链接/像素画面/明文互通及R8继续，验收标准保持。

<a id="coordinate-reset-sampling-source-state"></a>
## 当前状态

原三根/四窗口/4B数据/三尾及源前态已取得。七片段差异确认，先记本SPEC后实修并同步唯一进度、台账和任务。静态与汇总后另记实值。命名快照：cg2d-re-coordinate-reset-sampling-20261008。

BUG01必要静态已通过：完整Scene仅一正文七片段变化，两正确helper保持；三原根113条530B、四窗口/4B数据/三尾30B与证据/台账/任务模型一致。汇总及 -Check 通过，完整派生实值：语义1689/有效1682、合同1800（48.91%）、处置830/E968、R7已审398/余2663、LOOP101/MAP98。旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11及完整门禁保持；仅本步静态，未编译、测试或运行，完整正常寿命/数值/x87/机器ABI及R8继续。命名快照：cg2d-re-coordinate-reset-sampling-20261008。
