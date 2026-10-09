# 动作16投射物创建与载荷生产 SPEC（BPC01）

> 2026-10-10当前取样补充：[DPCS01](cg2d_battle_projectile_creator_cache_sampling_spec.md#projectile-creator-cache-source-state)在本阶段四处修正之外，另补两个创建器父Y/payload、SpiracleShot同一DX表/目标查询及BlastWave图号/目标查询四组；旧阶段原证据及限定结果保留，当前源须结合该补充合同，完整资源/owner/SDK与运行仍待。

2026-10-10；衔接[BPHIT01动作16](cg2d_battle_projectile_hit_spec.md#battle-projectile-hit-source-state)的真实创建端，修复四处原字段取样顺序。保持普通数值、原类型/阶段和失败路径，不称已观察错误运行。

<a id="projectile-creators-baseline"></a>
## 完整原基线

| 原身份 / 现提供者 | 完整根 | 真正直接调用 |
|---|---|---|
| F:00018B70 / NewBattleSpiracleShot_418B70 | 418B70–418D7E，161条526B | 407183，父4070E0 |
| F:00019180 / NewBattleBlastWave_419180 | 419180–41938D，161条525B | 4072A8，父4071C0 |

[完整原指令/字节、关系与现源差集](cg2d_battle_projectile_creators_evidence.json)保存两根322条1051B/六分页、27直接CALL/十种目标、十跳四RET，零间接调用。原xref还把40716D/407269/40728B记为type17：它们实际是转向各自父内407182/4072A7的JMP，不是调用创建器；原metadata保留、按实际指令另列，不虚增三个调用或改数据库。两个真正CALL的全部父/间接闭包未由本步重验。

两父现分别动作70/249，经既有分发进入；创建NewYobiAction(2,sizeof(BattleActorPayload))失败原样空返，成功沿原flags/callback、F32速度10、U8层203和父坐标写入。阶段16、200项效果/附加记录、图形加载、battlePosition/技能103C/攻击模式FF4/技术1044生产、两次移动/一次播放及屏幕X/Y发布已有；本步不注册新路由、预清未写字段或重复初始化。

## 四处差异与原取样

两个创建器在转向后以*effect整体复制给AppendBattleEffect，C++整体复制没有写明四次源字段读取的序。原418BF7..418C07、419251..419261严格先+0C position4、再+08 position3、再+04 flags、最后+00 position，随后才调用。聚合对象的目标字段顺序仍position/flags/position3/position4，不能为恢复读取序重排原16B布局。

418C9C..418CF0要求先取目标S16图高，随后采parent graphicIndex，再目标S32 graphicOffsetY/F32 offset44、F32 offset40；父S16图高后采action offset40，再parent graphicOffsetY和action offset44，按原0.5高度公式落F32后调用投影。现嵌套表达式把parent graphicIndex放到目标坐标之后，action offset40在参数表达式中无明确读取阶段。

4192E0..4192FF依次取projectionImage、target offset44/offset40、action offset44/offset40；现五共享字段直接出现在投影参数表达式，C++不保证此序。两字段集合和普通取值未证明错误，本步仅恢复明确持样。

<a id="projectile-creators-decision"></a>
## 实施决定

1. 两现正文各一效果记录片段，依position4(S32)→position3(S32)→flags(U32)→position(S32)四次取齐，再用持样值构造既有16B记录交AppendBattleEffect。原调用后effect->flags再次读取、800000门禁及跨TakeBattleAuxEffects持有该flags参数全部保持；不误改为回调后fresh flags或合并这两次取样。
2. 418B70创建器显式持有目标S16高度、parent S32 graphicIndex、目标S32偏移Y/F32坐标Y/X；目标Y仍double(height)*0.5+偏移+Y后落F32。父S16高度后取action X，再parent S32偏移Y、action F32 Y，原同公式落F32；投影只传局部持样，不加读取/owner/保护或改数学实现。
3. 419180创建器显式持有S32图号、target Y/X、action Y/X五个值，依原序取得后传同一NewBattleProjection。parent/target/action均原生非拥有T*，不建影子表、扩大寿命或把失败强制成成功。
4. 其余完整源保持：两根各自不同的flags/callback发布序、原分配空返、单圈16方向修正、S16图高、原三常量0.5/11.25/0.044444444444444446、辅助返回消费/四未预清输出、phase16和0/100/10/203、双图形加载、两特殊图W8写4、三父载荷独立重取、两次移动/一播放、X转换→读Y→发布X→转换/发布Y及原生返回不变。sizeof原生载荷和所有构析/释放域沿既有owner合同，不按旧13E8误裁指针。
5. 一CPP两个既有体四片段，零新函数/类型/公开API/源项。两原F独立纳管、BATTLE补两已知成员，枚举false；仅限定I/S各一，不计两函数新实现或完整创建/退出验收。U08/U09/R01/R04/V04只补来源、仍open，全部门禁/六closure保。
6. 正常原失败/共享取样与生命周期仍完整验收；RPLAN.24不专项异常/回绕/故障等价，不宣原x87/CW/生成ABI和实际波形等价。CRT排序仍待既有选择，WAV-only/明文合同不变，不构建、测试、运行客户端/算法、检查产物、计算摘要或引依赖。

<a id="projectile-creators-acceptance"></a>
## 静态核对

核两完整原根/原字节、四取样段、两个真实CALL与三个非call metadata点、三个原浮点常量；核S16图高现getter与原MOVSX对应。整CPP仅四片段、所有其它函数/头/数据/类型与数学表达式/失败分支保持，效果记录布局和API不改；原F来自库存，旧父/BPHIT记录与软件证据不重写。两语义/两F/两成员/限定I-S及五任务仅具名增量，政策/完整门禁/原分母保；显式UTF-8无BOM/LF、汇总/-Check一致。仅源/接线静态，完整资源/创建失败/借用及回收、所有父/callee/数据生产、SDK/数值ABI和运行仍未验。

<a id="projectile-creators-source-state"></a>
## 当前状态

四原取样段按本合同落位，既有创建→phase16→真实404DB0分发→4072E0正常命中/退场有具体提供者；不把这条限定源码连接当全战斗事件/生命周期完成。命名前态cg2d-re-battle-projectile-creators-history-20261010，提交后引用cg2d-re-battle-projectile-creators-20261010。整体SPEC未完成。

### 本步实际限定结果

必要静态、汇总/-Check通过；一CPP两既有体四片段按原序持样，效果聚合布局/投影数学/原失败/辅助flags持样及所有其它源/头保持。两完整原根322条1051B/六页、27直接call/十目标/十跳四RET、两实际CALL/三个非call metadata点和三常量可追。语义2035（1987F＋48X）、合同2097/3680（56.98%）、F1460/E1347，BATTLE274且枚举false，仅限定I/S各一；184叶完整验收0/60任务关闭11及六closure保持。十一自有文件显式UTF-8无BOM/LF；未构建、测试、运行、检查产物或计算摘要，完整资源/生产/owner回收及SDK/数值ABI和实际声音画面仍未验，整体SPEC未完成。
