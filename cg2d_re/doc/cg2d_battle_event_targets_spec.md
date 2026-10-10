# 战斗帧事件、目标启动与投射生产完整合同 SPEC（BTE01）

2026-10-10；动作64–72九完整正常根。现八点方向摆动、独立1014图号、入口事件/目标循环/哨兵推进、逐目标投射和caster计数正文保持；仅68/69两BYTE消费、70播放/可能推进后的事件一次持样及十八注释，不新增函数/API/保护/默认值、循环上限/声/播放/RNG或业务重构。

<a id="battle-event-targets-baseline"></a>
## 完整原根与真实分派

| 原身份 / 当前源 | 连续完整根 | 指令 / 字节 / CALL |
|---|---|---|
| F:0000CBB0 / UpdateBattleAction64_40CBB0 | 40CBB0–40CBC6 | 6 / 22B / 1 |
| F:0000C380 / UpdateBattleAction65_40C380 | 40C380–40C475 | 62 / 245B / 3 |
| F:0000C480 / UpdateBattleAction66_40C480 | 40C480–40C4C3 | 18 / 67B / 2 |
| F:000106B0 / UpdateBattleAction67_4106B0 | 4106B0–410730 | 38 / 128B / 5 |
| F:0000C0E0 / UpdateBattleAction68_40C0E0 | 40C0E0–40C11E | 24 / 62B / 3 |
| F:0000C120 / UpdateBattleAction69_40C120 | 40C120–40C160 | 25 / 64B / 3 |
| F:000070E0 / UpdateBattleAction70_4070E0 | 4070E0–4071BB | 64 / 219B / 6 |
| F:0000C4D0 / UpdateBattleAction71_40C4D0 | 40C4D0–40C515 | 24 / 69B / 4 |
| F:0000CAA0 / UpdateBattleAction72_40CAA0 | 40CAA0–40CABA | 9 / 26B / 1 |

共270条902B/九份最多64条分页、28E8/九唯一callee、28跳十RET，无间接CALL/JMP。九根各一实际入向CALL/同父404DB0；九case各11B push→CALL→JMP40563F，真实case标签64–72取齐。65保两个互斥播放点/实际S32返回；其它当前void及真实case弃返保持，不据局部分派外推全父/ABI或owner通过。

## 64/65：直接推进与八个不同取样点

64仅characterAction0→Advance，不播放/清帧/发布速率。65固定payload，先characterAction0；持旧FF0为previous，新count=previous+1发布。旧previous>=70才Advance后FF0=0→公共PlayFrame(action,0)返回，不改新count门禁或提前清零。

旧previous<70时保存的新count==8才direction减1；之后分别fresh FF0==16、24、32、40、48、56、64时按+1、+1、-1、-1、+1、+1、-1修改方向。七次独立重读，不缓存count、不合并switch/else-if。再读取direction，>=8只减8发布一次；重新读修正后direction，<0才加8发布并由该支PlayFrame(0)返回，否则公共尾PlayFrame(0)。每条正常路径一次播放，原单圈修正和业务70门禁保持，不改%8/循环纠正/输入钳制或异常回绕模型。

## 66：独立1014图号与推进后清事件

固定payload后characterAction5，fresh image1014完整W32发布imageNo，再PlayFrame(action,1)。严格返回1才Advance后frameEvent=0；不交换两操作、不清index/remain、不用reactionImage1010替代独立1014，也不按非零完成。现正文正确保持。

## 67/71：逐目标启动与哨兵额外推进

67入口先持frameEvent，再固定payload、characterAction6；仅保存的事件值>=10000且<20000才启动目标循环，不在角色动作发布或播放后重取门禁。每轮fresh effectRunningC9C和16B effects记录position，严格position==-1才离开，否则fresh全局目标T*→MoveState(target)→AdvanceEffect(source)弃返，再从固定payload重新读取下一项。不能用AdvanceEffect返回作循环终止或把目标缓存为整个列表。

到哨兵后仍额外一次AdvanceEffect(source)，再frameEvent=0；没有上限、目标父NULL保护或提前清事件。最后所有正常路径PlayFrame(source,1)，严格返回1才AdvanceCast(source)。71无事件/播放门禁、角色动作/速率发布，直接执行相同目标循环及哨兵额外推进，最后AdvanceCast(source)。两根原不同职责保，真实MoveState/AdvanceEffect沿BEV01，不按名统一行为/返回。

## 68/69：创建后的fresh低BYTE门禁

两根都固定payload并先读取reactionImage1010：68 NewFloatingIcon(source,image,0,0)，69 NewHitEffect(source,0,0,30,image)。实际创建返回弃用，无新失败保护；之后从固定payload fresh读取原BYTE FDC，低60h全清才PlayFrame(source,1)弃返，所有正常分支最后AdvanceCast(source)。仅把两消费明确为U8，不提前持FDC/跳过失败后的播放推进、不改0/1播放参数或补characterAction/rate。

BOH01浮动图标（旧1395号）与BHE01命中特效（2049号）完整既有创建/回调仅复用，源/独立身份/处置均保持，不重计或外推全资源/寿命验收。

## 70：播放后事件一次持样与逐轮投射

固定payload，characterAction5→rate100；按当前C9C/16B效果记录持position，非-1才TurnToward(source,当前全局目标,1,0F)。随后PlayFrame(source,1)，严格返回1才AdvanceCast(source)；不因此早退，之后才一次fresh读取frameEvent并持为event，保存值>=10000且<20000才进入生产。原40713D MOV EAX后两CMP复用同值，现两次字段表达改一个局部持样，不提前到播放/可能推进前，也不扩大为通用事件助手。

原do循环每轮fresh S32 castSkillLevel1044，<12选110016、其它111055；之后fresh C9C形成当前效果记录的真实非拥有指针，NewSpiracleShot(source,record,image)实际返回弃用。每轮AdvanceEffect(source)，严格返回1才结束；至少尝试一次创建，没有新增position==-1终止、失败早返或循环上限，不缓存图号/记录/C9C或跳过目标。结束后fresh X供WAV声254→frameEvent=0；原未由现二参WAV消费的额外Y不恢复/MIDI不引入。

BPC01/DPCS01的418B70投射创建及phase16/几何投影（旧2033号）已完整纳管，保持其正常分配/失败、数据复制、父重取/真实回调和资源接线，不在本步重复取整根或新增身份。探索的原xrefs中40716D JMP指向本父407182，只有407183实际CALL创建器；不把IDA type17所有引用都计CALL。BOG01转向（1385号）、现声1899号与BEV01推进仅复用，完整父/机器ABI/owner/SDK和声音画面仍待。

## 72：caster载荷计数与当前源推进

固定payload→caster1070 T*→caster当前payload，原00槽完整S32递增；然后AdvanceCast(source)。不改为推进caster、不换当前source载荷计数、不加NULL或数量保护。只按已证真指针承载x64，非地址字段不扩宽；现正文保持，RPLAN.24不模拟溢出/回绕。

<a id="battle-event-targets-decision"></a>
## 实施与登记边界

依第8条既有工程委托先SPEC后落实：一CPP仅68/69各一低BYTE消费、70一处播放后事件持样及九定义注释，公头九注释；其它六完整正文/所有签名/数值/真实调用/源、共享数据/工厂/回调与全部旧语义/处置保持。九原身份此前无独立语义/F处置/已知BATTLE成员，新增九adapted语义/处置/成员，枚举false；一限定I仅上述三片段，一S九完整当前正常源/局部分派/依赖接线，不计新全实现。

U08/U09/R01/R04/V04补来源/局部状态且open；原分母、所有门禁政策、旧记录和六closure保持。MSVC CRT、WAV-only、原格式明文与RPLAN.24保持，CRT排序此前取舍未答；不构建/测试/运行/算法/产物/摘要或引依赖，不扩业务或降低总SPEC§10。

<a id="battle-event-targets-acceptance"></a>
## 必要静态与剩余

核九完整270条902B/九页/库存、28E8/九callee/28跳十RET、九实际入向和11Bcase。核64直接推进/65旧70-新8-七fresh门禁和独立方向/互斥播放；66独立1014/严格1/推进后事件清零；67入口持事件、67/71每轮C9C/position/真实目标与哨兵额外推进；68/69创建后BYTE60h；70播放/可能推进后单次事件、每轮新图/record与至少一次/严格1结束/末音效清零；72 caster原00槽/源推进。

核整CPP仅三片段/九注释、公头九注释，所有其它本体/签名/数值/调用/复用旧记录保持；九语义/F/成员、限定I三片段/S九正常根、五open任务/所有原政策门禁，UTF-8无BOM/LF/新SPEC单一末尾LF、差异及文本汇总/-Check。完整1010/1014/1044/caster与事件/队列生产、其它父/全部callee/ABI/alias/owner、SDK正常失败/资源/容量/重进退出/数值与声音画面运行仍待，总SPEC§10不降。

<a id="battle-event-targets-source-state"></a>
## 当前状态

九完整原根/case与当前源已取齐，三读取片段/十八注释已落实；必要静态、文本汇总/-Check及精确派生范围通过。语义2126（2078F＋48X）、合同2188/3680=59.46%（仅关联率）、F1554、E1392仅限定I三片段/S九根、BATTLE366且枚举false；五任务open、旧记录/原分母/全部政策门禁保持。未构建/测试/运行/算法/产物/摘要。前态cg2d-re-battle-event-targets-history-20261010，提交引用cg2d-re-battle-event-targets-20261010。整体SPEC未完成。
