# Yobi Update：投射与特效

[分类总表与口径](../cg2d_yobi_update_coverage.md) · [唯一执行进度](../../plan/cg2d_full_reconstruction_progress.md)

## 登记边界

本表登记当前源码中的 35 个不同原地址回调。各行本体存在、所列直接或参数绑定已在源码出现；仅静态登记，不重验历史语义，不表示全部原端绑定已枚举、整链已接或运行通过。源码行号固定于本次登记快照，地址/符号为稳定身份；后续变动以符号重新定位。

[源码](../../src/YobiRuntime.cpp)；“绑定点”中的行号指赋值行，参数传入另写初始化路径。最终通用回收沿总表的公共生命周期，表内列局部退出责任。

## 回调明细

| 原地址/本体 | 创建与绑定点 | 载荷与生产者 | 阶段/依赖 | 退出、剩余与证据 |
|---|---|---|---|---|
| `418D80` / `UpdateBattleProjection_418D80` | `NewBattleProjection_418DF0`:8935 | BattleActorPayload；418DF0生产，+10借用父 | 双位移投影 | 父/距离门禁MarkDeath后仍播放；[详细证据](../ui_windows/shared_battle.md#battle-projection-dependencies) |
| `418F10` / `UpdateBattleArcProjection_418F10` | `NewBattleArcProjection_418FF0`:8867 | BattleActorPayload；418FF0生产，父/目标T*借用 | 弧线、残影与屏幕投影 | 标死后仍执行残影/投影尾；[详细证据](../ui_windows/shared_battle.md#battle-actions-251-254-arc) |
| `419BE0` / `UpdateBattleTransitionEffect_419BE0` | `NewBattleTransitionEffect_419C10`:10051 | BattleTransitionEffectPayload；419C10生产；回调不消费载荷 | 动画完成仅通知phase1 | 本体不跟随、不自动标死；owner负责退出；[详细证据](../ui_windows/shared_battle.md#battle-action-163) |
| `419CB0` / `UpdateBattleOffsetFollowEffect_419CB0` | `NewBattleOffsetFollowEffect_419CF0`:10098 | BattleFollowOffsetPayload；419CF0生产，parent00借用 | 按屏幕坐标与S32偏移跟随 | 父/生命周期门禁标死；[详细证据](../ui_windows/shared_battle.md#battle-actions-75-82) |
| `419FA0` / `UpdateBattleOneShotEffect_419FA0` | `NewBattleRandomEffect_419FC0`:9525<br>`NewBattleScreenEffect_41A0D0`:9555<br>`NewBattleScreenEffect_421610`:10601 | 无payload；419FC0/41A0D0/421610生产 | 一次动画播放 | 完成MarkDeath；各创建随机及坐标差异不合并；[详细证据](../ui_windows/shared_battle.md#battle-local-effects) |
| `41A160` / `UpdateBattleCompletingEffect_41A160` | `NewBattleEffect_41CD00`:8012<br>`NewBattleEffect_41CD00`:8030<br>`NewBattleEffect_41CD00`:8053<br>`NewBattleEffect_41CD00`:8292 | 无payload；41CD00生产，父BattleActorPayload仅借用消费 | 完成通知与动画门禁 | 完成标死；分支可能随后覆盖此初始callback；[详细证据](../ui_windows/shared_battle.md#battle-factory-callbacks) |
| `41A1C0` / `UpdateBattleParentEffect_41A1C0` | `NewBattleEffect_41CD00`:8122<br>`NewBattleEffect_41CD00`:8262<br>`NewBattleEffect_41CD00`:8343 | 41CD00按分支分配BattleActorPayload或0B；+10借用父 | 父关联播放/生命周期门禁 | MarkDeath；不凭回调统一反推分配大小；[详细证据](../ui_windows/shared_battle.md#battle-effect-factory) |
| `41A200` / `UpdateBattleRisingEffect_41A200` | `UpdateBattleRandomEmitter_41A310`:6647<br>`UpdateBattleIndexedEmitter_41A7D0`:6760<br>`NewBattleRisingEffect_420D50`:6799 | 无payload；父引用在+10，三个已知创建点 | 上升粒子/父完成计数 | 标死后仍保留帧播放/投影尾；[详细证据](../ui_windows/shared_battle.md#battle-factory-callbacks) |
| `41A270` / `UpdateBattleScalingEffect_41A270` | `NewBattleEffect_41CD00`:8377 | 无payload；41CD00 type24生产 | 缩放及帧播放 | 本体无MarkDeath；父/场景清理不能省略；[详细证据](../ui_windows/shared_battle.md#battle-factory-callbacks) |
| `41A310` / `UpdateBattleRandomEmitter_41A310` | `NewBattleEffect_41CD00`:8070 | 无payload；41CD00生产，+10借用父 | 发射41A200；phase/field58独立 | 父死亡早退标死，完成通知父1024后标死；[详细证据](../ui_windows/shared_battle.md#battle-factory-callbacks) |
| `41A530` / `UpdateBattleFallingEffect_41A530` | `UpdateBattleReverseEmitter_41A590`:6715 | 无payload；41A590发布下落粒子，+10借用发射器 | 下落至nameY后递增父计数 | MarkDeath后仍播放/投影；[详细证据](../ui_windows/shared_battle.md#battle-factory-callbacks) |
| `41A590` / `UpdateBattleReverseEmitter_41A590` | `NewBattleEffect_41CD00`:8094 | BattleRingEmitterPayload；41CD00 type33生产 | 按索引/计数发布41A530 | 128完成计数通知父1024并标死；[详细证据](../ui_windows/shared_battle.md#battle-factory-callbacks) |
| `41A7D0` / `UpdateBattleIndexedEmitter_41A7D0` | `NewBattleEffect_41CD00`:8097 | 分配BattleRingEmitterPayload但回调不读自身payload；41CD00 type25生产 | 索引发射41A200，失败仍推进nameX | 32完成计数通知父1024并标死；[详细证据](../ui_windows/shared_battle.md#battle-factory-callbacks) |
| `41A9E0` / `UpdateBattleImpactEffect_41A9E0` | `NewBattleEffect_41CD00`:8213<br>`NewBattleEffect_41CD00 → InitBattleTargetEffect_422E50`（参数） | BattleActorPayload；41CD00直接或422E50初始化 | 动画完成与HP/FP/场景value00消费 | MarkDeath及原完成通知；BattleScenePayload是借用父载荷；[详细证据](../ui_windows/shared_battle.md#battle-impact-callbacks) |
| `41AF00` / `UpdateBattleImpactSoundEffect_41AF00` | `NewBattleEffect_41CD00 → InitBattleTargetEffect_422E50`（参数） | BattleActorPayload；41CD00经422E50生产 | 与41A9E0共享命中结构，默认音效分类不同 | 完成标死/通知；不得因结构相近合并入口统计；[详细证据](../ui_windows/shared_battle.md#battle-impact-callbacks) |
| `41B440` / `UpdateBattleFrameEventEffect_41B440` | `NewBattleEffect_41CD00 → InitBattleLowerEffect_423350`（参数）<br>`NewBattleEffect_41CD00 → InitBattleScreenYEffect_4234B0`（参数） | BattleActorPayload；41CD00经423350或4234B0生产 | frameEvent命中、HP/FD0差异、渐消 | 双次消退/标死路径保留；[详细证据](../ui_windows/shared_battle.md#battle-frame-event-effect) |
| `41BB00` / `UpdateBattleMovingImpactEffect_41BB00` | `NewBattleEffect_41CD00 → InitBattleOffsetEffect_423040`（参数） | BattleActorPayload；41CD00经423040生产 | 追踪移动、命中、延迟/随机/尾迹 | 提前标死与帧尾按原合同共存；[详细证据](../ui_windows/shared_battle.md#battle-moving-impact-effect) |
| `41C3B0` / `UpdateBattleHomingImpactEffect_41C3B0` | `NewBattleEffect_41CD00 → InitBattleCenterEffect_4231D0`（参数） | BattleActorPayload；41CD00经4231D0生产 | 转向/距离命中与尾迹 | MarkDeath与完成尾；完整父载荷生产仍缺；[详细证据](../ui_windows/shared_battle.md#battle-homing-carry-effects) |
| `41E4D0` / `UpdateBattleScatterEffect_41E4D0` | `NewBattleScatterEffect_41E510`:9406 | 无payload；41E510生产 | 散射位移/播放 | MarkDeath；动作创建caller见原证据；[详细证据](../ui_windows/shared_battle.md#battle-actions-32-40) |
| `41E5C0` / `UpdateBattleAfterimage_41E5C0` | `NewBattleAfterimage_41E5F0`:1729 | BattleActorPayload；41E5F0复制父载荷，非浅别名 | valueFF0寿命递增；混合标志由创建者设置 | MarkDeath；借用附件字段不自动获得所有权；[详细证据](../ui_windows/shared_battle.md#battle-core-actions-0-6) |
| `41E640` / `UpdateBattlePositionEffect_41E640` | `NewBattlePositionEffect_41E680`:9439<br>`NewBattleCoordinateEffect_41E720`:9461<br>`NewBattleTrailEffect_41E7C0`:9488 | 无payload；41E680/41E720/41E7C0生产 | 坐标特效播放 | 完成MarkDeath；三生产点分开登记；[详细证据](../ui_windows/shared_battle.md#battle-action-209) |
| `41E920` / `UpdateBattleTiledEffect_41E920` | `NewBattleTiledEffect_41E9D0`:8572 | 无payload；41E9D0生产，对象CryptInt承载寿命 | 计数与平铺效果 | 比较寿命后MarkDeath，原递增次序保留；[详细证据](../ui_windows/shared_battle.md#battle-action-216) |
| `41F110` / `UpdateBattleNumber_41F110` | `NewBattleNumber_41F1A0`:9620<br>`NewBattleTimeNumber_4206C0`:9693 | 无payload；41F1A0/4206C0生产 | 数字的轨迹/寿命 | MarkDeath；计时数字复用本体但保留独立创建；[详细证据](../ui_windows/shared_battle.md#battle-actions-164-169) |
| `41F450` / `UpdateBattleFloatingIcon_41F450` | `NewBattleFloatingIcon_41F4C0`:9765 | 无payload；41F4C0生产 | 浮动图标轨迹 | 寿命MarkDeath；[详细证据](../ui_windows/shared_battle.md#battle-attribute-effects) |
| `41F570` / `UpdateBattleHitEffect_41F570` | `NewBattleHitEffect_41F5B0`:9909 | 无payload；41F5B0生产 | deltaX递增至deltaY寿命，投影创建时保存的40/44 | 寿命到期MarkDeath；本创建不保存父引用；[详细证据](../ui_windows/shared_battle.md#battle-local-effects) |
| `41F800` / `UpdateBattleDebuffEffect_41F800` | `NewBattleDebuffEffect_41F660`:9955<br>`NewBattleReverseEffect_41F860`:10006 | 无payload；41F660/41F860生产 | 两种状态效果共享播放/跟随 | MarkDeath；父槽1054/105C退出清理独立；[详细证据](../ui_windows/shared_battle.md#battle-state-effect-restore) |
| `41F950` / `UpdateBattleCenteredEffect_41F950` | `NewBattleCenteredEffect_41FA00`:10150 | 无payload；41FA00生产，父T*及1064发布 | 居中跟随、图形读取/播放 | MarkDeath；其他1064 caller/清理尚未全闭合；[详细证据](../ui_windows/shared_battle.md#battle-actions-219-220) |
| `41FB20` / `UpdateBattleAttachedEffect_41FB20` | `NewBattleAttachedEffect_41FBE0`:10229 | 无payload；41FBE0生产，父BattleActorPayload借用 | 附属效果跟随/计数 | MarkDeath；父1058槽清理单列；[详细证据](../ui_windows/shared_battle.md#battle-local-effects) |
| `41FCD0` / `UpdateBattleStatusEffect_41FCD0` | `NewBattleStatusEffect_41FE00`:10318 | 无payload；41FE00生产，父BattleActorPayload借用 | 状态效果跟随、13D8消费 | MarkDeath；父1068槽清理单列；[详细证据](../ui_windows/shared_battle.md#battle-state-effect-restore) |
| `41FF90` / `UpdateBattleFieldEffect_41FF90` | `NewBattleFieldEffect_41FFE0`:10390 | 无payload；41FFE0生产，经全局gBattleSceneAction5ABA94访问场景 | 场景deathFlag检查、帧播放及屏幕投影 | MarkDeath；场景B0完整生产/清理仍待；[详细证据](../ui_windows/shared_battle.md#battle-factory-actions-115-162) |
| `420300` / `UpdateBattleSequence_420300` | `NewBattleSequence_420100`:6403 | 无payload；420100生产，序列与CryptInt在对象字段 | 六态序列及420590推进 | 终态MarkDeath；同源RNG/临时构造顺序沿证据；[详细证据](../ui_windows/shared_battle.md#battle-sequence-effect) |
| `421480` / `UpdateBattleBackgroundFade_421480` | `NewBattleBackgroundFade_421570`:10482 | 无payload；421570生产 | phase0..4背景淡变，按counter28发布共享背景图号 | MarkDeath；175/176等owner退出见详细证据；[详细证据](../ui_windows/shared_battle.md#battle-actions-139-140) |
| `4216B0` / `UpdateBattleScreenEmitter_4216B0` | `NewBattleScreenEmitter_421700`:10643 | 无payload；421700生产 | 随机发布421610屏幕效果 | 退出标死，独立原随机消费；[详细证据](../ui_windows/shared_battle.md#battle-actions-170-207) |
| `421750` / `UpdateBattleText_421750` | `NewBattleText_4217F0`:6336 | BattleTextPayload；4217F0生产，panel00借用、lines04动态文本 | 就绪门禁、按原同坐标逐行绘制、remaining递减 | 依序释放每行和行表、标死panel与自身；[详细证据](../ui_windows/shared_battle.md#battle-text-effect) |
| `4219D0` / `UpdateBattleCarryingEffect_4219D0` | `NewBattleEffect_41CD00`:8180 | BattleActorPayload；41CD00 type35/skill400生产 | 携带目标、转向/测距、命中、尾迹 | 死亡仍可能执行公共尾；父计数/目标状态契约见证据；[详细证据](../ui_windows/shared_battle.md#battle-homing-carry-effects) |

## 生命周期任务

按每一实际创建/工厂类型推进 `创建 → 初始化 → callback → 子对象发布/完成通知 → 标死/释放`，不只按本体地址清零待办。`41CD00` 有初始绑定随后覆盖的路径；本清单登记每处源码赋值，不将每处赋值视为独立对象。载荷列区分自身分配与借用父载荷，不能按回调里的cast反推自身大小。工厂的48类分派、`404DB0`的动作槽及本表Update数量分开统计。各行均尚未运行验收，完整源/目标/场景载荷生产、其他caller和完整退出路径仍以对应证据为准。
