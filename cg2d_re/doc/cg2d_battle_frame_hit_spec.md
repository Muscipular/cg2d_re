# 阶段14完整帧命中与延时 SPEC（B14H01）

2026-10-10；补409680完整根及实际case14。只修六处归零复制的读取/发布、十二处独立BYTE读取和一处音效event-X取样；原选图/RNG、视觉与四类HP/FP分流、字段/调用和14-28当帧延时/FD4尾保持。

<a id="battle-frame-hit-baseline"></a>
## 完整原根、分派与数据

F:00009680 / UpdateBattleAction14_409680：连续409680–40A9DB，1371条4955B/22页，96个directCALL/23个callee、206跳两RET，无间接调用或本根分派表。唯一404F06来自404DB0的case14；三条11B后跳共尾，不消费本根EAX，现void签名/总分派保持。

入口固定payload及caster1070 T*，characterAction=5后读一次frameEvent，再framePlayingRate=100；仅保存事件>=10000进入命中，原没有入口<20000上限。先选图/执行两处互斥RNG，再按fresh effectRunning/position读取共享目标；目标空则不清事件、不处理命中，仍进入共同FD0/FD4尾。目标非空先固定targetPayload，hidden=0→Turn(target,source,1,0F)→targetSkill=-1→fresh source位置查共享battleOpponent→fresh效果flags发布target.FE4。

## 原选图与RNG消费

初始效果W32 flags持样：bit4主图110000；否则bit400按Find(8,position)<12选110098/111048；其余按fresh技能分支，主/第二图初始-1、direction初始0。所有原业务值保持：

| 技能 | 主图 / 第二图 / 方向 |
|---|---|
| 0、2007、2008、2021 | level<12：110133，否则111029 |
| 1 / 3 / 5 / 6 | 低档110134/110135/110139/110137，高档111047/111046/111052/111050 |
| 75..80、2096..2101、3019、3038 | 110074 |
| 81 / 84 / 85 / 87 / 105 | 110174/110175/110159/110160/110135 |
| 257 | 第二图110476；9..11再主图110014；>=12第二图111051，主图仍-1 |
| 258 | 第二图110477，主图仍-1 |
| 259 | 一次RAND(0,2)+110001，第二图110470 |
| 260 | level<12为110473，否则111054 |
| 262 | level<12为110473、方向1；否则111049、方向0 |
| 3100 | SelectIntValue(54D450,5,原level)，不减1或钳制 |
| 其余 | 一次RAND(0,2)+110001 |

54D450–54D464五S32为111256..111260，与既有constexpr逐项相同，复用BDI01不重计数据身份。选择器原合法索引[0,5)，不合法返回0；当前仍以image>=0决定创建，不把0改为跳过。257只持有一次level用于9/12比较；两原RNG点互斥，目标为空也不回退或取消已经发生的RNG。

## 高帧事件或1038分流

fresh frameEvent>=20000或value1038==1时，fresh效果W32 flags若无2000则走独立分流：bit8直接target.phase31；bit100声210、flash1004=1、phase31；其余只在mask1000C000清零且amount非0或skill6时显示。显示先target载荷X10转F32/pub offsetX，再读Y14转F32/pub offsetY；这组原顺序正确，不提前取齐坐标。

随后fresh bit100000才MarkDeath(target.effect1054)，取完整target FDC持样后先effect1054=null，再FDC清20；fresh BYTE bit2可创建target110008；主/第二图>=0分别创建，接Rebirth(source,target)、target.phase26。fresh当前事件>=20000声号减20000；否则减10000且音效后ApplyEffectFlags(source位置,source)。该分流末fresh事件<20000才递增固定caster当前payload.value00并清value1038；然后进入共同命中尾，不执行普通HP/FP分支。

## 普通视觉与四类命中

先ApplyEffectFlags；level>=9且skill259可NewTiledEffect(0,15)。fresh flags的bit8直接phase31后命中尾；bit100仅flash==0时声210/flash1/phase31，随后均命中尾。其余在相同mask1000C000/amount或skill6条件下按原X发布再读Y恢复坐标。

视觉bit2000时，缓存visualFlags的bit2在source创建110008，主/第二图在target创建，Rebirth之后先持有currentIndex再FE4=2000，读取该索引的fresh BYTE bit20，置位才source.FDC|2→gateMode1→alive1；fresh事件决定减10000或20000音效。非2000视觉在target清附属/建图/Rebirth，fresh BYTE bit20可对target作同样激活，随后fresh声事件→X→减10000音效。原缓存W32及两个bit20读取/字段发布保持，额外原音效Y不被52C450消费，现两参合同不变。

视觉之后fresh hitIndex/hitFlags按8000→4000→2000→普通优先分流，源中的共享尾/互斥调用按路径覆盖原物理点，不按CALL数量补重复操作：

| 命中标志 | 原正常行为 |
|---|---|
| 8000 | 先Advance(target)，caster.phase8时先增value00；75..80、2096..2101、259按80000在target建对应debuff，94以非零value1050建图；仅skill6处理target FP扣减/归零、type3数显/图标/属性，其余直接命中尾 |
| 4000 | 先Advance(target)；skill6处理target FP后直接命中尾；其余debuff后处理target HP恢复、Greater后的最大HP封顶、type5数显/图标/属性，末caster.phase8才增value00 |
| 2000 | 先Advance(source)再Advance(target)；skill6扣自身FP/type3，随后MoveEffect(source)；其余扣自身HP/type6，75..80/259/94的debuff给source，但2096..2101给target，末caster.phase8计数再MoveEffect(source)；均命中尾 |
| 普通 | skill6扣当前共享目标FP/type3；其余debuff/HP伤害：amount0建22331，非0且skill非84/85/87才扣HP/type6并图标/属性；最后Advance固定target |

上述HP/FP处理的原10000000免疫分支建当前共享目标22459并跳对应扣减/恢复；不把阶段20/22其它门禁套入。反伤2000的自身扣减没有amount!=0门槛，不因0金额跳过原CryptInt读取/RNG；普通伤害的amount0与84/85/87排除保持。所有CryptInt运算/比较按原fresh共享查找或保存自身fp/hp执行，不统一成固定target。

## 六处归零复制与十二BYTE读取

四共享目标归零（8000 FP、4000 FP、普通FP、普通HP）在Construct(0)之后先fresh取currentIndex、再读temporary.encoded W32，然后才该索引position/共享目标；先发布encoded，再读mask/pub，再BYTE readCount/pub，最后Destroy。现CopyPayload的字段次序正确，但调用前目标查找早于编码取样；本步在该原内联范围显式恢复，通用CopyPayload和其它调用不改。

2000自身FP归零必须先按mask→encoded→BYTE count读齐三值，再按mask→encoded→count发布；自身HP归零先BYTE count→encoded→mask读齐，再按count→encoded→mask发布。现交替读写改为原持样，不改两保存fp/hp或Constructor/Destroy/RNG。最大HP封顶的fresh共享目标及encoded/pub→mask/pub→BYTE count/pub原本正确，继续用既有CopyPayload。

另十二独立内存BYTE测试（两bit20、九bit2、命中尾bit8）只窄化当次逻辑读取，不改初始/visual/hit/number缓存W32，也不把fresh effectRunning换成旧索引。普通视觉一处音效补fresh event-X局部，保持真实两参/调用位置。合一CPP十九片段，另CPP/公头两注释，全部其余正文/签名/源保持，无新函数/类型/API/保护或额外RNG。

## 共同命中尾与FD0/FD4

fresh finishIndex，BYTE bit8清零且当项amount非0，或fresh skill6，才赋FD0=14*((target完整FDC>>1&1)+1)，原正常14/28，不除4。非空目标的命中尾均frameEvent=0；随后进入共同尾，保存FD0>0时减1并发布，因此命中当帧已消费一次延时。

fresh FD0==0才处理FD4。保存FD4非0且>=11：AdvanceCast(source)→MoveEffect(source)→FD0=0→FD4=0并返回；非0且<11直接保存值+1发布，不播放。FD4==0只PlayFrame(source,1)==1才fresh重读FD4并加1发布，其他播放返回直接尾退；不新增Move、MarkDeath(source)、计时上限/故障模拟或固定返回。

<a id="battle-frame-hit-decision"></a>
## 实施与依赖边界

新独立语义/F adapted/BATTLE已知成员各一项，枚举false；限定I十九片段、S完整当前根，U08/U09/R01/R04/V04只补来源/局部描述。既有BDI01表/选择、BOG01转向、追加效果及图标/属性/重生/CryptInt帮助体只复用，不重计根或声称所有callee已闭。原分母、六closure和完整门禁保持；依已获工程委托先SPEC后实施，WAV-only、MSVC排序待决、原格式明文及RPLAN.24不扩。

<a id="battle-frame-hit-acceptance"></a>
## 必要静态核对

核完整1371条4955B/22页/96directCALL/23callee/206跳两RET、唯一case14 11B、20B五S32表及两互斥RNG/选图条件；逐段核固定payload/caster/target、所有视觉/四类HP-FP/免疫/零额/特殊技能、六复制窗口和十二BYTE、唯一音效event-X、原14/28当帧延时及FD4>=11/播放==1后重读尾。整CPP只十九片段/一注释、公头一注释，正确最大HP封顶与坐标/其它全部源保持；不执行命中/数值/随机/计时样例。

核一身份/处置/成员、限定I/S/五任务和政策/分母/门禁、UTF-8无BOM/LF及静态汇总/-Check。全部输入/完整父-callee/间接alias、CryptInt实际RNG/正常数值、SDK正常失败/容量/重进退出、所有资源owner和声音画面运行仍待，不降低总SPEC§10；不编译/测试/运行/查产物/计算摘要或引依赖。

<a id="battle-frame-hit-source-state"></a>
## 当前状态

完整原根及十九片段已实施并完成必要静态核对，静态汇总与-Check通过；前态cg2d-re-battle-action14-history-20261010，提交引用cg2d-re-battle-action14-20261010。语义2059（2011F＋48X）、合同2121/3680（57.64%仅合同关联率）、F处置1486、软件证据1376，BATTLE已知成员299且枚举false；全部完整门禁保持，未编译或运行，整体SPEC未完成。
