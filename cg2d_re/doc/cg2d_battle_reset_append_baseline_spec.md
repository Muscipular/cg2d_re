# 战斗重置与动作/效果追加基线SPEC（BQA01）

2026-10-06。沿[BQR01就绪恢复](cg2d_battle_queue_readiness_sampling_spec.md)实际Reset4391E0/Append4392A0，以及同源指定索引追加4392F0/完整效果追加439400。当前四根正文已符合原端合同，本轮依工程委托补齐缺失原身份/基线/分类和可追验收记录，源码/接口保持。

<a id="battle-reset-append-baseline-baseline"></a>
## 原端完整根与取证边界

当前已打开cg2d.exe会话MCP只读四完整根，合99条439B、各单chunk且各≤80条实际指令/原字节：

| 原根 | 范围 | 指令/字节 | 普通入向/父 |
| --- | --- | --- | --- |
| ResetBattleCastList_4391E0 | [4391E0,439295) | 41/181 | 6/3 |
| AppendBattleCast_4392A0 | [4392A0,4392E9) | 15/73 | 1114/13 |
| AppendBattleEffect_439400 | [439400,439470) | 28/112 | 85/21 |
| AppendBattleCastAt_4392F0 | [4392F0,439339) | 15/73 | 1/1，427EAA来自427470 |

共1206普通call、26唯一父；四根均无出向call。全部引用只记录地址/目标/归属，不验26父完整正文或源调用表达式数量。原1114入向一次响应被截断而拒绝解析，未保存为完整证据；已按≤80条引用分页完整取回，四根引用18页计量（6一页/1114十四页/85两页/1一页），分母/页尾/全集一致，不重开/改数据库或通过权限配置绕过工具。

当前实际CheckBattleTurnReady_4394A0的Reset→Append11→Append40→Move消费保持；同一CPP还保存其它真实调用者。四已有公开声明及两个正确追加/helper/所有父正文不改，不从原引用数推当前表达式数或全部父验收。

<a id="battle-reset-append-baseline-decision"></a>
## 现有实现合同与范围裁决

1. Reset4391E0按22槽参数取一次入口action，null返回；入口payload固定，value54=1.0在入口对象发布，再重取全局对象hidden=0。既有ResetBattleCastEffects按原十W32零写、position0和1先-1、再200条仅position=-1，重复前两项正常哨兵保持；不清记录其它字段。
2. 重置后FC8=-1/CA4[1]=-1/queued[0/1]=-1先发布，然后FC4=0/CA4[0]=0/13A4=0/13A8=0。与Init4390E0顺序及门禁不同，不加FDC/phase13门禁，不清busyFD8/技能或转向，也不调用406260。
3. Append4392A0只检查actor，null返回；固定actor.payload，先pub非拥有caster1070，再用当时FC8+1保存next、pubFC8、**用同next**写state。其后重新读当前FC8给下一槽写-1，最后busyFD8=1；caster可null，不加caster/payload保护或把sentinel索引缓存到next。
4. AppendAt4392F0同样只检actor，casterpub后next由显式index+1产生，pubFC8/同next写state，然后重读FC8写下一-1，busy=1。不能当成普通Append转发、不得改为用旧FC8或把实参index直接写state。
5. AppendEffect439400依原六次CA0读取分别写position/flags/position3/position4、下一记录仅position=-1、最后计数递增；没有call、不预取整组count，不清其它下一项字段。当前原生接口通过既有16B BattleEntryEffectRecord值类型承载四原W32语义参数（S32/U32/S32/S32），原为action加四标量；这是现有语义承载，不声称逐栈机器ABI相同，不改声明/任何调用者。
6. 两Append的caster1070只是非拥有T*；四接口只拓宽原地址字段，其余S32/U32与200容量、失败/null/发布/哨兵/返回分工保持。不新框架/owner、额外Read/构析/RNG、立即free、公共布局或原ABI函数桥。
7. 不因正常越界/溢出未专项处理而加容量/索引/分配/空值保护、拒绝/补零/钳制或回绕/故障模拟。保留原正常S32+1/原显式零写；网络原格式明文、音频暂停及RPLAN.24保持。

<a id="battle-reset-append-baseline-acceptance"></a>
## 可度量验收

- 四完整99条439B/单chunk/真实指令字节及1206普通入向26父全集与18引用页一致；四根无出向call。当前四源正文、共享helper/CheckReady/事件父及头文件从HEAD保持，原Reset字段/门禁差异、两next发布和复读、效果六计数读取/四成员语义合同有限静态可追。
- 正确正文不为增加开发数量而重写；此步的实际缺口是原新身份、数据/调用合同和R1分类，必须完成记录及派生看板，不计源码新增或运行验证。
- 新增四原F，语义1333→1337/有效ID1328→1332、disposition435→439；YOBI34→38/BATTLE31→35同四F不双计、代码片段5保持。E317→321仅基于已有实现/本次有限原对照的双域四局部I/S，完整门禁不提升。
- 关联1446→1450/3680=39.40%，LOOP31/R7区间67/3061/队列175与60任务/已闭10及所有完整门禁保持，不以引用全集/正确正文或关联百分比推整体完成率。
- U08/U09/G01/G02/R01/R04/V04与R1/R7/R8/整体open。完整26父/其它动作实参/列表及数据资源生产消费、owner重进退出/浅别名/借用失效、SDK/ABI/PC53/时钟RNG/画面运行仍需逐项证据；正常范围静态不宣异常或机器ABI等价。
- 仅必要集中静态/引用/UTF-8无BOM/LF/差异及获准文本汇总/-Check后精确提交十三记录文件；不构建/测试/客户端/算法样例/摘要、全面核对或改总SPEC/索引/压历史。

<a id="battle-reset-append-baseline-source-state"></a>
## 当前源状态与剩余

四根正确既有正文完整基线和原生T*/16B值类型语义映射纳管、源码与四声明保持；1206原入向和26父元数据不等所有父正文/接线/完整owner验收。本组后继续真正源码/数据生产消费缺口和完整生命周期，整体目标不缩小。固定命名快照`cg2d-re-battle-reset-append-baseline-20261006`。
