# 投射物附属投影创建与更新 SPEC（BPR01）

2026-10-10；衔接[BPC01](cg2d_battle_projectile_creators_spec.md#projectile-creators-source-state)。核定已有创建器和回调的完整根、共享父引用及本轮标死后的处理。现正文正确保持，零新实现I；最终对象回收仍未闭。

<a id="battle-projection-baseline"></a>
## 完整原基线

| 原身份 / 现提供者 | 完整根 | 原关系 |
|---|---|---|
| F:00018D80 / UpdateBattleProjection_418D80 | 418D80–418DE3，32条99B | 418E14将回调地址写入新对象+08，非直接CALL |
| F:00018DF0 / NewBattleProjection_418DF0 | 418DF0–418F0E，81条286B | 三实际CALL：418CF0 / 4192FF / 4195BA，父分别418B70 / 419180 / 419390 |

[完整原指令/字节和真实入向](cg2d_battle_projection_evidence.json)保存两连续根113条385B、三分页、14直接CALL/六目标、七跳三RET，零间接调用。入向只核当前原数据库xref与三个CALL后邻窗，不代表未知间接alias或所有父/callee闭包已验。前两父沿BPC01既有完整根，第三父本步只核调用后返回消费，未重验其完整生产。

## 已有源码合同

创建器NewYobiAction(2,sizeof(BattleActorPayload))失败空返；成功先持有flags|40000，再发布更新callback及flags。原characterAction=0、framePlayingRate=10、父U8 displayPriority取样、非拥有父T*发布、层发布、payload重取、S32图号及F32坐标发布顺序保持。不按原13E8裁新载荷，不预清字段、不新增父空检查或额外owner。

targetX先截S32并写payload x00，再将targetY截S32；目标Y/X由这些整数转F32后交4060D0，原模式0、偏移0保持。转向后重新取图号，再写F32速度10/W16 field270=0；图号110016或111055时两W8写4。播放之后重新取图号，仍为这两图时连续移动两次，不能把前后图号读取合并。最后X转换、取Y、发布X、转换并发布Y，字段宽度与阶段保持。

更新器先取父T*，写本对象F32速度10，再取该父payload的S32 valueFD0。只有valueFD0==0才移动；这个门禁不影响其余处理。移动后从共享+10重新取父，deathFlag严格等于1才MarkDeath；标死后再次从+10取父U8 displayPriority并发布，随后PlayFrame(action,0)和屏幕X/Y转换发布仍继续。不能缓存首个父贯穿回调、增加早返或把非零标死值均当1。

MarkDeath只沿既有标死服务调用。本组原根未清父引用、释放父对象或自行移除投影；非拥有引用不是寿命证明。本步不能据标死后继续读取宣称调度/释放顺序安全，最终回收、全部creator/cleanup及SDK正常结果仍须U09和总SPEC§10验收。

<a id="battle-projection-decision"></a>
## 本步决定

1. 两现完整正文、公开签名和所有调用保持；仅补两个CPP合同注释及一个公开头注释，关联本SPEC与具名原返回适配，不制造修复。
2. 原创建成功EAX=新对象、失败EAX=0；三个实际caller中前两立即以自身图号覆盖EAX，第三先读自身X再由ftol覆盖EAX，均不消费创建器返回。当前void接口保留；不因此删除真实创建/失败处理、改分配域或声称原机器ABI等价。未知原间接引用仍待。
3. 原x86对象/载荷地址只在已证指针槽拓宽为原生T*；S32/U32/U16/U8/F32字段不扩宽。标准数学/整数转换沿ACP01与MSVC许可，不新增溢出保护、补值、钳制或故障样例；RPLAN.24与完整正常失败验收保持。
4. 补两独立语义身份、adapted F处置和BATTLE两个已知成员，枚举false；仅一限定S，零新I。U08/U09/R01/R04/V04只追加本步来源，任务/所有门禁与六closure保留。
5. CRT已确认库职责直接MSVC，剩余七组/八排序调用的业务比较关系继续既有待决项；本组不依赖该选择，也不将未回复视作允许修改比较器。WAV-only/原格式明文保持，不构建、测试、运行、查产物、计算摘要或引外部依赖。

<a id="battle-projection-acceptance"></a>
## 静态核对范围

核原库存身份/区间、两连续根完整指令与字节、回调地址生产与三实际CALL后返回消费；核全部现正文/签名保持、共享父各阶段重取、原FD0/标死门禁、两次图号取样、目标截断/转向与X/Y发布。两个CPP和一个头注释精确差集；两新语义/F/成员与限定S及五任务仅具名增加，全部原记录/政策/分母/完整门禁保持。显式UTF-8无BOM/LF，静态汇总/-Check一致；不代生成ABI、完整资源/所有父-callee、SDK、owner借用与最终回收或画面/运行验收。

<a id="battle-projection-source-state"></a>
## 当前状态

现创建→真实更新callback发布、三个调用来源及更新阶段有具体源提供者；完整事件与最终回收仍开放。命名前态cg2d-re-battle-projection-history-20261010，提交后引用cg2d-re-battle-projection-20261010。整体SPEC未完成。

### 本步实际限定结果

必要静态、汇总与-Check通过；首次汇总因新增差异对象缺description/decision_ref失败，修正本步对象结构后通过，失败原因保留在证据中。两完整根113条385B/三页、14call/六目标/七跳三RET、一个回调地址生产及三个CALL返回消费有据；两个CPP注释/一个公开头注释精确差异，全部正文/签名与其它源保持。语义2035→2037（1989F＋48X）、合同2097→2099/3680（57.04%仅合同关联率）、F1460→1462、软件证据1347→1348仅一限定S/零新I，BATTLE274→276且枚举false。原分母/导入16/218/X3060/3061余1、184叶完整验收0、60任务关闭11和六closure保持；十一自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-battle-projection-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。未知间接alias、全creator/cleanup与最终父/投影回收、资源/SDK数值ABI/画面运行仍待，整体SPEC未完成。
