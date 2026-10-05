# 战斗参与者初始化、追加与就绪恢复取样SPEC（BQR01）

2026-10-06。依AGENTS.md第8条工程委托先记SPEC再实施；沿[BEV01事件创建](cg2d_battle_event_production_sampling_spec.md)的4390E0参与者初始化和当前动作/目标列表、4394A0就绪/超时恢复链，不扩业务或添加新队列框架。

<a id="battle-queue-readiness-sampling-baseline"></a>
## 完整原端基线

当前已打开cg2d.exe会话MCP只读四个完整根，共211条751B、各单chunk；4394A0分80/36两页，其余各一页≤80条实际指令/原字节：

| 原根 | 范围 | 指令/字节 | 普通入向/父 |
| --- | --- | --- | --- |
| InitBattleCastList_4390E0 | [4390E0,4391DE) | 58/254 | 3/2 |
| AppendBattleEffectList_4393A0 | [4393A0,4393FF) | 24/95 | 77/4 |
| AppendBattleQueuedTarget_43B7F0 | [43B7F0,43B82B) | 13/59 | 1/1 |
| CheckBattleTurnReady_4394A0 | [4394A0,4395F7) | 116/343 | 20/2 |

合101普通call入向/7唯一父，不代七父本体验收；原8出向分列：初始化406260一次，就绪Reset4391E0一次、Append4392A0四次、Move439380两次；两追加叶无call。

四个实际W8读取：4390FA的payload.FDC入口test、439192的尾部FDC取样、4391C2的全局playerFlags test、4394DC的actorPayload.FE0 test。当前cast低U8使用W32读取，必须改原字节访问；全局声明为std::uint32_t，使用const unsigned char*合法对象字节观察，不改类型/volatile/公开布局。

就绪坐标原43950B/51F FILD S32点坐标、FCOMP actor.F32，再只看C3（AH bit40）：C3不置位才走恢复。点表索引一次、X后Y短路；当前!=不等判定需改普通有序位移条件(<或>)。只恢复该条件，不新增非有限检测、异常/FP状态模拟或故障门禁；PC53/ABI及实际浮点状态运行继续按总SPEC，不声明异常等价。

<a id="battle-queue-readiness-sampling-decision"></a>
## 五处差异裁决与原分工

仅改src/YobiRuntime.cpp的初始化/就绪两个根五片段；两个追加叶、既有ResetBattleCastEffects、所有父与头不改：

1. 三初始化W8点：入口直接读FDC低字节供bit2/phase13原短路门禁；尾部在FC4/首队列/13A4与13A8写后实际读一字节flags，再写六个原-1，之后按flags bit2/当时playerFlags低W8&3决定是否406260。各读取位置/先后保持，不沿用W32截断helper或缓存入口flags到尾部。
2. 就绪FE0 bit2按原当时actorPayload低W8读取；只在parentPhase非24、enable=0分支消费，aliveFF8 S32==0/actor.phase!=27等原门禁保持，不给parent/payload新增空保护。
3. 坐标沿同point行，先取得X点值再当前actor.offset40，普通有序<或>保存displaced；仅X未位移时再取Y点值/当前actor.offset44并同样判定。恢复原C3对应的有序位移和X→Y短路，保留已有S32/F32到double精确承载，无isnan/额外输入限制或特殊故障模型。
4. 初始化原入口action/payload固定，value54=1.0发布在入口对象、hidden=0写当时重新取的全局对象；ResetBattleCastEffects的十零写/position0和1先-1再200项position逐项-1保持，其他记录成员不清。FC4/CA4[0]/13A4/13A8=0及FC8/CA4[1]/targets[0/1]/skill/level=-1次序保持，不加全结构memset。
5. 两追加叶正确正文保持：4393A0依原五次CA0读取分别发布position、flags、position3、下一记录仅position=-1、最后计数递增；不写position4/其他记录字段。43B7F0依原三次13A8读取发布当前value、下一项-1、最后递增；各null直接返回。不把计数固定到入口快照、一次批量发布或添加200容量保护。
6. 就绪入口payload固定、每位置独立取得actor/当前mask和parentPhase/actorPayload；busyFD8严格等1才waitCount+1/pub后判>1200。超时扫描全部22槽，按当时槽/mask逐个Reset(index)→重新取槽Append11→再次取槽Append40→再取槽Move→waitCount=0，之后返回0；不只恢复首个busy对象，不缓存跨调用槽/整mask或把计数前移。
7. 非phase24坐标未归位同样两次Append11/40及Move后返回0；全部已遍历才能返回1。busy/null/跳过/enable规则和超时/归位分支返回保持，原正常S32计数/22槽边界不改。
8. 不立即free/新owner、RNG、公共helper或接口/布局；不追加越界/分配/空值/溢出保护、拒绝/补零/钳制、回绕/故障模拟。x64仅已证指针拓宽，W32/W8字段及原失败/返回保持，网络原格式明文和音频暂停不变。

<a id="battle-queue-readiness-sampling-acceptance"></a>
## 可度量验收

- 四完整211条751B/五页原实际字节、连续边界/单chunk、101普通入向7父/8出向一致；四W8原指令与坐标C3/X后Y原片段明确；引用全集不当七父验收。
- 全CPP从HEAD两个根五片段精确重放；四字节读取、坐标普通有序短路/同对象各轴取样、原两个追加叶/Reset helper和初始化/超时/归位各字段/调用/返回及四已有公开头保持。PC53/ABI/浮点运行不以源形式或正常条件代门禁。
- 新增四原F，语义1329→1333/有效ID1324→1328、disposition431→435；YOBI30→34/BATTLE27→31同四F不双计、代码片段5保持。E313→317仅共用本组五片段/原证据的双域四局部I/S，完整门禁不提升。
- 关联1442→1446/3680=39.29%，LOOP31/R7区间67/3061/队列175和60任务/已闭10及所有完整门禁保持，不以关联/五片段或正确叶数量推整体完成率。
- U08/U09/G01/G02/R01/R04/V04和R1/R7/R8/整体open；完整七父/所有其他动作实参/列表与数据资源生产消费、owner重进退出/浅别名/借用失效、SDK/ABI/PC53/时钟RNG/画面运行继续按总SPEC。
- 只必要集中静态、UTF-8无BOM/LF/引用/差异及获准文本汇总/-Check后精确提交；不构建/测试/客户端/算法样例/摘要、全面核对或改总SPEC/索引/压历史，RPLAN.24保持。

<a id="battle-queue-readiness-sampling-source-state"></a>
## 当前源状态与剩余

两个CPP根五片段按原W8读取和有序坐标条件修复；两个正确追加叶、Reset helper/所有父正文和四已有声明保持。四原完整211条751B纳管及101入向/7父只有限静态，不称完整战斗/所有数据/owner或运行验收。固定命名快照`cg2d-re-battle-queue-readiness-sampling-20261006`。
