# 地图窗口请求与索引表基线 SPEC（BUN01）

日期：2026-10-08。承接BUM01，沿地图父尚缺基线的窗口搜索与查表依赖推进。按AGENTS.md第8条具名裁决。四个现有正文正确，源码变更0。[完整原端/五局部后窗/源前态](cg2d_map_window_search_baseline_evidence.json)。

<a id="map-window-search-baseline"></a>
## 本步完整范围

| 原根 | 完整范围 | 原端量 | 当前源码 |
| --- | --- | --- | --- |
| 469870 | [469870,469C87) | 318条1047B/五页/20call/47jump/五RET | 四方向窗口生产正确保持 |
| 46AF30 | [46AF30,46AFDE) | 44条174B/一页/四directcall、一间接call/三jump/一RET | 请求参数与共享状态生产正确保持 |
| 46CF00 | [46CF00,46CF1A) | 九条26B/一页/两jump/两RET | 当前表读取正确保持 |
| 46CF20 | [46CF20,46CF57) | 17条55B/一页/三jump/一RET | 当前表填充与借用返回正确保持 |

合388条1302B/八页四块、24directcall/一GetTickCount间接call/55jump/九RET，十入向/六父。五局部后窗及既有CF60/4693F0/467F30/469FC0完整基线引用不等于全部父/生命周期/ABI已验。

## 搜索生产合同

S16跨度X/Y任一小于51返回0；四条件中的未消费-11/+11仍调用，不删其读取/RNG消费。先清W16窗口数，再按W/N/E/S四方向组各23步短路检查坐标和hit WORD==0。
每组span在两Player运算后读取并持有；第一组XBound逐轮fresh，另三组各held XBound；YBound逐轮fresh。方向组为6/7/5、0/7/1、2/1/3、4/3/5，保持次序，两次S16窗口数>=2提前返found，不新加过滤/保护。

W分支的窗口W16写序为endX→startX→startY→endY，N/E/S为startY→startX→endX→endY；四次都传原四个共享字段的native地址交既有468290裁剪，调用后W16 inc计数/设置found。四个两项W16数组及一个W16计数沿同一owner，不造新窗口表。原相应起止公式、held状态与found0/1返回均保持。

## 请求、查表及初始化消费

46AF30先取pending direction S16交搜索，失败返回0。成功按当前S16窗口数循环，各轮endY/endX/startY/startX S16先取，再floor/mapId W32/native socket，传原510710的socket/mapId/floor/startX/startY/endX/endY；调用后fresh count。尾W32等待标志、W16方向、PlayerX读pub→Y读pub→GetTickCount W32pub/返保持。

510710当前源码为既有UUN六整数构造与发送；本步只核定原AF30调用点/参数序及现源码接线，不称510710完整原体或网络互通已验。保留原格式明文合同，不新增、更名或修改协议，不恢复加解密叶。

46CF00首判定实际jnb，以U32>=65536原样返回入参位型；负S32也在首门禁返回，不擅改为拒绝或0。其后的原负门禁源码保持；正常索引从唯一W32表当前内容取值，不用填表公式代替消费。
46CF20前20000项identity，后45536项index+200000，原前段pointer<=last/后段pointer<one-past对应现正常两循环；末返native非拥有gValuesC35010+65536，不截W32或复制表。原相邻槽位别名与全部ABI不据one-past局部观察闭合。

§11旧“467F30未实现”和旧查表段“父/三个caller未落位”仅历史；当前467F30已1577，CF60/4693F0及469030已有接线，本步同步当前状态，正确源不改。四尾9+2+6+9=26B实际逐BYTE NN_NOP/90，无入向/后真入口，仅R7用途。

<a id="map-window-search-decision"></a>
## 具名裁决

四根具名登记adapted：1704/469870、1705/46AF30、1706/46CF00、1707/46CF20；正确四正文、所有声明/布局/owner/父及网络源保持，代码新增/变更0，不为纳管制造改动。
只新增两域S静态证据，不新增I开发成果。U10/G02/R04/V03只current/引用，仍open；原格式明文/内部LegacyCryptInt/音频暂停/RPLAN.24持续，不专项异常/越界/回绕模拟。

<a id="map-window-search-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 边界 |
| --- | --- | --- |
| 源码 | 四既有正文/CPP全文保持，源码0改动 | 原正常取样/宽度/状态/返回与当前接线证据，不造新实现 |
| 原端 | 388条1302B/八页、五后窗/26B用途 | 全四根及字段/调用归档，不代所有callee/父/510710原体/运行 |
| 身份 | 四新adapted/四R7用途 | 旧1704条/冻结定位/pending18与ECX pending7保持 |
| 功能 | LOOP116→120/MAP113→117，两S各25来源，新增I为0 | 完整枚举/生产消费/寿命/像素/明文互通与运行门禁继续 |
| 派生 | 语义1708/有效1701、合同1819（49.43%）、处置849/E994、R7已审415/余2646 | 汇总器生成，静态后记实值 |
| 任务 | U10/G02/R04/V03仅current/引用，四任务open | closed11/差队列及完整门禁保 |

仅必要静态/文本汇总，不构建、测试、运行或计算摘要。完整正常生产消费/owner-alias寿命、全父/callee/数值/x87/机器ABI/链接/全像素画面/明文互通及R8继续，不降低标准。

<a id="map-window-search-source-state"></a>
## 当前状态

完整四根/五局部窗/尾区及源前态已取得，正确取样/返回/接线确认，先记本裁决后纳管并同步唯一进度/台账/四任务；静态与汇总后记实值。命名快照：cg2d-re-map-window-search-baseline-20261008。

BUN01必要静态已通过：四既有正确根及SceneState/相关CPP/声明/owner/网络源全文保持，源码0改动、仅新增两域S/新增I为0。四原根388条1302B/八页、五局部后窗/四尾26B用途与完整证据/机器及文档模型一致。汇总及 -Check 通过，派生实值：语义1708/有效1701、合同1819（49.43%）、处置849/E994、R7已审415/余2646、LOOP120/MAP117。旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11及完整门禁保；FUNC-NETWORK原57成员/既有门禁保持，510710原体不由本步外推。未编译、测试或运行，完整正常寿命/数值/x87/全父/callee/ABI/像素/明文互通及R8继续。命名快照：cg2d-re-map-window-search-baseline-20261008。
