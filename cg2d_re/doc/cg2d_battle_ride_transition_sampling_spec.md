# 战斗动作224骑乘切换、运动与恢复取样SPEC（BRC01）

2026-10-06。依AGENTS.md第8条工程委托先记本SPEC再实施；沿[BAR01](cg2d_battle_attachment_recovery_spec.md)两个helper与[BRA01](cg2d_battle_ride_exit_sampling_spec.md)/[BRB01](cg2d_battle_ride_animation_sampling_spec.md)继续，只处理真实动作224，不把三动作分支抽成同一模板。

<a id="battle-ride-transition-sampling-baseline"></a>
## 原端完整基线与本次增量

当前已打开cg2d.exe会话经MCP只读：[414050,4150EB)单chunk、1136条4251B，十四页各80条及末16条，共十五页真实指令与原字节；唯一405562/type17普通call父404DB0。UpdateBattleActor_404DB0已有case224实际消费，既有公开声明保持。

90个原call分列：51C0D0图号映射1、4221F0骑乘创建1、404D00标死35、406260状态8、517DA0帧10、4062A0距离5、4060D0朝向4、52C450音效4、405EA0移动4、53B498截断6、422A30装备创建2、422370状态3、4238F0相册1、439470推进效果2、439340推进动作2、439380转态2。原16组CryptInt为内联encoded W32读写→mask W32读写→readCount W8读写，不造16个原call；既有CopyPayload完整三写序相符，不碰token/保留字节或添加Read/构析/RNG。六__ftol沿已有转换承接，原strcpy/SDK/ABI/PC53仍待相应门禁。

BAR01本根四窗201条700B及零比较前置8条32B，已观察209条732B；本次新增927条3519B，不与旧片段重复累计。至此224/226/227三父全部原指令已有有限静态覆盖，本组父正文未核差集为0；完整所有动作参数/生产与资源/owner及运行不因此验收。

原414A12被IDA显示为offset符号的test实际字节F7C100000001、立即数01000000，保持现有四album位门禁，不将注释或显示类型当新地址。

<a id="battle-ride-transition-sampling-decision"></a>
## 具体差异与具名实施裁决

仅修改src/YobiRuntime.cpp的UpdateBattleAction224_414050，三十一具名源片段；其它根、两helper、CopyPayload和公开头/节点布局保持。

1. flags非零初建：name、field1D0、四CryptInt及x/y/offset40/44各先取当前first来源，再取该阶段rider，再消费来源值写该rider；name与数值/四载体/四坐标之间全部原重新读取保持。不以函数实参求值顺序代替来源/目标取样，不缓存整个first/rider阶段。
2. 初建先取action.frameX，再取effect.offset44/40，之后发布effect.deltaX并距离判定；另朝向先Y再X。初建rider朝向保留deltaX→Y→rider→offset40读取/加法/F32参数发布顺序。
3. flags非零移动：action距离先Y再X；rider距离先Y、deltaX、当时rider，再offset40加法。近距写rider.offset40先deltaX再取X目标/offset40，写offset44另取Y目标再读effect.offset44。原两float计算/失败分支和运动调用不改。
4. 两路径位置x/y各一次取得当轴rider供同一对象delta读取和坐标发布；X后再取Y，delta轴已有同对象借用保持。两轨迹先取得同movingRider的field58与当前y，再算(time-15)^2-225并以原y基值加offset发布，不将y读取拖至轨迹计算后。
5. 非零路径先保存oldTime>15，再pub递增；action.frameY<=offset仍在计数pub后，原短路保持。零路径先保存oldTime>15且offset>=0的判定，再pub递增、按同判定恢复；不能沿227统一把offset判定移到pub后。
6. 非零恢复中gear/aux0/aux1及转移second/目标position各一次取指针供判空/MarkDeath，消费后原槽清空与下一阶段读取保持。该段payload固定，不改成全局当前payload复读；与已有RestoreBattleRideGear的跨调用复读分工不同。
7. 转移先发布真实first到position，再计算原priority低U8值，清first全局槽，再pubpriority；不加低宽回绕/溢出模拟，保持现有正常S32表达式及U8转换。gBattleCurrentPosition只读一次供first/second两比较，匹配原共享快照。
8. 零flags路径四个点表参数场景（action/rider距离、朝向）各仅取一次S32 pointIndex供同一行Y后X；rider参数段在X源值读取后才捕获当次rider，保留现有F32参数转换和原函数签名。不把原一索引扩成X/Y独立重取；碰撞归位两轴仍按原各独立索引→rider→坐标值取样。
9. 零路径初段方向先取rider，再读action.direction发布。预推进循环每轮field58只取一次，固定该轮warmTime供<15、轨迹与递增；原t*t之后取action.frameY再减225比较，成功才pubwarmTime+1，之后重新取rider/field58进入下轮。原break/上限和普通S32轨迹保持，不新增循环保护或故障模拟。
10. 零路径first/second旧槽各取一次供原判空/MarkDeath；恢复时carrier角色各一次取到实际T*、pub槽后用同角色写callback，后续原当前slot复读保持。carrier.second04/first00清空时点、装备恢复、状态/转态及返回分工保持。
11. 保留入口payload、flags分流/各场景固定effect与carrier、初建工厂原失败无额外门禁、全部名字/载体和图号/字段宽度、S32轨迹及F32/X87现有承载、八独立点表坐标发布/四album独立门禁、三清理/一次恢复helper消费、全部mark/清槽/音效/帧与早退。原音效多余Y压栈未被两参PlaySe消费，不扩API或恢复音频。
12. 不立即free/引新owner、RNG、公共helper或不可达排除；不加空值/分配/索引/溢出保护、补零/钳制、回绕/故障模拟。指针用已证原生非拥有T*，非地址字段宽度不扩。

<a id="battle-ride-transition-sampling-acceptance"></a>
## 可度量验收

- 十五页原1136条4251B连续首尾/单chunk/实际原字节、唯一入向与90call分工一致；三十一范围各不超过80条，与完整根真子集对应，原16载体三读写序/一立即数及旧209条732B与新增927条3519B可追。
- 全CPP从HEAD三十一片段精确重放；逐字段来源/目标、五距离/四朝向参数、四点表同索引取样、两位置对、两个不同计数门禁、五清槽借用、priority/当前position、循环每轮一次field58、两恢复callback/原跨调用复读与全部早退/尾序核实；case224/两个helper/CopyPayload和已有头保持。
- 仅在已有index1321/F:00014050及adapted追加当前完整根依据，BAR01历史局部字段保持；不增身份/语义/disposition或双域成员。E305→309为共用同根的四局部I/S，完整门禁不提升。
- 语义1324/有效ID1319、disposition426、YOBI25/BATTLE22、代码片段5/LOOP31/R7区间67/3061/队列175、关联1437/3680=39.05%和60任务/已闭10保持，不以差集归零或关联比推整体完成率。
- 三退场父本体局部原静态差集归零；U08/U09/G01/G02/R01/R04/V04继续open。所有其它动作/实参/数据资源生产消费及owner重进退出/浅别名/借用失效、SDK/ABI/PC53/时钟RNG/画面运行仍需逐项证据，不能把三根完整静态当完整功能或运行通过。
- 只必要集中静态、引用/编码/差异及获准文本汇总/-Check后精确提交；不构建/测试/客户端/算法样例/摘要、全面核对或改总SPEC/索引/压历史。原格式明文、音频暂停及RPLAN.24保持。

<a id="battle-ride-transition-sampling-source-state"></a>
## 当前源状态及剩余

三十一源片段按原实际取样、同一载体/当轴对象及同点表行、两不同计数阶段与原早退/恢复时点修复；完整1136条4251B为本根有限原静态覆盖。三父本体差集归零后继续完整战斗数据生产消费、其它动作和所有owner寿命及运行门禁，整体目标不缩小。固定命名快照`cg2d-re-battle-ride-transition-sampling-20261006`。
