# 路径追加与坐标复位取样 SPEC（BTT01）

日期：2026-10-07。沿BTS01真实441F60消费/442AB0复位链，工程裁决按AGENTS.md第8条既有委托先记后实施；[完整原指令/调用路径与前态](cg2d_actor_path_append_sampling_evidence.json)。

<a id="actor-path-append-baseline"></a>
## 原端基线

当前cg2d.exe完整441F60 23条90B、441FC0 114条327B、442AB0 66条281B，共203条698B/四归档页三块、12直接call/19jump/5RET、13入向七父局部路径。单点原NULL后U16 count>=10先W16清0再fallback；否则同已取count S16索引写X，再一次S16索引写Y后实际INC W16。插值原S16 count>0才取同末点两轴，另分支两个401EA0取得坐标；普通S32差/取负、距离2才按向零除2插中间点，再末目标；S16现计数取样、U32合计>10原门槛及先清0/fallback，正常逐点两轴各独立索引与W16增长。现这两正确正文完整保持，不造新限制。

复位442AB0四次401CC0临时构造，依次X1E0/Y1F0/X200/Y210。每次先完整读取临时mask W32、encoded W32、readCount W8，之后才按mask→encoded→readCount逐字段发布并401CF0销毁；现lambda每读一字段即发布，取样与发布交错有差异。原最后先取targetX为浮点、clear27C→clear280→F32 worldX→worldY乘64；已加载64.0数据复用BTS01，不重新猜常数。401CF0两原指令只写临时vptr再RET、无EAX写入，既有host内部承接保持，不拓为网络加解密。

返回不是统一业务类型：F60非满正常EAX action指针/NULL0、fallback尾透传encoded；FC0成功EAX循环点数、NULL未定义、fallback值；AB0非NULL最后临时encoded、NULL未定义。13局部路径包含重新覆盖/下一call或RET透传，不能称全部直接弃返或伪造纯指针结果。46AFE0那条父链经50A3E0到512E40清0有既有原窗；其它完整上游返回闭合仍需机器ABI验收，不外推该一链为七父全证明。三尾区6/9/7共22B逐BYTE90/NN_NOP无入向/后真入口，只对齐用途。

<a id="actor-path-append-decision"></a>
## 具名裁决及实现

只修改YobiRuntime既有442AB0 lambda一片段：构造后先const std::uint32_t mask/encoded、const std::uint8_t readCount取齐，再发布三值。四次构造/发布/销毁及完整后半复位、两追加全文、头/所有其它源保持，无新helper/owner/状态或布局，不改原正常/失败门槛与调用/RNG序。

三现有void接口明确保持局部寄存器观察适配，13直接路径逐项保留真实覆盖/透传，原混合值不升级成业务返回，不截W32模拟原生地址或填未定义NULL值；此裁决不冒充全上游返回/SDK机器ABI已验。适用原W16储存/计数、普通S32和原容量10门槛保，不专项加数学溢出/越界处理、拒绝/补零/钳制/回绕模型。网络原格式明文、内部LegacyCryptInt/音频暂停/RPLAN.24保持。

<a id="actor-path-append-acceptance"></a>
## 本步验收

1. 完整cpp仅该三字段取样/发布片段，三值W32/W32/W8均先取后pub；四原临时构造/销毁和所有其它正文/声明保，两个正确追加不造代码修改。
2. 三完整根203条698B/四页、12call/19jump/5RET/13七父局部路径、销毁叶两原指令与复用64.0/一条两级清0链、22B用途有据，不误称全部直接弃返或全七父闭合。
3. 新1661-1663/adapted/三个实际F处置、LOOP73→76/MAP70→73各两局部I-S真实来源，三任务current/引用保open，全部旧对象和完整门禁保。
4. 只必要静态与纯文本measure/-Check，追加实际终态；不编译、测试、运行或摘要，不以正确源码未改/具名void/完整原根静态代全部正常owner-alias寿命或全上游ABI/运行R8。

<a id="actor-path-append-source-state"></a>
## 完成边界

BTT01三完整路径追加/插值/复位根441F60/441FC0/442AB0共203条698B/四归档页三块、12直接call/19jump/5RET及13入向七父局部路径；只修YobiRuntime 442AB0既有lambda一片段：每次Construct后按原先预取全部W32 mask/W32 encoded/W8 readCount再按同序发布，四个坐标Construct/publish/Destroy及字段27C→280→worldX→worldY/乘64和RNG消费次数保。两正确追加全文保持、原U16>=10/S16末点及中间点/普通S32差值和除2/原U32>10门槛、两轴独立S16索引/W16计数更新与fallback先清计数保，不添边界限制/回绕模型。原F60正常action指针/NULL0或fallback编码值、FC0正常点数/NULL未定义或fallback值、AB0最后encoded/NULL未定义是局部寄存器观察，不伪造纯指针/统一业务整数；当前void观察具名保持，直接父有覆盖/下一call/RET透传，不能记全部直接弃返，完整上游机器ABI未验。Destroy原只写vptr/RET不写EAX两条原证据及既有host承接保。三NOP区22B仅用途，原64.0复用BTS01已加载数据；网络原格式明文/内部LegacyCryptInt/音频暂停/RPLAN.24保持，无新helper/owner/布局/保护。全部正常生产消费/owner-alias寿命、全父/机器ABI/运行R8未验。

预期语义1664/有效1657、文档1775（48.23%）、处置805/E916、LOOP76/MAP73、R7 373余2688；原pending18/pending7、差队列172/缺定位171/待归属43、closed11及完整叶0/globalfalse保。全父/全部正常生产消费及寿命、SDK-ABI与运行R8按原门禁继续。

## 本步静态终态（2026-10-07）

完整cpp具名根内一lambda片段、两正确追加全文及全部其它源头、三原根203条698B四页全部原MCP字段/13局部路径全字段、三临时值预取/Destroy两指令与22B用途及复用原数据、完整机器台账两域具名增量及五文档/三任务定向静态通过。检查器原将空入向集合聚合多计一项，逐区核实实际均0后改逐区检查并通过，未修改原证据或降低标准。纯文本measure/-Check终态0；完整派生快照仅三新语义/两域成员和候选/三X/三任务与六指标，十三文件UTF-8无BOM/LF、四局部I-S各25真实来源通过。

实际语义1664/有效1657、文档关联1775（48.23%）、处置805/E916、LOOP76/MAP73、R7 373余2688；完整叶验收0、整体未通过。未编译、测试、运行或摘要；全七父/全部正常生产消费与owner-alias寿命、SDK-ABI/链接/画面/明文互通及运行R8仍按原门禁推进，不把局部覆盖/RET透传和void裁决当全闭合。
