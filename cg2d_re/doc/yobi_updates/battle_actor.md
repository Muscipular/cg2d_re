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

## BML01 骑乘载体与群体队员取样（2026-10-06）

[九组裁决](../cg2d_battle_m_ride_gmn_sampling_spec.md#battle-m-ride-gmn-sampling-decision)使RID效果记录与actor索引共用单次caster，三FDC门禁W8、效果Y点索引先当前effect、骑手坐标caster→rider→source；GMN入场点索引先current actor/payload、linked比较先W8 pub与caster比较先member借用。保两原生非拥有T*及16B非地址骑乘载体、固定旧payload/之后当前角色及原三路径/两路径。

[两目标与三尾](../cg2d_battle_m_ride_gmn_sampling_evidence.json)1198条4711B只有限合同；RID十二构造/析构和四rider copy、GMN五载体/正确401EA0尾跳/入口I8/单Rand及MarkDeath不立即free、工厂结果先pub、正常零写与各成员消费保持。全部owner重进退出/借用失效、列表消费者和SDK/ABI/画面仍待，不新空保护/默认对象或异常模型。

## BMM01 道具/连击中的当前角色与目标寿命（2026-10-06）

[二十四组裁决](../cg2d_battle_m_item_combo_sampling_spec.md#battle-m-item-combo-sampling-decision)恢复target条件/pub、caster/target与actor/payload的取样、参与者count先caster和player比较先同位置actor、循环/末165-27两种原序，COM末11/46跨分支目标位置借用及11调用后target先flags。序列count和就绪FDC均原W8对象字节读取，两图非地址索引S32，不改共享布局/owner或新增保护。

[两主目标/三尾和实际四态表](../cg2d_battle_m_item_combo_sampling_evidence.json)968条3545B仅有限合同。原重复列表、未补empty amount、调用后live消息flags与当前槽重借、RID/58及其它源头正确核心保持；全部资源owner重进退出/借用失效、消费者与SDK/ABI/画面继续。

## BMN01 等待态角色借用与原字节门禁（2026-10-06）

[四组裁决](../cg2d_battle_m_skill_entry_waiting_sampling_spec.md#battle-m-skill-entry-waiting-sampling-decision)恢复一次58同值参数、LastCaster/BothCasters两点/CasterFlash四FDC原W8读取；不把其他W32 flags/phase或寄存器位运算误改成byte。state2的caster仅作phase门禁，真正Advance沿已借最后参与者；Both第一调用后重新借last，CasterFlash调用后重新借caster，Clear先Finish后逐次借caster清字段/3D门禁后的flags保持原序。五整数两条件/pub差异也已落码，不改变共享布局或owner。

[五区间和五实际表](../cg2d_battle_m_skill_entry_waiting_sampling_evidence.json)490条1894B/512B不代42AE37后技能主体和完整父；skill98固定casterPayload/effects16B序列、全部资源owner重进退出/借用失效及消费者/SDK/ABI/画面继续。

## BMO01 技能4的角色与效果参数取样（2026-10-06）

[九组裁决](../cg2d_battle_m_skill4_sampling_spec.md#battle-m-skill4-sampling-decision)恢复caster/target同解析位置借用与target比较先pub，初始payload先skill和下一字段caster-actor-level-payload；439400与43BC10分别按原图号/flags/位置及actor次序取样，不跨调用合并。player首256/52沿同目标、51重新live目标，165/27先kind再target/actor；两图索引S32，不新owner/布局/保护。

[完整正常有界主体](../cg2d_battle_m_skill4_sampling_evidence.json)256条894B四页26call，原W32 flags入寄存器后AL/AH只局部操作，不误改byte读取；图值跨目标/各空出口/调用后live与Move/RID/58保持。其它各技能消费者及资源owner重进退出/借用失效、SDK/ABI/画面继续。

## BMP01 83中点与累计/反向效果角色取样（2026-10-06）

[十九组裁决](../cg2d_battle_m_skill61_sampling_spec.md#battle-m-skill61-sampling-decision)恢复X/Y分别两索引先两坐标、kind先actor/player首动作同caster，各target/last字段actor先skill或level，累计caster字段actor先live total，最后效果total-flags-caster-last-actor。反向效果沿已取位置/flags/amount，动作后重新借当前目标；仅42BB9A全局flags20原W8，其他W32和AL/AH寄存器局部运算保持，不改共享布局或owner。

[完整正常有界主体](../cg2d_battle_m_skill61_sampling_evidence.json)566条2220B八页56call，参与者重复/原正常零/不同空出口、调用后live与累计/Move-RID-58=2保持。其它各技能全部消费者及资源owner重进退出/借用失效、SDK/ABI/画面继续。

## BMQ01 自身效果角色与空条件回写（2026-10-06）

[五组裁决](../cg2d_battle_m_skill50_sampling_spec.md#battle-m-skill50-sampling-decision)恢复actor空比较先位置pub及同条件消费、两字段actor先live skill或level、自身4393A0沿同caster样本作actor和target、level比较后caster先偏移读取。每次调用后重新借当前角色，不把初始actor跨调用用于字段/效果/终结或末Move；不改共享布局或owner。

[完整正常有界主体](../cg2d_battle_m_skill50_sampling_evidence.json)103条423B两页13call，三Take/flags255/各正常常数和Move-增58无RID保持；其它各技能消费者与资源owner重进退出/借用失效、SDK/ABI/画面继续。

## BMT01 82交错坐标取样与两循环角色借用（2026-10-06）

[二十二组裁决](../cg2d_battle_m_skill12_82_97_sampling_spec.md#battle-m-skill12-82-97-sampling-decision)恢复82 X两索引/base坐标后借同position actor，再other坐标/delta/该payload后中点pub，Y另读两索引/坐标后live caster；12与97图号次序分别恢复，不统一采样。两原目标循环的target条件pub、主辅参数/两S32索引/两字段和player-kind都核，原W32 flags入寄存器的AL/AH局部位保持，不误改成byte；不改owner或布局。

[三完整正常有界主体](../cg2d_battle_m_skill12_82_97_sampling_evidence.json)749条2827B十一页78call，初始空/目标空不同Finish消费、12无RID/82-97共用尾RID、正常初值/图值复用与调用后live保持。其它各技能消费者与资源owner重进退出/借用失效、SDK/ABI/画面继续。

## BMU01 角色重建中的固定对象与live槽（2026-10-06）

[五组裁决](../cg2d_battle_m_skill93_sampling_spec.md#battle-m-skill93-sampling-decision)恢复同解析caster-target借用/条件pub与字段、Album后caster-target-actor。旧actor MarkDeath后重新取target创建，工厂后再取target发布；created与其payload固定跨各Take/四临时数值载体，后续效果/列表另借当前槽，不混为同一缓存。W32 flags高位和W16 fields288槽3/2/1/0正常零、各坐标逐次live与原无RID尾保持，不新owner/空保护。

[完整正常有界主体与尾](../cg2d_battle_m_skill93_sampling_evidence.json)241条1085B四页39call；所有actor owner重进退出/借用失效、98及其它技能消费者/SDK/ABI/画面继续。

## BMV01 98准备字段的角色和值读取（2026-10-06）

[五组裁决](../cg2d_battle_m_skill98_prepare_sampling_spec.md#battle-m-skill98-prepare-sampling-decision)恢复同解析caster位置发布和actor借用，固定初始caster payload先skill；level/mode字段各次caster→actor→live值→payload，不跨字段复用角色或跨Take固定mode。图号level比较12后caster先低段原偏移，fpUsed与六次动作各次live借用保持，不新owner/空保护或额外状态消费。

[入口准备](../cg2d_battle_m_skill98_prepare_sampling_evidence.json)86条362B两页11call；42D0D0之后各模式消费与所有actor owner重进退出/借用失效、SDK/ABI/画面继续分段推进。

## BMW01 mode6解析目标与调用后借用（2026-10-06）

[四组裁决](../cg2d_battle_m_skill98_mode6_sampling_spec.md#battle-m-skill98-mode6-sampling-decision)恢复同解析target/255比较先pub、首次效果沿解析target而后live caster-actor；两字段各次live target→actor→技能或level→payload，不复用空门禁actor。288/22404/168和五动作重新借live目标；空Finish后仍走原尾，不新caster保护/owner或异常模型。

[主体与两个实际尾](../cg2d_battle_m_skill98_mode6_sampling_evidence.json)98条405B三页12call纳管；其它98模式及全部actor owner重进退出/借用失效、SDK/ABI/画面继续。

## BMX01 mode7/8固定槽与各次字段借用（2026-10-06）

[十八组裁决](../cg2d_battle_m_skill98_mode7_8_sampling_spec.md#battle-m-skill98-mode7-8-sampling-decision)恢复mode7八坐标的position-index-actor/payload-point次序、initialOther payload先live target以及当前rider/gear/两aux指针链和值共用同一次target。native非拥有oldSlot地址和moving跨首次效果/换位固定，后其它字段/附件各次live重借，原不对称空门禁/parent重绑与槽发布保持。mode8 flags对象低W8、Reset用该次target，不新owner/立即释放/null保护。

[完整有界两个主体与三个尾](../cg2d_battle_m_skill98_mode7_8_sampling_evidence.json)341条1410B七页23call，BMW既有两尾22条82B非新增；其它模式/所有actor owner重进退出借用失效、SDK/ABI/画面继续。

## BMY01 中点、字段与按条件重新借用（2026-10-06）

[三根32组裁决](../cg2d_battle_m_skill98_mode12_13_14_sampling_spec.md#battle-m-skill98-mode12-13-14-sampling-decision)恢复12两indices-baseX-固定位置actor-otherX-delta-payload、Y中点后live selected，13两轴各两indices/中点后live selected；效果参数和两字段各次actor先值、S32图选择/player同位置保持。13只有0动作实际调用后重取target给149，否则沿比较所得位置；14金额Take后另live位置和kind条件、金额payload/另类型payload、caster-target-actor效果原序，不复用解析位置或新owner/空保护。

[三个完整有界主体与实际尾](../cg2d_battle_m_skill98_mode12_13_14_sampling_evidence.json)681条2625B十二页72call；两种原终结/正常失败保持，其它mode与全部actor owner重进退出借用失效、SDK/ABI/画面继续。

## BMZ01 伤害效果参数与调用后取样（2026-10-06）

[五组裁决](../cg2d_battle_m_skill98_mode15_16_sampling_spec.md#battle-m-skill98-mode15-16-sampling-decision)恢复target比较pub、两次效果参数原序、两个S32选择计数与player同位置；首次效果和后续aux分别live caster/actor，不跨调用缓存，图号两值/三数组仅正常零一次跨目标。两路径6/3和111011/金额字段各次live、原flags W32到EAX/AL-AH与条件重读及47/46保持，不新owner/空保护。

[两主体和三个实际尾](../cg2d_battle_m_skill98_mode15_16_sampling_evidence.json)363条1280B九页35call；BMX尾18条67B非新增，所有actor owner重进退出借用失效、状态目标/17-默认/SDK/ABI/画面继续。

## BNA01 固定flags、发布与字段各次借用（2026-10-06）

[11组裁决](../cg2d_battle_m_skill98_mode17_status_sampling_spec.md#battle-m-skill98-mode17-status-sampling-decision)恢复17 Take返回flags先pub/仅aux后重读、aux graphics-caster-fixed flags-target-actor，再普通Effect live参数；状态Take flags后先caster再pub，首次效果沿固定resultFlags/后live目标和actor，各基础/0..5重写字段target-actor-live值-payload。状态mode字段仍live、不替本次分支快照，原10同side/11 Reset-Phase0-F32 8.0与金额5、18基础消费保持，不新owner/空保护。

[完整有界两个主体及尾](../cg2d_battle_m_skill98_mode17_status_sampling_evidence.json)306条1163B八页31call，既有26条97B非新增；原98连续指令合并1833条7116B不代所有actor owner重进退出/借用失效与SDK/ABI/画面运行。

## BNB01 状态载荷与字段原序（2026-10-06）

[99族14组15处](../cg2d_battle_m_skill99_family_sampling_spec.md#battle-m-skill99-family-sampling-decision)恢复初caster payload先skill/actor先level、图号位置先表、target比较先pub/Take固定flags和AddAppendEffect target-level-skill、EffectList target先caster、四组成功/基础字段actor先值、99状态skill先actor/payload、2019 level-actor-减一-payload-余数。固定状态payload跨144正确，原两个纯比较helper与现代短路保持；2019写13E0前另读live level，EDI跨目标默认不补零。原400条/40call及实际尾见[证据](../cg2d_battle_m_skill99_family_sampling_evidence.json)，其它正文/头和全部运行门禁保持。

## BNC01 技能107字段取样原序（2026-10-06）

[五处实修](../cg2d_battle_m_skill107_sampling_spec.md#battle-m-skill107-sampling-decision)保初payload先skill、actor先level、同解析角色/target与比较先pub和固定flags-caster先pub/target-actor。原fpUsed/25-151-55-104、取包及两种失败、末终结Move-增58无RID保持；[93条355B原证据](../cg2d_battle_m_skill107_sampling_evidence.json)不代完整父或消费-owner运行，所有其它正文和声明保持。

## BND01 技能108目标与结果发布（2026-10-06）

[单根十处](../cg2d_battle_m_skill108_sampling_spec.md#battle-m-skill108-sampling-decision)明确初payload先skill/actor先level、同解析caster/target与比较先pub，Take flags后live target先flags pub/actor先skill、level独立取样；比较后的0/106或57角色沿原位置，两个图号路径先target后偏移表，EffectList最后另读live flags-caster-target-actor。固定amount、149/184原选择/159门禁、失败和末55共用尾保持；[实际239条981B](../cg2d_battle_m_skill108_sampling_evidence.json)不代完整父/消费-owner运行，所有其它正文/声明不改。

## BNE01 技能109角色与三级flags选择（2026-10-06）

[单根九处](../cg2d_battle_m_skill109_sampling_spec.md#battle-m-skill109-sampling-decision)明确payload先skill/actor先level、同解析caster/target与比较先pub、Take flags后target先pub/actor先skill与独立level、比较后0/106或57沿同位置和最后EffectList另live flags-caster-target-actor。105后仅一次内存W32 flags沿EAX/AL作80000-200000-bit2选择、原22329/22331及各次live/55-Move无RID保持；[251条原证据](../cg2d_battle_m_skill109_sampling_evidence.json)不代完整父/消费者-owner运行，其它正文/声明保持。

## BNF01 技能112字段与两位置原序（2026-10-06）

[单根九处](../cg2d_battle_m_skill112_sampling_spec.md#battle-m-skill112-sampling-decision)明确payload先skill/actor先level、同解析caster/target和比较先pub、target两字段actor先值；原flags先pub再读target不改。caster/target两个位置先current、256/52沿同位置而51另live，最后EffectList另读flags-caster-target-actor。0后一次W32/EAX-AH选择与25/55/161/75/109/110等原动作、初空/循环失败及Move尾保持；[226条872B证据](../cg2d_battle_m_skill112_sampling_evidence.json)不代完整父/消费者-owner运行，其它正文/声明保持。

## BNG01 技能114具体图号和flags原序（2026-10-06）

[单根八处](../cg2d_battle_m_skill114_sampling_spec.md#battle-m-skill114-sampling-decision)明确payload先skill/actor先level、同解析caster/target和比较先pub、低图号表-caster位置-加法后actor、Take固定flags-target-flags pub-caster-actor/固定参数及target两字段actor先值。原成功字段固定payload两写、143之后其它字段/动作另live和原失败/终结尾保持；[166条701B证据](../cg2d_battle_m_skill114_sampling_evidence.json)不代完整父/消费者-owner运行，其它正文/声明不改。

## BNH01 技能117字段与flags原序（2026-10-06）

[单根七处](../cg2d_battle_m_skill117_sampling_spec.md#battle-m-skill117-sampling-decision)明确同解析caster/target与比较先pub、payload先skill/actor先level、Take固定W32-caster-target-flags pub-actor/固定参数及target两基础字段actor先值。原成功56/失败68后另live22331、73/74和Move后缀保持；[146条585B证据](../cg2d_battle_m_skill117_sampling_evidence.json)不代完整父/消费者-owner运行，其它正文/声明不改。

## BNI01 技能73非零角色与效果原序（2026-10-06）

[单根六处](../cg2d_battle_m_skill73_sampling_spec.md#battle-m-skill73-sampling-decision)恢复初actor先level、同解析target比较先pub、两Effect原参数取样/两S32索引及target34后先位置比较，256/52沿同位置、51另live。初skill原先于caster/value位置pub、两字段先后续caster判空和原flags pub后amount Take/live门禁保持；[305条1051B证据](../cg2d_battle_m_skill73_sampling_evidence.json)不代73零模式完整消费者/全case/完整父和所有owner运行，其它根头/声明不改。

## BNJ01 技能106族角色与参数原序（2026-10-06）

[单根八处](../cg2d_battle_m_skill106_family_sampling_spec.md#battle-m-skill106-family-sampling-decision)恢复同解析caster/target、payload先skill/actor先level，普通Effect firstImage-flags-target-caster-actor、Aux选中图-second-first-flags-target-caster-actor、两S32索引及34后target先比较/256-52沿同位置/51另live。原2002消费/live门禁与初末动作及真实Move-RID增58保持；[409条1436B证据](../cg2d_battle_m_skill106_family_sampling_evidence.json)不代完整父/所有owner运行，其它根头/声明不改。

## BNK01 公共攻击父参数与结束角色（2026-10-06）

[两根四处](../cg2d_battle_m_attack_targets_call_sampling_spec.md#battle-m-attack-targets-call-sampling-decision)将73零/3100三值原序与普通入口currentSkill快照显式传包装。公共core后沿另live value5ABA7C位置取actor/Move，再RID和原固定1/增58；不改为caster位置、不新255/空对象门禁或终结效果。[80条304B证据](../cg2d_battle_m_attack_targets_call_sampling_evidence.json)含BNI24条复用，不代原完整父/所有owner和运行；core/其它正文与声明保持。

## BNL01 普通非零角色与mode3原取样（2026-10-06）

[单根23定义24处](../cg2d_battle_m_attack_nonzero_sampling_spec.md#battle-m-attack-nonzero-sampling-decision)恢复同解析caster/target、payload/actor先字段和2007表、258坐标S32、普通/自身转向两类Effect-Aux与100C字段原序，固定tailAmount沿原两分支EDI取样，四索引S32/两比较位置及末W8保持。原未指定局部不补值，原图分工/所有字段与动作-早返/终结Move-RID增58及其它根头声明保持；[985条3600B证据](../cg2d_battle_m_attack_nonzero_sampling_evidence.json)不代完整父/所有owner运行。

## BNM01 263、400、401字段与动作取样（2026-10-06）

[具体SPEC](../cg2d_battle_m_skill263_400_401_sampling_spec.md#battle-m-skill263-400-401-sampling-decision)：三根同parsed caster/target，固定caster payload先skill/各actor先level，400初Effect actor/记录同caster样本，低图分别位置-S32表-110052/110044/110081-actor，高111001/111011/111014保持。400两独立level/两图写不合并，401两target基础字段独立live位置-actor-值-payload，之后live flags效果；400用Take返回固定W32 flags，263target-image-caster-actor，两Cast0条件重读target各按原序。

[证据](../cg2d_battle_m_skill263_400_401_sampling_evidence.json)保存536条2090B八页66call、23补丁/改前根及旧812。原fpUsed和其它所有生产-取包/动作/feedback-成功门禁，初空/循环两出口与各尾/固定58值无RID保持。native非拥有地址/非地址S32-W32不扩宽，不新默认/owner/异常模型；其它根头不改。有限I/S不代所有owner重进退出失效、SDK-ABI-PC53、画面或完整父/运行。

## BNN01 402、405字段、图号与取样（2026-10-06）

[具体SPEC](../cg2d_battle_m_skill402_405_sampling_spec.md#battle-m-skill402-405-sampling-decision)：402第三Take后live caster；405同GetPos返回caster/target，各payload先skill/actor先level，402低图位置先S32表，405首图表先位置/次图位置先表，再加110052/110044、actor/payload，高图原常量。402EffectList固定position及共同递增比较先pub，405初Effect caster/记录同位置，五目标Effect按原图值/位置/actor不同顺序与两S32索引落实。

[证据](../cg2d_battle_m_skill402_405_sampling_evidence.json)342条1303B五页44call、20补丁/改前根/旧812完整前态；BNM原路由及BMP图偏移仅上下文。全部原字段/取包-转换/动作、402各槽路径和405首正项或默认0与各失败/终结尾-固定58值无RID保持，不补数组值、不新增owner/保护/异常模型。native非拥有指针及非地址宽度保留，所有owner重进退出失效/SDK-ABI-PC53/画面和完整父/运行另验。

## BNO01 404族角色字段与取样（2026-10-06）

[具体SPEC](../cg2d_battle_m_skill404_family_sampling_spec.md#battle-m-skill404-family-sampling-decision)单根23处：初同GetPos actor、预读比较缓存、两坐标各分支caster/点-偏移取样、初Effect位置先effectCount、rider先指针再field1D0值/两轴caster位置先rider及source、普通Effect/Aux和四S32索引-位置比较/恢复尾。原两子角色先后创建/四W16业务零置一、优先级W8-坐标/album、native carrier已证字段、八CryptInt正常临时-四rider复制和未写flags08保持；没有新增owner/默认/保护/异常模型。

[1286条4898B十七页147call](../cg2d_battle_m_skill404_family_sampling_evidence.json)保存实际字节/出口和23补丁/改前根，406三节点18B与404既有路由/BMO业务初始化仅上下文。原self/普通取包-字段/动作与各出口/真实Move-RID固定58=1、末selfFlags W8 bit40（200000仍W32）及second flagsFDC W8 bit2恢复，selfFlags未指定不补值。既有rider X算术/Y复制保持，全部owner重进退出失效/SDK-ABI-PC53画面与完整父/运行另验。

## BNP01 2028/2005族字段与效果取样（2026-10-06）

[具体SPEC](../cg2d_battle_m_skill2028_2005_sampling_spec.md#battle-m-skill2028-2005-sampling-decision)：两根24处恢复同parsed caster/target、payload先skill/actor先level、2028低高图的角色/payload-表各次取样、累计总量先target/固定flags与实际W8响应、反射字段/总量和最终AEL；2005族Effect/Aux-两S32索引/target先比较。原全部取包/字段-list-count、固定响应优先和149后双位置重读、动作/各失败-2028固定58=2与2005增一的真实Move/RID尾保持，其它根头不改。2028 target/反射两基础字段各actor先live值，caster反馈总量actor先global金额，末AEL amount-flags-caster target-reflected source-actor；相邻10000000仍W32，response bit20原W8。2005数组入口一次业务零/首正项默认0与原cached-live flags/反馈保持。

[证据](../cg2d_battle_m_skill2028_2005_sampling_evidence.json)：四范围713条2702B十一页72call连续字节/全部出口、24补丁/改前两根与旧812完整前态；BMP79条348B/9call+BMT7条26B/2call共86条374B/11call复用非新增，新627条2328B/61call分列。2028五S32/20B图表、实际56B表和4003顺序入口/2005既有表仅本组定位，不代全路由。native非拥有地址与非地址宽度不扩，不新增owner/保护/异常模型；全部owner重进退出失效/SDK-ABI-PC53/画面及完整父/运行待验。

## BNQ01 状态32族角色字段与分档图（2026-10-06）

[具体SPEC](../cg2d_battle_m_skill32_family_sampling_spec.md#battle-m-skill32-family-sampling-decision)：单根13定义14处恢复同parsed caster/target、payload先skill/actor先level、三初图各取样、附加效果target-level-skill/固定flags、基础字段与两成功skill actor先值、一S32分档行和八分档图按表先actor或payload先表。原状态类型/成功门禁-比较、固定payload跨替换动作、正确debuff默认0与未指定statusType、所有消费-字段/动作和各失败/End-Move-RID增58保持，其它根头/图表不改。三初图/八分档图依各次样本写1028/102C，target-level-skill附加效果后live flags、原固定flags供EffectList；基础和32..49/99..104成功字段各actor先值，后续重新读skill作原类型分派。status载荷在142/144前固定、调用后原字段顺序和141/143/37另live角色保持。

[证据](../cg2d_battle_m_skill32_family_sampling_evidence.json)：1065条4656B十四页76call/全部直接和十二表间接出口，十二表117W32槽468B+93W8索引=561B、169S32/676B图矩阵与旧BMP48B偏移/具名入口上下文，13补丁/改前根和旧812完整前态保存。当前三个constexpr矩阵/重复列、S32默认0与未指定局部保持；不新owner/类型/保护/异常模型，全部owner重进退出失效/SDK-ABI-PC53与完整父/运行待验。

## BNR01 累计半额与反弹字段取样（2026-10-06）

[具体SPEC](../cg2d_battle_m_skill19_family_sampling_spec.md#battle-m-skill19-family-sampling-decision)：单根26定义27处恢复同parsed角色、初低图位置-表-加法、累计先target后发布、普通Effect/Aux与两AEL、半额反馈/反弹字段-copy和末效果取样，四S32业务索引及五处实际W8门禁。原entry0/1比较、业务零/未指定lastAmount、全部图号-金额门禁/字段/动作与真实Move-RID固定58=2保持，其它根头不改。半额actor先累计值，反弹skill/level各actor先值；图copy source actor后先reflected位置，再source payload和dest actor/payload，末反馈W8各按实际取样。状态动作/载荷所有者保持。

[原端证据](../cg2d_battle_m_skill19_family_sampling_evidence.json)：43412E..434EDF914条3505B十二页67call/全部直接和三个内部表间接出口，三表121B、3037入口36B及原48B偏移/具名低中入口只上下文，26补丁/改前根及旧812完整前态保存。不新owner/类型/保护或异常模型，完整父和owner重进退出失效、SDK/ABI/PC53运行仍待。

## BNS01 技能267角色/图copy和累计字段（2026-10-06）

按[具体SPEC](../cg2d_battle_m_skill267_sampling_spec.md#battle-m-skill267-sampling-decision)，单根25定义27处恢复角色字段、effectCount/初图、累计发布-普通与循环/末Effect-Aux参数、图copy/反馈-state取样；四S32业务索引和两W8门禁，原全部图号-字段/金额门禁/动作与真实Move-RID固定58=1保持，其它根头不改。[证据](../cg2d_battle_m_skill267_sampling_evidence.json)：434EDF..43573F576条2144B八页45call及全部直接出口，无内部表/复用尾；267/3029原180B/36B入口表和十二S32/48B偏移仅上下文，25补丁/改前根与旧812完整前态保存。循环累计source先清flags pub，末Effect沿固定flags/Aux另live flags；入口两个W32业务零和未指定lastAmount、所有原调用/门禁-状态动作保持。

旧812只五引用/一既有源码symbol登记/本步对象，E491→495仅双域四局部I/S，七open及全部其它指标/完整成员-门禁保持。完整父17298/SEH、其它技能/全路由、全部consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8/运行另验。

## BNT01 技能目标图号与fixed结果消费（2026-10-06）

依[具体SPEC](../cg2d_battle_skill_targets_core_spec.md#battle-skill-targets-core-decision)，单根四处恢复target255比较先发布、普通Effect/Aux的target输出先source输出，以及redirect输出发布后的参数原序；十参/四native非拥有S32输出、原每轮八业务零/最后零values扫描、两图表-固定flags/value及所有门禁/两返回保持。[证据](../cg2d_battle_skill_targets_core_evidence.json)：独立426FC0完整369条1185B五页32call/全部直接出口和两ret，十三条31B选择叶/两40B表及唯一4369CC caller26条94B上下文分别计数；四补丁和改前根/独立ID前态保存。初initial/source同GetPos返回依序写；正常255只sourceEnd/74、初/target空只Finish。caller原十实参和返回后重读initial-Move/RID固定1保持，其它根/header不改。

原ID已在库存，新增独立语义记录1350/adapted分类与FUNC-BATTLE显式成员52→53，旧812整记录保持。语义1351/有效ID1346、分类457、文件关联1464/3680=39.78%，E495→497仅BATTLE两局部I/S；175定位队列不变（此前已有历史定义）、七open/60源码closed10及全部完整门禁保持。完整战斗/父SEH、全部consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8/运行仍待。

## BNU01 转向字段与辅助旧槽语义（2026-10-06）

[具体SPEC](../cg2d_battle_redirect_aux_baseline_spec.md#battle-redirect-aux-baseline-decision)：两既有正确根保持，新增三方转向与辅助效果的独立原ID/基线、语义映射、adapted分类和战斗成员。[证据](../cg2d_battle_redirect_aux_baseline_evidence.json)：229条693B四页31call/两ret和一条件出口：转向180条28call/24Cast-四EffectList-三字段及S32返回，辅助49条三Effect/fixed flags800000-payload/CA0-1跨两Append-旧四字段读取；五转向/37辅助入向与返回消费窗口分列。source/redirect/target的完整序列/每次actor取样保持，不合并别名；三W32字段各写后另取actor，辅助第三记录沿旧槽position4-flags-position3-position，非输入flags替代。既有源码/头和caller不改，BQA/BQR已过未变生产叶不复验。

新增语义1351/1352绑定原F:000242B0/F:0003BC10，战斗成员53→55但完整枚举仍open；语义1353/有效ID1348、分类459、关联1466/3680=39.84%、E497→501仅各函数I/S四条。两ID已有历史定义、不在原175队列，不虚减队列；所有旧语义/其它域成员、60源码closed10/七open和完整门禁保持。完整父/所有consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8/运行另验。

## BNV01 特效坐标与flags发布（2026-10-06）

[具体SPEC](../cg2d_battle_effect_creation_sampling_spec.md#battle-effect-creation-sampling-decision)和[证据](../cg2d_battle_effect_creation_sampling_evidence.json)：两个完整创建函数125条412B三页九call/六分支/四ret。随机与位置flags各恢复一次取样两次发布，位置X取样先image/direction、截断后取Y再发布X，三个补丁/两个根解决；两次RNG/Load与live S16图宽、graphicOffsetY、原失败返回及S32/F32/W32/W16/W8字段保持。其它源/头和caller不改；原__ftol沿CPH01 host裁决。入向80/28父和13/9父仅列身份，不代全部父或owner运行。

新增独立语义1353/F:00019FC0、1354/F:0001E680和adapted分类，FUNC-BATTLE55→57仍未全枚举；语义1355/有效ID1350、分类461、关联1468/3680=39.89%、E505仅各函数两局部I/S。两ID原有历史定义，队列175保持。全部旧语义/其它功能、七open任务与所有完整门禁保持；完整consumer-owner/资源重进退出、SDK/ABI/PC53/实际RNG画面及R1/R7/R8另验。

## BNW01 标死后位置投影（2026-10-06）

[具体SPEC](../cg2d_battle_effect_callbacks_sampling_spec.md#battle-effect-callbacks-sampling-decision)与[证据](../cg2d_battle_effect_callbacks_sampling_evidence.json)：两callback/三创建入口完整168条576B五页17call/14分支/八ret。位置callback恢复X截断→Y取样→X发布→Y截断发布，严格PlayFrame(1)==1才MarkDeath，标死和未完成都继续投影；MarkDeath仅标死不立即free/摘链。其它四正确根保持：坐标/轨迹两创建绑定position callback、屏幕创建绑定one-shot callback；轨迹RNG仅无设备路径/结果后取flags，mode只1加20000后另live flags，低W8优先级/固定输入坐标保。原__ftol沿CPH01 host，六callback绑定和11直接创建call分别列身份，非所有父已验。

新增语义1355..1359五原ID/分类、BATTLE57→62但枚举open；语义1360/有效ID1355、分类466、关联1473/3680=40.03%、E515仅五函数各I/S。位置回调一块问题已解决，四既有正确源不计新增；原有历史定义/队列175及所有旧语义-其它域/七open-完整门禁保持。全部consumer-owner/资源重进退出、SDK/ABI/PC53-RNG画面及R1/R7/R8继续。

## BNX01 上升取样与阶段消费（2026-10-06）

[具体SPEC](../cg2d_battle_rising_emitter_fade_sampling_spec.md#battle-rising-emitter-fade-sampling-decision)和[证据](../cg2d_battle_rising_emitter_fade_sampling_evidence.json)：七完整340条1215B八页19call/19普通内部出口-14ret、背景五槽switch20B/两16项S32局部表128B。上升callback一处恢复X截断后Y取样先X发布，原未舍入speed/Y中间值、F32存储/父deathFlag严格1早退与低于phase标死后仍Play(0)保。其它六正确源保持：创建先RNG再分配、无设备可二次RNG；屏幕发射先Y随机再X、创建后另live phase--；背景0贯穿1可两次发布/phase4而本次不标死，2归零贯穿3/4置flags2后标死。原业务0/4/8/255/失败和字段宽度、packed颜色高W8访问器保24位及所有caller/声明保持，全部父/ABI-PC53-RNG画面不外推。

七独立原ID纳入语义1360..1366/分类、BATTLE62→69仍枚举open；语义1367/有效ID1362、分类473、关联1480/3680=40.22%、E529各函数I/S共14E。五callback绑定/七普通入向分列，历史定义已有/队列175保持；其它旧记录/功能域、七open/所有完整门禁与验收0保持。实际取样问题已解决，完整consumer-owner/资源重进退出、SDK-ABI-PC53-RNG画面及R1/R7/R8继续。
