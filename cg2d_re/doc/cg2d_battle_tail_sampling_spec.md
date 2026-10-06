# BTS01 战斗主回调公共尾与附件消费取样 SPEC

<a id="battle-tail-sampling-baseline"></a>
## 原基线与实际差异

沿[BCS01克隆/残影创建](cg2d_battle_copy_sampling_spec.md#battle-copy-sampling-source-state)推进[既有战斗回调](yobi_updates/battle_actor.md)的真实消费者404DB0；[原完整指令、257槽表与源差异](cg2d_battle_tail_sampling_evidence.json)分列保存。

404DB0完整[404DB0,40573C)为684条2444B、单chunk，13个type1回调地址发布/10个已知父，没有直接call入向。入口取scene、先比较7、再固定action.payload，scene7直接返回；非7读取完整S32 phase，原unsigned >256走公共尾。表[40573C,405B40)另为1028B/257个W32非拥有代码地址，全部指向200个已知动作入口；当前源257个case/200个调用目标逐槽已对应，没有分派缺口，不重造或变更表/动作本体。

原共用尾沿入口载荷：1004严格等1才递增1000；405652保存递增值到EAX、405654发布count、40565A用保存值与2比较，>=2置0；405669再读取当前1000与1比较，等1才调用41E5F0。800000标志清才投影：X截断→取S32屏幕X→读取Y的F32→正常S32 X相加/发布→Y截断→取S32屏幕Y/相加发布；不提前缓存两屏幕偏移或Y。

附件路径的真实次序：4056A8一次读取rider13C8并用同值判空/调用421FB0；调用后4056BB一次读取gear13B4并用同值调用4225D0；调用后4056CE重新读取rider并先判非空，4056D6读取firstAux13B8后才按该骑乘判定分流。无骑乘走aux0同步→再取aux1同步；有骑乘走aux0 MarkDeath→清0→再取aux1 MarkDeath→清1。第二槽必须在第一槽调用及可能清除之后读取，不跨调用缓存全部槽；不改附件类型13C0/13C4、不立即free或重绑owner。

四具名源差异：
1. scene比较写在固定payload之后，未表达原先判定后固定载荷。
2. 残影计数用++直接发布后再次读取判>=2，丢失原递增值寄存器快照；后续==1原确需重新读取，不能一并缓存。
3. Y截断和屏幕Y读取放在同一个+表达式，未表达原先截断再取屏幕Y的次序。
4. rider/gear/aux的判空与传参重复读槽，分流后才取firstAux，未表达调用前指针快照和原third-rider判定→firstAux读取→分流；原不同阶段重新读取rider必须保留。

<a id="battle-tail-sampling-decision"></a>
## 裁决与实施边界

依AGENTS.md第8条工程委托采用，仅src/YobiRuntime.cpp一根四片段：
- scene读取后先保存isSceneSeven，再固定payload，按同判定返回；不增scene/对象/载荷空保护。
- 先保存正常S32递增count，再发布1000，以count判>=2，保原>=2置0及后续直接重读1000==1；不把本步误套成BCS01残影“判定先于发布”，两原序不同。
- 保X/Y原取样/发布次序，仅X发布后显式保存Y截断的S32，再读取同源屏幕Y并相加发布。无钳制/回绕/故障模拟，不替换原正常算术。
- 首次rider、gear各保存原生非拥有YobiAction*供原判空与调用；之后独立重读rider并保存hasRider，再读取firstAux供两个原分支。secondAux分别在该分支第一槽消费结束后读取一次；原MarkDeath后清槽、无骑乘两同步索引0/1与全部其它字段/动作分派/头文件/载荷布局保持。不跨阶段复用rider，不遍历合并两aux，不新建owner、句柄、状态或公共工具。

此前257case/200动作已落位，原表逐槽与源静态对应不验收200个完整本体/参数、13绑定父或整个战斗。只纳管404DB0原身份与本组实修；不借函数数量或局部修正关闭功能。

<a id="battle-tail-sampling-acceptance"></a>
## 可度量验收

1. 原684条2444B完整/单chunk/连续末界、13地址绑定与10父分列；257槽/1028B单列，不计入函数代码字节。九页原指令及四页表记录可追，不把回调绑定计普通call。
2. 全CPP从HEAD仅精确重放四片段；原257case/200目标保持并逐槽对应、原入向角色与声明可追。场景判定/固定payload、count发布后用递增快照且==1重新取值、Y截断/偏移顺序、各指针单次读取与第二槽在第一槽消费后读取成立。
3. 原函数新语义1308→1309/有效ID1303→1304、非函数片段5保持；一adapted410→411。YOBI9→10/BATTLE6→7同F不双计；E281→285仅双域局部I/S共用同四实修/原证据，LOOP31/R7区间67/队列/完整门禁保持。
4. U08/U09/G01/G02/R01/R04/V04及全R1/R7/R8/整体保持open；全部动作、父/callback参数、战斗数据和资源生产、owner/浅别名/重进退出/借用失效、SDK/ABI/PC53/时钟RNG和画面运行仍需相应证据。
5. 仅必要集中静态、获准文本汇总/-Check/新引用/UTF-8无BOM/LF与精确提交；不构建/测试/客户端/算法样例/摘要，不全面复查、例行改总SPEC/索引或压历史。

<a id="battle-tail-sampling-source-state"></a>
## 源状态与剩余

既有真实UpdateBattleActor_404DB0与include/cg/YobiRuntime.hpp声明保持单一action/入口payload和共享节点槽；实际NewBattleActor_438290等13原绑定仍按各自父合同，本步不修改绑定或子动作。内存数值载体不属于网络cipher叶排除；原格式明文及音频暂停保持。

只本组消费取样与静态表对应通过，不证明本机逐指令/原异常重叠或并发等价，不放宽RPLAN.24。完整所有父/动作及资源owner寿命、SDK/ABI/PC53/时钟RNG/实际画面仍未闭。固定快照`cg2d-re-battle-tail-sampling-20261006`。

## BPO01 既有战斗分派表的原X用途关联（2026-10-07）

[第二批区间合同](cg2d_unassigned_range_classification_spec.md#range-second-batch-baseline)/[原区间与表证据](cg2d_unassigned_range_second_batch_evidence.json)将X:0000573C-00005B40完整1028B登记为257×W32跳转表，404DD5以EAX×4读表、CMP100/unsigned JA到405642，200不同标签均属404DB0，完整表字节等于本SPEC已保存的BTS原表。200是原父内标签，未新增200函数。

现有UpdateBattleActor_404DB0、BTS取样修正/257槽与源动作、完整684元数据/13发布/10父和公开声明保持。仅补R7该原X全部字节用途，BATTLE105不增/无新软件验收证据；完整战斗/所有子动作与父、间接alias/owner寿命/SDK/ABI和运行仍未闭。

## BPP01 动作114/141/143原表与真实字段消费（2026-10-07）

[第三批合同](cg2d_unassigned_range_classification_spec.md#range-third-batch-source-state)/[实际表及消费](cg2d_unassigned_range_third_batch_evidence.json)纳管40F0A0/410E90/410FC0三已存在源的四W32跳转表44槽和48索引表。三动作只是数据/源角色显式映射，原489/57/136仅元数据，完整动作未新验，函数处置null；BATTLE105→108枚举open。

另修40F0A0一消费片段：原+04非零启用/+00类型，源恢复同一S32[2]的[1]/[0]选择，保生产两值0/1序、pairPosition/门禁/else、1..6及原图号/工厂/owner。两BATTLE局部I/S只这六条30B字段消费，不代128X或三个完整动作/684主回调/全战斗/生命周期与运行；BTS既有源及表其它内容保持。

## BPQ01 两动作从数据角色到完整正常本体（2026-10-07）

[动作141/143合同](cg2d_battle_action_141_143_spec.md#battle-action-141-143-source-state)/[完整193条649B](cg2d_battle_action_141_143_evidence.json)沿BPP三表/48索引补两动作完整分支和六原调用。既有1429/1430全文前态保；图号/效果参数/default和公共尾一致，源仅各一旗位读取从W32恢复原低W8、正常旧判断值相同。原四图号物理W32/16B及唯一父参数可追，没增加容量/回退或重做正确创建依赖。

两equivalent只完整正常源静态，BATTLE108成员保持，E734两局部I/S；完整404DB0父/全部子依赖/状态生产/owner-alias寿命/SDK机器ABI/时钟RNG/画面与运行未验。BPP动作114字段修复及其489条未完整核仍单列，不外推完整战斗。

## BPR01 动作114完整正常本体（2026-10-07）

[动作114阶段合同](cg2d_battle_action_114_spec.md#battle-action-114-source-state)/[完整489条1745B](cg2d_battle_action_114_evidence.json)补BPP当时仅字段/数据角色的余下本体。34实际call/57内部jump/六槽switch、一唯一父参数弃返邻窗、54D4B4实际四字节可追；命中/名称与两轴fresh除10、业务0/maxHp及创建/图标/公共尾正常源对应，三个源片段恢复四原TEST W8点，正常旧判断值相同。

既有1428全文前态保，BPP启用[1]/类型[0]修复保持；一equivalent只完整正常源静态，BATTLE108枚举open、E736两局部I/S。完整父与全部子依赖/其它生产/owner寿命/SDK及生成机器ABI和x87实际精度/RNG/画面运行未验，完整战斗不关闭。

## BPS01 运动/缩放四动作与效果回收（2026-10-07）

[正常阶段合同](cg2d_battle_motion_scale_lifecycle_spec.md#battle-motion-scale-source-state)/[完整339条1170B](cg2d_battle_motion_scale_lifecycle_evidence.json)补113/154/155/156完整本体。原155/156判断与MarkDeath分别复用一次指针，源两片段改同阶段非拥有借用，保真实清槽/推进；113/154正确本体不改。原播放旗位是W32先读后测寄存器，原缩放中间/F32落位/两seed与RNG/五原声点及三个常数保，音频暂停。

四已有定义首次明确语义与正常equivalent，BATTLE108→112仍枚举open，E738两局部I/S；完整父/全部子依赖与其它生产/owner寿命、SDK和生成机器ABI/x87实际精度/RNG/画面运行未验，完整战斗不关闭。

## BPT01 资源变化两本体与复位体完整合同（2026-10-07）

[资源复制合同](cg2d_battle_resource_copy_spec.md#battle-resource-copy-source-state)/[三完整原根](cg2d_battle_resource_copy_evidence.json)补159/160/162完整540条1709B/九页。两资源体各六临时复制先读齐载荷再写，源码既有共享helper六片段恢复取样，两个W32/一个W8及各读/写序保；原目标token/reserved、内存CryptInt构造/析构与0/最大值业务上下限保持。159仍推进、160仍置phase37，default确实走公共尾，162正确复位入口全文保持。

三原已有定义首次明确正常语义/处置，BATTLE112→115枚举open、E740两局部I/S；完整父/全部子依赖与其它生产/owner寿命、SDK/生成机器ABI/时钟RNG/画面运行未验，完整战斗不关闭。

## BPU01 图像切换/淡入淡出与恢复两完整正常动作（2026-10-07）

[状态合同](cg2d_battle_rebirth_transition_spec.md#battle-rebirth-transition-source-state)/[完整570条2302B](cg2d_battle_rebirth_transition_evidence.json)补163/164进入/退出/恢复。源一位置类型消费恢复原直接低W8及已有INC/S8落位；四主效果与三回收循环各同阶段取指针供判断/MarkDeath，gear一循环对应两原槽，合七模式/八片段。保持第四持久创建的真实第三槽判定、309写后fresh槽310、所有state/default/负方向与业务alpha条件。

七原栈即时初始化组52值逐项对五源常量组40值，不把同名代码位置当全局数据表，R7不新增。BATTLE115→117枚举open、E742两局部I/S；全父/全部子依赖及其它生产/owner寿命、SDK/生成机器ABI/时钟RNG/画面运行未验，完整战斗继续。

## BPV01 计数/计时与四字节状态发布（2026-10-07）

[完整合同](cg2d_battle_count_timer_flags_spec.md#battle-count-timer-flags-source-state)/[六原根243条815B](cg2d_battle_count_timer_flags_evidence.json)补165..170。165移动及达到门槛后的两旗位恢复真实低W8，初次W32缓存/中段fresh W32与未落位角度/速度比较保持；166原S32内联timer扣1000*seconds/负值业务归零由既有共享helper承接，不造新call/F/owner。167..170原308/309 W8发布和公共推进正确，另五本体保持。

六完整正常语义纳管，BATTLE117→123枚举open、E744两局部I/S；全父/所有子依赖与其它生产/owner寿命、SDK/生成机器ABI和实际x87/时钟RNG/画面运行未验，完整战斗继续。
