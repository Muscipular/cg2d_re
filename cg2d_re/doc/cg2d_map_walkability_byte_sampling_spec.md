# 地图第四平面 BYTE 取样与空格短路 SPEC（BUL01）

日期：2026-10-08。承接BUK01地图绘制父，沿实际调用的4693F0/46D9C0第四平面生产推进。按AGENTS.md第8条先记后实施。[完整原端/跳表/局部后窗/源前态](cg2d_map_walkability_byte_sampling_evidence.json)。

<a id="map-walkability-byte-baseline"></a>
## 原端与差异

| 根 | 完整原范围 | 原端量 | 本步变化 |
| --- | --- | --- | --- |
| 普通4693F0 | [4693F0,4697C9) | 308条985B/五页，十call/55jump/一RET | 两hit BYTE读取与obj→tile→walk短路，三片段 |
| CUT46D9C0 | [46D9C0,46DA72) | 73条178B/两页，一call/八jump/两RET | mode1 hit高BYTE bit0读取，一片段 |

合381条1163B/七页两块、11directcall/63jump/三RET；四入向，父去重46CF60/469030/46D9C0。原CF60已BUK01全量归档，本步只补三个外部调用后局部窗口，转发边界在完整CUT根内，不复审全父/所有callee。

原469497 test byte [hit+2*index+1],40h，原469766 cmp byte [hit+2*index],1；当前分别读整个WORD再掩码/截低BYTE。原46DA17 test byte [hit+2*index+1],1，当前整WORD掩码。虽然静态位值相同，读取宽度不同；按原地址直接BYTE视图承接，不改公开WORD平面/owner/布局。
原46972F→469739→469744依次obj WORD、tile WORD、walk WORD短路，现tile先于obj；仅该条件调回原序。

## 保持的正常生产与接线

两根各先清2601 WORD/5202B，共享第四输出仍gMapWalkabilityArray，参数的tile/obj/hit/输出沿原生非拥有指针传入，不复制平面。普通非正宽或高返回0，正宽高完整tile轮后obj轮，最终2*width保持。

tile/obj均先零扩展原WORD交既有46CF00。映射大于999或60..79交既有GetGraphicMapNoIndex/GetGraphicFlag1：flag0写1且不覆盖2，flag2写2，其它保持；代码2/5/6/9/10写1但不覆盖2，代码4写2。tile代码0只按hit高BYTE 0x40。obj大于999且flag0/2时沿既有GetGraphicSxSy，输出S16宽高向右向上遍历，只保原行非负/列小于宽度；flag0不覆盖2，flag2无条件写2。obj空格按原三WORD短路后补1，最后hit低BYTE==1可无条件覆盖2。所有资源查询/RNG和业务表达式保持；三个已纳管getter每次都写输出，失败清零已证，初始化不是新造失败默认。

CUT只按完整W32模式==1进入简化分支，非正宽或高返回原Y跨度，正宽高按hit高BYTE bit0写WORD1、否则保持零，结束0。其它模式按原八实参转普通并透传返回。不造固定成功、不加保护/限长/异常或回绕模拟。

三个外部后窗证实原CF60两处立即覆EAX，469030局部后续投影覆EAX；现调用显式弃局部返回，保持已有接线。§84旧“尚未接线”是2026-09-25历史，当前已接线；该节WORD掩码是值级表达，本步恢复实际BYTE读取。只证明已列局部，不称全父/生命周期/全ABI或画面已验。

## 跳表与用途

[4697C9,469810)71B包含63B数据：4697CC四个W32目标、4697DC十一BYTE case槽、4697E8九个W32目标。原两个间接jump/一case读取引用与实际目标均归档，对应当前if分支，不增加新F。
其余8B为3B NN_LEA ECX,[ECX]与5B NN_NOP；不是71B全NOP。[46DA72,46DA80)另14B逐BYTE NN_NOP，无入向/后真入口。两R7仅真实用途分类，不排除功能范围。

<a id="map-walkability-byte-decision"></a>
## 具名裁决与实施

只改SceneState.cpp两个既有正文四片段：普通高/低BYTE和obj-first短路、CUT高BYTE。使用reinterpret_cast<const std::uint8_t*>(hit)在2*index(+1)处按原宽度读，原S32索引表达式和WORD输出/其它源码保持。无新helper/lambda/header/共享owner/依赖或业务。
新1698/F:000693F0、1699/F:0006D9C0均adapted。普通宿主memset/Graphic承接和两正常返回合同保持；不据源码局部表达外推完整机器ABI。网络原格式明文、内部LegacyCryptInt/音频暂停/RPLAN.24持续。

<a id="map-walkability-byte-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 边界 |
| --- | --- | --- |
| 源码 | 一cpp，两正文四片段 | 完整模型只三BYTE与一条件变化，其它全源/声明/owner保持 |
| 原端 | 381条1163B/七页、三局部窗口/85B用途 | 原字节/跳表/分支/宽度/正常返回归档，不代全callee/父/运行 |
| 身份 | 两新adapted，两个R7用途 | 旧1698条/冻结定位/pending18与ECX pending7保持 |
| 功能 | LOOP110→112/MAP107→109，各局部I/S，共4E各25来源 | 完整成员/正常生产消费/寿命/运行门禁继续 |
| 派生 | 语义1700/有效1693、合同1811（49.21%）、处置841/E988、R7已审407/余2654 | 汇总器生成，静态后记实值 |
| 任务 | U10/V03/R04仅current/引用，仍open | closed11/差队列/完整门禁保持 |

仅必要静态和纯文本汇总，不构建、测试、运行或计算摘要。完整正常生产消费/owner-alias寿命、全父/callee/数值/x87/机器ABI/链接/全像素画面/明文互通与R8继续，不降低标准。

<a id="map-walkability-byte-source-state"></a>
## 当前状态

两原根/三个局部后窗/两跳表与尾区、源前态已取得，四差异确认。先记本SPEC，再修复并同步唯一进度/台账/三任务，静态与汇总后记录实值。命名快照：cg2d-re-map-walkability-byte-sampling-20261008。

BUL01必要静态已通过：完整SceneState仅两既有根四片段，普通/CUT三hit BYTE取样和obj→tile→walk短路恢复，原共享WORD平面/5202B全清/两轮资源查询-flag及S16覆盖/模式转发/正常返回和全部其它源码/声明/owner保持。两原根381条1163B/七页、三个外部后窗、63B跳表及3B LEA/19B NOP用途与证据/台账/文档模型一致。汇总及 -Check 通过，派生实值：语义1700/有效1693、合同1811（49.21%）、处置841/E988、R7已审407/余2654、LOOP112/MAP109。旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11及完整门禁保持。未编译、测试或运行，完整正常生产消费/寿命/数值/x87/机器ABI/像素及R8继续。命名快照：cg2d-re-map-walkability-byte-sampling-20261008。
