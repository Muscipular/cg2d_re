# 战斗动作0/1待机与播放门禁 SPEC（BIDLE01）

2026-10-10；补动作0/1完整原根及实际分派，恢复四段低BYTE消费；原完整FDC持样、特殊技能上一动作门禁和动作归零后的fresh FDC保持。阶段236归零入口复用已有BQD01，不按名称误拆成阶段2/3/6或重复纳管。

<a id="battle-idle-baseline"></a>
## 完整原根与调用

| 原身份 / 当前源 | 连续完整根 | 原调用与分支 |
|---|---|---|
| F:00006650 / UpdateBattleAction0_406650 | 406650–40671F，64条207B/一页 | 四directCALL/两callee、12跳三RET；404DE8 case0与406643两实际caller |
| F:00006720 / UpdateBattleAction1_406720 | 406720–40675A，14条58B/一页 | 一directCALL/一callee、两跳一RET；404DF3 case1唯一caller |

合78条265B/两页、五directCALL，无间接调用/本根分派表。case0/1各三条11B，CALL后转共尾不消费本根EAX；现void签名和总分派保持。406643来自已纳管F:00006630阶段236入口，原九条26B先取payload、发布phase=0/busyFD8=0再调用动作0，CALL后pop ECX/RET；本轮只复用该已证入口和局部不消费窗，不新计根/实现或修改源。

## 动作0：列表推进与特殊上一动作门禁

入口固定payload，取一次S32 FC4索引；仅index<=FC8且列表该项==166才AdvanceCast。原没有额外index>=0、容量或payload门禁，不补保护。调用后继续原流程，不提前返回或复用初始索引到后续“上一项”。

先framePlayingRate=100，再读取一次完整W32 flagsFDC持样；原MOV ECX后TEST CL，当前保完整flags读取，再以cachedFlags U8表达同一值的低BYTE消费。bit2置时characterAction=10、PlayFrame(action,1)弃返后返回，不执行特殊技能/站立分支。

否则fresh取skill；仅72/113检查fresh FC4-1，>=0才读对应上一项。上一项57或59时直接返回，保现characterAction并不播放；不能仅凭skill跳过，也不能使用Advance前的索引。其余进入站立门禁。

## 动作0：归零后重读与保持站立

characterAction非0时先发布0，再fresh取flagsFDC低BYTE；仅该fresh值的0x60清零才PlayFrame(action,0)弃返。此处不得改用前面的cachedFlags，原读取在动作发布之后。

characterAction已经0时先测试action.flags低BYTE bit0x40，置位才检查前面完整FDC的cachedFlags低BYTE 0x60；后者清零才PlayFrame(action,0)弃返。不在此支fresh重读FDC或增加其他位要求；原三处播放为互斥路径，不按静态CALL数量补重复播放。

## 动作1：归站立后的独立FDC门禁

先保存S32 characterAction，再固定payload指针，随后发布framePlayingRate=100；仅保存的动作非0才characterAction=0。不管是否归零，接着fresh flagsFDC低BYTE测试0x60，清零才PlayFrame(action,0)弃返。原没有动作0的166推进、bit2、72/113、上一项或action.flags40要求，不合并成共享待机实现。

<a id="battle-idle-decision"></a>
## 四段实施边界

一个CPP动作0三段/动作1一段：完整W32 flags读取后派生cached U8供CL两处消费、归零后fresh U8、action.flags的U8测试与cached U8门禁、动作1归零后fresh U8。另CPP/公头各两条合同注释；原列表/索引/skill门禁、字段/读取发布序、播放/真实弃返及其它全部正文/签名/源保持，无新函数/类型/API/字段/RNG/保护。

新独立语义/F adapted/BATTLE已知成员各两项，枚举false；限定I四段低BYTE消费、S两个完整现源，五任务只补来源/局部描述。BQD01已有236语义/处置/源码不变，原分母、六closure与完整门禁保持。依已获工程委托先SPEC后实施；RPLAN.24、MSVC排序待决、标准数学/double、WAV-only和原格式明文边界不扩。

<a id="battle-idle-acceptance"></a>
## 必要静态核对

核两连续根/库存/78条265B/两页、五directCALL/各callee-跳-RET、case0/1各11B和236已证调用/返回不消费窗；原W32持样与CL消费、fresh BYTE MOV/TEST、object BYTE TEST有据。核166条件/Advance后fresh索引、72/113且上一项57/59门禁、动作归零后fresh FDC与已站立使用cached FDC的区别、动作1独立条件和原三互斥播放点；完整W32读取和所有正确正文保持。

整CPP仅四段/两注释、公头两注释，BQD01/其余全源不变；两身份/处置/成员、限定I/S/五任务与政策/分母/门禁、UTF-8无BOM/LF及静态汇总/-Check准确。不执行待机/列表/动画或算法样例；所有输入/完整父-callee/未知间接alias、动画生产/最终owner、SDK正常失败/容量/重进退出与声音画面运行仍待，不降低总SPEC§10。

<a id="battle-idle-source-state"></a>
## 当前状态

两完整原根与四段低BYTE消费已实施并完成必要静态核对，静态汇总与-Check通过；前态cg2d-re-battle-idle-history-20261010，提交引用cg2d-re-battle-idle-20261010。语义2058（2010F＋48X）、合同2120/3680（57.61%仅合同关联率）、F处置1485、软件证据1374，BATTLE已知成员298且枚举false；236及全部完整门禁保持，未编译或运行，整体SPEC未完成。
