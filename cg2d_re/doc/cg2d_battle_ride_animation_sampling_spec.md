# 战斗动作227骑乘退场与动画复制取样SPEC（BRB01）

2026-10-06。依AGENTS.md第8条的工程委托先记SPEC再实施；只处理真实动作227。沿[BAR01](cg2d_battle_attachment_recovery_spec.md)已有两个helper以及[BRA01](cg2d_battle_ride_exit_sampling_spec.md)计数/剩余分列继续，不复制上一动作的分支顺序。

<a id="battle-ride-animation-sampling-baseline"></a>
## 原端基线与当前差异

已打开cg2d.exe会话MCP只读完整[415940,415FF2)，389条1714B、单chunk、80/80/80/80/69五页指令及原字节。唯一405583/type17普通call父404DB0，当前UpdateBattleActor_404DB0的case227已有实际调用；已有公开声明保持。

28call逐点可追：404D00标死14、53B498截断4、422370状态2、439380转态2、517DA0帧3、52C450音效2、422A30装备创建1。两个CPP helper展开原七标死和三标死/一次条件创建，不造独立原函数身份；四__ftol由既有F32到S32转换承接，SDK/ABI/PC53仍需实际门禁。463/457原额外Y压栈未被既有两参PlaySe_52C450消费，保留当前两参调用与音频暂停。

BAR01本根旧两窗100条429B已核，无额外比较前置；本次新增289条1285B，完整389条1714B不与旧窗重复累计。BRA01之后两父未核1216条4804B减本次新增，仅224余927条3519B；其它功能/所有动作与完整owner寿命不因此完成。

源差异是入口rider/计数与carrier取样序、恢复回调使用重新取槽、复制目标晚于读取源字段、drawMode整块赋值、位置同槽二次读取及计数/旧首槽借用未显式固定。指针仅非拥有T*；具体布局继续由runtime私有类型承载，不能用原偏移直接索引本机节点。

<a id="battle-ride-animation-sampling-decision"></a>
## 具名裁决与实施

仅修改src/YobiRuntime.cpp的UpdateBattleAction227_415940十二源片段：

1. 入口payload/index/position3/4/flags保持。先取entryRider、carrierAction，再读entryTime，之后才取carrierAction.payload；按entryTime==0进入原恢复阶段，不加空门禁。
2. second04/first00各一次取得实际角色，发布全局槽后用同角色写callback；后续阶段按原点独立重取全局槽，不贯穿缓存。
3. second的offset40/44分别先取当轴目标，再读action的F32并发布；已有action.offset截断→实际second.x/y路径和carrier.second04清空时点保持。
4. 动画复制中22标量分别先取当前second目标，再读源字段并发布：graphicIndex、framePlayingRate、lastImageNo、characterAction、lastCharacterAction、direction、lastDirection、frameIndex、frameRemain、graphicOffsetX/Y、frameEvent、hidden、field2D8、completedLoops、playRevert、frameX/Y，以及batchType/rebirthValue1/color2/rebirthValue2。每项重新取目标，不合并为一个固定动画对象；末三U8及color2的S32宽度保持。
5. drawMode原2F8..307的16B是四次W32读→同偏移写：0、4、8、12依次；仅当此段入口固定实际目标，按既有memcpy承载未对齐W32，不整块预取/赋值，不扩公开helper或改descriptor类型。
6. 两轴从action.offset40/44截断、各读原volatile U32屏幕偏移并按现有S32使用，再取当轴rider写delta；每轴另一次取rider供同对象delta读取与x/y写，X发布后再取Y。
7. 原两条正常S32轨迹保持：flags非零为time*time/5+second.frameY；零为(time-15)*(time-15)-225。movingRider只用于当次field58及y加法；不改成溢出/回绕模型或新浮点算法。
8. finish仍先按flags非零或当时rider.field58>15取样；然后另取counterRider，读field58+1并发布。**offset>=0仍在计数发布后且finish成功时判断**，这是原415DF3位置，与226的发布前取样不同，不抽共用模板或前移该判定。
9. 旧first一次读取供判空/MarkDeath，消费后按原点清全局槽。first的offset40/44、x/y、direction五项各先取得当项当前目标再读取源值发布；value54的F32 6.0/原40C00000位型和后续装备helper保持。
10. 原入口payload/carrier固定、所有跨调用当前slot/payload复读、状态/转态/帧/音效顺序、七清槽/三附件恢复、carrier.first00清空时点、rider/effect标死后清与最后共用帧尾保持。不给未核父或数据生产补业务/伪成功/不可达排除。
11. 不改其它CPP根、头文件/节点载荷布局、公开接口/owner域，不立即free/新增RNG。不加分配/空值/索引/溢出保护、补零/钳制或故障/回绕模拟；正常非地址W32/W8/F32字段不扩宽。

<a id="battle-ride-animation-sampling-acceptance"></a>
## 可度量验收

- 五页完整389条1714B/原字节逐页首尾连续、单chunk、唯一入向及28call分工一致；十二源片段与原指令子集对应，77条动画复制片段含22标量/四W32及尾段三U8/一W32真实序。
- 全CPP从HEAD十二片段精确重放；22标量独立目标、descriptor四读四写、入口/两恢复/位置/计数及五首角色属性取样可核；flags两轨迹/finish读与计数pub后offset判定、尾序/case227/两helper和原公开头保持。
- 在已有index1323/F:00015940及其adapted记录追加当前完整根依据，BAR01历史局部字段保持；不新增原身份/语义/disposition或双域成员。四局部I/S共用本根，E301→305，完整门禁不提升。
- 语义1324/有效ID1319、disposition426、YOBI25/BATTLE22、代码片段5/LOOP31/R7区间67/3061/队列175、文档关联1437/3680=39.05%和60任务/已闭10保持。旧100条429B/新增289条1285B分列，224余927条3519B未核，不将关联比称整体完成率。
- U08/U09/G01/G02/R01/R04/V04继续open，只增本根证据。所有动作实参/数据资源生产消费、owner重进退出/浅别名/借用失效、SDK/ABI/PC53/时钟RNG/画面运行仍需相应证据，完整原函数静态不等完整功能或运行通过。
- 仅必要集中静态、编码/引用/差异和获准文本汇总/-Check后精确提交；不构建/测试/客户端/算法样例/摘要、全面核对或改总SPEC/索引及压历史。原格式明文、音频暂停与RPLAN.24保持。

<a id="battle-ride-animation-sampling-source-state"></a>
## 当前源状态与剩余

动作227十二源片段按真实原读取/发布与四字组复制修复，case227/入口/两helper和已有公开声明保持；完整389条1714B为本根有限静态覆盖，不外推整个战斗/Yobi验收。224还余927条3519B未核，全部数据/资源和owner寿命及运行继续按总SPEC收口。命名快照`cg2d-re-battle-ride-animation-sampling-20261006`。
