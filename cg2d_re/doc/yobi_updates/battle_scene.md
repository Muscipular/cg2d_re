# Yobi Update：战斗场景

[分类总表与口径](../cg2d_yobi_update_coverage.md) · [唯一执行进度](../../plan/cg2d_full_reconstruction_progress.md)

## 当前实现与绑定

| 原回调 | 创建/绑定证据 | 载荷/阶段 | 当前实现与接线 | 退出与剩余 |
|---|---|---|---|---|
| `427470` / `UpdateBattleEvent_427470` | `NewBattleEvent_43B610` 分配场景载荷并绑定；原绑定43B63D | `BattleScenePayload` 按已消费字段承接，M解析/角色与效果发布已有 | 完整回调与创建父已接；RID及C父调用已有 | 424620、完整载荷归属/清理及运行仍待 |
| `43B360` / `DispatchBattleEvents_43B360` | `NewBattleScene_4383B0` 创建并绑定；`InitializeBattleScene_4383F0` 发布 | 同类型场景载荷，事件/组计数和调度 | 43CBB0→4383F0→4383B0→43B360→43B610→427470已有真实链 | 跨回合、退出、附属清理和完整战斗未验收 |

[原绑定与载荷证据](../ui_windows/shared_battle.md#battle-action-14)已记原指令位置，当前源码59个已绑定原目标中包含本类2个目标/2个绑定。当前实现见 [场景组](../../src/Yobi/battle_scene.cpp) 与 [消息组](../../src/Yobi/battle_message.cpp) 的上述符号，场景父见 `world/SceneState.cpp`；本次纠正文档漏记，不表示原端全部场景回调已枚举或运行通过。

## 生命周期任务

1. 438290基础创建与4239D0 RID已经实现，保持404DB0完整回调接线；438550 C解析/缺席清理也已实现，43CBB0等初始化/发布链已有调用，不把基础创建、RID与C子集当完整角色生产。
2. 沿427470每种解析分支落实载荷写者、动作/效果队列、32事件槽和子对象发布，核已接的[RID入口](../ui_windows/shared_battle.md#battle-actor-create-rid)；未知封包可参考cg3a网络实现，冲突仍以已证cg2d为准。
3. 沿已接43B610创建与43B360调度，收口场景转换、CHT/BST退出和附件其他清理，逐一核借用T*清槽和重绑定。
4. 424620的var_50入口一次置0已按[局部确定化SPEC](../cg2d_local_determinism_spec.md)决议，完整攻击生产仍待实施；分类清单不扩大其适配范围。

这里只维护依赖任务和覆盖状态，完成小步记唯一进度，不添加第二份时间线。不构建、测试、运行客户端；整场战斗未验收。
