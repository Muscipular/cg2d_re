# Yobi Update：战斗角色

[分类总表与口径](../cg2d_yobi_update_coverage.md) · [唯一执行进度](../../plan/cg2d_full_reconstruction_progress.md)

## 登记边界

本表登记当前源码中的 1 个不同原地址回调。各行本体存在、所列直接或参数绑定已在源码出现；仅静态登记，不重验历史语义，不表示全部原端绑定已枚举、整链已接或运行通过。源码行号固定于本次登记快照，地址/符号为稳定身份；后续变动以符号重新定位。

[源码](../../src/YobiRuntime.cpp)；“绑定点”中的行号指赋值行，参数传入另写初始化路径。最终通用回收沿总表的公共生命周期，表内列局部退出责任。

## 回调明细

| 原地址/本体 | 创建与绑定点 | 载荷与生产者 | 阶段/依赖 | 退出、剩余与证据 |
|---|---|---|---|---|
| `404DB0` / `UpdateBattleActor_404DB0` | `NewBattleSpiracleShot_418B70`:8981<br>`NewBattleBlastWave_419180`:9036<br>`NewBattleRangeHit_419390`:9096<br>`NewBattleArcShot_419DA0`:9158<br>`NewBattleCastCopy_4205F0`:9208<br>`NewBattleBoomerang_4195F0`:9242<br>`UpdateBattleAction224_414050`:20059<br>`UpdateBattleAction224_414050`:20072<br>`UpdateBattleAction226_4154C0`:20107<br>`UpdateBattleAction226_4154C0`:20142<br>`UpdateBattleAction227_415940`:20181<br>`UpdateBattleAction227_415940`:20242 | BattleActorPayload；六种效果创建已生产局部字段，438290完整角色生产仍缺 | 257动作槽→200目标；公共尾残影及三类附件同步 | 224/226/227恢复绑定已接；角色/解析/场景完整清理仍待；[详细证据](../ui_windows/shared_battle.md#battle-dispatch-closure) |

## 生命周期任务

先推进 `438290` 真实角色创建及其初始化/发布依赖，补齐完整 `BattleActorPayload` 生产；再检查 `4239D0` RID与 `427470` 解析所发布的对象。六种已实现创建与三种恢复不是角色全量创建集合。`404DB0` 的257动作槽只形成200个子分派目标，既不计257个Update回调，也不表示角色上游或整场战斗已完成。公共尾的三个附件同步属于直接调用边，见[附件类](attachments.md)。
