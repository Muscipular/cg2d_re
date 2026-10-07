# 跟随目标坐标取样 SPEC（BTS01）

日期：2026-10-07。承接BTR01的真实46AFE0下游，按AGENTS.md第8条工程委托先裁决后实施；[完整原指令、常数及前态](cg2d_actor_follow_target_sampling_evidence.json)。

<a id="actor-follow-target-baseline"></a>
## 原端基线

当前cg2d.exe完整[46AFE0,46B1A3)：128条451B/70+58两归档页一块、11直接调用点/4目标、19jump/1RET、唯一50A3E0@50A4EF入向。首槽T*一次取样NULL后以S16 path270>0退出；两次401EB0各单次读S32转浮点后乘原54DD30 F32常数64.0，再分别一次F32 worldX/worldY做原C3观察，X未到不读取Y。现Read_401EC0+Double乘64承接原已映射401EB0，不加F32中间落点或新浮点门槛。

原前一槽反向搜索活动W16与非NULL pointer。邻接X符号先取当前槽T*、前一槽T*后401E70；按符号决定再取current→previous两指针求幅值，负分支普通S32取负，>=2才继续移动而不读Y，否则Y同序两次差值，Y<2直接退出。四对指针每次独立取样，不把共享槽统一缓存到整个helper，也不依赖C++参数求值次序。

原9DAD78写3F800000后index<5才逐跟随槽处理；原活动W16非零才一次取action到EDI，NULL后同EDI读首X200，之后独立取槽T*读Y210；441F60仍传同EDI，而非另一次fresh槽。调用后先x=nextX再y=nextY，活动为零/NULL不推进坐标。当前NULL后的action和首X均再次读槽有差异，movement本来held正确，不改为fresh。

唯一父50A3E0调用后尾RET透传残值，唯一上游512E40清EAX0后RET，两窗复用BTR01/BTP01已取证据。原残值非统一有定义业务值，现void观察可承接；不模拟x86寄存器。尾[46B1A3,46B1B0)十三BYTE90/NN_NOP无入向/后真入口，仅对齐用途。54DD30四字节实际已加载00008042、完整flags值位及数据项/逐字节loaded核实64.0；不以仅get_flags的has_value结果作空数据或常数猜测。

<a id="actor-follow-target-decision"></a>
## 具名裁决及实现

两cpp四唯一片段/三既有正文：Scene46AFE0活动检查后只取一次action供NULL、首X和movement，调用BTR01已有四参数ReadMapActorTarget(action,slot,x,y)，Y按原fresh共享槽保；首对象IsMapActorIdleAtTarget在每轴读取/乘64后一次world F32取样供原比较；AreMapActorTargetsAdjacent四对current→previous指针各独立固定次序，先缓存符号条件再重新取两指针求幅值，保四次正常差值调用及X短路。

不改声明或添helper/重载/owner，不扩T*寿命到函数外；首first持有、S16/普通S32/阈2/乘64、反向活动槽搜索、原global发布和五槽边界及双轴推进序保持。原既有到格比较表达式保，不专项添加浮点异常或数学溢出处理/回绕/限长/拒绝/钳制；不是故障等价验收。父void观察仅依原两级清0路径具名批准，SDK机器ABI与完整正常寿命仍未验。网络原格式明文、内部LegacyCryptInt、音频暂停/RPLAN.24保持。

<a id="actor-follow-target-acceptance"></a>
## 本步验收

1. 完整Scene仅循环片段，完整Yobi仅两world单读片段与邻接helper段，头及其它源保持；新调用复用现有重载，movement仍同held action、Y fresh，不伪造反方向取样。
2. 128条451B/两页、11call/19jump/1RET、唯一父及两级返回窗、实际64.0数据与13B NN_NOP用途完整对照；不重复已取父根或把工具flag缺值当数据事实。
3. 新1660语义/adapted/实际F处置，LOOP72→73/MAP69→70，各两局部I-S真实来源；三任务仅current/引用保open，全部旧对象/门禁保。
4. 必要静态及纯文本measure/-Check，终态追加实值；不编译、测试、运行或摘要，不以本父完整原指令与局部生产消费代全部callee/正常owner-alias寿命或机器ABI/运行R8。

<a id="actor-follow-target-source-state"></a>
## 完成边界

BTS01完整46AFE0跟随目标传播128条451B/70+58两页一块、11直接调用点/4目标、19跳转/1RET及唯一50A3E0入向；两文件四片段/三既有正文实修：同一次循环action取样用于NULL/首X/441F60，Y独立fresh共享槽，复用BTR01已有重载无新helper/声明；首对象两world F32在对应CryptInt单次S32读取/Double乘64后各取一次，原S16路径/逐轴门禁保持；四对邻接指针按current→previous依原位置明确取样、先符号条件再重新取两T*求幅值，X距离>=2不读Y，普通S32差值/取负及阈2保。原first held对象、前一活动槽反向找、9DAD78 W32发布及index到第5槽的循环/原W16flags与双轴赋值序、内部LegacyCryptInt调用/RNG消费次数保持；父混合EAX残值透传50A3E0尾到512E40清0，具名void观察不模拟寄存器。原54DD30四加载字节00008042/F32 64.0实证、尾13B NN_NOP只用途。无owner/范围保护/故障回绕模型、网络原格式明文/音频暂停/RPLAN.24保；全部callee/正常owner-alias寿命/机器ABI及运行R8未验。

预期语义1661/有效1654、文档1772（48.15%）、处置802/E912、LOOP73/MAP70、R7 370余2691；原pending18/pending7、差队列172/缺定位171/待归属43、closed11及完整叶0/globalfalse保。集中静态及汇总后追加实际终态，全部适用整体验收继续。

## 本步静态终态（2026-10-07）

完整两cpp四片段、原根128条451B两页所有原MCP字段/11直接call/19jump/1RET及两级原返回窗、已加载64.0常数与13B NN_NOP用途、完整机器台账两域具名增量及五文档/三任务定向静态通过。纯文本measure/-Check终态0；完整派生快照仅一新语义/两域成员和候选/一X/三任务与六指标，十四文件UTF-8无BOM/LF、四局部I-S各25真实来源通过。

实际语义1661/有效1654、文档关联1772（48.15%）、处置802/E912、LOOP73/MAP70、R7 370余2691；完整叶验收0、整体未通过。不编译、测试、运行或摘要；全callee/全部正常生产消费及owner-alias寿命、SDK-ABI/链接/画面/明文互通及运行R8仍按原门禁推进。
