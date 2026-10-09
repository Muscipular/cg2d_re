# RangeHit创建、效果与投影生产 SPEC（BPRH01）

2026-10-10；补齐[BPR01](cg2d_battle_projection_spec.md#battle-projection-source-state)的第三个真实创建来源，承接[BPHIT01](cg2d_battle_projectile_hit_spec.md#battle-projectile-hit-source-state)的阶段16正常源。本步修复三个既有取样片段，普通旧数值未证明错误，不宣已观察运行故障。

<a id="battle-range-hit-baseline"></a>
## 完整原基线与实际消费

原F:00019390 / Battle::RangeHit::newEffect / 当前NewBattleRangeHit_419390，完整419390–4195E2连续根176条594B、三分页、13直接CALL/十目标/四跳两RET、零间接调用。[完整字节/指令、xref、五CALL邻窗及等级分支](cg2d_battle_range_hit_evidence.json)保留原身份/区间，不猜类型名称对应的中文技能。

五真实CALL：40702D、407079、4070A2来自406E10；408277来自408170；4083AA来自4082F0。406E10中技能258的等级<9和>=9有两个原CALL/循环，当前共用一个循环表达式；低等级选择29105，另一等级每轮仍读等级并在29106/104217间选择。非258另一个循环、408170与4082F0各一个现表达式，合四表达式覆盖原五点，不按静态数量差新增调用。五邻窗均未消费创建返回；本步仍保持原生新对象/空返回，不删除真实分配失败。全部三父完整状态/重入与callee闭包未在本步重验。

原创建NewYobiAction(2,13E8)成功后flags|60000、callback404DB0、速率10、层203及父坐标复制，转向模式1后两次图形加载。父DX低S16宽乘0.5落F32后移动；角度+11.25再乘0.044444444444444446截S32，只各一次加/减16，最后加输入图号+16。三浮点常量沿BPC01既有实读证据，不重复计算摘要或运行数学样例。之后效果/辅助生产、phase16/W16清零、一次播放、四次父payload重取、投影创建和X/Y发布及返回已有。

## 三处真实取样差异

1. 4193E0先取父F32 Y，4193E3再取新对象payload，4193E6才发布Y。现payload读取在父Y之前；显式持有initialY后取payload，再发布同一值。
2. 419499..4194A9按效果+0C(S32 position4)→+08(S32 position3)→+04(U32 flags)→+00(S32 position)读取，再调用AppendBattleEffect。现*effect整体复制没有明确四次源读取序；按原顺序持样，再构造既有16B聚合，目标字段顺序和API不改。Append之后fresh flags及跨TakeBattleAuxEffects持有该flags仍原样。
3. 41954C..4195BA先取效果position，再取action图号，随后从gBattleEntry取目标指针并减16形成投影图号；41955F读取DX表指针槽103CFA4，目标与父高度均借同一表。目标低S16高度后先采父graphicIndex，再目标S32 graphicOffsetY/F32 Y/X；父低S16高度后先取action F32 X，再父S32偏移Y和action F32 Y。现先取目标指针、两次fresh表getter和嵌套参数没有这些持样阶段。使用既有BorrowGraphicDxCacheEntries/ReadGraphicDxHeightFromEntries持有本调用期同一非拥有表，显式局部样本，再按原double*0.5F+偏移+Y公式落F32并传投影。

<a id="battle-range-hit-decision"></a>
## 实施决定

按工程委托先记本SPEC，采用上述三片段：只改一个CPP的既有创建体，不新建函数/类型/公开API或源项。原生action/parent/target/payload与DX表都是实际非拥有指针，指针拓宽按原x64承载；借表不新增owner、加载、保护、默认值或扩大缓存寿命，初始半宽读取阶段不与播放后借表阶段合并。

其余整CPP、所有头、三个父更新体、BPC01/BPR01/BPHIT01及所有callee正文保持。图形加载顺序、原分配空返、flags/callback发布序、F32/S16/S32/U32/U16/U8宽度、单圈修正和图号表达式、未预清的四辅助输出及flags持样、阶段16/播放、四次父payload独立重取、X转换→读Y→发布X→转换/发布Y与原生返回保持；不复刻旧13E8字节域或为数值异常增加钳制/回绕/故障模拟。

新增一独立语义/F adapted与BATTLE已知成员，枚举false；限定I仅三片段、S仅本组，零新函数。U08/U09/R01/R04/V04只追加来源，完整门禁、六closure和总SPEC§10保持。标准数学/整数转换沿ACP01/MSVC许可，不宣原x87/CW/ABI等价；CRT排序业务比较关系仍待既有选择，WAV-only和原格式明文保持。不构建、测试、运行、查产物、计算摘要或引依赖。

<a id="battle-range-hit-acceptance"></a>
## 静态核对范围

核原库存身份、完整连续根/字节、三原取样段与表指针槽、五实际CALL/三个父及共享等级表达式；核现S16 getter和借表服务无加载/副作用/保护、原16B效果布局及API。以固定前态精确重建整CPP三片段差集，所有其它正文/头/数学表达式/失败与发布序保持；旧语义/软件证据前缀、一个新身份/F/成员、限定I/S和五任务具名增量及全部分母/政策/门禁保持。显式UTF-8无BOM/LF、静态汇总/-Check一致；不代完整所有caller-callee、资源/SDK数值ABI、创建失败/借用和最终回收及画面/运行验收。

<a id="battle-range-hit-source-state"></a>
## 当前状态

当前第三来源创建→阶段16→原404DB0/4072E0正常命中及418DF0/418D80投影有具体提供者，三个取样片段按合同实施。正常失败、全数据生产/资源、所有父/callee及最终owner生命周期仍待，不以此源码链关闭整体战斗。命名前态cg2d-re-battle-range-hit-history-20261010，提交后引用cg2d-re-battle-range-hit-20261010。整体SPEC未完成。

### 本步实际限定结果

必要静态、汇总与-Check通过：原完整176条594B/三页、13call/十目标/四跳两RET、五实际CALL/三父和两个等级点的共享现循环、DX指针槽103CFA4及S16 getter/借表服务有据。整CPP精确仅本体三片段，所有其它函数/头、数学表达式/单圈/图号/分配空返/辅助flags/四父payload重取及发布保持。语义2037→2038（1990F＋48X）、合同2099→2100/3680（57.07%仅合同关联率）、F1462→1463、软件证据1348→1350仅限定I/S各一，BATTLE276→277且枚举false。原分母/导入16/218/X3060/3061余1、184叶完整验收0、60任务关闭11与六closure保持；十份自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-battle-range-hit-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。全数据/资源/全部父-callee/创建失败及借用与最终回收、SDK数值ABI/声音画面运行仍待，整体SPEC未完成。
