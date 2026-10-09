# 投射创建器父Y与投影表取样补充 SPEC（DPCS01）

2026-10-10；沿[BPRH01](cg2d_battle_range_hit_spec.md#battle-range-hit-source-state)新发现的取样阶段，补充[BPC01](cg2d_battle_projectile_creators_spec.md#projectile-creators-source-state)两个已实现创建器。采用既有原根证据，不重复取证/计新函数或原身份。

<a id="projectile-creator-cache-baseline"></a>
## 已有原基线与新增关注点

原F:00018B70 / NewBattleSpiracleShot_418B70及F:00019180 / NewBattleBlastWave_419180的完整161条526B/161条525B、原xrefs/真实CALL、常量与身份沿[BPC01证据](cg2d_battle_projectile_creators_evidence.json)保存。本步只核新差异所在四个指令窗及整CPP源码差集，不把旧322条1051B重新计取证或重验完整父/callee闭包。

SpiracleShot：418BD6先读父F32 Y，418BD9读新对象payload，418BDC发布Y。投影阶段418C8B读取effect position，418C8E从103CFA4取DX表指针，418C94再查询gBattleEntry目标；418CA2与418CCC两次低S16高度都用该同一表指针。现初始化先取payload后读父Y，投影先查目标且两getter分别fresh读取DX表，未明确原取样阶段。

BlastWave：4191DF读取父F32 Y，4191E3读取新对象payload，4191E6发布Y。4192DE读取effect position，4192E0读取action图号，4192E6才查询gBattleEntry目标；现初始化同样先取payload，投影先查目标再读图号。此投影没有DX高度读取，不增加借表服务。

<a id="projectile-creator-cache-decision"></a>
## 实施决定

1. 两创建器各一初始化片段：发布父X后持有initialY，再读取payload，随后发布initialY；原字段宽度和其余初始化/flags/callback顺序保持。
2. SpiracleShot投影片段先持有effect position，再调用已有BorrowGraphicDxCacheEntries获取调用期非拥有DX表，然后查询目标。目标与父高度改用已有ReadGraphicDxHeightFromEntries，显式传同一表指针；不跨图形加载/投影创建/播放保存表，不新增owner、缓存、加载、保护或扩大寿命。原目标高度→父图号→目标偏移/Y/X、父高度→action X→父偏移/action Y的全部样本和double*0.5F公式保持。
3. BlastWave投影片段先持有effect position，再持有projectionImage，再查询目标。既有target Y/X→action Y/X局部样本及投影调用保持；不新读尺寸/表。
4. 一个CPP的两个既有体共四片段，零新函数/类型/公开API/源项。BPC01先前效果四字段、辅助flags跨回调持样和坐标样本修正保持；两根各自不同的flags/callback发布序、原分配空返、phase16、未预清辅助输出、图形加载、数学/单圈/图号、特殊两W8、三父payload独立重取、两移动/一播放、X/Y发布及原生返回保持。BPRH01/BPR01/BPHIT01和其它完整业务源/头不改。
5. 两既有语义与F处置只追加本步来源/当前提供者说明，原身份/机器字段/旧阶段对象保留，前态沿固定标签及本证据原索引可读；不新增F/语义/成员或改变分母。仅限定I/S各一，I只四片段；BATTLE成员枚举false、五任务仍open、全部门禁/六closure和总SPEC§10保持。旧BPC01四片段结论是当时范围，不当作本步新增表指针阶段已验。
6. 标准数学/整数转换沿ACP01/MSVC许可，RPLAN.24不专项异常/回绕/故障等价，不宣生成ABI/原x87/CW或实际画面已等价。WAV-only与明文合同保持；CRT排序比较关系继续既有待决项，不构建、测试、运行、查产物、计算摘要或引依赖。

<a id="projectile-creator-cache-acceptance"></a>
## 静态范围

复用完整原根与库存身份，精确核四原指令窗、103CFA4表指针及原两同表高度、effect position→图号→目标查询序；核现借表/getter无加载/保护/状态写入、原S16/F32和原生指针、所有公式及辅助flags保持。以固定前态重建整CPP四片段，所有其它函数/头/API/数学/原失败逐字不改；两既有记录/处置原机器字段和完整前态保留，旧软件证据前缀及限定I/S增量、所有成员/分母/政策/门禁/五任务保持。显式UTF-8无BOM/LF、静态汇总/-Check一致；不代完整父/callee/资源、正常创建失败、DX表和对象最终owner回收、SDK数值ABI及画面/运行。

<a id="projectile-creator-cache-source-state"></a>
## 当前状态

两既有创建器各两处取样按本合同补齐，与RangeHit/BPR01/BPHIT01源码链衔接；所有依赖/资源生产与最终生命周期继续开放。命名前态cg2d-re-projectile-creator-cache-sampling-history-20261010，提交后引用cg2d-re-projectile-creator-cache-sampling-20261010。整体SPEC未完成。

### 本步实际限定结果

必要静态、汇总与-Check通过：BPC01原根/库存复用及四指令窗、同表两高度/103CFA4、图号先于目标查询有据，零新机器取证计数。整CPP仅两体四组/五精确替换，辅助flags/原数学/发布/失败、前一RangeHit及所有其它函数/头保持；两旧语义/处置只追加来源，完整前态和原机器字段/阶段对象保留。语义2038（1990F＋48X）、合同2100/3680（57.07%仅合同关联率）、F1463及BATTLE277/枚举false保持，软件证据1350→1352只限定I/S各一，无新函数/身份/成员/分母。导入16/218、X3060/3061余1、184叶完整验收0、60任务关闭11及六closure不动；十一自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-projectile-creator-cache-sampling-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。全数据/资源/全部父-callee、DX及对象最终owner回收和SDK数值ABI/声音画面运行仍待，整体SPEC未完成。
