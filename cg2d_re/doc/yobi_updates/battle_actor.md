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
