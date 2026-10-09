# 动作15回旋与范围效果生产 SPEC（B15P01）

2026-10-10；沿4195F0唯一父追完整406E10，覆盖模式3回旋生产及其余模式范围效果/258分支。修四组事件/音效X/子对象发布取样，既有调用、循环、分支及创建器/21—23保持；不以局部生产链证明全资源owner完成。

<a id="battle-action15-baseline"></a>
## 完整根与真实分派

F:00006E10 / UpdateBattleAction15_406E10，原连续406E10–4070DE，218条718B/四页、18直接CALL/十目标、32跳两RET，零间接调用/分派表。唯一实际CALL来自404DB0 case15的404E56；push对象→CALL→JMP40563F共三条11B。公共尾原EAX在消费前覆盖沿BTS01/BPEX01既有证据，当前void与真实case15保持；不重计公共尾原根或外推未知间接alias/所有父-callee闭包。

固定入口BattleActorPayload；FD0严格0才FD0=1→characterAction7→ProbeAttackEvent(source)，返回严格0才characterAction5。所有路径rate100，读取attackModeFF4，严格3分流；其他值进入普通路径，不合并成固定成功。原字段继续S32/W32/原生非拥有T*，无新状态镜像、帧事件保护或RNG消费。

## 模式3回旋生产

取当前effectIndex/position，严格-1跳过转向；否则固定共享target，取ownPosition后保存target.X/Y S32。position>=10且ownPosition在10..14时sourceY<=targetY使X-100，其余ownPosition下sourceY>targetY使X+100；position<10且ownPosition在0..4时sourceY>=targetY使X+100，其他ownPosition下sourceY<targetY使X-100。Y不改，TurnToPosition(source,X→F32,Y→F32,1,0)；现正确条件/公式保持。

PlayFrame(source,1)严格返回1才AdvanceCast(source)后固定payload.FD0=0；随后只读取一次frameEvent并用同一S32值作[10000,20000)门禁。命中NewBoomerang(source)，其同步总回调/失败可改变当前现场；原返回保存后source.X先取，再发布返回T*到source+10，随后fresh frameEvent计算音效号event-10000，用保存X播放，最后frameEvent=0并返回。空返回同样发布/播放/清事件，不新增空值处理或重试；不能用入口事件快照代创建后的音效事件。

当前旧表达式先发布child再读X，且门禁两次读取frameEvent；恢复为held gateEvent、held created T*→held soundX→child发布→fresh sound event。原另读取Y只作为PlaySe的额外压栈值，已证52C450本体不消费；保持既有两参音效合同，不恢复第三参数或以未消费Y建立新依赖。模式3之后不落入普通路径。

## 普通模式、258与范围效果

独立重取当前position，非-1才TurnToward(source,共享target,1,0)；PlayFrame(source,1)==1同样先AdvanceCast后FD0=0。之后另读一次frameEvent作[10000,20000)门禁，不沿用模式3快照或把非1播放结果视为事件成功。

技能258先level<9决定低档路径并持样整个循环；低档每轮29105，高档每轮重新读level<12选择29106、否则104217。按fresh current.position共享target创建HitEffect(0,0,30,image)，调用后fresh current记录地址调用RangeHit(source,record,22090)，再MoveEffect(source)严格返回1才结束。原两个Hit点和两个Range点分别位于高/低互斥循环，当前一个Hit/Range表达式依lowLevel持样覆盖，不补重复调用；高档后来降到9以下也仍在高档原路径，不能每轮重算lowLevel。

其余技能每轮fresh current记录调用RangeHit(source,record,22090)后MoveEffect==1结束；原第三个Range点对应当前另一表达式。原record是调用期同一载荷的非拥有引用，既有RangeHit内部四值/回调取样合同保持，不在父提前复制整个活记录。所有循环无新哨兵/数量/迭代保护或创建失败补值。

循环结束原先取frameEvent再取source.X，之后算event-10000并播放，最后清frameEvent0；恢复这个取样序，不沿用门禁事件。与模式3不同，这里没有新child发布。原额外音效Y不被本体消费，同样维持两参。全部输入、Probe/播放/Advance/三创建器完整callee、失败和原返回消费/owner/SDK运行另验。

<a id="battle-action15-decision"></a>
## 四组实施边界

一CPP四组：模式3/普通门禁各一次held frameEvent；回旋返回T*与soundX在child发布前持样；普通尾event→X→音效号。仅另CPP/公头两注释，零新函数/类型/API/保护；原转向/坐标/FD0/字段、PlayFrame/Advance、低档持样/高档fresh等级、所有循环和创建器/21—23及其它源保持。两条门禁前与创建后/循环后事件各自不同阶段，不错误共享一个缓存。

新增一独立语义/F adapted/一个BATTLE已知成员，枚举false；I只四组，S覆盖完整本根，U08/U09/R01/R04/V04只补来源/局部描述，完整门禁/六closure/原分母保持。按已获工程委托依SPEC落实正常合同；RPLAN.24、MSVC/WAV-only/原格式明文与排序未决边界保持，不编译/测试/运行/算法/查产物/摘要或引依赖。

<a id="battle-action15-acceptance"></a>
## 必要静态核对

核库存/完整218条718B/四页、18directCALL/十目标/32跳两RET/唯一case15 11B；原FD0/action/两模式/坐标条件、两单次事件门禁、child返回与X/发布/fresh事件、普通尾事件-X和258低/高三物理循环对应现两表达式有据。核所有当前正文差异仅四组/两注释，其余字段/接口、播放==1/Advance/原循环/失败/等级持样与创建器/21—23和所有其它源保持；已有子合同/公共尾复用，不重取/重计或宣称全callee/owner验收。核一身份/处置/成员与限定I/S、五任务、全部政策/门禁/分母、UTF-8无BOM-LF及静态汇总/-Check。不运行事件/循环/音效样例；所有生产数据/原间接alias、SDK正常失败/重进退出与完整资源owner/声音画面运行仍待。

<a id="battle-action15-source-state"></a>
## 当前状态

完整父生产根及四组差异已落实，必要静态与统一汇总检查通过；前态cg2d-re-battle-action15-history-20261010，提交引用cg2d-re-battle-action15-20261010。整体SPEC未完成。

### 实际限定结果

必要静态、汇总与-Check通过：完整218条718B/四页、18directCALL/十目标/32跳两RET和唯一case15三条11B有据；两个独立单次门禁、回旋returned T*/X先child pub/音效fresh事件与普通尾event-X、原258低9持样/高12逐轮fresh及三原物理循环对应现两表达式匹配。整CPP仅本根四组/一注释，公头一注释，全部其它源/签名与原FD0/坐标/播放==1-Advance/原循环/创建失败、两参音效/事件清0及创建器/21—23保持；既有子合同/公共尾复用不重计新原根或全owner。语义2048→2049（2001F＋48X）、合同2110→2111/3680（57.36%仅合同关联率）、F1475→1476、E1364→1366仅限定I/S各一，BATTLE288→289且枚举false。导入16/218、X3060/3061余1、184叶完整验收0、60任务关闭11与六closure保持；十一自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-battle-action15-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。全部输入/父-callee/间接alias、SDK正常失败/重进退出/数值和资源owner/声音画面运行仍未验，整体SPEC未完成。
