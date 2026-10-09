# 回旋对象创建与同步推进 SPEC（B22C01）

2026-10-10；沿阶段21—23消费合同追到真实创建器。补完整根与原返回发布来源，修十一组调用前/字段发布的取样；既有几何分支、正常整数比例、DX宽度和同步推进保持，不将局部创建合同判为完整owner验收。

<a id="boomerang-creator-baseline"></a>
## 完整原根与生产入口

F:000195F0 / NewBattleBoomerang_4195F0，原连续4195F0–419BDA，481条1514B/八页、15CALL（14直接、1实际callback槽间接）、54跳两RET。唯一实际入向来自406E10的406F35；原返回EAX是新action T*，caller将其发布到parent+10，当前真实caller保存于battleCastEffect10。同窗X/Y/音效读取的完整父合同未验，不以该窗口或原+10共享字段推全部alias/父语义已闭。

入口先固定parent.payload，New(priority2,原payload13E8)；现sizeof(BattleActorPayload)仅按已证原生指针拓宽，非地址字段不扩。分配空即返空；成功保存新action，读flags、发布总回调404DB0、action0、flags|40000、rate100、priority205和非拥有parent+10。原回调槽调用的operand元数据8是字段偏移，不能当函数地址/固定空体；现每轮读取action.callback真实调用，接既有完整总分发。

## 坐标、效果与借用发布

原先parent.X F32→新payload取样→新offsetX发布；parent.Y F32→读取新offsetX→offsetY发布→X正常截S32→读取新offsetY→screenX发布→Y正常截S32发布。恢复这些阶段，不改变正常数学/失败或引入限值。image为parent.skill2022的111141，否则103000；direction0/field270低W16清0，payload.position来自固定parentPayload。

效果循环每轮先重读parent.skill；258按level<9/9..11/>=12选29105/29106/104217，以fresh current.position共享目标创建HitEffect(0,0,30,image)。随后取parent.effectIndex，按position4→flags W32→position3→position读齐四值再Append(source,{position,flags,position3,position4})；不将原压栈样本改为callee读取活引用。Append之后再fresh current.flags，bit800000才TakeAux(parent)及AppendAux，原flags跨TakeAux持样；MoveEffect(parent)严格返回1结束，否则下一轮，不加循环上限/失败补值。

循环后固定新payload当前effect引用：fresh共享对象X发布payload.X00；同引用position重新取共享对象Y，随后先取effects[0].position再发布Y04，固定parentPayload.caster1070发布到新payload；第一目标查找使用此前保存的首位置。原两个S32位置/屏幕坐标与真实caster T*各自保持，不新建owner或改成W32指针令牌。

两次RelativeDirection调用都按target.offsetY→target.offsetX→source.offsetY→source.offsetX取F32，参数仍(sourceX,sourceY,targetX,targetY,5F)，返回按S8。首次direction1/0/-1在effectCount>=2时按两目标X的原比较修路点；第二次重取首目标和方向，再按fresh两目标X求firstOnRight。保原独立比较/未知方向与数量分支，不用第二次方向覆盖第一次持样。

## 几何与同步推进保持

首次direction1的大于路径保持Y-100/X+20、第二独立小于或unordered路径X-100；direction0的大于路径仅路点X>=sourceX才X+150，小于或unordered路径仅路点X<=sourceX才Y+150；direction-1对应Y+100/X+20和X+100。两处Y/X同时调整原先读齐Y/X再发布Y/X，恢复该读写序，原其他条件/公式保持。

第二次方向与左右关系分流，原四个S32比例按相应坐标差做100*差/差，正常只上限100，不加下限/分母检查/回绕模拟；/2保原有符号向零截断。竖向调整的两路径先读齐待发布Y和旧X再发布Y/X；横向正向路径先读齐待发布X和旧Y再发布X/Y。其余既有比例输入、比较、整数公式、钳制100的原正常业务及字段发布正确保持，不删除原业务上限或借RPLAN.24补异常保护。

最后TurnToPosition先取payload.Y→X转F32；LoadGraphic(parent.graphicIndex)→phase21→speed10F→acceleration10F→field1D0=0。此后parent.graphicIndex先取，现ReadGraphicDxWidth的既有实现再读取DX表、返回低S16；0.5F乘后保存F32 halfWidth一次，正确保持，不改为表先取、U16或每轮刷新宽度。

原do至少执行一次fresh callback槽调用，phase!=21立即结束，否则S32 Distance(source,parent)<持有halfWidth才重复；不新迭代上限、直接固定阶段21调用、空callback保护或延后总回调。结束PlayFrame(source,0)后从固定parentPayload按skill103C→attackModeFF4→level1044读取/各自发布到固定新payload，返回保存的action T*；字段不能提前到同步回调前，也不重取已保存payload/parentPayload。完整回调可推进到21以外，当前21入位/22命中/23退场合同保持；所有callee/创建重进及借用/最终回收仍待。

<a id="boomerang-creator-decision"></a>
## 十一组实施边界

一个CPP十一组：初始坐标/载荷取样；四效果值读齐；首Y/首位置的发布取样；两方向各四F32样本；首几何两处Y-X读齐；竖向比例两处Y-X及横向正向X-Y读齐；最终Turn的Y-X。只另补CPP/公开声明两注释，零新函数/类型/API；全部几何判定/数值公式、DX宽度、分配空返/总回调/flags/辅助效果/原返回及其它源不改。原数值/CRT差异沿已批合同，SDK实际结果不据静态验。

新增一独立语义/F adapted/一个BATTLE已知成员，枚举false；限定I只十一组/S覆盖完整本根，U08/U09/R01/R04/V04只同步来源/局部描述，全部原分母/六closure/完整门禁保持。按已获工程委托依SPEC实施，无新增业务设计；WAV-only、原格式明文、MSVC与排序未决边界/RPLAN.24保持，不编译/测试/运行/算法/查产物/计算摘要或引依赖。

<a id="boomerang-creator-acceptance"></a>
## 必要静态核对

核完整连续481条1514B/八页与原库存、14实际直接CALL/一callback槽/54跳两RET、唯一返回T*真实发布窗口；原四字段/坐标/方向/几何读齐及字段发布有据。核width低S16/图号先表后和一次halfWidth、同步总回调/phase21/Distance谓词/三尾字段与返回。既有New、Append/TakeAux/MoveEffect、总分发与21—23、DX和数学原合同复用，不重计新callee根或全owner验收。核整CPP仅本根十一组/一注释，公头一注释，所有其它正文/签名/源和分支/失败/字段/循环保持；一新身份/处置/成员、限定I/S与五任务、政策/门禁/分母/UTF-8无BOM-LF和静态汇总/-Check。所有父-callee/输入数据/间接alias、SDK容量失败/数值/重进退出及资源owner/声音画面运行仍待。

<a id="boomerang-creator-source-state"></a>
## 当前状态

完整创建器与十一组差异已落实，必要静态与统一汇总检查通过；前态cg2d-re-battle-boomerang-creator-history-20261010，提交引用cg2d-re-battle-boomerang-creator-20261010。整体SPEC未完成。

### 实际限定结果

必要静态、汇总与-Check通过：完整481条1514B/八页、14实际directCALL/一真实callback槽/54跳两RET、原返回T*发布父+10窗口有据；间接元数据8核为字段偏移，真实生产404DB0/同步总回调保。十一组效果四值/坐标/首位置/两方向/几何读齐及最终YX取样实修，整CPP仅本根十一组/一注释、公头一注释，全部其它源/签名和原几何比较/正常比例/上限100/分配空返/flags跨TakeAux、DX低S16/一次halfWidth/原do-loop及尾三字段/返回保持。首轮检查遗漏提取体末LF导致尾断言误失败，已按实际含LF片段纠正并留证，未改业务或验收。语义2047→2048（2000F＋48X）、合同2109→2110/3680（57.34%仅合同关联率）、F1474→1475、E1362→1364仅限定I/S各一，BATTLE287→288且枚举false。导入16/218、X3060/3061余1、184叶完整验收0、60任务关闭11与六closure保持；十一自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-battle-boomerang-creator-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。父同窗与完整父/生产数据/所有callee/间接alias、SDK容量失败/数值/重进退出与资源owner/声音画面运行仍未验，整体SPEC未完成。
