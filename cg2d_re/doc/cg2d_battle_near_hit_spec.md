# 战斗阶段20近距命中 SPEC（B20H01）

2026-10-10；沿追加效果链生产/查询合同补其一个真实命中消费者。覆盖阶段20完整正常源，修三处低BYTE读取和一组HP归零复制阶段；不将同用查找的其它阶段或全部战斗寿命一并判通过。

<a id="battle-near-hit-baseline"></a>
## 完整原根与字段

F:00008410 / UpdateBattleAction20_408410，原连续408410–408745，238条821B/四页、21直接CALL/18目标、19跳一RET，无间接调用/分派表。唯一已知实际入向404E98来自404DB0 case20，push对象→CALL→JMP40563F共三条11B；公共尾已有BTS01/BPEX01证据证明原残留EAX在消费前覆盖，现void和原真实分派保。未知间接alias/所有父-callee仍待。

入口固定action及其BattleActorPayload；原非地址字段继续S32/W32，effects1C的position/position3为S32、flags为W32，effectRunningC9C独立读取。原共享对象表59E980在现世界运行时为真实非拥有T*；目标及其payload只在距离命中后缓存，后续按各阶段重取索引/位置/共享目标。HP对象沿LegacyCryptInt的encoded+04 W32、mask+08 W32、readCount+0C BYTE；旧虚表token和reserved不复制，不拓宽或另建HP状态。

## 转向、图号与命中阶段

408428只读当前记录flags低BYTE bit8；清时以入口已取索引的position查共享目标，先Turn(source,target,1,0)，然后距离阶段重新取当前索引/position/目标。距离S32严格小于70才命中，等于或大于70仅进入共同Move尾，不加空目标/索引保护。

命中先position→flags W32→共享target→targetPayload；缓存flags bit4选110000，bit400调用FindTechnique(8,position)，S32结果<12选110098、否则111048，剩余一次RandomRange(0,2)+110001。查找的-1正常进入低级图；该分支没有资格再查、RNG重试或新增等级钳制。图号选择先于目标转向及后续应用flags，即使后续bit8/100走特殊命中，原图号选择/RNG消费也保持。

随后Turn(target,source,1,0)→fresh ownPosition共享对象发布target.battleOpponent10→fresh current.flags发布targetPayload.FE4→fresh ownPosition调用ApplyFlags。之后重新取currentIndex及flags W32，bit8令target.phase=31；否则bit100播放210/source.x、flash1004=1、target.phase=31。两特殊支路均不MarkDeath(source)，仍source.phase=17并走共同Move；原额外压栈Y不被现音效本体消费，保既有两参音效合同。

## 普通命中、HP与显示

普通支路沿应用后缓存索引的position3非零才进入效果/HP：缓存flags bit2可创建110008，非负主图可创建主效果；然后NewRebirth(fresh ownPosition共享对象,target)。重生调用后重新取currentIndex及flags W32，10000000清才取同一索引的amount→position→共享对象并SubtractHP；负值比较另取currentIndex/position/共享对象，不用缓存target代替。

原负值时Construct(temporary,0)，随后严格currentIndex→temporary.encoded→position→共享copyTarget→发布encoded→取temporary.mask/发布mask→取temporary.readCount/发布count→Destroy。现通用CopyPayload自身按三字段交替复制，但调用前已取位置/copyTarget，再读取temporary.encoded；原先取encoded再取位置/目标。本根改为显式三字段交替发布，先取索引和临时encoded，再取位置/目标，不更改全局CopyPayload或其它调用；原正常负值归零保持，不按RPLAN.24删除或扩为溢出模拟。

408666重新读current.flags低BYTE bit20，命中才targetPayload.FDC W32|2→gYobiUpdateGateMode=1→aliveFF8=1。该激活位位于10000000清的HP支路内，不移到免疫支路/普通支路公共尾。

普通支路共同Advance(target)后重取currentIndex及numberFlags W32；10000000置位时fresh对应position的共享对象创建22459。其余按target.Y→amount→target.X读取，NewNumber(X,Y-80,amount,6,2,numberFlags)；其后4086F8再独立读current.flags低BYTE bit2，可创建22454，随后Attribute(fresh ownPosition共享对象,target)。两显示路径最后都MarkDeath(source)，然后source.phase=17；所有分支共同Move(source)，不能因已标死提前返回或当场free。普通amount为0仍走该显示/标死公共尾；原音效/RNG/失败、接收者/返回消费及其它调用顺序保持。

<a id="battle-near-hit-decision"></a>
## 四处修正与实施边界

一个CPP四组：入口flags8、HP后activation20、数显后icon2改为原低BYTE取样；HP归零复制改为原索引/encoded/目标取样与三字段交替发布。保W32初始图号、应用后寄存器flags及免疫/数显缓存，不将所有flags一律窄读或改为每次fresh。只另补CPP合同注释/公开声明注释；不改结构/API/现分派、主图关系、RNG、其它阶段或已有提供者。原3次BYTE读取不是新业务保护，源字段和值保持。

新增独立语义/一F adapted/一个BATTLE已知成员，枚举false；限定I只四组，S覆盖完整本根当前源，不代全部callee/数据/owner或SDK实际结果。U08/U09/R01/R04/V04同步局部来源，其状态和所有分母/完整门禁/六closure保持。依已获工程委托按本SPEC实施，无新依赖/功能或未决业务设计；CRT排序待决与WAV-only/原格式明文、RPLAN.24及总SPEC§10保持。未编译/测试/运行/算法/查产物/计算摘要。

<a id="battle-near-hit-acceptance"></a>
## 必要静态核对

核库存单chunk与238条连续指令/821B、21直接CALL/18callee、19跳一RET和唯一case20实际11B；三BYTE原读取及归零复制完整窗口有据。核本体全部转向/距离70、三主图/RNG、fresh索引/共享目标与固定targetPayload、bit8/100及amount/免疫/激活/显示/标死后移动分支和字段位宽/真实生产者。复用已有查找/基础调度、CryptInt和公共尾合同，不重复记其新原根/全生命周期验收。核整CPP仅本根四组/注释及公头一注释，其余全部正文/签名/源/分派保持；新增语义/处置/已知成员与限定I/S、五任务、政策/全部门禁/分母、UTF-8无BOM-LF及静态汇总/-Check。所有生产输入、间接alias、SDK数值/正常失败、完整资源owner/重进退出与声音画面运行仍待。

<a id="battle-near-hit-source-state"></a>
## 当前状态

完整阶段20正常源及四组差异已落实，必要静态与统一汇总检查通过。前态cg2d-re-battle-near-hit-history-20261010，提交引用cg2d-re-battle-near-hit-20261010。整体SPEC未完成。

### 实际限定结果

必要静态、汇总与-Check通过：完整238条821B/四页、21直接CALL/18目标/19跳一RET和唯一case20三条11B有据；三BYTE读取、HP归零索引/encoded先于位置-目标及三字段交替pub窗口匹配。首轮检查把距离CMP地址误写408488，已按实际408468/83F846纠正，原因留证，未改业务或验收。整CPP仅本根四组/一注释，公头仅一注释，全部其它正文/签名/源和原图号/RNG、特殊/免疫/amount/显示/标死后phase17与Move保持；公共尾/既有依赖复用，不虚增原根或全生命周期。语义2042→2043（1995F＋48X）、合同2104→2105/3680（57.20%仅合同关联率）、F1469→1470、E1357→1359仅限定I/S各一，BATTLE282→283且枚举false。导入16/218、X3060/3061余1、184叶完整验收0、60任务关闭11与六closure保持；十一自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-battle-near-hit-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。全部生产输入/父-callee/间接alias、SDK数值/容量失败/重进退出与所有资源owner/声音画面运行仍未验，整体SPEC未完成。
