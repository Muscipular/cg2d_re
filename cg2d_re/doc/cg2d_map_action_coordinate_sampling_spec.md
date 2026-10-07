# 地图动作坐标载荷取样 SPEC（BUJ01）

日期：2026-10-08。承接BUI01注册表与真实地图绘制依赖，按AGENTS.md第8条先记后实施。[完整原端/两窗口/12B常量/源前态](cg2d_map_action_coordinate_sampling_evidence.json)。

<a id="map-action-coordinate-baseline"></a>
## 原端与差异

| 原入口 | 完整范围 | 原端量 | 本步处理 |
| --- | --- | --- | --- |
| 46DE50 | [46DE50,46DFE7) | 107条407B/两页/十二call/一jump/两RET | 四坐标临时三载荷先读齐后发布实修 |
| 46DD90 | [46DD90,46DE50) | 59条192B/一页/七call/无jump/一RET | 当前投影/Multiply/FrameMove/偏移及图形消费正确保持 |

合166条599B/三页两块、19directcall/一jump/三RET；各唯一父46CF60，创建@46D281/更新@46D306，仅两局部后窗，不代父全CFG/ABI。已有46DFF0/MapDrawCache语义1257与全文保持，不重复计量。

## 四临时载荷实修

原46DE50的mapX200、mapY210、mapX1E0、mapY1F0各自Construct后按encoded W32→mask W32→count W8读取齐，再同序发布到同一created action，再Destroy；复用原同一临时槽，实参X/Y/X/Y。

当前通用AssignFromInt通过CopyPayload逐字段取即写，与原预取序不同。仅该创建正文四片段展开既有Construct/Destroy：同一局部LegacyCryptInt复用四次，各三字段具名预取后原序发布。通用工具保持，避免外推其他调用者的采样合同；四构造/析构及唯一RNG次数保持，无新helper/lambda/接口/所有者。

## 保持的创建、绘制及正常生命周期

创建仍New(2,0)，原NULL失败返回NULL；成功沿同一created指针，callback空、imageNo/action/direction W32、priority W8=10、flags W32=30、field270 W16=0及两W32零写保持。原mapX/Y乘64、F32世界坐标/局部输入、投影/每轴F64加0.5及ftol、先X后Y屏幕发布、最终返回created借用指针保持。

更新绘制先worldY/X输入投影，X截断后Y biased先计算再X发布/Y截断发布；两次Multiply64各单读并落F32，再FrameMove(action,100,0,0)。之后先offsetY/X，再screenY/X相加、graphicIndex和原16B描述符，低W16坐标交已有46DFF0。当前正文正确，保持数值/字段/调用及已决void动作观察；原尾EAX沿46DFF0，不造固定返回。

创建后父把原返回指针发布到共享槽，offsetX使用刚created指针；之后offsetY按原fresh取槽。当前MapSceneDrawing父正确保持，不统一两次偏移的取槽方式、不加父NULL保护。绘制后收栈/跳转仅局部观察，完整父/回收/owner-alias寿命和机器ABI不因此闭合。

已加载54DD30四BYTE F32位值42800000（64）和54DD28八BYTE F64位值3FE0000000000000（0.5），只证明位值，不代原x87数值/舍入等价。创建尾9B逐BYTE实际NN_NOP/90，无入向/后46DFF0，仅用途；更新根紧接创建，无额外区间。

<a id="map-action-coordinate-decision"></a>
## 具名裁决与实施

只改Yobi一个既有创建正文四片段。四组临时三预取、原构造/析构/发布序恢复；正确更新绘制、父发布/偏移fresh取槽及通用工具和MapDrawCache保持。所有参数、字段宽度、创建/失败、返回和数值表达式保持，不造影子状态/保护或新增功能。

新1695/F:0006DE50、1696/F:0006DD90均adapted。网络原格式明文、内部LegacyCryptInt、音频暂停/RPLAN.24持续；源数值承载/void局部观察不外推全ABI。

<a id="map-action-coordinate-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 验收边界 |
| --- | --- | --- |
| 源码 | 一cpp、两既有正文，仅创建四片段变化 | 全文精确模型；四组W32/W32/W8预取，同一临时四构造析构，正确其余源码保持 |
| 原端 | 166条599B/三页、两局部窗口/12B常量/9B用途 | 原字节/时序/正常字段及返回归档，不代全父/数值/x87/ABI |
| 身份 | 新1695/1696，两个adapted | 旧1695条/1257/冻结定位/pending18与ECX pending7保持 |
| 功能 | LOOP107→109/MAP104→106，各两局部I/S，共4E各25来源 | 完整成员/正常生产消费/寿命与运行门禁不伪闭 |
| 派生 | 语义1697/有效1690、合同1808（49.13%）、处置838/E980、R7已审404/余2657 | 汇总器生成，静态后记录实值 |
| 任务 | U10/V03/R04仅current/引用，仍open | closed11/差队列和完整门禁保持 |

仅必要静态/纯文本汇总，不构建、测试、运行或计算摘要。完整正常生产消费/owner-alias寿命、全父/callee/数值/x87/机器ABI/链接/像素画面/明文互通及R8继续，验收标准保持。

<a id="map-action-coordinate-source-state"></a>
## 当前状态

原两根/窗口/常量/尾区和源码前态已取得，四载荷取样差异确认，先记本SPEC后实修并同步唯一进度/台账/任务。静态与汇总后记录实值。命名快照：cg2d-re-map-action-coordinate-sampling-20261008。

BUJ01必要静态已通过：完整Yobi仅创建一正文四片段变化，四组W32/W32/W8预取、同一临时四Construct/Destroy及原创建/返回/数值保持；正确更新绘制、父/通用工具/1257全源保持。两原根166条599B/三页、两窗口/12B常量/9B用途与证据/台账/文档模型一致。汇总及 -Check 通过，完整派生实值：语义1697/有效1690、合同1808（49.13%）、处置838/E980、R7已审404/余2657、LOOP109/MAP106。旧对象/1257/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11和完整门禁保持；仅本步静态，未编译、测试或运行，完整正常寿命/数值/x87/机器ABI及R8继续。命名快照：cg2d-re-map-action-coordinate-sampling-20261008。
