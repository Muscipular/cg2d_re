# 头顶聊天文本标记与取样 SPEC（BTI01）

日期：2026-10-07。沿BTH01接收后的两个实际文本入口推进，先裁决再实施；原格式明文、音频暂停、单线程/执行限制与RPLAN.24保持。

<a id="actor-chat-text-baseline"></a>
## 原基线与差异

| 原根 | 完整基线 | 唯一消费 |
|---|---|---|
| 442DC0 非玩家 | [442DC0,442F6D)156条429B/三页70+70+16，六direct五目标/一Tick、15jump三RET | 50CAE0@50CDEE，action/duration1000/snapshot/color/font/S32 length |
| 51A6A0 玩家 | [51A6A0,51A835)148条405B/三页70+70+8，五direct四目标/一Tick、15jump三RET | 50CAE0@50CDBD，duration1000/snapshot/color/font/S32 length |

合304条834B/六页两chunk、11direct五唯一目标与两Tick导入、30jump六RET。现两个void入口与唯一caller已接，不以原EAX残值发明业务返回；两F原仅冻结源码定位，新语义1614/1615补完整基线，不造新原函数。

非玩家先action非NULL，payload一次；442DDE仅OR flags42高BYTE的0x20，现OR整个W16，恢复高BYTE访问。Tick返回加原duration后发布deadline14，再43D860(action)，之后取旧text1C地址一次供NULL/free，free后清槽。玩家51A6B1同样只OR flags18A高BYTE，现MarkPlayerChatActive写整个W16；payload取样在标记之后，Tick/deadline/旧text释放保。两旧槽现判空后再次读取作为free参数，恢复同旧地址样本，不改变释放域或加新清理。

两根原名字测长+2后按文本GP/MIC/家族三分支分别额外+4/+5/+6。GP只四BYTE短路，MIC/家族用原真实StrNCmp_53BFC0(5/6)，源正文不补NUL。玩家51A6DF在释放后再次取当前action名字，不能用入口action替代；非玩家仍使用入口action名字。共享AllocateActorChatText现正文与两caller保持此差异，不另造原helper身份。

remaining=原S32 length-offset；分配数量由LEA remaining+3推入原U32 malloc参数。现直接S32转宿主size_t，恢复先实际U32参数再拓宿主数量；S32加减仍普通运算，不以U32绕算替代、模拟溢出或加负容量门禁。分配返回地址先发布text1C，再以同native return判NULL；现判已发布槽，恢复复用返回样本。失败保已发布NULL且不写color/length/font；成功依原color20→textLength24→font28发布，CRT库内机制按D02，不把应用字段/分配计数扩宽。

remaining不大于40时，原每轮51A81B/442F53先取当前text1C地址，之后取source BYTE、写目标BYTE；现赋值表达式先求右侧源再求左侧地址，改显式三步，每轮仍新取目标槽，不缓存整个循环。S32 index/remaining比较与末次重取text1C写NUL保。

remaining大于40才44B570(body,40)，仅AL=2走40B第一行/NUL@40、从body+40复制第二行到+41、末NUL@remaining+1；其它走39B/NUL@39、body+39复制至+41、末NUL@remaining+2。保原未写gap@40，不预清分配块、不改为连续39+40布局/新分隔符或UTF-8行宽；两长分支各次现场text槽读取保，末NUL均原位置。

X:00042F6D-00042F70 3B、X:0011A835-0011A840 11B均逐BYTE90/NN_NOP、无入向、前RET后真实入口；仅14B对齐用途，不造函数/helper/范围排除。

<a id="actor-chat-text-decision"></a>
## 实施裁决

YobiRuntime.cpp五片段：非玩家高BYTE标记、非玩家/玩家旧free地址样本、共享分配U32数量/native return与短复制目标地址先读。SceneState.cpp既有MarkPlayerChatActive一片段改高BYTE；共六片段、两已有原业务体及两已有helper，无新接口/helper/外调或共享owner。

W16字段及相邻跨字段观察保持，只取高BYTE视图；原action/payload/名字取样时点、NULL失败、Tick/deadline、前缀/两strncmp、三字段发布、原40/39布局/未写gap/末NUL及所有其他源头保持。只拓真实返回指针/宿主API数量承载，不把普通S32算术改回绕模型，不新增异常保护、拒绝、限长或编码。

补两既有真F语义1614/1615、TEXT25→27/两adapted、两局部I-S及两个区间14B用途；三任务仅current/引用。正常分配释放、绘制过期清理的完整owner-alias寿命和全部provider/接收父/ABI/链接/画面/明文互通/运行未验；不降低门禁。

<a id="actor-chat-text-acceptance"></a>
## 可度量验收

| 项 | 标准 |
|---|---|
| 原根/接线 | 两完整根304条834B/六页两块，11direct五目标/两Tick/30jump六RET，两个唯一50CAE0调用保 |
| 标记/释放 | 两W16 owner仅高BYTE OR0x20；各旧text地址取一次供NULL/free/原清槽 |
| 分配 | 普通S32 remaining+3→实际U32→宿主size_t，返回地址先pub后同native return判NULL，原失败/成功字段写序 |
| 短复制 | 每轮现场text1C目标地址→source BYTE→写BYTE；S32循环/末次槽取样保 |
| 保持项 | payload/玩家末次action名、非玩家43D860、Tick/前缀/两strncmp、原40/39布局/+41/未写gap/三NUL、所有其它源头 |
| 源差异 | 两cpp完整=HEAD+六唯一片段，原业务体与两个已有helper，无新helper/状态/规则 |
| 登记/派生 | 两新语义/TEXT27/两处置/两I-S/14B用途，measure/-Check终态0、完整快照增量/编码与真实引用 |
| 完整门禁 | 全接收父/provider/全部正常分配释放域与绘制过期寿命/ABI/链接/画面/明文互通/运行R8不代验，原任务/门禁保 |

<a id="actor-chat-text-source-state"></a>
## 实施状态

按工程委托裁决后实施；集中静态与派生结果在本节和唯一进度追加。不构建、测试、运行客户端或算法样例、计算摘要，不手改派生看板/快照，不改变现网络明文或音频暂停。

### 集中静态与派生完成记录

2026-10-07：两源六片段已实施。完整两源=HEAD+六唯一片段、高BYTE/旧释放指针/分配实际U32参数及同native return判NULL/短复制每轮地址先取，原三字段发布/名字时点/断行gap与所有其他源头保持；两原根304条834B/六页/两唯一接收消费、真实W8标记/两Tick/三前缀与14B逐BYTE用途，整个机器增量，四文档和三任务六列，四组集中静态全部通过。measure/-Check同会话终态0；完整派生快照仅两语义/TEXT成员及候选、三任务/两个X与六指标增量。13文件UTF-8无BOM/LF、四锚点及两局部I-S各23实际来源/差异文本通过。

真实结果：语义1616有效1609/文档1727（46.93%）/处置757/E884、TEXT27/R7 327余2734。全部旧1614语义/六其他域/冻结定位/pending18/差队列/原任务与完整门禁保。本步确认的标记/指针及参数取样差异已修，未造新helper/原F或异常模拟。全接收父/provider/全部正常owner-alias文本分配释放域、绘制过期寿命/ABI/链接/画面/明文互通/运行R8继续；未构建/编译/测试/运行客户端或算法样例、未计算摘要。
