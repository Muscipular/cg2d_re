# 阶段12计时与13载荷坐标移动 SPEC（B12P01）

2026-10-10；补阶段12/13完整原根、周期坐标特效生产和实际总分派。只恢复两入口flagsFDC低BYTE取样；RNG消费、已正确计数/速率、播放/转向/移动及坐标写入保持，不重做已有几何或特效创建器。

<a id="battle-coordinate-action-baseline"></a>
## 完整原根与分派

| 原身份 / 当前源 | 连续完整根 | 实际调用与分支 |
|---|---|---|
| F:00006930 / UpdateBattleAction12_406930 | 406930–406A78，94条328B/两页 | 九directCALL/五callee、六跳两RET；404E14唯一case12 CALL |
| F:00006A80 / UpdateBattleAction13_406A80 | 406A80–406B93，82条275B/两页 | 九directCALL/七callee、五跳一RET；404E1F唯一case13 CALL |

合176条603B/四页、18directCALL，无间接调用或本根分派表。case12/13各三条11B，调用后转共尾，不消费本根EAX，现void签名/真实总分派保持。两根入口固定payload后原TEST低BYTE bit2，现明确entryFlags U8取样再判bit2，不扩大为整个flags快照合同或新输入门禁。

## 周期坐标特效与计数

仅入口bit2未置时，fresh valueFF0==0才生产110006坐标特效。先RAND(-10,10)取得Y扰动，再读action.offsetY并相加、发布F32参数；第二次RAND(-10,10)取得X扰动，再读action.offsetX并相加、发布F32参数，之后NewCoordinateEffect(X,Y,110006)弃返。两原RNG点/正常负值和两F32参数顺序保持；不能提前持样全部坐标、颠倒RNG或将两次随机数合成一次。分配失败也继续后续计数/播放，不能按空返回跳过原动作。

创建后fresh读取FF0旧count，保存count+1并发布FF0，仍用保存旧count>=2判断再清FF0=0；现正文正确，保原三步周期，不改成用递增值比较。原异常/溢出不专项模拟或处理，不增加循环/数值保护。

## 阶段12：特殊直接推进与普通计时

bit2置时characterAction=10→PlayFrame(action,1)弃返→载荷Y0C先/X08后各S32转F32→TurnPosition(action,X,Y,0,0F)→value54=6F→AdvanceCast(action)，随后返回。该分支不生产特效、不更新FF0/FCC/播放速率、不Move；不从普通分支补入这些操作。

普通分支完成周期生产/计数后characterAction=1→PlayFrame(action,0)弃返，fresh载荷Y-X转F32→TurnPosition(action,X,Y,1,0F)。接着fresh FCC+1保存frames、发布FCC；仅frames>70时速率30，否则100-frames，按保存frames计算原正常业务分段，不新增范围或负值钳制。发布速率后fresh FCC>=120才依次FCC=0、FF0=0、value54=6F、AdvanceCast；未满足即尾返回。没有Move或距离判定，不能按“入位”称此根会移动。

## 阶段13：播放后坐标移动

bit2置时characterAction=10、PlayFrame(action,1)弃返、updateDirection=0；未置时先周期特效/FF0计数，再characterAction=1、PlayFrame(action,0)弃返、updateDirection=1。共同fresh载荷Y0C先/X08后转F32，TurnPosition(action,X,Y,updateDirection,0F)，随后value54=6F、framePlayingRate=20，再MoveByAngle(action)。

Move之后另一组fresh载荷Y-X转F32交真实DistanceToPosition；S32返回严格<5才进入到位尾。尾与原一致：fresh X08转F32并发布offsetX，再fresh Y0C转F32并发布offsetY，然后AdvanceCast；不提前取齐这组坐标、不补screenX/Y、末尾PlayFrame或MarkDeath/free。距离>=5沿原尾返回，原载荷坐标与对象位置继续下一次正常调用。

<a id="battle-coordinate-action-decision"></a>
## 实施与复用边界

一个CPP两个入口各一段entryFlags U8取样，另CPP/公头各两条合同注释；两根其余全部正文/签名、总分派、动作15/射击/命中特效及所有其它源保持。无新函数/类型/API/字段/保护或额外RNG。

复用[BOG01](cg2d_battle_motion_angle_dependency_spec.md#battle-motion-angle-dependency-source-state)已完整的转向/移动/坐标距离及[BNW01](cg2d_battle_effect_callbacks_sampling_spec.md#battle-effect-callbacks-sampling-source-state)已完整的41E720/41E640创建-回调，不重复登记根/实现或声称全owner；新独立语义/F adapted/BATTLE已知成员各两项，枚举false。限定I仅两BYTE片段、S两完整当前根，五任务只补来源/局部描述，原分母、六closure及完整门禁保持。

依已获工程委托先SPEC后实施，标准数学/double与MSVC许可沿既有具名边界；排序待决、RPLAN.24、WAV-only和原格式明文保持，不引依赖，不恢复编译/测试/运行/产物/摘要权限。

<a id="battle-coordinate-action-acceptance"></a>
## 必要静态核对

核两连续根/库存、176条603B/四页、18directCALL及各callee/跳/RET、两case各11B/真实void返回消费、原BYTE TEST。核两RNG的Y先X/实际参数和周期生产、旧FF0计数/递增发布/旧值比较、12特殊提前返回与普通held frames速率/fresh FCC门禁/清零序，13两次fresh Y-X参数及移动后<5/顺序X发布再读Y，正确正文保持。整CPP仅两BYTE片段/两注释、公头两注释，旧几何/创建器及其它所有源不变；原依赖与证据只复用不重计。

核两语义/处置/成员、限定I/S/五任务及全部政策/分母/门禁、UTF-8无BOM/LF和静态汇总/-Check。不执行随机/计时/移动或算法样例；全部输入/父-callee/原未知间接alias、RNG实际序列、SDK正常失败/数值/容量/重进退出、最终资源owner及声音画面运行仍待，完整验收不降低。

<a id="battle-coordinate-action-source-state"></a>
## 当前状态

两完整原根与两BYTE差异已实施并完成必要静态核对，静态汇总与-Check通过；前态cg2d-re-battle-actions12-13-history-20261010，提交引用cg2d-re-battle-actions12-13-20261010。语义2056（2008F＋48X）、合同2118/3680（57.55%仅合同关联率）、F处置1483、软件证据1372，BATTLE已知成员296且枚举false；全部完整门禁保持，未编译或运行，整体SPEC未完成。
