# 战斗场景/事件工厂与动作推进取样SPEC（BEV01）

2026-10-06。依AGENTS.md第8条工程委托先记本SPEC再实施；沿真实InitializeBattleScene→NewBattleScene→DispatchBattleEvents→NewBattleEvent以及既有战斗动作的推进依赖，不新建事件框架或影子状态。

<a id="battle-event-production-sampling-baseline"></a>
## 原端完整基线与接线范围

当前已打开cg2d.exe会话经MCP只读五个完整根，每根≤80条指令，合110条362B，单chunk/真实指令及原字节和全部入向分列：

| 原根 | 范围 | 指令/字节 | 原普通call/父 |
| --- | --- | --- | --- |
| NewBattleEvent_43B610 | [43B610,43B6BB) | 53/171 | 1/1，43B581来自43B360 |
| NewBattleScene_4383B0 | [4383B0,4383EC) | 15/60 | 1/1，4383F1来自4383F0 |
| AdvanceBattleCastState_439340 | [439340,439378) | 16/56 | 304/180 |
| MoveBattleCastState_439380 | [439380,43939C) | 8/28 | 60/11 |
| AdvanceBattleEffect_439470 | [439470,43949F) | 18/47 | 73/43 |

五根共439普通入向、188唯一父；这是引用元数据，不是188父完整正文/场景流程验收。两个工厂合3个原出向：404AE0分配两点、4390E0参与者初始化一点；三个推进叶无call。

当前真实调用：InitializeBattleScene_4383F0发布NewBattleScene返回到gBattleSceneAction5ABA94并清32事件/22角色表；DispatchBattleEvents_43B360用kind/slot/participants/pendingGroup四实参创建事件，成功才发布active mask并继续找槽。该两父本体不改，不据当前源码当两父全部原端验证。两个工厂分别绑定43B360/427470已有完整源入口，不伪造回调或排除战斗功能。

原分配都是优先级1/载荷B4；当前BattleScenePayload只拓宽已证B0场地特效T*，其它S32/U32与20位置列表保持原宽，native sizeof分配/通用零化保持。当前五公开声明均存在，不暴露私有载荷布局。

<a id="battle-event-production-sampling-decision"></a>
## 三处差异裁决与保持项

仅修src/YobiRuntime.cpp三个CPP根各一片段，另两根正确正文保持；其它CPP/头和所有callsite不改：

1. NewBattleEvent先读取flags并OR2，再捕获返回节点的payload，之后发布同flags；匹配原43B628/62F/633/636，不把OR拖至payload捕获之后。原是W32读、AL OR2、W32发布，不改成独立低W8读写。
2. NewBattleScene先读flags、写callback，再OR2、发布imageNo=0、发布更新flags，之后才读payload/A8=FFFFFFFF；将OR显式放在image发布前。原失败空返回、节点身份/返回与原生非拥有指针保持。
3. AdvanceBattleCastState先用当前FC4+1保存有符号>=200判定，再发布FC4，按同判定写phase236或读取当前castList[next]，-1哨兵仍236。原null直接返回、正常S32+1、不钳制next到200，不增其它门禁。
4. MoveBattleCastState现有正确正文保持：null返回，入口payload→当前FC4→同payload列表值→phase；不推进索引、不新增200/-1处理。
5. AdvanceBattleEffect现有正确正文保持：null返回0；入口payload/C9C+1先发布，再按同next读取effects.position是否-1，返回真实1/0。不能与439340统一“先判断再发布”或新增200保护/索引规则。
6. NewBattleEvent其余原序保持：callback427470、image0/phase=kind、packetSubState58=0/eventSlot60=slot，先取MScanOffset，再entryMaskA8=participants、packetOffset64=offset；22个位置每轮重新读payload.entryMaskA8，按当轮位调用4390E0。所有调用后才重取payload.eventSlot60发布事件T*，然后waitCountAC=0/groupBase68=22*group，返回实际创建节点。不缓存整个mask或入口slot、不换bit遍历/slot门禁，不提前发布事件或造失败成功。
7. 原通用分配/zero-init、两个工厂失败分支、三个叶各自null规则/返回与字段宽度、回调绑定、别名和owner分工不改；不立即free、额外Read/构析/RNG、影子队列或公共布局/API。异常遵RPLAN.24，不加分配/空值/索引/溢出保护、拒绝/补零/钳制或回绕/故障模拟。

<a id="battle-event-production-sampling-acceptance"></a>
## 可度量验收

- 五原完整110条362B/单chunk、每根真实指令字节/边界及439普通入向188父、3出向一致；原引用全集不冒充所有父正文/运行。三源片段精确对应原flags/比较与发布位置。
- 全CPP从HEAD三个根三片段精确重放，另两个叶及Initialize/Dispatch当前调用本体、五已有声明/所有其它源保持；核五叶不同分配/null/返回/计数与哨兵，22动态mask读/初始化→slot重取pub→wait/group顺序。正常S32与B0原生T*保持。
- 语义1324→1329/有效ID1319→1324、disposition426→431为五新F，非函数代码片段5保持；YOBI25→30/BATTLE22→27同五F不双计。E309→313只共用本组的双域四局部I/S，完整门禁不提升。
- 文档关联1437→1442/3680=39.18%，LOOP31/R7区间67/3061/队列175与60任务/已闭10及其它完整门禁保持，不以关联比、元数据或五函数纳管代整体完成率。
- U08/U09/G01/G02/R01/R04/V04仍open。所有188父/其它动作/实参/数据资源生产消费及owner重进退出/浅别名/借用失效、SDK/ABI/PC53/时钟RNG/画面运行继续按总SPEC收口，R1/R7/R8/整体不关闭。
- 仅必要集中静态/引用/UTF-8无BOM/LF/差异及获准文本汇总/-Check后精确提交；不构建/测试/客户端/算法样例/摘要、全面核对或改总SPEC/索引及压历史。原格式明文、音频暂停及RPLAN.24保持。

<a id="battle-event-production-sampling-source-state"></a>
## 当前源状态与剩余

两工厂flags及439340计数判定三片段已按原取样/发布时点修复，439380/439470全文保持；五真实原身份/当前公开来源与两工厂父接线纳管。不提升全部188父、其它战斗动作/数据资源与所有owner寿命/运行验收，整体目标继续。固定命名快照`cg2d-re-battle-event-production-sampling-20261006`。
