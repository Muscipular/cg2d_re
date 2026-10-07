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

## BPW01 附件透明度/背景状态与真实同步依赖（2026-10-07）

[完整合同](cg2d_battle_attachment_alpha_background_spec.md#battle-attachment-alpha-background-source-state)/[七原根736条2670B](cg2d_battle_attachment_alpha_background_evidence.json)纳管171、172/173共体、174、175、176、420F10和204。九非拥有槽每次操作后fresh/重复对象处理及真实mask/alpha W8/状态推进保；204两原零临时载荷mask/encoded W32/readCount W8先齐读再发布，源码两片段恢复，正常旧值一致。

204五源sync语句与四原call点是41259A共享汇合，额外调用初判已撤回，原主体/子槽同步均保持；14实际普通入向同属171/172共体/204三个真实父，172/173一原入口不重计。BATTLE123→130枚举open、E746两局部I/S，递归父/其它生产和依赖/owner寿命/SDK/运行及R8未完成。

## BPX01 转场/目标坐标与209取样（2026-10-07）

[完整合同](cg2d_battle_transition_effect_sampling_spec.md#battle-transition-effect-sampling-source-state)/[九完整原根757条2516B](cg2d_battle_transition_effect_sampling_evidence.json)纳管177/178/179/180..203共体、205/206/207/208及209。只有209六处低W8、两处FDC先FCC后读取与零临时encoded先目标查找/原交替发布九阶段恢复，正常旧值一致；八其它体及188无创建仍转204保，24phase共一根不重计。

九真实入向27邻窗/32父项、共体23立即type/六常数、source/target/attacker别名、所有业务/返回/RNG和未落位浮点中间保。BATTLE139枚举open/E748仅两局部I/S，完整递归生产/生命周期/生成机器/运行及R8未关闭。

## BPY01 场景分字/死亡状态与原读取宽度（2026-10-07）

[完整合同](cg2d_battle_scene_words_death_sampling_spec.md#battle-scene-words-death-sampling-source-state)/[七原根439条1445B](cg2d_battle_scene_words_death_sampling_evidence.json)纳管210..216。211四W8条件/四W16值、212一W8门控、214先flags/X后发布、216每轮索引/自身/目标位置顺序七源码阶段恢复；正常旧值一致，三其它体和现有同场景owner/原分字发布、回收及RNG/seed保持。

210真实四项switch与同依赖X:0001305D-00013070共19B用途可追：三LEA对齐/四W32项16B，不称纯NOP、不新增F或范围排除。BATTLE146枚举open/E750仅两局部I/S，递归生产/生命周期/生成机器/运行及R8继续。

## BPZ01 克隆/基础图还原与两个源帮助父（2026-10-07）

[完整合同](cg2d_battle_clone_restore_sampling_spec.md#battle-clone-restore-sampling-source-state)/[三完整397条1508B](cg2d_battle_clone_restore_sampling_evidence.json)纳管218/219/220及RestoreBattleBaseImage的两个实际使用者。218与共同Restore恢复1060指针先于图号的取样，219末尾FDC低W8；三源阶段只读取序/边界恢复，正常旧值一致，220包装正确保。原同步终帧循环/槽空检查、每操作fresh和条件回收/alpha业务下限与状态保。

三个原F，不造源帮助体原callee/F/owner；BATTLE149枚举open/E752仅两局部I/S，递归依赖/其它生产及寿命/生成机器/运行和R8未完成。

## BQA02 成对效果/距离状态与原HP/图标接收者（2026-10-07）

[完整合同](cg2d_battle_effect_pair_distance_sampling_spec.md#battle-effect-pair-distance-sampling-source-state)/[两完整346条1169B](cg2d_battle_effect_pair_distance_sampling_evidence.json)纳管221/222；只221四实际低W8消费恢复，其余W32 image/damage缓存及原零临时三齐读/写源对象HP保持。恢复图22454接target，扣减图22454接source，原不按直觉更换接收者。

222正确体和未落位角度比较/四原F32常数、两个fresh目标与普通业务/返回保持；BATTLE151枚举open/E754两局部I/S，递归依赖/生产和寿命/生成机器/运行及R8未完成。

## BQB01 宠物快照与骑乘读取合同（2026-10-07）

[完整合同](cg2d_battle_pet_rider_sampling_spec.md#battle-pet-rider-sampling-source-state)/[两完整289条1074B](cg2d_battle_pet_rider_sampling_evidence.json)纳管223/225；225九阶段恢复各当前对象先源字段、两实参deltaX先Y后offsetX、抛物线time/baseY先取后算。正常旧值一致，223正确体与四普通CopyPayload交替、真实重复清字段和坐标/image还原保持。

224已有BRC01完整adapted记录和正确体保，不重复纳管。BATTLE153枚举open/E756两局部I/S，完整递归生产/owner寿命/生成机器/运行及R8未完成。

## BQC01 计时、坐标与恢复扣减取样（2026-10-07）

[完整合同](cg2d_battle_timing_coordinate_damage_sampling_spec.md#battle-timing-coordinate-damage-sampling-source-state)/[五完整583条2104B](cg2d_battle_timing_coordinate_damage_sampling_evidence.json)纳管228..232；只232三数显currentIndex先Y、反伤零临时三齐读/写source、普通扣减encoded先共享目标且后两字段交替五阶段恢复。两种复制不能合并；普通上限CopyPayload交替正确保持。

228..231正确体及229真实250重复发布/228透传返回、230 flags完整S32坐标、231两原F32/独立if/previous返回保。232原W32缓存寄存器旗位、独立RNG/HP及图标接收者/共享尾/返回保持。BATTLE158枚举open/E758两局部I/S，完整生产/owner寿命/生成机器/运行与R8未完成。

## BQD01 转段、反弹与反伤取样（2026-10-07）

[完整合同](cg2d_battle_bounce_reflection_sampling_spec.md#battle-bounce-reflection-sampling-source-state)/[四完整834条2947B](cg2d_battle_bounce_reflection_sampling_evidence.json)纳管233/234/236/237。只233三/234三/237五共十一阶段：初始FDC/FCC先取、四实际内存W8、下边界角度两读、三数显index先Y、反伤encoded/mask/count齐读后写source及普通扣减index/encoded先position/共享目标。后者原序与232不同，按各原体落实。

236正确体/唯一父弃返边界保；S16width既有getter、普通CopyPayload/AssignFromInt helper不改。原W32寄存器旗位/正常业务阈值、HP图标/Attribute接收者/独立RNG/同步帧循环/调用与返回保。BATTLE162枚举open/E760两局部I/S，完整生产/owner寿命/图形链/生成机器/运行与R8未完成。

## BQE01 共用施法与弧射取样（2026-10-07）

[完整合同](cg2d_battle_shared_cast_arc_sampling_spec.md#battle-shared-cast-arc-sampling-source-state)/[七完整729条2516B](cg2d_battle_shared_cast_arc_sampling_evidence.json)纳管235、238/239共体、240..242、244/245，八逻辑状态只增七身份。原两表项同4055E1/417410，既有R7表分类保。

只共体五/241一共六阶段：归零index/encoded先position/共享目标、三W8源表达式对应四原点、速度零比较先phase239后、声事件先X。其它五正确体/普通helper、原W32缓存及HP图标/Attribute接收者/RNG/两播放/原循环/哨兵后额外推进/返回保。BATTLE169枚举open/E762两局部I/S，完整生产/owner寿命/生成机器/运行与R8未完成。

## BQF01 技能显示与移动参数取样（2026-10-07）

[完整合同](cg2d_battle_skill_display_motion_sampling_spec.md#battle-skill-display-motion-sampling-source-state)/[四完整633条2207B](cg2d_battle_skill_display_motion_sampling_evidence.json)纳管243/246/247/248；只243首次转向、属性共用两路径、时长条件三个W8源表达式对四原点，正常旧值一致。

原技能CMP链/图值/模式/级别、六RAND物理点各路径消费及W32缓存保持，不造跳转表/R7身份。243无扣HP，其它分支图22459后仍Attribute；246原F32系数0ad7233c及247/248正确移动/正常转换保持，原上限CopyPayload/两条件播放/退场/接收者/返回保。BATTLE173枚举open/E764两局部I/S，完整生产/owner寿命/生成机器/运行及R8未完成。

## BQG01 工厂、批次显示与退出取样（2026-10-07）

[完整合同](cg2d_battle_factory_batch_exit_sampling_spec.md#battle-factory-batch-exit-sampling-source-state)/[七完整704条2305B](cg2d_battle_factory_batch_exit_sampling_evidence.json)纳管249/40FC10/251/253/254/255/256；249一次event与255归零encoded先目标、三W8源表达式对四原点，共五片段/两体，正常旧值一致。

工厂30创建/31父case、118默认无创建仍phase162与250/252同父根；原CMP无物理switch表，不新增R7。251齐读临时、253W32 FDC/后取phase/保存+10、254先借指针回收、256两全局写正确体保。255恢复量确实来自healTarget.counter1DC，固定effect及普通免疫fresh C9C的差别、W32缓存/HP上限copy/RNG/批次播放尾保持。BATTLE180枚举open/E766两局部I/S；完整生产/owner寿命/生成机器/运行及R8未完成。

## BQH01 参战遍历、朝向与名字取样（2026-10-07）

[完整合同](cg2d_battle_participant_turn_name_sampling_spec.md#battle-participant-turn-name-sampling-source-state)/[六完整171条467B](cg2d_battle_participant_turn_name_sampling_evidence.json)纳管40CAC0/40CAF0/43C910/43C9A0/43CA50/43CA80；三个源体五片段：两FDC低W8、两独立玩家旗位低W8、两阵营共享投影四S32读序/F32 Y后X发布。正常旧值一致。

20槽span直接借22槽数组前20，先10..19后重读旗位再0..9，角度发布/运动/播放均按原重新取共享对象。名字测量前Y/后X、position>=20颜色继承、正常向零半宽与既有公开S16/S8边界、两推进正确体保持。八父八窗24条107B及局部返回比较可追，父switch仅邻近记录、不新R7/父完整验收；BATTLE186枚举open/E768两局部I/S，完整生产/owner寿命/生成机器/实际x87/运行及R8未完成。

## BQI01 命令目标读取宽度（2026-10-07）

[完整合同](cg2d_battle_command_target_width_sampling_spec.md#battle-command-target-width-sampling-source-state)/[完整266条901B](cg2d_battle_command_target_width_sampling_evidence.json)纳管43C560；四片段/六低W8表达式对应七原点，正常旧值一致。同阵营FDC低W8共用两个阵营原路径、400下玩家位低W8与两前排四个低W8条件恢复；原flags参数/目标全局W32、四源选中OR/两清除AND的W32写保持。

六HP比较物理点/四源表达式、调用后共享目标重取、十槽同侧排除/对侧选择、20项原栈矩阵/400短路/最后匹配/每列fresh别名与0/1返回保持。六UI父八入向/八窗40条141B可追，不新R7；BATTLE187枚举open/E770两局部I/S，全部目标生产/owner寿命/递归父/生成机器/运行及R8未完成。

## BQJ01 选择清理与命令入口借用（2026-10-07）

[完整合同](cg2d_battle_selection_cleanup_entry_sampling_spec.md#battle-selection-cleanup-entry-sampling-source-state)/[三完整145条467B](cg2d_battle_selection_cleanup_entry_sampling_evidence.json)纳管43C060/496F10/4972B0；两文件四借用阶段：Flush首T*判空/AND共用、Close三个T*各当阶段一次取样供判空/标死。正常旧值一致；保Flush后两fresh槽、各非空回收/清槽、九S32/三个W32关闭请求及原空分支。

callback原正确图值/禁用/hover旧字节/点击关闭清理选择链与返回保持。45代码入向45窗135条693B及1数据xref/实际32B prototype中56B72C callback字段/15B旧字节可追，32B原型与36B运行元素不混；现窗口26首元素/初始化发布/事件消费有据，不外推全部窗口/57C368 token/owner寿命。BATTLE190枚举open/E772两局部I/S，不新R7，完整运行及R8未完成。

## BQK01 快捷入口与防御状态取样（2026-10-07）

[完整合同](cg2d_battle_shortcut_defense_state_sampling_spec.md#battle-shortcut-defense-state-sampling-source-state)/[四完整197条767B](cg2d_battle_shortcut_defense_state_sampling_evidence.json)纳管497000/497100/497210/4973C0；只三个快捷状态读各低W8→S8，保字段W32存储/同值两次比较。正常旧值一致，原门槛/技能已用/模式2-3-4与各专属清零差别/真实创建结果、关闭清理回收声和返回保。

防御正确体/默认悬停选中图优先级、15B旧hover、原G含NUL2B及原格式明文发送顺序/弃返保持，不加加解密或发送失败早退。快捷九入向九窗27条119B、防御数据xref数组头及第二行56B730..56B750 callback字段56B74C有据，不混prototype/runtime，不新R7；BATTLE194枚举open/E774两局部I/S，完整UI/owner寿命/组包连接/运行及R8未完成。

## BQL01 按钮再操作点击取样（2026-10-07）

[完整合同](cg2d_battle_button_reopen_click_sampling_spec.md#battle-button-reopen-click-sampling-source-state)/[三完整359条1191B](cg2d_battle_button_reopen_click_sampling_evidence.json)纳管4975A0/497770/497900；只各已选分支全局鼠标掩码2低W8读，共享edgeButtons存储仍W32，pending/flag本已有W8保，正常旧值一致。

技能重选/重建/关闭返回、物品待选重开先存槽后清flag/Flush、宠物window28 W32关闭请求各原分支保持；不照搬再操作逻辑或宠物尾。原初次主mode先专属mode后、真实创建空结果/图先尾清/返回与三份43B旧hover保持。三数据xref实际索引2-3-4三条32B字段有据，不虚计代码call/重验已过通用链或新R7；BATTLE197枚举open/E776两局部I/S，完整UI/owner寿命/运行及R8未完成。

## BQM01 技能窗口与列表取样（2026-10-07）

[完整合同](cg2d_battle_skill_window_click_sampling_spec.md#battle-skill-window-click-sampling-source-state)/[八原根](cg2d_battle_skill_window_click_sampling_evidence.json)纳管技能列表创建/技术创建/返回、列表重建、窗口动画包装/创建/更新与点击八完整573条1739B。仅47C570两真实W8索引及四低W16坐标读顺序三片段/一体实修，共享索引W16/坐标和flags W32保，正常旧值一致。其它七正确体、真实失败/null/六RNG/末尾gate/回调和MarkDeath后取非拥有T**槽保持。37B七项返回表/九NOP用途mapped，R7 261/3061余2800。BATTLE205仍open，完整生产/父/owner寿命/窗口10-26及运行未验。

## BQN01 技术选择与命令消费（2026-10-07）

[完整合同](cg2d_battle_technique_selection_sampling_spec.md#battle-technique-selection-sampling-source-state)/[三根313条1108B](cg2d_battle_technique_selection_sampling_evidence.json)纳管47A000/47C700和989既有4974C0完整追加。两体四片段恢复selectedSkill条件W32重读/cost两W32取序/hover形参实际W8和四坐标W16序，正常旧值一致。NUL/W16配对/共享W32、三FP比较/双S32借用回调/null仍关闭消费和989原格式明文/真实发送后写序保；原三提示83B与S格式11B保。208成员枚举open/两局部I-S，不新R7；完整内容生产/其它模式/全部父/owner寿命与运行未验。

## BQO01 技能窗口收尾与初始化取样（2026-10-07）

[完整合同](cg2d_battle_skill_window_lifecycle_sampling_spec.md#battle-skill-window-lifecycle-sampling-source-state)/[四完整260条837B](cg2d_battle_skill_window_lifecycle_sampling_evidence.json)纳管496AE0/47A620/47B110/4742C0。三体九片段恢复window10先userState、三槽分阶段一次T*、返回按钮hover缓存/两W8及助手两height W32序；正常旧值一致，初始化正确体/原关闭请求/回退/正常S32与别名/旧42B提示保。真实48B原型callback54E444及index4/32B callback561C14有据，不将原型或数组头当函数指针。42B X七表项与对齐用途mapped，R7 262/3061余2799；212成员仍open/两局部I-S，全部父/其它子依赖/生产与owner寿命、运行未验。

## BQP01 技能列表与制造回退取样（2026-10-07）

[完整合同](cg2d_battle_skill_lists_rollback_sampling_spec.md#battle-skill-lists-rollback-sampling-source-state)/[七完整393条1373B](cg2d_battle_skill_lists_rollback_sampling_evidence.json)纳管普通列表/技术、计数回退/字节减计数、两宠物模式与原401F80。四体六片段恢复held position/counter减后held谓词与旧cell序、loop尾界先index++/骑宠名称目标取序；一typed cost-first桥/两父接原单右FP读。普通值/FP单读次数和计数RNG同旧源，401EA0实际尾跳401EC0/坐标取值正确不改，不称不同解码。三正确体/原失败/負maximum/模式callback/原MIS明文及尾转移保；四无IDA父代码区间CALL只登记不造F/排除。219成员open/两I-S不代生产/其它模式callback/完整父/owner寿命与运行，无新R7。

## BQQ01 窗口关闭代码区间取样（2026-10-07）

[完整合同](cg2d_battle_window_release_range_sampling_spec.md#battle-window-release-range-sampling-source-state)/[两完整42条145B](cg2d_battle_window_release_range_sampling_evidence.json)沿上批四个真实CALL，纳管窗口10/12关闭原代码区间；一制造体两片段恢复旧W8→借window12 T*→减后发布及D67AFE减后先发布/同held谓词。普通旧值一致，窗口10正确体/初后mode取样差别、原W8/W32储存和S32动作条件保。两48B原型release真实字段与471276局部弃返核清，void适配正确保。两X整236B分项mapped/两个有效code_identity，F分母及219成员保持；完整制造/技能生产、其它父/owner寿命和运行另验。

## BQR02 技能计数/选择与动作状态生产（2026-10-07）

[完整合同](cg2d_battle_skill_state_producer_sampling_spec.md#battle-skill-state-producer-sampling-source-state)/[十一完整112条361B](cg2d_battle_skill_state_producer_sampling_evidence.json)沿关闭/回退依赖纳管五清增计数、技能回复、mode5/6 selector、动作状态读/发布与真实范围常量。两动作体三片段恢复FDA88E两个W8读及发送后W8 OR2/动作0 fresh W16 ANDFFFD，2B memcpy保持别名/共享W32，普通值一致。九正确体、原低S8返回/短路/Y-X单Read/计数RNG、held值/fresh socket、原fS与klF明文及发送后状态发布保持。71入向/25父、CC4D14两写四读只登记；230成员open，完整数据/consumer/owner寿命和运行另验，无新R7。
