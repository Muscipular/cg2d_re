# 聊天气泡消费与释放取样 SPEC（BTJ01）

日期：2026-10-07。接BTI01文本生产沿真实消费窗口推进，先裁决再实施；原格式明文/音频暂停/执行限制与RPLAN.24保。

<a id="actor-chat-consumer-baseline"></a>
## 原基线与差异

父442F70原元数据1220条@[442F70,443FF1)、唯一4417E0@441D9B；已有1073十名称格式点及AAS01入口/附件字段保。本步只完整取聊天窗口[4433C6,443562)134连续指令412B/三页53+70+11，8direct六目标、一Tick、11jump无RET；不复验或宣称其余1086条和完整父已验，不重复既有名称/附件工作。

原flags bit2000非零才读Tick，deadline W32与tick无符号<=走过期。443541先取旧text1C地址，再清本地flags bit2000，44354A判同旧指针，free后原清槽；现清flags后取槽且free再次取槽，恢复同旧地址与原序。保现本地flags、后续真实发布/索引，不把它改成即时清共享字段。

未过期先气泡开关、action flags bit180短路，再4433FB取text1C并判NULL；443406读取S32 length并覆盖原var114/stateIndex，即使零仍覆盖，零直接跳后续名称，非零包括负数继续。443415才取fullS32 font，首TextMeasure复用谓词里的text地址；现重新读text，恢复原样本。

首测宽后取imageNo/4417A0图高，再现场action Y，计算y=height+Y-32，443439重新取fullS32 font计算底部指标与bottom。length>40时443453重新取text+41，第二TextMeasure复用上述metricFont；现再次取font，改为同指标font样本。max两行宽和原零/非零长度分支保持，不新增长度门禁。

第二行绘制44346F text+41→443472真实color低BYTE→443475真实font低BYTE，再计算Y+4，44348C真实低W16 action X；宽度half向零，原W16减法/最终S16传递。当前color/font及X先读全W32再截，恢复真实W8与W16取样，字段owner仍S32。DrawText后44349E重新取fullS32 font计算doubleHeight并扣y，不能复用先前指标。

第一行4434B2重新取text，4434BA color低BYTE、4434C0 font低BYTE、4434C5 X低W16，然后Y+4及向零half/S16 X。当前也读全W32后截，恢复真实宽度与先X后Y。两BYTE均按原S8视图传递，两个低W16用定宽memcpy取本机原字段，不造新helper/改owner字段宽度。

第一DrawText后4434DE九宫格X另读完整W32，不能套用两文本的W16读取；保原半宽缓存、图3BE84/原边距/宽高/颜色80FFFFFF和参数CE/-1/74/54。fallback仍原Y→X采样与图573F；原全部后续名称/格式/附件/状态别名和其它正文保持。

<a id="actor-chat-consumer-decision"></a>
## 实施裁决

YobiRuntime.cpp既有Sub_442F70六唯一片段／一个原业务体：旧指针及谓词text样本、两测宽font/指针、两绘制真实W8/W16和读序、九宫格另取全W32 X。短路内才取drawableText，局部非拥有地址不延长生命周期，无新状态/helper/接口/外调或规则。

1073所有原role/source/end/count/十格式及AAS01字段保后追加；原LOOP/MAP成员与adapted处置完整保持后追加新证据引用，新增实际TEXT成员27→28/两局部I-S。无新语义/F/处置/R7或范围排除。字段/owner布局与普通S32算术保，真实W8/W16边界不等于异常回绕模拟；不加保护、拒绝、钳制、补零/限长或编码。

本步局部消费/过期窗口静态，不代全父/六callee/全部正常文本分配—绘制—过期释放域、SDK-ABI/链接/画面/原格式明文互通或运行。

<a id="actor-chat-consumer-acceptance"></a>
## 可度量验收

| 项 | 标准 |
|---|---|
| 窗口 | [4433C6,443562)134条412B/三页、8direct六目标/一Tick/11jump无RET，父元数据/唯一caller保 |
| 释放/短路 | 旧指针先取再本地flags清位，NULL/free同样本；气泡/flags短路后取text并复用首测宽 |
| 指标/测宽 | fullS32 font现场读；底部metricFont复用第二测宽，第二text新取+41，第二Draw后font指标新取 |
| 绘制宽度/序 | text→真实W8 color/font→真实W16 X，第二Y先/第一X先；向零half/最终S16保，九宫格另读全W32 X |
| 保持 | unsigned deadline<=、零length/stateIndex别名/负数路径、两行max/原调用/九宫格/fallback、所有其它正文 |
| 差异 | 整源=HEAD+六唯一片段/一个业务体，1073全原字段/AAS01/原两成员及处置保后追加 |
| 登记/派生 | 无新语义/F/处置/R7，TEXT28/两I-S、measure/-Check终态0/完整快照增量/编码与真实引用 |
| 门禁 | 原其余1086条不代验；全父/provider/全部正常寿命/SDK-ABI/链接/画面/明文互通/运行R8保持未验，原任务门禁保 |

<a id="actor-chat-consumer-source-state"></a>
## 实施状态

按工程委托裁决后实施；集中静态/派生结果在本节与唯一进度追加。不构建、测试、运行或计算摘要，不手改派生看板/快照，不改变网络明文与音频暂停。

### 集中静态与派生完成记录

2026-10-07：一源六片段已实施。整源=HEAD+六唯一片段、旧释放/首测宽text与第二测宽metricFont复用、两绘制真实W8/W16/原读序及另全W32边框X、原别名和所有其它正文及Font.cpp提供者保持；134连续原指令412B/三页、原真实宽度/八direct六目标/一Tick及父元数据和局部门禁边界、完整机器增量、四文档和三任务六列，四组集中静态全部通过。measure/-Check同会话终态0，完整派生快照增量通过；12文件UTF-8无BOM/LF、四锚点及两局部I-S各21真实来源/差异文本通过。

真实结果：TEXT28/E886；语义1616有效1609/文档1727（46.93%）/处置757/R7 327余2734保持，1073十格式点/AAS01/原两成员与处置、全部其他域/冻结定位/pending18/差队列/原任务和完整门禁保持。本步确认的消费/释放取样差异已修；其它1086条未复验、全父/provider/全部正常文本生产绘制过期寿命、SDK-ABI/链接/画面/明文互通/运行R8继续。未构建/编译/测试/运行客户端或算法样例、未计算摘要。
