# 战斗计时、坐标与恢复扣减取样SPEC（BQC01）

日期：2026-10-07。按工程委托先记录裁决再实施；聚焦228..232未纳管基线和232五个真实读取阶段差异。

<a id="battle-timing-coordinate-damage-sampling-baseline"></a>
## 原端完整基线与缺口

| 原入口 | 当前源符号 | 条数/字节/页 | call/jump/ret | 本步调整 |
| --- | --- | --- | --- | --- |
| 0x00416000 | UpdateBattleAction228_416000 | 8/32/1 | 1/0/1 | 正确完整体保持，补合同 |
| 0x00416020 | UpdateBattleAction229_416020 | 31/117/1 | 3/1/2 | 正确完整体保持，补合同 |
| 0x004160A0 | UpdateBattleAction230_4160A0 | 55/164/1 | 6/1/1 | 正确完整体保持，补合同 |
| 0x00416150 | UpdateBattleAction231_416150 | 52/193/1 | 5/5/1 | 正确完整体保持，补合同 |
| 0x00416220 | UpdateBattleAction232_416220 | 437/1598/6 | 43/42/1 | 五源取样阶段 |

五完整根583条2104B/十页，58原call/26callee、49普通内部jump/六ret，无switch。五404DB0入向各三邻窗合15条52B，另228真实416019→229内部call且透传返回，共六入向。229重复写frameRate250及两原PlayFrame返回分支保持；231原F32 54D4E0/54D420实际字节6666e63e/00000000，不把0.45十进制替为不同常数。

| 232原阶段 | 具体原指令 | 当前缺口/裁决 |
| --- | --- | --- |
| 恢复数显 | 41658F/416595/41659E/4165A2 | 索引先Y、amount后、X最后；缓存本阶段索引 |
| 反伤数显 | 41667C/416682/41668B/41668F | 同序，原source坐标接收者保持 |
| 普通扣减数显 | 41678D/416793/41679C/4167A0 | 同序，原cached target坐标保持 |
| 反伤归零 | 416655/659/65D→416661/66B/671 | mask/encoded/count全部先读，再按原序写source HP |
| 普通扣减归零 | 416753..416781 | Construct后index→position→encoded→共享目标T*；先写encoded，mask/count再交替 |

flags原4162B6一次W32缓存到EBX，后续BL/BH属于寄存器消费，不能误改为fresh内存W8。普通上限CopyPayload确实encoded/mask/readCount交替，保持；归零两路径必须按各自原序。正常旧值一致，不称运行数值或并发故障。

<a id="battle-timing-coordinate-damage-sampling-decision"></a>
## 工程裁决与完整正常合同

依既有委托决定：只232三局部索引和两归零复制阶段调整；普通helper/结构/公开头/228..231及其它源保持。不新造原身份、调用/状态，保原接收者、返回、RNG和真实业务阈值；不增加异常、保护或故障模拟门禁。

1. 228：固定action.payload，valueFF0 S32清0→source.phase229→直接call229(action)，透传其返回S32。原416019真实call和原ret，非推测fallthrough/新状态身份。

2. 229：固定payload，characterAction3，fresh valueFF0 S32有符号<60才先framePlayingRate250、再fresh计时250-4*t发布，随后另fresh计时++，PlayFrame(action,0)返回；>=60清计时0→439340→PlayFrame(action,0)返回。保真实250重复写、三次计时读取/正常S32和两个原返回，不做饱和/回绕/额外限制。

3. 230：固定payload/currentIndex，effects.flags按原S32整数FILD语义转F32 Y后同cached index.position S32转F32 X，Turn(action,X,Y,1,10)→Move(action)→PlayFrame0弃返。调用后fresh index再按Y后X构造Distance(action,X,Y)，返回有符号<8才fresh index.position→offset40 F32发布，再另fresh index.flags按S32→offset44 F32发布，439470→439340。flags槽此处是完整32位坐标位模式，不当位旗筛选、不共用第二次索引，正常源保持。

4. 231：先取source.value54 F32减原54D4E0 F32字节6666e63e（0.44999998807907104），中间再固定payload/characterAction9→F32速度发布；原未落位中间和原54D420 F32零比较，负值才正常业务置0。framePlayingRate100→Move后fresh速度<=0才439340后速度6，PlayFrame0弃返。fresh valueFF0==0创建source图110006；调用后另fresh valueFF0==1再创建110007，允许前调用改变计时。最后再次取previous，++发布；previous>=2再清0但仍返回previous。保原正常下限/两个独立if/三取样/返回，实际x87机器未验。

5. 232：固定payload，characterAction3/value54=10/frameRate100，fresh current.position共享目标Turn(action,target,1,0)→Move；另fresh position/共享目标Distance<20才处理，未近只共同RNG/尾PlayFrame。近时fresh同index.position先目标T*、flags W32后、目标payload T*后缓存，Turn(target,action,1,0)→targetPayload.castSkill=-1，fresh own battlePosition18/共享attacker写target+10、cached flags写目标FE4，另fresh ownPosition ApplyFlags(position,action)。flags bit8置target.phase31；否则100且flash1004==0声210(x)→flash1/phase31，非零只共同尾；其它进入恢复/反伤/扣减。所有BL/BH测试消费已缓存W32寄存器旗位，不改成共享内存W8读取。

6. 232其余近距：cached flags C000全清且fresh current.position3!=0才目标payload x10/y14 S32→target offsets F32。2000反伤预效果：bit2给source图110008；RAND(0,2)+110001给target图→Rebirth(action,target)，取source skill后source FE4=2000；skill非404/406且cached bit20才source FDC|2/gate1/alive1。非2000预效果：100000回收target.effect1054，调用后目标FDC先读、槽null后FDC&~20；bit2给target110008，再独立RAND(0,2)+110001图和Rebirth，bit20激活目标FDC/gate/alive。原业务、共享目标别名/缓存和两次独立RNG路径保。

7. 232 HP/共同尾：8000只439340(target)；否则4000先439340(target)，10000000 fresh共享当前目标图22459；否则fresh current同record.position3先amount再position/共享目标AddHP，调用后fresh共享目标GreaterHP/max，超过原上限才另fresh目标普通CopyPayload encoded/mask/count交替。恢复数显先currentIndex→cached target.Y→同index.amount→target.X，type5/style2/cached W32 flags；bit2 target图22454→Attribute(action,target)。否则2000先439340(target)，10000000同当前图22459；否则fresh amount扣source HP，<0 Construct0后mask W32/encoded W32/readCount W8三齐读，再按同序写source HP→Destroy；反伤数显先index→source.Y→amount→source.X/type6/style2/flags，bit2 source图22454，后Attribute仍(action,target)，原4166B5跳共享4165C4/5CC call点保逻辑且不新增原call计数。其它普通扣减：10000000当前图22459，否则fresh record.amount=0给当前目标图22331，非0amount先、position/共享目标后扣HP；fresh共享目标Less0，负值Construct0消耗RNG后再fresh index→position→temporary.encoded W32→共享目标T*，发布encoded，随后mask W32/readCount W8交替读写→Destroy。普通扣减数显先index→cached target.Y→amount→target.X/type6/style2/flags，bit2 target图22454→Attribute(action,target)，最终439340(target)。近距共同cached bit8清且fresh current.amount!=0才按缓存目标payload当前FDC W32的bit1计算14*(bit+1)写source FD0，随后439340(action)→439470(action)。所有距离路径共同NextRandom&1恰0图110004/1图110006给source NewPosition，PlayFrame(action,0)返回。保两个HP归零接收者、图标/Attribute接收者、原上限、调用共享尾/返回/RNG消费和非拥有指针/生命周期边界。

五完整原根583条2104B/十页、58call/26入口/49普通内部jump/六ret，五404DB0入向15邻窗/52B及228→229真实内部入向共六入向，两原F32常数。只232五源阶段：三数显currentIndex先Y，反伤零临时mask/encoded/readCount齐读，普通扣减零临时encoded先于共享目标指针且后两字段交替。正常旧值一致，不称并发或数值/机器故障；228..231正确整源、232其它W32缓存/寄存器旗位/独立RNG/原HP图标接收者、上限普通CopyPayload/共享调用尾/返回保持。只当前完整正常源接线静态，不外推递归父/全部子依赖/其它生产与owner寿命、SDK/生成机器ABI/实际x87/画面运行；音频暂停，RPLAN.24不专项异常/保护/回绕/故障等价保。

<a id="battle-timing-coordinate-damage-sampling-acceptance"></a>
## 可度量的限定验收

- 五根583条/2104B、十页、58call/26callee、49jump/六ret、五父15邻窗/52B、228内部call229与两F32常数全部可追；五源码阶段分别绑定原指令。
- 整YobiRuntime只232五片段；三数显按index→Y→amount→X，各使用本阶段cached索引。反伤归零三字段先齐读/写source；普通扣减encoded先于共享目标T*、后两字段交替。其它原W32旗位缓存/普通上限复制/共享尾/接收者/状态/RNG/返回保持。
- 新增1476..1480五原已有正常定义/equivalent，五冻结定位全文保。语义1476→1481/有效1471→1476、关联1589→1594/3680（43.32%）、BATTLE153→158枚举open、分类601→606/E756→758。仅两局部I/S、各十三独立实际来源。
- R7 260/3061/余2801、差队列172/待归属43及其它全部机器对象/完整门禁保持；U08/R01/R04/V04只当前描述/引用仍open，原60任务关闭条件保。完整战斗/生产与owner寿命/生成机器/运行及R8仍open。

<a id="battle-timing-coordinate-damage-sampling-source-state"></a>
## 当前交付

232五取样阶段实修，228..232计时/坐标/恢复扣减完整正常基线纳管，快照 `cg2d-re-battle-timing-coordinate-damage-sampling-20261007`。定向静态及measure/-Check记录到唯一进度；未编译/调试/测试/运行/摘要，音频暂停、网络原格式明文及原验收标准保持。
