# 战斗共用施法与弧射取样SPEC（BQE01）

日期：2026-10-07。按工程委托先记裁决再实施；聚焦235、238/239共体、240..242与244/245未纳管基线和六个读取阶段。

<a id="battle-shared-cast-arc-sampling-baseline"></a>
## 原端完整基线与差异

| 原入口 | 当前源符号 | 条数/字节/页 | call/jump/ret | 源调整 |
| --- | --- | --- | --- | --- |
| 0x00416DB0 | ApplyBattleCastEffects_416DB0 | 12/29/1 | 2/0/1 | 0阶段 |
| 0x00417410 | UpdateBattleActions238_239_417410 | 485/1700/7 | 39/50/3 | 5阶段 |
| 0x00417AC0 | UpdateBattleAction240_417AC0 | 51/184/1 | 4/7/1 | 0阶段 |
| 0x00417B80 | UpdateBattleAction241_417B80 | 90/316/2 | 6/11/4 | 1阶段 |
| 0x00417CC0 | UpdateBattleAction242_417CC0 | 61/191/1 | 5/9/1 | 0阶段 |
| 0x00418550 | UpdateBattleAction244_418550 | 12/47/1 | 1/0/1 | 0阶段 |
| 0x00418580 | UpdateBattleAction245_418580 | 18/49/1 | 3/0/1 | 0阶段 |

七完整根729条2516B/十四页、60原call/26callee、77普通内部jump/十二ret，无switch。七唯一404DB0入向21邻窗合56B。原404DD5表238/239两个W32项405AF4/405AF8字节均e1554000，目标同4055E1，调用同417410；覆盖八逻辑状态只增七身份，既有全表R7记录保，不重复分类。

| 实施阶段 | 原证据 | 裁决 |
| --- | --- | --- |
| 共体扣HP归零 | 4176DA..41770C | index→encoded→position→共享目标；encoded先发布，后两字段交替 |
| 共体激活低W8 | 417724 | fresh current.flags bit20 |
| 共体属性图标低W8 | 417877、41794F | 两原路径，一共用源表达式，各在数显调用后fresh读取 |
| 共体时长条件低W8 | 417989 | 保同tailIndex，低W8 bit8先、amount后 |
| 共体速度/phase239 | 417A28/417A2B/417A31 | fresh speed/零比较先，phase239后；负值再fresh speed取反 |
| 241声参取样 | 417C95/417C9B/417C9F | fresh frameEvent→x→event-10000，两参数实际消费 |

共三W8源表达式承接四原消费点，不能把共用属性表达式计成两源片段。初始/命中/主效果/数显/最终回收flags W32缓存和寄存器AL/AH/CL/CH保持。原0.5/1/0三个F32字节0000003f/0000803f/00000000已冻结；按正常业务F32存储后独立比较，不套未落位中间。其它五正确完整体/普通CopyPayload不改。

<a id="battle-shared-cast-arc-sampling-decision"></a>
## 工程裁决与完整正常合同

依既有委托决定：只共体五片段和241声参一片段，局部展开归零复制/显式取样；不拆原238/239身份、不改共享结构/头和其它本体。保240原非空检查、241真实等待、242无限原循环及哨兵后额外推进、共体归零当帧两次播放、原HP/图标接收者/状态/返回/RNG。正常旧值一致，不新空保护/迭代上限/异常专项门禁；241只保已有声参数，音频暂停。

1. 235：先借action.payload取battlePosition18 S32，ApplyFlags(position,action)→439340(action)，两个原真实call及父弃返保持，现正确体不改。

2. 238/239共体：固定payload/fresh initialIndex，初始flags W32 bit20000000且同index当前共享target Distance<70、fresh source.frameEvent==0才fast退出：另fresh current共享target先捕，MarkDeath(source)→phase17→source随机图110473，原返回非空才y S32减68；cached target图110474→439340(target)后返回。否则另fresh position/共享target Distance<20且fresh frameEvent0才普通处理。七物理根/八逻辑状态中只有一个F:00017410，原404DD5两W32表项238/239都4055E1，不新身份/新R7。 238/239普通处理：fresh current同index.position→共享target T*→flags W32→targetPayload T*，image/secondImage初始-1。cached bit4 image110000，400为FindTechnique(8,position)<12图110098否则111048；其余skill267/3029只second110477，其它RAND(0,2)+110001 primary。Turn(target,source,1,0)后fresh ownPosition/共享attacker写target+10，fresh current.flags W32写targetPayload.FE4，再fresh ownPosition ApplyFlags(position,source)。fresh hitIndex/flags W32，bit8 phase31；100无flash旧值条件，先target.flash1→声210(x)→phase31。其余hitFlags E000清且same hitIndex.amount!=0才bit2 target110008、image>=0/second>=0各独立target图、Rebirth(fresh共享ownPosition attacker,target)→声255(x)；随后fresh current record.amount先/position后shared target HP扣减，fresh shared目标Less0，<0 Construct0消费RNG后fresh index→temporary.encoded W32→position S32→共享target T*，先encoded发布，mask W32/count W8再交替→Destroy；fresh current.flags低W8 bit20才cached targetPayload.FDC W32|2/gate1/alive1。hitFlags及初始/image缓存W32/寄存器AL/AH/CL/CH不改内存W8，正常旧值一致。 238/239恢复/显示：fresh current.flags W32，8000只439340(target)；4000先439340(target)，fresh healIndex.flags W32 10000000当前共享目标图22459并跳共同尾；否则same healIndex.amount先/position后AddHP，fresh shared target GreaterHP/max，超过上限再fresh目标普通CopyPayload encoded/mask/count交替；数显fresh record.flags W32→amount→cached targetY→targetX/type5/style2。2000只439340(target)。其它100000回收cached targetPayload.effect1054，调用后FDC先读/清槽后FDC&~20；439340(target)后fresh displayIndex/flags W32，10000000当前图22459且无数显/Attribute，否则targetY→sameindex.amount→targetX/type6/style2/flags。恢复和普通数显各在原417877/41794F独立fresh current.flags低W8 bit2才target22454，随后Attribute(fresh共享ownPosition attacker,target)。源showAttributes共用一个W8表达式承接两真实路径，不把两个原消费点计成两个新源片段。共同fresh tailIndex.flags低W8 bit8清且sameindex.amount非0，才按cached targetPayload当前FDC W32的bit1求14*(bit+1)写source FD0，frameEvent=1。 238/239计时/速度/退场：处理或未近均fresh remainingFD0，>0减1并PlayFrame0弃返；之后另fresh FD0==0再PlayFrame0，允许归零当帧两次播放/回调改变frameEvent。调用后phase S32先、speed F32后，phase238减原F32 0.5，其它加0.5，F32发布后fresh speed<=原F32 1，恰此分支另fresh speed与原F32零比较先取、再phase239发布；若负，另fresh speed取反/F32发布。Move后fresh frameEvent==1，fresh current.flags W32/寄存器AL/AH两bit8/100均清才MarkDeath(source)，phase17；其它frameEvent fresh current共享target Turn(source,target,1,6)。保原三ret、三个原F32字节0000003f/0000803f/00000000、普通业务低速分支、两次播放、接收者/别名/状态/返回/RNG；不新异常或钳制。

3. 240：fixed payload，characterAction6/frameRate100，fresh value1024==0才NewEffect(source,0,offsetY0,type34,0,1)返回T*写source+10；PlayFrame1后fresh event S32在[10000,20000)时fresh+10非空MarkDeath后槽null，value1024=0/source.phase241/FF0=0。之后另fresh+10非空且skill267或3029才DrawTiled(5)，不加失败保护/不重用先前槽，原四call及回收/发布序保持，正确体不改。

4. 241：初始source.frameEvent先缓存，再fixed payload/character6/frameRate100；初始event0只PlayFrame1返回恰1才439470→439340→FD0=0后返回，其它返回也直接退出。event非0时FF0 delay>0减1并返回；否则fresh current record，position=-1先FF0=0/source.frameEvent0后返回。skill267按level<12 image114177/114196；skill3029用114263/114264；正确原图常量均六位，调用NewArcShot(source,已借effect,image,fresh image102C,0)弃返。其它skill也继续439470→FF0=6；此后fresh frameEvent S32先、source.x S32后，声(event-10000,x)，原第三Y未消费，音频暂停，不新增声功能。保四ret/唯一原419DA0物理点及选择分支。

5. 242：fixed payload，PlayFrame(source,0)弃返后无限原循环：每轮fresh current record，position=-1退出；skill267/3029与241同四图/level12和image102C实参/已借effect，其它skill不创建也推进439470，再回fresh循环。不限制迭代/不猜容量。哨兵后原额外一次439470再439340，不能合并/删额外推进；正确体及一原ArcShot物理点/循环/返回保持。

6. 244：fixed payload，FDC W32读/|2写→gate1→alive1→439340(source)，既有正确体保持，不能因OR bit2就改成内存W8。

7. 245：fixed payload/currentIndex，same record.position3 S32寿命先、position S32类型后，NewTiled(type,lifetime)→439470→439340，原实参次序和正常源正确体保持。

七完整原根729条2516B/十四页、60call/26入口/77普通内部jump/十二ret，七唯一404DB0入向21邻窗/56B、238/239两个实际共用表项与三F32常数。七物理身份覆盖八逻辑状态，不重复计共体/不新R7。只238/239五片段和241一片段共六：归零index/encoded先position/共享目标；三W8源表达式承接四真实原W8点，属性共用表达式对应两原路径；speed先零比较再phase239、声事件先X。正常旧值一致，不称并发/数值或机器故障。其它五完整正确体/共享helper、W32寄存器旗位/原HP图标/Attribute接收者、两播放/原循环和额外推进/状态/返回/RNG及真实失败/非空分支保持。只当前完整正常源接线静态，不外推递归父/全部子依赖/其它生产与owner寿命、SDK/生成机器ABI/实际x87/画面运行；音频暂停，RPLAN.24不专项异常/保护/回绕/故障等价保。

<a id="battle-shared-cast-arc-sampling-acceptance"></a>
## 可度量的限定验收

- 七根729条/2516B、十四页、60call/26callee、77jump/十二ret，七父21邻窗/56B、两共用表项、三个F32及六源码片段可追。
- 整YobiRuntime只共体五/241一片段；三W8源表达式对应四原点，归零序/速度先phase后/声Event先X分别落实。其它五完整体、普通helper/公开头、原W32缓存/失败及非空/接收者/RNG/两播放和原循环/额外推进/返回保持。
- 新增1485..1491七原已有正常定义/equivalent，七冻结定位全文保持。语义1485→1492/有效1480→1487、关联1598→1605/3680（43.61%）、BATTLE162→169枚举open、分类610→617/E760→762。仅两局部I/S、每项十五独立实际来源。
- R7 260/3061/余2801、差队列172/待归属43及其它全部机器对象/完整门禁保持；U08/R01/R04/V04只当前描述/引用仍open，原60任务关闭条件保。完整战斗/生产与owner寿命/生成机器/运行及R8仍open。

<a id="battle-shared-cast-arc-sampling-source-state"></a>
## 当前交付

共体与241六阶段实修，七物理根/八状态完整正常基线纳管；快照 `cg2d-re-battle-shared-cast-arc-sampling-20261007`。定向静态及measure/-Check记录唯一进度；未编译/调试/测试/运行/摘要，音频暂停、网络原格式明文及原验收标准保持。
