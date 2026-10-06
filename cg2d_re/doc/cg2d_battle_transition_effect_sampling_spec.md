# 战斗转场/目标坐标与209取样SPEC（BPX01）

日期：2026-10-07。按工程委托先记裁决再实施；上承战斗收尾、字段/别名、正常CRT承接和追踪验收体系。

<a id="battle-transition-effect-sampling-baseline"></a>
## 原端基线与缺口

| 原入口 | 当前源符号 | 条数/字节/页 | call/jump/ret |
| --- | --- | --- | --- |
| 0x00412290 | UpdateBattleAction177_412290 | 15/42/1 | 2/0/1 |
| 0x004122C0 | UpdateBattleAction178_4122C0 | 8/23/1 | 1/0/1 |
| 0x004122E0 | UpdateBattleAction179_4122E0 | 14/42/1 | 2/1/1 |
| 0x00412310 | UpdateBattleActions180_203_412310 | 161/455/3 | 2/47/1 |
| 0x004127B0 | UpdateBattleAction205_4127B0 | 17/56/1 | 2/1/1 |
| 0x004127F0 | UpdateBattleAction206_4127F0 | 17/50/1 | 2/0/1 |
| 0x00412830 | UpdateBattleAction207_412830 | 18/62/1 | 1/0/1 |
| 0x00412870 | SetBattleAction208Target_412870 | 42/144/1 | 4/1/2 |
| 0x00412900 | UpdateBattleAction209_412900 | 465/1642/6 | 32/58/5 |

九完整原根合757条2516B/16页、48实际call/24入口、108普通内部jump/14ret，无根内跳转表；180..203原体是23组固定CMP/立即参数链。九真实普通入向属于404DB0，两段27条邻窗及原总分派表的32条实际项证明父case：177..203、205..209。其中180..203共一个F/一普通call点；188不创建效果但仍phase204，不因未列源type而删除接线。原总分派表已由既有完整基线承担，本组仅所需32项参数接线，不重列R7。

209源六处W32表达式对应原实际W8消费：412996 FDC2、4129E1 FE4低2、412A2F FDC低字节、412C01 current.flags低2、412CCC current.flags低20、412E4E current.flags低2。原初次W32/寄存器AL-AH后用字段、image/damage/target W32不混改。412A07/412A1C和412AC7/412ADE先FDC后FCC取样，源复合比较未固定两字段读取顺序；改当前两个阶段具名捕获，不刷新原初始缓存。

209零临时载荷原412DD9捕currentIndex、412DDF取encoded、412DE6/DEA查目标、412DF1写encoded；随后412DF7/E00读写mask、412E03/E0B读写readCount。源先查目标后进入普通CopyPayload，把encoded读取推后；本地展开原阶段，不把本组原交替读写改成齐读齐写，不改普通helper/目标token与reserved。

208原54D4D8/54D4D0是F64 -0.75/480，字节000000000000e8bf/0000000000007e40；209角度/速度54D438/54D424/54D464/54D420是F32 180/360/0.3/0，实际字节00003443/0000b443/9a99993e/00000000。保未落位中间比较及字段F32发布，不以数学算式代真实x87运行证明。

<a id="battle-transition-effect-sampling-decision"></a>
## 实施裁决与正常完整合同

只209修改九个阶段：六处unsigned byte对象表示读取（原共享字段仍W32、写者保持）、两个fresh FDC/FCC显式按原顺序取样、一个临时encoded/共享目标查找与原交替载荷发布。正常旧值一致，边界/顺序差异不能宣称运行数值错误。

1. 177：同一payload currentIndex W32→effects1C[index].position4 W32，421570(image,2,1)返回弃用且不写1060槽→439340弃返。原无新借用资源槽或回收，保持。

2. 178：439340弃返后fresh action.phase W32写0；不是先设0再推进，原顺序保。

3. 179：characterAction W32先6→517DA0(action,1)，返回恰等1才439340；其它返值不推进，原参数/返回消费保。

4. 180..203共一原根/入向：先FDC W32读后测AL60，清则PlayFrame0弃返；fresh value1024非0直接phase W32=204。值0才一次W32捕phase，原23个CMP/固定立即type依序到41CD00(action,0,0,type,0,1)，返值弃用；188及原其它未匹配分支不创建效果，仍进入共同phase204发布。保23对实参、24父case和原空default，不造24个F或表物理身份。

5. 205：characterAction先16→PlayFrame(action,1)，只有返回1才frameIndex2C0/frameRemain2C4 W32清0后439340弃返，保等待分支及清字段顺序。

6. 206：固定payload currentIndex→position W32→共享59E980非拥有目标T*，406190(action,target,1,0.0F)弃返后439340；真实target读取/原0浮点参数保，不加范围或空保护。

7. 207：固定一次currentIndex W32；effects1C[index].position W32写FE8后同index.flags W32按S32位样写FEC，原(index+2)*16是effects flags20别名，非另条记录；439470弃返后phase6。源已有W32 bit_cast对应，不改正确缓存/别名。

8. 208：currentIndex→position→共享59E980目标T*；target.offset40 F32×原F64 -0.75+480中间保，以fresh offset44 F32与未落位边界比较；再fresh取offset40作原__ftol正常整数承接。低于边界branch坐标x+44/y+40，否则x-44/y-30；原X先FE8发布后fresh Y/__ftol到FEC，phase6。四原ftol点同当前CRT正常转换边界、两个真实F64常数对应；不把算式换F32，也不造机器精度/异常通过。

9. 209：8/FE4 W32 C04条件9与rate50；FCC先增、FDC W32初始缓存bit1与14*(bit+1)门槛。首段角度+180保未落位中间比较360、speed5/fresh低W8 FDC2设gate1/alive1→Move→PlayFrame1返。中段fresh低W8 FE4 bit2决定方向+1/达8减8，fresh W32 FDC先于FCC读取决定14*(bit+1)相等时speed5/fresh低W8 FDC2和CountActive<=1才gate0/alive0，FF0清0；FF0为0建110006、fresh旧FF0先增旧值>=2归0，speed减真实F32 0.3中间负值业务归0→Move。之后fresh W32 FDC先于FCC读取命中14*bit+24才走命中链：sourceEntry从自身position、attacker=sourceEntry原+10先保存，target再读current.position，缓存targetPayload先发布target+10/source、freshcurrent.flags→FE4、FCC/1024清0、ApplyFlags。后W32 flags缓存决定8设phase31或100仅flash0时sound210(x)/flash1/phase31（原声第三Y未消费，不扩音频）；其它保C000/position3/2000/100000条件，MarkDeath后fresh target FDC W32再清1054/清20；fresh低W8 flags2决定110008、fresh W32 imageFlags决定4图110000、400技术8/阈12图110098或111048、其它attacker skill260等级12/9图111054/110473和direction1，否则一次RNG0..2图110001起；NewRebirth后fresh低W8 flags20设targetFDC2/gate1/alive1。fresh W32 damageFlags按E000/10000000图22459、amount0图22331、skill84/85/87跳伤害；否则扣fresh共享目标readiness→另fresh比较目标<0则Construct0，先捕currentIndex再读临时encoded W32，再读position/共享目标写encoded，随后mask W32读写、readCount W8读写→Destroy。保原交替发布，目标token/reserved不改。数显fresh flags W32→targetY→amount→targetX，type6/style2/flags，之后fresh低W8 flags2图22454，再Attribute。共同target439340、Turn(target,action,1,0)、action439470；整体尾PlayFrame1。FCC达14*初始bit+54时清0，原初始FDC缓存bit2决定6/100推进后phase11改4/100，或freshFE4/3010决定6/100或1/250推进。source/target/attacker共享别名捕获、所有真实调用/RNG/条件/无额外保护保持。

源共有24个phase经180..203共体，原type映射180→13、181→1、182→2、183→25、184→16、185→21、186→22、187→23、189→4、190→5、191→7、192→8、193→9、194→10、195→11、196→12、197→26、198→29、199→14、200→15、201→6、202→37、203→39；188和原default无效果创建后仍写204。此处是原固定指令参数，不新增独立类型表/函数或依赖。

九完整原根757条2516B/16页、48实际call/24入口、108普通内部jump/14ret、九唯一404DB0入向27条参数-弃返邻窗与32原父分派表项、共体23type固定参数及六实际常数。只209九源码阶段：六实际低W8消费、两FDC先FCC后真实取样、临时encoded在目标查找前及原交替三字段发布；正常旧值相同，不称数值/并发或生成机器故障。八正确体与字段W32/F32/非拥有T*、source/target/attacker别名、业务边界/返回/RNG及音频暂停保；不新造24根/物理type表或R7身份。只当前完整正常源接线静态，递归父/全部外依赖与其它生产/owner寿命、SDK/生成机器ABI/真实x87/时钟RNG/画面运行未验，RPLAN.24不专项异常/模拟/故障等价保。

<a id="battle-transition-effect-sampling-acceptance"></a>
## 限定验收

- 九完整原根、实际字节连续边界、48call及每点参数/返回消费、108内部jump/14ret、九入向/27邻窗/32父项、23组type实参和六常数可追。
- 整个YobiRuntime只209九阶段，八正确体及全文件其它内容保持；六W8消费、两真实读取序、临时encoded与原交替字段发布、别名/业务上下限和原x87中间/时钟RNG消费保持。
- 新增1453..1461九原已有定义正常语义及equivalent：语义1453→1462/有效1448→1457、关联1566→1575/3680（42.80%）、BATTLE130→139枚举open、分类578→587、E746→748仅两局部I/S，每项十七独立来源。R7 259/3061/余2802、差队列172/待归属43和其它全部完整门禁保持。
- U08/R01/R04/V04只当前和引用仍open，60任务原关闭条件不改，完整战斗/生命周期/画面及R8未关闭；局部正常源静态不代递归父、全部子依赖、其它生产与owner寿命、生成机器/环境和运行。

<a id="battle-transition-effect-sampling-source-state"></a>
## 当前交付

209九阶段字段/目标查找取样恢复，九完整转场/目标/效果根覆盖32既有状态接线纳管；快照 `cg2d-re-battle-transition-effect-sampling-20261007`。未编译/调试/测试/运行/摘要；音频暂停、原格式明文及原验收标准保。
