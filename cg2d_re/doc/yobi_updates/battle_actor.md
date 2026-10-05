# Yobi Update：战斗角色

[分类总表与口径](../cg2d_yobi_update_coverage.md) · [唯一执行进度](../../plan/cg2d_full_reconstruction_progress.md)

## 登记边界

本表登记当前源码中的 1 个不同原地址回调。各行本体存在、所列直接或参数绑定已在源码出现；仅静态登记，不重验历史语义，不表示全部原端绑定已枚举、整链已接或运行通过。源码行号固定于本次登记快照，地址/符号为稳定身份；后续变动以符号重新定位。

[源码](../../src/YobiRuntime.cpp)；“绑定点”中的行号指赋值行，参数传入另写初始化路径。最终通用回收沿总表的公共生命周期，表内列局部退出责任。

## 回调明细

| 原地址/本体 | 创建与绑定点 | 载荷与生产者 | 阶段/依赖 | 退出、剩余与证据 |
|---|---|---|---|---|
| `404DB0` / `UpdateBattleActor_404DB0` | `NewBattleSpiracleShot_418B70`:8981<br>`NewBattleBlastWave_419180`:9036<br>`NewBattleRangeHit_419390`:9096<br>`NewBattleArcShot_419DA0`:9158<br>`NewBattleCastCopy_4205F0`:9208<br>`NewBattleBoomerang_4195F0`:9242<br>`UpdateBattleAction224_414050`:20059<br>`UpdateBattleAction224_414050`:20072<br>`UpdateBattleAction226_4154C0`:20107<br>`UpdateBattleAction226_4154C0`:20142<br>`UpdateBattleAction227_415940`:20181<br>`UpdateBattleAction227_415940`:20242<br>`NewBattleActor_438290`:原4382BC | BattleActorPayload；六种效果创建、438290、4239D0 RID及438550 C生产已有；43CBB0/427470父调用已有，完整载荷/清理仍待 | 257动作槽→200目标；公共尾残影及三类附件同步 | 224/226/227恢复绑定已接；C缺席清理已补，场景完整清理仍待；[总分派](../ui_windows/shared_battle.md#battle-dispatch-closure)、[创建/RID](../ui_windows/shared_battle.md#battle-actor-create-rid) |

## 生命周期任务

`427470` 的[RID主消息内部段](../ui_windows/shared_battle.md#battle-m-rid-message)已补三分支对象生产；与4239D0的坐标、缩放和消费顺序分开保留。模式0的两名新角色在原429588/429920清空回调，通过载体和动作224交付，不提前发布角色表；这两处仅计源码清空点，内部段已接427470总入口，不新增唯一回调或直接绑定边。

`438290` 基础创建已绑定完整404DB0，`4239D0` RID已完成双对象属性、载体及动作/效果追加；两个对象先清回调，交已实现226恢复发布，不提前写入角色表。详见[基础创建与RID](../ui_windows/shared_battle.md#battle-actor-create-rid)。[438550 C解析](../ui_windows/shared_battle.md#battle-c-role-production)已补属性、附件与缺席对象清理；`43CBB0` C父入口与 `427470` RID caller已接；完整 `BattleActorPayload` 生产及其他清理仍须沿实际调用闭合，尤其424620攻击链不能由本表替代。六种效果创建、基础角色创建与三种恢复不是原端全量创建集合。`404DB0` 的257动作槽只形成200个子分派目标，既不计257个Update回调，也不表示角色上游或整场战斗已完成。公共尾的三个附件同步属于直接调用边，见[附件类](attachments.md)。

## BTS01 公共尾取样修正与当前基线（2026-10-06）

[本轮具名SPEC](../cg2d_battle_tail_sampling_spec.md#battle-tail-sampling-baseline)及[完整原指令/表](../cg2d_battle_tail_sampling_evidence.json)保存404DB0完整684条2444B/单chunk、13个type1绑定与10父，257槽1028B另列；源全部257case/200调用目标对应保持，本轮不重写各动作本体。

四片段已修：先判scene7再固定payload；count递增值保存后发布，>=2用该快照而==1仍重新读取；X发布后Y截断再取屏幕Y；首次rider/gear单次读判空及传参，第三阶段独立判骑乘后先取firstAux再分流，secondAux在first消费完成后读取。原非拥有指针、MarkDeath后清槽与无骑乘索引0/1同步保持。局部静态不证明全部父/200动作参数、资源生产/owner浅别名寿命或SDK/ABI/PC53/时钟RNG/实际画面；沿[验收边界](../cg2d_battle_tail_sampling_spec.md#battle-tail-sampling-acceptance)继续推进。

## BAS01 三附件同步的当前取样基线（2026-10-06）

[先落SPEC与委托裁决](../cg2d_battle_attachment_sampling_spec.md#battle-attachment-sampling-decision)修三附件十三片段，按[原完整421条1507B](../cg2d_battle_attachment_sampling_evidence.json)及四表44槽176B可追。五低W8门禁只读取已有W32字段首字节；位置/方向/身份比较及payload、三阶段flags保存发布、各轴frame/scale和辅助类型3/4/5单次动作及截断后delta复读恢复，所有其它映射/字段/播放保持。

主回调已有1/1/2真实消费不改；主装备X发布后再取rider、骑乘两轴parent.scaleX及自身scale不写保持；辅助原action/parent/payload三空返回和direction先于表身份检查保持，另外两根不添新guard。原非拥有指针和MarkDeath不代释放；完整附件生产/各动作与父参数、资源owner浅别名寿命/SDK/ABI/PC53/时钟RNG/实际画面另验，沿[验收边界](../cg2d_battle_attachment_sampling_spec.md#battle-attachment-sampling-acceptance)继续推进。

## BAP01 附件生产/发布与清理的当前基线（2026-10-06）

[本轮SPEC](../cg2d_battle_attachment_producer_spec.md#battle-attachment-producer-baseline)与[完整原指令/表](../cg2d_battle_attachment_producer_evidence.json)纳管三工厂、四发布和已有公开声明的共用421F90；原442条1393B/24普通call与3地址发布15父、6槽24B分列。

七根九片段恢复BC0独立原正文/直接factory，C20 index合同与各发布/清理指针快照/复读、辅助type3先判后pub/三F32与scaleY原复读、骑乘OR与低W16捕获及callback位置→注册项→parent顺序。正确主装备工厂与BAS三同步/BTS主回调保持，不新guard/owner或立即free；原两caller的index0/1只计数/传参/循环尾有界对应。完整全部父/动作参数、资源生产及owner浅别名寿命/SDK/ABI/PC53/时钟RNG/画面运行沿[验收边界](../cg2d_battle_attachment_producer_spec.md#battle-attachment-producer-acceptance)另验。

## BAR01 附件清理与恢复的当前取样基线（2026-10-06）

[本次SPEC](../cg2d_battle_attachment_recovery_spec.md#battle-attachment-recovery-baseline)及[原完整状态/父八片段](../cg2d_battle_attachment_recovery_evidence.json)分列177条608B和401条1549B、比较前置16条66B，明确三父余1385条5495B未核；父完整元数据只识别边界/身份。

三个CPP根十一片段已修，七槽次序105C/1054/1068/1058/13B4/13B8/13BC各一次读取和消费后清、恢复三指针快照与所有原global/当前payload复读、先判400后scale两轴pub保持原失败/字段/调用顺序。源三父本体未改且原五清理/三恢复消费保留；不清rider/effect1060/类型槽或立即free。全部父/动作/资源生产及owner重进退出和浅别名/SDK/ABI/PC53/时钟RNG/实际画面沿[验收边界](../cg2d_battle_attachment_recovery_spec.md#battle-attachment-recovery-acceptance)继续。

## BRA01 动作226的当前完整根依据（2026-10-06）

[原完整根/十二指令对照](../cg2d_battle_ride_exit_sampling_evidence.json)补4154C0的277条1145B与28调用，唯一405578普通call来自404DB0；当前case226已有实际消费，旧两窗/前置108条454B与本次新增169条691B分列。

[实施](../cg2d_battle_ride_exit_sampling_spec.md#battle-ride-exit-sampling-decision)固定入口rider与各阶段同对象借用、恢复发布和四offset目标先取、方向/volatile两轴及位置取样，counter发布前保存退场判定，旧首槽同指针判空标死。原入口payload/carrier、helper/跨调用复读、S32与退出尾序不改，公开声明保持。224/227余1216条4804B和全部数据资源/owner寿命及运行沿[验收](../cg2d_battle_ride_exit_sampling_spec.md#battle-ride-exit-sampling-acceptance)继续。

## BRB01 动作227的当前完整根依据（2026-10-06）

[完整原指令/字节和十二对照](../cg2d_battle_ride_animation_sampling_evidence.json)补415940的389条1714B、28call与唯一405583普通call父404DB0，当前case227真实消费保持。原22动画标量（含末三U8/一S32）各重新取目标、16B descriptor四W32依次读写，与五first属性目标序已实修。

[十二源片段裁决](../cg2d_battle_ride_animation_sampling_spec.md#battle-ride-animation-sampling-decision)保留入口payload/carrier与跨调用复读、flags两条正常S32轨迹、finish前取样/计数pub后offset门禁及标死清槽/帧尾，两helper/公开头不改。旧100条429B与新增289条1285B分列，仅224余927条3519B和全部数据资源/owner寿命及运行沿[验收](../cg2d_battle_ride_animation_sampling_spec.md#battle-ride-animation-sampling-acceptance)继续。

## BRC01 动作224当前完整根依据（2026-10-06）

[原1136条4251B/十五页及31范围](../cg2d_battle_ride_transition_sampling_evidence.json)、90call/唯一405562普通call父404DB0与真实case224可追。31源片段修来源/目标、同对象坐标/运动基Y和同点表参数、两个计数阶段、预推进每轮field58一次、槽消费及恢复回调；原16载体encoded/mask/count序、三清理/一次恢复helper、全部其它字段/失败/早退保持。

[本次验收](../cg2d_battle_ride_transition_sampling_spec.md#battle-ride-transition-sampling-acceptance)分列旧209条732B/新增927条3519B，已有F/index1321仅追加依据。三父本体有限原静态差集0后仍需所有其它动作/实参/数据资源生产消费及owner/SDK/ABI/PC53/时钟RNG/画面证据，不升完整门禁或引guard/free/RNG/回绕模拟。

## BEV01 事件创建与队列推进当前基线（2026-10-06）

[五完整110条362B/指令原字节及全部入向](../cg2d_battle_event_production_sampling_evidence.json)纳管4383B0/43B610两个工厂和439340/439380/439470三个推进根。439340先判next>=200再pubFC4、-1写236；439380只按当前索引取值pubphase；439470先pubC9C+1再判position==-1返回1/0，各null规则保持，不追加200/-1统一处理。

[本次三处修复](../cg2d_battle_event_production_sampling_spec.md#battle-event-production-sampling-decision)还落实两工厂flags取样/发布时点；原B4仅B0已证T*拓宽、22动态mask/调用后slot发布/wait-group及实际失败/返回不改。439call188父是引用元数据，完整其它动作/数据资源、共享owner和运行沿[验收](../cg2d_battle_event_production_sampling_spec.md#battle-event-production-sampling-acceptance)继续。

## BQR01 初始化与追加的原字节/哨兵（2026-10-06）

[四完整根指令原字节](../cg2d_battle_queue_readiness_sampling_evidence.json)纳管4390E0/4393A0/43B7F0/4394A0，211条751B/五页与101普通call7父、8出向可追。初始化三W8/就绪FE0一W8、C3对应普通有序位移/X后Y短路已修，原200项仅position/重复0和1哨兵、各S32/六-1、两追加五/三次计数读取和最后pub保持，未写position4。

[局部验收](../cg2d_battle_queue_readiness_sampling_spec.md#battle-queue-readiness-sampling-acceptance)保留busy==1/计数pub后1200和22槽调用后复读恢复、原返回与四声明；不加容量guard/非有限检测/异常模型，不宣PC53/ABI/浮点状态或完整七父/数据/owner/画面运行通过。

## BQA01 原重置与队列追加合同（2026-10-06）

[四完整原指令字节](../cg2d_battle_reset_append_baseline_evidence.json)99条439B/1206普通call26父、无出向，补当前四正确根原身份。Reset和Init分工不合并，重复position哨兵/200项仅position及先负值后零值保持；两Append同next写state后当前FC8复读写下一-1，4392F0的index+1独立于旧FC8，busy最后。

[验收边界](../cg2d_battle_reset_append_baseline_spec.md#battle-reset-append-baseline-acceptance)记录439400六CA0读与四W32到既有16B值语义承载、源码/四声明保持，不称逐栈ABI同形或源码新增。18引用页只补全集，完整26父/所有其他数据列表/owner/SDK/ABI/PC53/时钟RNG/画面仍待。

## BMR01 M读取与目标查找合同（2026-10-06）

[裁决与原基线](../cg2d_battle_m_read_sampling_spec.md#battle-m-read-sampling-decision)恢复整数next计算和字符串先S8判定/先游标取样三片段；原双字节截断扫描/双次游标发布、分隔与NUL、大写十六进制保持。Find的0x800000经实际原字节及操作数确认，保留原2+2*3项跳过与返回条件。

[证据](../cg2d_battle_m_read_sampling_evidence.json)四完整220条548B/77call5父/14出向；Peek/Find正确正文保持，旧对象头/Input与所有父未改，不称表达式差异已证明运行故障或完整M/owner/画面通过。

## BMS01 对象64与预览5AAA60读取差异（2026-10-06）

[原合同与裁决](../cg2d_battle_m_scan_sampling_spec.md#battle-m-scan-sampling-decision)恢复四根七片段：对象整数next、字符串S8条件/currentOffset、对象剩一格同next两pub与S32头索引。对象字符串第二字节先pub64再源读、预览字符串先源读再pub5AAA60，预览剩一格第二次复读自增及439600正确双pub不合并。

[完整字节/引用](../cg2d_battle_m_scan_sampling_evidence.json)328条911B/696call8父/27出向；RID既有失败恢复和两W8自增→offset发布保持。同索引仍两次计数、不新增位置转换/清零/保护，完整M/owner/画面运行仍待。

## BMD01 M参与计数和特效对/名称表生产（2026-10-06）

[原生产合同](../cg2d_battle_m_dispatch_sampling_spec.md#battle-m-dispatch-sampling-decision)恢复入口flag0后五W32/末W16清计数、同源门禁在W8计数pub前、2002门禁在S32对清零前。父保所有分派/字段数量与源flag输出；5A8A40对和20条36B名称表沿原首空/首同名/全无匹配规则，共享实际数组不改。

[三完整字节与调用](../cg2d_battle_m_dispatch_sampling_evidence.json)1725条6015B/41call6父/435出向、6逻辑表646B及全技能分类可追。两个攻击/辅助读取正确根保持，原RID19与试读20整数差异保留，不据完整生产根宣全部消费/owner/画面运行通过。

## BME01 M事件合并与状态发布（2026-10-06）

[裁决](../cg2d_battle_m_event_dispatch_sampling_spec.md#battle-m-event-dispatch-sampling-decision)恢复两merge先取5C/A8再pub5C/同mask，CHT14/BST20先取lastCreated再A8 pub；入口sceneType/exit、回合在三零写前取样、共享22计数五W32末W16与S32索引保持。Check先mask再bit，new事件先bit再live activeMask，两处不抹平。

[三完整302条966B](../cg2d_battle_m_event_dispatch_sampling_evidence.json)/2call1绑定3父/10出向与实际工厂/初始化消费纳管，失败/未知头/非拥有表和正确初始化保持；不把完整根外推全部资源消费、owner重进退出/浅别名/借用失效或画面运行。

## BMF01 完成恢复与延迟退休（2026-10-06）

[完成根合同](../cg2d_battle_event_finish_scene_lifecycle_sampling_spec.md#battle-event-finish-scene-lifecycle-sampling-decision)保持remaining先S32−1/pub后判正数、NUL亦copy3/25头首次匹配或保旧phase、live64与eventSlot复读、22轮每次先A8再bit并调用4390E0；注销先构slot bit再取活动mask、取反AND/pub，清槽后22轮恢复、清busy再MarkDeath。只标死，回收仍归Yobi实际寿命链。

[850条3188B两完整根](../cg2d_battle_event_finish_scene_lifecycle_sampling_evidence.json)同时纳管真实十状态清表/清Yobi/转场/明文退出顺序；完整427470消息执行、五父与全部角色/附件资源owner和重进退出/ABI/画面未闭，不新增保护/free或异常模拟。

## BMG01 队列动作166的计时条件消费（2026-10-06）

[共享倒计时裁决](../cg2d_battle_event_execution_gates_spec.md#battle-event-execution-gates-decision)核411CD2..411CFD原12条43B，与两失活BST和43BAA0共四inline/四源码表达式。只恢复现代timer helper的同remaining负条件先pub，原FDA70C唯一owner/S32秒×1000和负值0不变；动作166的原队列index/值、数字特效和状态推进正文保持，原inline不造独立函数身份。

[312条1201B有界证据](../cg2d_battle_event_execution_gates_evidence.json)不代动作166整个父/全部排空链或427470所有消息目标、资源owner/SDK/ABI/PC53/画面验收，恢复/回存保持、完整范围继续。

## BMH01 角色借用与图号字段读取（2026-10-06）

[角色消费取样](../cg2d_battle_m_data_sampling_spec.md#battle-m-data-sampling-decision)修复FP/变身借actor门禁在caster pub前保存；取包/Construct/Subtract/Less/SetGear/SetAuxGear后live角色读取不提前缓存。Chat图形加载后先caster再lifetime/角色，render新字段引用读取先native DX表后同角色graphicIndex S32再S16高度；借用仅调用期，不新增资源owner、底层对象暴露或原F/X。

[六块/共用尾原证据](../cg2d_battle_m_data_sampling_evidence.json)同时保四载体Construct/Copy/Destroy、两gear轮/骑乘与End Finish后scene7、A4真正复读。所有其它角色资源/完整分派/owner重进退出及SDK/ABI/画面未闭，现代helper不代整体寿命验收。

## BMI01 转场列表与宠物位置借用（2026-10-06）

[角色/载荷取样](../cg2d_battle_m_transition_sampling_spec.md#battle-m-transition-sampling-decision)恢复Escape/PetLeave存在条件先caster pub、Escape结果第一68和末玩家分支取样、Support FDC W8及PetLeave不同X/Y顺序；X借角色后读点再取payload，Y先借角色/payload再读点，不跨第二Rand缓存。原所有Append/Move、Reaction/1018字段、原解析position一次seed/两Rand(0,19)及空返/58增量保持。

[六目标及三共用尾](../cg2d_battle_m_transition_sampling_evidence.json)当前有界448条1743B十二页/60call不代所有列表消费者或全部资源owner重进退出/借用失效/SDK/ABI/画面。其它源/头/布局和正确附件/角色/RNG核心保持，完整范围继续。

## BMJ01 装备/状态中的角色与载荷取样（2026-10-06）

[三存在条件/技能等级/两W8/复活参数裁决](../cg2d_battle_m_status_sampling_spec.md#battle-m-status-sampling-decision)恢复caster发布之前保存actor存在条件、caster/actor之后technique再取payload写1044，flags byte门禁独立live取样，Rebirth flags→target保持原{position,flags,0,image}值语义。当前角色与原固定payload区别、取包/动作后重读、Debuff/Power字段及Queued preview busy/phase门禁保持。

[六目标与三共用尾](../cg2d_battle_m_status_sampling_evidence.json)有界643条2575B不代全部角色/列表状态消费和资源owner重进退出/借用失效/SDK/ABI/画面。原模式3/4追加80后重读mode必须保留，正常Power仅1/3、原零mode标签记录不造新业务或排除，完整范围继续。

## BMK01 位置交换与怪物目标的借用区别（2026-10-06）

[九项裁决](../cg2d_battle_m_position_monster_sampling_spec.md#battle-m-position-monster-sampling-decision)恢复POS四moving坐标与pet位置/四坐标各次取样，不提前缓存整组点值；保固定旧槽T**及旧位置，原两次新辅助附件重绑/旧主gear非对称合同不去重。MON效果缓存跨分支S32 caster位置而每次借actor，失败W8 bit2、Reset后才defender重读，成功工厂同位置/同槽先pub，再固定对象与payload解析。

[两目标/三尾证据](../cg2d_battle_m_position_monster_sampling_evidence.json)647条2787B不代完整owner重进退出/借用失效、全部列表消费与SDK/ABI/画面。四载体逐字段CopyPayload/析构、W16 fields288原零序和live W32高位、各坐标/正常零写及调用消费序保持，不新增空保护、默认对象或异常模型。
