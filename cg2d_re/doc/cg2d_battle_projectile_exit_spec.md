# 投射物阶段17退场与延迟回收边界 SPEC（BPEX01）

2026-10-10；承接[BPHIT01阶段16](cg2d_battle_projectile_hit_spec.md#battle-projectile-hit-source-state)及[BPR01投影](cg2d_battle_projection_spec.md#battle-projection-source-state)，补正常退场依赖。现正文正确保持，零新实现I；完整owner寿命不在本步关闭。

<a id="battle-projectile-exit-baseline"></a>
## 完整原根与正常合同

原F:000093B0 / 4093B0–409409，连续30条89B、一页、两直接CALL（405EA0/404D00）、四条件跳转、一RET，零间接调用。[完整指令/字节、唯一case17、原三F32常量及公共尾窗口](cg2d_battle_projectile_exit_evidence.json)可追。原名称sub_4093B0不作为业务语义证据；阶段17来自原404DB0实际调用点404ECF及当前完整分派。

原4093B6先调用移动405EA0，随后按X下界/X上界/Y下界/Y上界分别读取移动后的F32坐标；-200、840、680来自54D478/54D474/54D470实读字节。正常有序数值下，X在[-200,840]且Y在[-200,680]时不标死，等于边界仍保留；任一超界只MarkDeath，不播放、推进效果队列、摘链、释放或断开父子引用。现两次X/两次Y短路读取与分支正确，不合并成一次坐标快照，不添加空指针或新的边界保护。

原唯一调用点：404ECE push / 404ECF CALL / 404ED4 JMP，共三条11B，转40563F既有公共尾。返回在计数分支由EAX=ECX覆盖，另一分支在坐标转换或rider读取前覆盖，无阶段17返回消费；现void接口保持，不模拟x86残留寄存器。公共尾仍执行已证计数、屏幕坐标和附件同步，不把本叶MarkDeath误解为404DB0整体立即早返。公共尾窗口只核该返回消费，与BTS01旧完整分派合同衔接，不重验全部257槽。

<a id="battle-projectile-exit-lifetime"></a>
## 本轮可证的回收阶段及未闭条件

既有[YAS01](cg2d_yobi_allocation_sampling_spec.md#yobi-allocation-sampling-source-state)按U8 priority稳定插入、同值FIFO。BPC01/BPRH01三投射创建器与418DF0投影均NewYobiAction(2,...)，当前父和新投影在创建时有明确先后。两者flags含40000，调度使用同一四相门禁；只沿已证创建源，不外推全体Yobi父子。

既有[YLC01](cg2d_yobi_cleanup_sampling_spec.md#yobi-cleanup-sampling-source-state)在每节点入口先判断deathFlag；活节点callback返回后只重取next，不返回同节点死亡分支。因此4093B0在父callback内标死不会由这一层当场释放父。若后继投影的callback在父最终释放前实际执行，它能沿BPR01读父deathFlag==1并将自身标死；其本轮仍播放/发布坐标，不能插入即刻删除或早返。

当后续调度入口两对象已经标死，死亡分支只延迟或摘链/删除，不再调用各自callback。Delete404D10先释放非空payload，再释放action，不释放借用父或递归销毁附件。这个阶段顺序不证明全部父/投影寿命：尚须核所有标死来源、门禁/链与引用变更、重生/重入、清场/退出、全部callback与资源owner。不能据FIFO或相同标志推出投影一定在父释放前完成标死，也不能将条件性源结论记完整owner通过；本步不新增寿命保护/owner或改变原回收时机。

<a id="battle-projectile-exit-decision"></a>
## 实施决定

现4093B0正文、签名、分发及所有依赖保持；只补一个CPP和一个公开头的合同注释。新增一独立语义/F adapted与BATTLE已知成员，枚举false；仅一限定S、零新I。S范围是完整本叶/三常量/唯一case17与已有调度阶段对应，不包括全部父/资源/最终回收验收。U08/U09/R01/R04/V04只补来源，全部门禁/六closure/总SPEC§10保留。

标准数学/F32合同沿ACP01及MSVC许可，RPLAN.24不专项异常/回绕/故障模拟或等价，不宣原x87状态/生成ABI与数值运行已验。WAV-only/原格式明文保持，CRT排序业务比较关系继续既有待决项；不构建、测试、运行、查产物、计算摘要或引依赖。

<a id="battle-projectile-exit-acceptance"></a>
## 静态范围

核原库存/连续根/字节、两callee/四跳一RET、三原F32常量与当前字面量、三条case17和两公共尾返回覆盖分支；核本叶两次X/Y短路序及现全正文/签名正确保持。调度/分配/释放、三creator和投影/分发及全部其它源码/头不改，只两注释精确差异。旧语义/处置/软件证据前缀、一新身份/F/成员与限定S及五任务精确增量，全部政策/分母/完整门禁保持，UTF-8无BOM/LF、静态汇总/-Check一致；正常结果、全部父/callee/资源/SDK与完整owner/画面运行仍待。

<a id="battle-projectile-exit-source-state"></a>
## 当前状态

阶段16结束进入阶段17→移出原矩形→MarkDeath→后续调度摘链/释放的具体提供者已明确；投影退出沿BPR01父标记读取，完整寿命仍开放。命名前态cg2d-re-battle-projectile-exit-history-20261010，提交后引用cg2d-re-battle-projectile-exit-20261010。整体SPEC未完成。

### 本步实际限定结果

必要静态、汇总与-Check通过：完整30条89B/一页、两call/四跳一RET、三F32原值/字节和唯一case17三条11B及公共尾返回覆盖有据；全部现正文/签名/分发及所有其它源保持，仅两个合同注释。调度/分配/删除基线复用，四创建来源和phase16→17有具体源；寿命阶段仍严格条件性，完整父子/资源owner未验。首轮检查脚本误用与FIFO <= 条件矛盾的否定断言，已删除无依据断言并按原条件重新核对，原因保在证据，未改业务或验收。语义2038→2039（1991F＋48X）、合同2100→2101/3680（57.09%仅合同关联率）、F1463→1464、E1352→1353仅一限定S/零新I，BATTLE277→278且枚举false。导入16/218、X3060/3061余1、184叶完整验收0、60任务关闭11及六closure保；十一自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-battle-projectile-exit-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。全部标死/门禁/链与引用变更、重生重入/清场退出/全父-callee与资源、SDK数值ABI/最终借用回收及画面运行仍未验，整体SPEC未完成。
