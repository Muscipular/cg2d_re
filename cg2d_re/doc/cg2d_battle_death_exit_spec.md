# 战斗倒地、退场与中心附属效果完整合同 SPEC（BDE01）

2026-10-10；动作45–53九完整正常根，以及46实际依赖的41FA00创建/41F950回调两完整根。现清理、退场、宠物追加、缩放和附属投影正文正确，仅46四个宽度消费表达及二十二注释；不新增函数/API/保护/默认值、播放/声/RNG或业务重构。

<a id="battle-death-exit-baseline"></a>
## 完整原根与真实接线

| 原身份 / 当前源 | 完整连续根 | 指令 / 字节 / CALL |
|---|---|---|
| F:0000B130 / UpdateBattleAction45_40B130 | 40B130–40B178 | 20 / 72B / 3 |
| F:0000B200 / UpdateBattleAction46_40B200 | 40B200–40B3B9 | 139 / 441B / 10 |
| F:0000B3F0 / UpdateBattleAction47_40B3F0 | 40B3F0–40B406 | 7 / 22B / 1 |
| F:0000B490 / UpdateBattleAction48_40B490 | 40B490–40B5A1 | 87 / 273B / 5 |
| F:0000B5B0 / UpdateBattleAction49_40B5B0 | 40B5B0–40B5D2 | 7 / 34B / 0 |
| F:0000B3C0 / UpdateBattleAction50_40B3C0 | 40B3C0–40B3EA | 14 / 42B / 2 |
| F:0000BE70 / UpdateBattleAction51_40BE70 | 40BE70–40BE8D | 8 / 29B / 1 |
| F:0000BE90 / UpdateBattleAction52_40BE90 | 40BE90–40BF14 | 47 / 132B / 7 |
| F:0000BF20 / UpdateBattleAction53_40BF20 | 40BF20–40BFD3 | 58 / 179B / 5 |
| F:0001FA00 / NewBattleCenteredEffect_41FA00 | 41FA00–41FB16 | 82 / 278B / 5 |
| F:0001F950 / UpdateBattleCenteredEffect_41F950 | 41F950–41F9F9 | 53 / 169B / 5 |

合522条1671B、44E8/十六唯一callee、38跳二十一RET，无间接CALL/JMP。九动作387条1224B分十二份最多64条分页，中心两根135条447B各全根实读，共十四取证批次。九动作各一入向实际CALL/同父404DB0，九case各11B push→CALL→JMP40563F；中心创建另有40B200/413750/438550三CALL，合十二直接入向/四父；回调只有41FA30的字段+8 MOV地址生产，不伪计CALL。局部分派不代完整父/所有callee/ABI或owner验收。

## 45：临时CryptInt零与三字段复制

ConstructFromInt(临时对象,0)后按mask、encoded、readCount全部读齐，再按原mask(+178)→encoded(+174)→BYTE count(+17C)发布，保目标token/未复制字节；Destroy临时对象后Advance(action)。不改通用赋值/整对象复制、不复制token、不提前销毁。现正文正确，仅注释/独立原身份登记；CryptInt现真实合同沿既有legacy_cryptint，不在本步重建或宣称其全父/随机数/寿命验收。

## 46：倒地门禁、骑乘短路及不同清理分支

固定一次payload；完整W32 FDC的10000000h先测，置FF0=当前phase再phase164，立即返回，不播放/清理/推进。否则rider13C8和effect1060都非空才固定carrier载荷；first T*及其载荷分别判空后测原BYTE bit2，已死短路不读second；否则从固定carrier再取second及载荷、判空后测BYTE bit2。任一已死Advance后fresh phase==46才再Advance，并返回。只修两BYTE消费，不去掉原空门禁/增carrier保护。

普通路径characterAction10→rate100→PlayFrame(action,1)，严格返回1才进入后续；非1直接返回。随后完整FDC读取后AH10h（低WORD1000h）门禁：先scaleX=1、scaleY=1，再fresh完整FDC清1000h并整W32发布。仅门禁改U16，清位和所有高位/其它位保持。

依次从固定payload取105C、1054、1068；每项非空MarkDeath后清对应槽，下一个在前调用/发布后fresh取，不提前读齐/合并释放。随后13B0非零直接phase219返回，保1058。否则fresh原BYTE FDC的0Ch：清则取1058，非空MarkDeath后清槽，fresh FE0 OR2整字段发布→phase48；置位则尝试NewCentered(action,110169)，不论返回都fresh X供WAV声215，然后fresh FE0 OR2整字段发布→Advance。两分支不交换，不在创建失败/已有1064时跳声或推进，不把MarkDeath当free。原声额外Y不由现二参WAV边界消费，不恢复第三参数/MIDI。

## 47、49、50、51：不同死亡/完成职责

47直接完整W32 FDC OR2后Advance，不缩成BYTE写。49只有完整S32 1024严格==2才1024=0→phase51；不按非零或>=2。50仅characterAction10→PlayFrame(action,1)，严格返回1才Advance，原没有rate写入。51先固定payload/battlePosition18，清全局对应战斗槽，再MarkDeath(action)并保真实T*返回；不增范围/空保护、不立即释放或交换两操作。

## 48：两退场路径和原正常上限

先固定payload再调用现真实设备边界。无设备时fresh FF0+1发布，新值持样；随后field58>=8优先，先持flags/X→field58=0→flags OR8000h整字段发布→声218→NewBattleEffect(action,0,offsetY0,type36,0,1)→phase49。不因创建失败跳过phase49，不额外清FF0/播放/推进。field58<8且新count>4才FF0=0，然后flags XOR2与field58+1全部取好，再flags→field58发布；其余返回，原field58阈值8/count阈值4保持。

有设备时FF0!=255才FF0=255后声218；之后先持BYTE 30F旧alpha，再BYTE 308=8。旧alpha==0才field58=0→FF0=0→phase51返回；否则每帧NewRising(action)，调用后fresh BYTE alpha，<4归零、其它减4发布，不将该业务零归删掉/扩大为新输入钳制，不把创建放在仅首帧。BNX01已纳管420D50真实随机/创建回调，本步源和旧身份均复用不重计。

## 52：固定caster与逐次重取宠物目标

固定payload，先取原BYTE FDC后固定caster1070；bit4置位才以battlePosition18求petPosition并查一次pet，非空才Append(caster,pet,13)。随后同petPosition每次重新读全局目标，依次Append47、Append51；三追加后fresh payload.battlePosition18再求位置/查目标，MoveState(当前宠物)。不缓存三次pet或第二次位置，不把caster当当前actor、不加后续NULL保护；全部正常分支最后Advance(action)。现正文已正确持样/fresh及BYTE，不新增消费/RNG/队列算法。

## 53：缩放、type22与公共播放

固定payload/调用设备后fresh flags，按设备OR100000h或8000h；以原54D490 F32位模式3CA3D70A（约0.02）减scaleX，保未舍入中间值，再flags→F32 scaleX发布，然后fresh scaleY减同常量并F32发布；原负值分支将两scale归零，比较不能提前改用已舍入scaleX。现double正常中间/float字段保持，标准数学与编译器边界沿已有许可，不在本步改数值精度/异常模型。

fresh完整1024==0才NewBattleEffect(action,0,offsetY0,type22,0,1)，保存实际返回到+10，无新失败保护。创建后fresh F32 scaleX等于零才取+10，非空MarkDeath后清+10，再Advance；所有正常路径最后PlayFrame(action,1)，保实际S32返回。原零常量54D420和step各4B完整取齐/位模式对照，不新计X，不把新构造调用折成缓存或跳过末播放。

## 中心效果：创建、固定parent回调与投影尾

创建先读parent payload的1064；已存在直接空返，不换图、不调用New/RNG。否则New(2,0)，空即返。成功先持新flags，发布真实41F950 callback至+8、priority BYTE C8、rate100、flags OR40000h；LoadGraphic(parent graphicIndex)后fresh取索引，读真实DX低S16 width并S32除2向零，计算parent graphicOffsetX＋offset40后发布新offset40；再fresh取索引/DX低S16 height/2，计算并持centerY后imageNo→direction0→两BYTE4→F32 offset44→PlayFrame(new,1)。不把除2改half浮点、不提前固定两次graphicIndex/两DX基址或提前发布Y。

播放后fresh parent.payload再发布1064=new，new.parent10=parent；原投影先X转换/全局X偏移读取，Y在X发布前持样，然后X发布→Y转换/全局Y偏移→W16 field270=0→Y发布→T*返回。现正文/真实S16宽高提供者正确，全保持；不将1064提前发布、加owner或复制parent payload。

回调固定parent T*；parent.deathFlag严格==1才MarkDeath(new)后进入共同投影尾，不返回、不清parent1064或物理释放。否则PlayFrame(new,0)→LoadGraphic(parent graphicIndex)，按与创建相同的fresh索引/两S16向零除2/X先Y后发布offset；共同投影仍先持Y后X发布、Y转换/偏移和Y发布。正常S32转换沿CPH01宿主边界，原__ftol身份复用，不模拟私有ST0/异常数值。完整创建/借用/最终释放/其它父和SDK仍待，不据本回调标死称寿命闭合。

<a id="battle-death-exit-decision"></a>
## 实施与登记边界

依第8条既有工程委托先SPEC再落实：一CPP仅46四消费表达（两骑乘BYTE、一WORD1000h、尾BYTE0Ch）和十一定义注释，公头十一注释；其它十全文/所有签名/数值/真实调用/提供者和旧语义/处置保持。十一原身份此前无独立语义/F处置/已知BATTLE成员，新增十一adapted语义/处置/成员，枚举false；一限定I仅46四表达，一S十一完整当前正常源/局部分派/常量与callback生产，不计新全实现。

U08/U09/R01/R04/V04补来源与局部状态且open，原分母/所有政策/门禁、旧记录和六closure保持。设备边界沿SDL3渲染既有豁免、WAV-only、原格式明文、MSVC CRT与RPLAN.24保持；CRT七排序此前选择仍未答，不扩许可，不构建/测试/运行/算法/产物/摘要或引依赖。

<a id="battle-death-exit-acceptance"></a>
## 必要静态及剩余

核十一完整522条1671B/库存/十四取证批次、44E8/十六callee/38跳二十一RET；九11Bcase、十二实际入向CALL/四父与callback字段+8 MOV分列；两4B常量。核45三字段读齐/pub与临时构析，46原BYTE/WORD消费/完整高位/短路、fresh播放后缩放/三清理/不同219-48-创建推进分支；47/49/50/51不同严格门禁及槽清理先标死；48持样发布/8-4-255及旧alpha前置-新alpha重取；52逐次目标/位置重取；53未舍入中间/F32发布/创建后重读与共同播放，中心创建/失败/callback/两S16向零除2及标死后投影尾。

核整CPP四消费/十一注释、公头十一注释及其它所有本体/签名/提供者/数值/旧记录不变；十一语义/F/成员、限定I四表达/S十一根、五open任务/全部原门禁政策，UTF-8无BOM/LF、必要差异与静态汇总/-Check。完整输入/13B0与1024生产、其它父/全部callee/ABI/alias/owner、SDK正常失败/资源/重进退出/数值及声音画面运行仍待，总SPEC§10不降。

<a id="battle-death-exit-source-state"></a>
## 当前状态

十一完整原根/数据与当前源已取齐，46四宽度消费/二十二注释已落实；必要静态、文本汇总/-Check及精确派生范围通过。语义2106（2058F＋48X）、合同2168/3680=58.91%（仅关联率）、F1534、E1389、BATTLE346且枚举false；五任务open/旧记录/原分母/所有政策门禁保持。未构建/测试/运行/算法/产物/摘要。前态cg2d-re-battle-death-exit-history-20261010，提交引用cg2d-re-battle-death-exit-20261010。整体SPEC未完成。
