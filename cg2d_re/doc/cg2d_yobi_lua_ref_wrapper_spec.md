# Skill Lua 移除 SPEC（YLR01）

日期：2026-10-27。状态：按用户要求完成 Skill Lua 移除及静态检查；未构建、测试或运行。沿用原文档路径与编号，覆盖此前薄引用封装方案的本轮范围。

## 1. 目标与范围

移除 E10 Skill Lua 的接口、运行时及原生接管入口，让技能继续由现有 C++ 流程处理，不保留旧 Lua 技能兼容层或空接口。

本轮不实现 Yobi Lua 薄引用，不设计其属性、方法、回调、引用绑定或失效机制；自定义技能的新方案另行设计。此前关于薄引用及 markDeath 后不可操作的讨论不作为本次移除任务的实施或验收项。

## 2. 移除内容

- `src/Yobi/battle_lua_skill.cpp` 中的 Skill Lua 实现，包括 `Skill`／`Skill.register`、`PacketReader`、动作计划、阶段回调和逐帧视觉回调。
- `LuaSkillRuntime`、`RuntimeNode`、`RuntimeCastBinding`、`PendingBatch` 及仅供其使用的节点树、批量提交、子树等待、完成／失败清理与辅助状态。
- Skill Lua 专用的 `gSkillEffectL` 创建、注册、关闭，以及 `lua/battleSkillEffect.lua` 加载入口；不迁入其他 Lua 状态。
- SKL 分派、角色更新、动作推进、标死、物理释放及技能元数据绑定中的 E10 接管调用与专用声明。
- E10 专用且已无其他消费者的元数据字段、复制／清理代码及源码列表项；逐项确认用途后移除。

不改写或删除运行目录的用户脚本资源；旧脚本不再由客户端加载。

## 3. 保留内容

- 原生 C++ SKL 报文解析、动作队列、角色更新、动作推进及原生技能表现；封包格式和字段消费顺序保持。
- 原生 Yobi 链、标死和物理释放流程。撤除 Lua 接管后直接调用现有原生正文，不改造对象引用模型。
- E02～E09 原生特效配置与图号替换，以及仍被这些功能使用的 techId／skillId 元数据；不整体删除 BattleSkillMeta。
- 公共 Lua 支持、现有 UI／战斗 Lua 状态、UI Lua 和自动战斗等非 Skill Lua 功能。

## 4. 实施步骤

1. 核对 Skill Lua 的声明、调用、元数据消费者及源码列表，确定移除集合。
2. 撤除 `battle_message.cpp`、`battle_actor.cpp`、`battle_base.cpp`、`base.cpp`、`battle_skill_effect.cpp` 中的 E10 接管，接回已有原生调用链。
3. 删除 Skill Lua 实现、专用状态和加载入口，清理失去消费者的声明、字段与源码列表项；不扩大到其他 Lua 模块。
4. 集中静态核对差异，同步本 SPEC 和唯一进度，精确暂存提交。

## 5. 静态验收

- 活跃源码中不再存在 Skill.register／PacketReader 的实现与注册、Skill Lua 专用 VM／脚本加载、私有 runtime 或 E10 接管调用；历史文档不计残留。
- SKL 解析、角色更新、动作推进、标死和释放的调用链均指向现有原生实现，没有空实现、固定成功或重复执行。
- 原生特效所需技能元数据的生产、复制、消费和清理闭合；其他 Lua 功能的初始化与关闭接线保持。
- 未引入薄引用、替代技能运行时、自定义技能新框架或外部依赖。
- 单线程，仅进行必要静态检查；不构建、不测试、不运行、不检查产物、不计算摘要。静态结论不等同于编译、链接或运行验收。

本 SPEC 已覆盖 EFM01 的 E10 保留与接管约定；历史迁移及验收记录保留，不作为本次移除的验证结果。

## 6. 实施与静态核对结果

- 删除 `battle_lua_skill.cpp` 全部 2961 行及本子项目 CMake 源码列表项，移除 Skill／PacketReader、独立 Lua 状态、专用脚本加载和私有 runtime；删除相应头文件声明及三处失去用途的包含。
- SKL 入口直接调用 `ProcessBattleMSkillHeader`；`UpdateBattleActor_404DB0`、`AdvanceBattleCastState_439340`、`DeleteYobiAction_404D10` 直接承载既有原生正文。三份正文与修改前逐字比对一致；`MarkDeath` 恢复直接置死亡标志的既有路径。
- `BattleSkillMeta` 保留两个 S32 字段 techId／skillId，移除 E10 专用四字段；原生绑定条件保留。已核对归一化前捕获、主角色绑定、11处局部复制、完整载荷复制、重置及图号／日志消费；分配按 `sizeof(BattleActorPayload)`，元数据仍位于载荷末尾。
- 活跃源码、公开头及本子项目源码列表中，旧 Skill Lua 符号、临时原生包装名及加载入口无残留。`YobiActionFlag_PendingBatchFF` 属于原生绘制标志，与已删除的 `PendingBatch` 类型无关，保持原样。
- 公共 Lua、自动战斗、原生技能解析、特效及局部复制实现无源码差异；原生特效初始化／关闭仅移除专用 Skill Lua 调用。Git 差异检查和变更文件严格 UTF-8 解码通过。
- 验证限于静态源码与差异，不代表编译、链接或客户端运行通过；未构建、测试、运行、检查产物或计算摘要。
