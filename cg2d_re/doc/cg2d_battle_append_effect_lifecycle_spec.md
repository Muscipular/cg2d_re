# 战斗追加效果链与资格 SPEC（BAEL01、BAES01）

> BAES01补充：[完整技能资格合同](#battle-append-skill-source-state)核43BD00整根、54项集合及唯一生产门禁；现正文正确保持，只补注释/一项限定S。下文BAEL01的“完整资格仍待”保留为该阶段结论，完整业务数据、SDK与owner运行仍待。

2026-10-10；沿BPEX01后的外部清理来源继续核资源owner，补独立追加效果链。三个既有完整根纳管，仅修43BED0释放参数持样一片段，不扩展Yobi父/投影生命周期。

<a id="battle-append-effect-baseline"></a>
## 完整原根与数据归属

| 原身份 / 当前提供者 | 完整根 | 实际入向 |
|---|---|---|
| F:0003BE00 / AddBattleAppendEffect_43BE00 | 43BE00–43BE96，56条150B | 三CALL，同一父427470 |
| F:0003BEA0 / FindBattleAppendEffectTechnique_43BEA0 | 43BEA0–43BEC9，16条41B | 19CALL/13父 |
| F:0003BED0 / ClearBattleAppendEffects_43BED0 | 43BED0–43BEFE，15条46B | 43D335，父43CBB0状态7 |

[完整原指令/字节、原入向、链头六引用与当前源差集](cg2d_battle_append_effect_lifecycle_evidence.json)保存三连续根87条237B/三页，四直接CALL/三目标（资格判定、malloc、free）、11跳六RET，零间接调用。23实际入向/15父只定位；生产三个调用后窗口和清理状态7邻窗明确实际消费，不代表15父/所有callee或机器寄存器ABI闭包已验。

5ABA8C六已知直接引用仅来自这三个原根。原节点20B：skill/technique/position各S32，previous在+0C、next在+10；当前私有BattleAppendEffectNode仅两链指针拓宽，使用sizeof原生节点，不误按20B截断。当前链头是同CPP私有static T*，实际直接消费者为生产/查询/清理，不新增公开指针、第二owner或句柄表；原未知间接alias及全局寿命仍待。

## 生产与查询的既有正确合同

43BE00先调用43BD00资格判定，仅消费AL非零；该已有资格源码/原索引证据保持，本步不重验其整根或全部消费者。链空时std::malloc(sizeof节点)，按skill→next空→previous空→position→technique写完后发布链头。非空逐节点先比position，再比skill；命中只更新technique，不重新分配。未命中在尾部malloc，按next空→skill→position→technique→previous→原尾next发布，正常链顺序和原调用参数S32保持。不新增malloc空检查、清零、错误回退或构析。

43BEA0取一次链头后按position→skill短路扫描，命中返回S32 technique，未命中/空链返回-1；只返回值，不把节点借用指针传出。现生产和查询完整正文/接口正确保持。原生产三个caller前两立即覆盖EAX，第三仅推两个既有参数后调用4394A0；当前void生产接口保留，不模拟x86残留寄存器。完整父及其所有返回/机器ABI关系仍待，不从该局部窗口推全协议或事件闭包完成。

<a id="battle-append-effect-cleanup"></a>
## 一处清理差异与释放顺序

原43BEDB先将当前ESI保存EAX，43BEDD发布链头=EAX，43BEE2取得next到ESI，43BEE5推保存的EAX，43BEE6调用free。当前源在node=next之后以std::free(gBattleAppendEffects5ABA8C)重新读取共享头；原共享头此时仅是发布槽，free参数来自此前持有的当前节点。

修为current=node→发布head=current→node=current->next→std::free(current)，每轮仍先缓存next再释放当前节点；终止后才清全局头为空，不提前发布head=next、清previous/next、递归释放、改循环或借原清理添加Yobi标死。原瞬时head可指向刚释放节点的发布阶段保持，不据其存在推其他owner安全/添加保护。

现两个生产分配点与清理同域std::malloc/std::free，直接MSVC按MC01；库内部策略/错误与旧私有CRT不复刻。当前只释放节点本身，三个整数与两非拥有链指针没有另外的资源析构；正常SDK/重进退出/间接alias及运行仍须完整验收。

<a id="battle-append-effect-decision"></a>
## 实施决定与场景边界

只一个CPP的Clear既有体一片段，零新函数/类型/公开API/源项；Add/Find、资格判定、三原字段宽度/原生指针布局及全部其它业务/头保持。43D335之后EAX被xor清零用于22项表清理，当前Clear void保。BMF01已有状态7：UpdateAll→43BAA0扫描→43BED0清追加链→清22项对象表；状态9沿其原不同顺序，不追加该清理，不把差异猜为缺功能/泄漏。YobiClearUp对Yobi整链标死，不代此私有heap链清理；父/投影及全部场景资源回收仍开放。

新增三独立语义/F adapted及BATTLE三个已知成员，枚举false；限定I只43BED0的一片段，S覆盖本组完整源，零新函数。U08/U09/R01/R04/V04只补来源，完整门禁/六closure/总SPEC§10保持。RPLAN.24不专项异常/回绕/故障模拟或等价；WAV-only/原格式明文及CRT排序待决项保持，不构建、测试、运行、查产物、计算摘要或引依赖。

<a id="battle-append-effect-acceptance"></a>
## 静态范围

核原库存/三完整连续根与原字节、字段布局/链头六引用、malloc/free及资格AL门禁、命中更新/未命中值、唯一清理实际CALL后返回覆盖和生产三入向邻窗。核当前私有类型/三消费者/两个分配点同释放域，一CPP一片段完整差异及Add/Find/其余源码和所有头、场景7/9保持；原15父与原未知间接alias不计闭包验收。旧语义/处置/软件证据前缀、三新身份/成员、限定I/S与五任务具名增量，全部政策/分母/完整门禁/UTF-8无BOM-LF和静态汇总/-Check保持。仅静态，完整资格/全部调用数据、SDK容量/失败、重进退出与所有owner/实际画面运行仍待。

<a id="battle-append-effect-source-state"></a>
## 当前状态

已确认实际生产→按键值查找→状态7清理的源链，free参数改为原当前节点样本；不会宣称全部战斗结束/父投影/资源owner完成。命名前态cg2d-re-battle-append-effect-cleanup-history-20261010，提交后引用cg2d-re-battle-append-effect-cleanup-20261010。整体SPEC未完成。

### 本步实际限定结果

必要静态、汇总与-Check通过：三完整87条237B/三页、四call/三目标/11跳六RET、六头引用/三根、23实际入向/15父定位、生产三窗口和清理后返回覆盖有据；当前私有三S32/两T*、两malloc/同域free及场景7/9原差异保持。整CPP只清理free current一片段，Add/Find/资格、所有其它源与头/场景逐字不改。语义2039→2042（1994F＋48X）、合同2101→2104/3680（57.17%仅合同关联率）、F1464→1467、E1353→1355仅限定I/S各一，I只清理一F/S三F；BATTLE278→281且枚举false。原分母/导入16/218/X3060/3061余1、184叶完整验收0、60任务关闭11和六closure保持；十份自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-battle-append-effect-cleanup-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。完整资格/所有15父-callee与数据、原未知间接alias、SDK容量失败/重进退出/全部资源owner和画面运行仍未验，整体SPEC未完成。

<a id="battle-append-skill-baseline"></a>
## BAES01：完整资格根与生产消费

F:0003BD00 / IsBattleAppendEffectSkill_43BD00，原连续43BD00–43BD91，43条145B/一页，17跳（含两间接表跳转）、两RET、零CALL。唯一已知入向43BE08是43BE00内实际直接CALL；已有BAEL01整根保留其参数取样、AL检查和拒绝后早返，不把直接引用枚举外推为全部间接alias。

两组W32目标表43BD94/43BDD4都指向43BD8E（AL=1）和43BD48（AL=0）；BYTE索引分别43BD9C的53项和43BDDC的34项。原CAC07完整X:0003BD91-0003BE00及其165号记录复用，本次只补完整F及四个表的103B当前只读核对，不重复增加X身份/分类计数，也不把对齐字节当函数尾体。

| 原判定段 | 准入技能 | 当前表达 |
|---|---|---|
| 43BD04–43BD10，先按S32分流，命中高端两值 | 2014–2015 | 闭区间比较 |
| 43BD12–43BD1A，S32比较 | 64–66 | 闭区间比较 |
| 43BD1C–43BD2C，skill-8的U32索引守卫与53项BYTE表 | 8、11、55–60 | 两个case及闭区间 |
| 43BD33–43BD46，单值与S32比较 | 2006、2010–2012 | 一个case及闭区间 |
| 43BD4B–43BD57，S32比较 | 2058–2067 | 闭区间比较 |
| 43BD59–43BD6B，skill-2019的U32索引守卫与34项BYTE表 | 2019、2027、2050–2052 | 两个case及闭区间 |
| 43BD72–43BD8C，S32比较 | 2089–2095、3002–3016 | 两个闭区间 |

合计54个值，当前S32入参/uint8_t返回只产生0或1。原只写AL，不保证EAX高位；43BE10只test al，当前接口与真实消费正确，不扩大为原全EAX返回合同。函数不读取追加链、对象/技能资料，不分配、调用、写全局或消耗RNG；技能准入来自已证常量集合，不附加名称、职业或有效性检查。

<a id="battle-append-skill-decision"></a>
## BAES01 实施边界

当前switch五单值与八闭区间吻合全部原分支/索引数据，正文和签名保持，仅CPP/公开头各一合同注释；Add/Find/Clear、两个malloc/同域free、场景7/9与所有其它源不改。拒绝时Add直接返回，合格后才读取链头并进入原生产合同；资格返回不能代替生产三调用数据、链节点owner或全部失败/重进退出验收。

1916号既有语义保原身份/机器与CAC07字段、来源和完整前态，只补完整资格合同。新增一项F adapted处置、一项限定S，零新I/函数/成员；FUNC-BATTLE已知成员本来包含本F，数量与枚举false保持。U08/U09/R01/R04/V04只补来源/局部描述，六closure/完整门禁和总SPEC§10保持。不重建CRT内部表跳转或x86寄存器ABI，不做异常/回绕模拟，不改字段/原数据或正常集合，不解除编译、测试、运行、产物、摘要与依赖限制。

<a id="battle-append-skill-acceptance"></a>
## BAES01 静态范围

核库存完整chunk、43条连续指令/145B、一实际调用、17跳两RET零CALL，四表103B与CAC07原完整X记录一致；逐段核输入S32比较、两U32索引守卫及全部表项对应值/返回AL，与现五case/八闭区间作源码数据对照，不执行资格函数或枚举算法样例。消费沿BAEL01已有Add整根，核真实CALL目标/AL门禁和拒绝早返。核整CPP/公开头仅两注释，全部正文/签名及其它源、旧资格语义完整前态与CAC07结构保。核一F/一S和五任务的具名增量，分母/政策/全部门禁、UTF-8无BOM-LF与静态汇总/-Check保持。所有生产数据/间接alias/资源owner、SDK正常结果和运行仍待。

<a id="battle-append-skill-source-state"></a>
## BAES01 当前状态

完整资格与追加链生产门禁已纳管，现正文保持；没有新增实现或把局部静态登记记作全功能完成。前态cg2d-re-battle-append-skill-history-20261010，提交引用cg2d-re-battle-append-skill-20261010。必要静态与汇总检查通过，完整业务和整体SPEC未完成。

### BAES01 实际限定结果

必要静态、汇总与-Check通过：完整43条145B/一页、17跳两RET零CALL、唯一原43BE08实际CALL/AL门禁与四表103B、54项源集合有据；CAC07完整X数据及BAEL01已有Add整根消费复用，未增加X身份/处置。整CPP/公开头仅两注释，全部正文/签名/链资源与场景7/9及其它源保持；旧1916完整前态/机器字段/CAC07结构保。语义2042（1994F＋48X）、合同2104/3680（57.17%仅合同关联率）与BATTLE282/枚举false保持，F1468→1469、E1356→1357只一限定S/零新I、函数与成员。导入16/218、X3060/3061余1、184叶完整验收0、60任务关闭11及六closure保；十一自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-battle-append-skill-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。全部生产数据/间接alias、SDK容量失败/重进退出与所有资源owner/画面运行仍未验，整体SPEC未完成。
