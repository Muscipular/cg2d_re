# 旧面板按钮桥接取样 SPEC

编号 BVM01；2026-10-08。沿BVL01公共旧面板入口继续真实绘制/关闭查询依赖；修复现有按钮兼容桥的源字段取样顺序。

<a id="panel-button-bridge-baseline"></a>
## 原基线与缺口

[完整原证](cg2d_legacy_panel_button_bridge_sampling_evidence.json)保存三个完整根、原指令/字节/入向、取样核心邻窗及源码/台账前态。

| 原身份 / 新语义 | 完整根 | 本步 |
|---|---|---|
| F:00103440 / 1766 | 503440–503585，116条325B/两页 | 正确公共绘制保持/补映射 |
| F:00103590 / 1767 | 503590–5035D8，29条72B/一页 | 正确关闭查询保持/补映射 |
| F:001035E0 / 1768 | 5035E0–5036E9，107条265B/两页 | 正确图形按钮保持/补映射 |

合252条662B/五页，10直接调用/五目标、无间接、15跳/四返回；三根97/68/208实际CALL合373处，仅引用定位，不能当全部父验收。各未归父点与重复入向保原证。

503990已有[BSP01完整基线](cg2d_modal_button_geometry_sampling_spec.md#modal-button-geometry-source-state)正确保持。本步取其前25条仅核源top/left/height/width四次读取与首命中，不重复纳管该原根/472调用处，也不修改核心正文。

现有QueryPanelButtonState为防止图号尾被当作文本，先构造局部CharacterModalUiRecord；初始化表达式却按left→top→width→height读取原按钮。原503998/50399B/50399E/5039A1要求top→left→height→width。应在桥接边界恢复四个源字段的先后，再用持样值构造同一局部记录；正常值不变，不声称已观察错误运行结果。

<a id="panel-button-bridge-decision"></a>
## 实施决定与保持项

1. 仅LegacyUiRecord.cpp::QueryPanelButtonState一个现有桥接片段：先top、left、height、width四次S32取齐，局部记录仍为0/0/left/top/width/height/nullptr，交原503990并透传完整结果。零新helper/owner/API或实现函数，不给桥造原地址或独立F身份。
2. 三绘制/关闭主体已正确，全文保持。原503440悬停字段读取/四边窄化、五页每轮S8选中/零图号仍绘、height/y/width/x和有符号除2、flags低bit0关闭钮、随后fresh选页/背景及unsigned图号<=999透传，否则212层绘制/原返回保。
3. 503590保持y→height→x→width、矩形bottom/right+1；只区域命中才查询关闭按钮，低AL&40非零→1，其余0。5035E0保持首四字段/按下BYTE及style短路、各image→y→x、hoverMode后第二独立查询/第三fresh四边与±2/低W16、hoverMode2细节1及void结果。
4. 两处原16B图形descriptor确实全清；面板在此基础上FFFFFFF/mode4，按钮FFFFFFF/mode0，现实现正确，不能把合法清零误删为“额外保护”。层次、图号、参数及所有其它描画/提示/关闭数据保。
5. 保留BVL01的共享action-dirty原生取样、99项唯一owner及0/100空handler；LegacyAudio源/头与全部音频调用保持，暂停不解除。核心CharacterModalButton、类型及描画头不改，不引入或改变网络格式/加解密。
6. 新增三真实原新1766..1768，按既定宿主描画/原类型兼容边界登记三adapted处置；公共UI流16→19。该流一个I仅现有桥接实修，一个S仅三根/本链兼容；三正确主体不记新增实现I，也不向全部373调用处/所有旧页泛化。
7. U04/U11/V03/R04只追加进展/来源、仍open；其他域、原语义及完整成员/门禁保持。RPLAN.24只正常几何/原失败，不专项处理溢出/越界，不模拟故障等价，不新增限制/钳制。
8. 只必要静态与文本汇总，不构建/调试/测试/客户端或算法样例运行、产物或摘要。真实旧输入外路由/全部父-callee、数据生产/正常owner-alias寿命、像素/机器ABI链接/运行与R8仍按原标准验收。

<a id="panel-button-bridge-acceptance"></a>
## 度量与验收

| 项目 | 要求 | 边界 |
|---|---|---|
| 原证 | 三完整252条662B/五页、10直接五目标/15跳4RET及373引用 | 50399025条仅取样邻窗 |
| 源实修 | 一cpp现有桥一片段，四源字段按原序取齐/原矩形与完整返回保 | 不新造原地址/函数身份 |
| 正确实现 | 三主体、BVL公共入口/97入向基线、核心/音频/类型头及其它源全文保 | 不称已观察值错误或像素运行通过 |
| 追踪 | 新1766..1768及三F，UIRUNTIME19两个限定I-S | 仅桥接新I，完整门禁不解锁 |
| 汇总 | 语义1769/有效1762，合同1879（51.06%）、F917/E1092 | R7已审467余2594、pending18/closed11/全叶0/globalfalse保 |

<a id="panel-button-bridge-source-state"></a>
## 收口要求

先行SPEC、原证/前态后修现有桥一片段，同步公共旧面板/运行期索引、语义/验收、四任务及唯一进度；集中静态与文本汇总/Check后記实际值，精确提交固定命名快照cg2d-re-legacy-panel-button-bridge-sampling-20261008。原完整运行门禁保持，整体SPEC目标继续推进。

### BVM01本步实际静态结果

集中静态通过：三个完整根252条662B/五页、10直接五目标/零间接/15跳4RET及373真实引用；503990本步25条仅核原top/left/height/width取样和首命中，完整核心沿BSP已有基线。七整源仅现有QueryPanelButtonState一个桥接片段，四次S32源字段按原序取齐，局部矩形/核心与完整返回保；三正确主体与BVL公共入口/所有其它源头保持。两处原16B descriptor确实全清，面板mix4/按钮mix0、五页/原除2和unsigned999、层次/悬停/按下/几何/结果保，不误删合法初始化。完整原新/登记和派生仅具名差集，两个限定I-S各18来源，I仅桥接实修、四任务仍open。

文本汇总及Check均退出0。实际语义1769/有效1762、合同1879（51.06%）、F917/E1092、公共UI流19；R7已审467余2594、pending18/closed11/全叶0/globalfalse、原差队列/CRT43及所有其它成员和门禁保持。三正确主体无新实现I，核心不重计；不新造外路由/99回调或恢复暂停音频，原格式明文/RPLAN.24与执行限制保。所有父-callee/数据生产/正常owner-alias寿命、像素/机器ABI链接运行/旧输入外路由与R8未验，不构建/调试/测试/运行/产物或摘要，整体SPEC目标继续推进。
