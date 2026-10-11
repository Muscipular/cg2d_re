# 战斗退出消息扫描与CHT/BST消费 SPEC（BEMS01）

2026-10-10；衔接[BMF01场景结束](cg2d_battle_event_finish_scene_lifecycle_sampling_spec.md#battle-event-finish-scene-lifecycle-sampling-source-state)与[BAEL01追加链](cg2d_battle_append_effect_lifecycle_spec.md#battle-append-effect-source-state)。将既有816号局部头查找记录扩至完整原根/现源合同，正文正确保持、零新实现I。

2026-10-11逐项验收补充：[BMSA02](cg2d_battle_m_skill_item_acceptance.md#battle-m-skill-item-acceptance-exit-counter)确认消息种类索引原以W32 EDI计数，当前size_t扩为64位；已恢复32位承载及同型固定25项比较。上文“正文正确保持”是当时的有界结论，此处明确修正该遗漏；原CHT/BST字段、取样、继续扫描和返回合同保持，完整owner/资源/运行未关闭。

<a id="battle-exit-message-baseline"></a>
## 完整原基线

原F:0003BAA0 / 43BAA0–43BC0B连续107条363B、两页、13直接CALL/七目标、一RET、零间接调用。[完整原指令/字节、两个实际CALL邻窗与原记录前态](cg2d_battle_exit_message_scan_evidence.json)可追。原两个CALL43D209/43D330均在43CBB0：状态9与7分别在UpdateAll之后扫描，只有状态7随后清追加效果链；父完整合同沿BMF01/BAEL01，不重复取证或计全场景/退出owner已验。

入口先读取gBattleSceneAction5ABA94，取其payload一次并在整个循环持有；原栈104h对应4B header和256B message，两当前数组未预清。每轮GetBattleMHeader_439070严格返回1才结束，原EAX=1沿尾返回，当前S32返回1保；其余返回按25项3B头/当前4B字符串逐项StrStr查找，首命中后仅14(CHT)/20(BST)进入处理，未命中或其他种类继续扫描。不改成所有M指令分派、不同步两个游标或增加保护。

## 两消息的数据与取样

两种命中先从共享gBattleMScanOffset5A8A3C取得S32游标并写入口固定payload的packetOffset64。随后每次TakeInt/TakeStr参数都fresh读取gBattleSceneAction5ABA94；固定payload和后续对象指针不能统一缓存或改为每轮fresh payload，原非拥有关系及未知生命周期保持。

CHT依次TakeInt位置→GetBattlePosition（结果丢弃）→TakeInt颜色→TakeInt字体→TakeInt写固定payload.messageValueA4→TakeStr容量256。颜色/字体保存局部跨字符串与音效消费，message原字节、容量和NUL/续读规则沿既有M取字符串合同，不转码/补零/重排字段。字符串后读554644非零才调用音效74/声像X320，真实返回不消费；原另压240不被52C450本体读取，沿既有LegacyAudio.hpp两参数合同保持。随后Sub_44B050(message,color,font)写真实聊天/日志并继续扫描，不新增代替UI或日志缓存。

BST先TakeInt位置→GetBattlePosition保存S32结果，再TakeInt seconds，随后fresh读取gBattleCurrentPosition5ABCA4比较。仅匹配当前玩家才按原内联公式timerRemaining0C-1000*seconds，先比较计算结果为负、发布结果、负时再发布0；当前既有ConsumeBattlePlayerTimer执行相同正常S32合同。该负值置零是原业务逻辑，保持；不模拟溢出/回绕或增加秒数门槛。位置不匹配不更新timer，之后继续扫描，不转换为绝对时钟或扩大场景计时范围。

<a id="battle-exit-message-decision"></a>
## 本步决定

1. Sub_43BAA0及所有原M读取/时间消费/聊天音效/场景正文、公开签名和数据类型保持，只CPP与公开头各一合同注释。StrStr当前已沿MC01直接MSVC、25项头及原匹配语义保持；不重做CRT/改编码/协议/资源、重建MIDI或WAV。
2. 既有816号语义保原身份/机器字段/查找连接/两入向组和旧证据，追加完整根/当前提供者及本步说明；完整前态固定于本证据/原索引及命名前态，不把旧局部证据伪称当时完整。新增一adapted F及BATTLE已知成员，枚举false；语义/关联总数不增，限定S仅本根，零新I。
3. U08/U09/R01/R04/V04只补来源，完整门禁/六closure/总SPEC§10保持。音效/聊天及M字段/主计时槽来源有具体提供者，不代完整资源/SDK、所有父-callee/输入数据、scene借用与owner回收或声音/画面验收；无构建、测试、运行、算法样例、产物、摘要或外部依赖。
4. 跨根CRT与数值差异沿已批MC01/ACP01；RPLAN.24不专项异常/故障等价。原格式明文、WAV-only及CRT排序业务比较待决项继续，不以静态扫描完成外推整体SPEC。

<a id="battle-exit-message-acceptance"></a>
## 静态范围

核原库存/完整连续根/字节、真实两CALL、25项既有头关系及14/20分支、固定payload/fresh每次对象、六次TakeInt/两GetPosition/一次TakeStr、CHT字段/音效/聊天及BST内联计时和原负值发布。核现整正文/签名、三个M读取接口/共享timer/聊天与两参音效声明、场景7/9源保持；仅两注释差异。旧816号完整前态/原机器身份、旧语义/软件证据与一F/一成员/限定S增量及五任务、全部政策/分母/门禁保，UTF-8无BOM/LF与静态汇总/-Check一致。原三B/字段和正常计时不代所有数据生产/生命周期或实际运行。

<a id="battle-exit-message-source-state"></a>
## 当前状态

完整退出扫描已有实际CHT文本/日志和BST当前玩家计时消费，源保持。命名前态cg2d-re-battle-exit-message-scan-history-20261010，提交后引用cg2d-re-battle-exit-message-scan-20261010。全数据/资源/父-callee/scene借用及最终owner/SDK与声音画面运行仍待，整体SPEC未完成。

### 本步实际限定结果

必要静态、汇总与-Check通过：完整107条363B/两页、13call七目标/13跳一RET、两个真实调用及原25头/14和20处理、固定payload/fresh取包对象、六Int两Pos一String与CHT/共享timer正常源有据。整CPP/公开头仅两合同注释，全部正文/签名/原表和M读取/计时/聊天音效及场景/其它源逐字保持；816号旧完整前态与原身份/机器字段/查找连接保。语义2042（1994F＋48X）、合同2104/3680（57.17%仅合同关联率）不增，F1467→1468、E1355→1356仅一限定S/零新I，BATTLE281→282且枚举false。导入16/218、X3060/3061余1、184叶完整验收0、60任务关闭11及六closure保持；十一自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-battle-exit-message-scan-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。全部M数据/所有父-callee/scene借用与最终owner、SDK声音画面/正常寿命和实际运行仍未验，整体SPEC未完成。
