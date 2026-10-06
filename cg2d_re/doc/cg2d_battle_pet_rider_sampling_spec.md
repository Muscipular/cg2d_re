# 战斗宠物快照与骑乘取样SPEC（BQB01）

日期：2026-10-07。按工程委托先记裁决再实施；依据当前原会话指令，聚焦223/225未纳管基线和225读取顺序缺口。

<a id="battle-pet-rider-sampling-baseline"></a>
## 原端基线与具体差异

| 原入口 | 当前源符号 | 条数/字节/页 | call/jump/ret | 本步改动 |
| --- | --- | --- | --- | --- |
| 0x00413FE0 | UpdateBattleAction223_413FE0 | 37/111/1 | 6/3/2 | 正确源码保持，补完整身份/合同 |
| 0x004150F0 | UpdateBattleAction225_4150F0 | 252/963/4 | 19/7/2 | 九处读取阶段恢复 |

合计289条1074B/五页、25call/14入口、十普通内部jump/四ret，无switch；两唯一404DB0入向各三条实参/调用/弃返邻窗。223/225已有真实定义，冻结定位有而明确语义/文档/处置空，本步补齐。224已有BRC01完整基线、31处修复及adapted记录，保持既有成果，不重复计新增。

| 225原阶段 | 原指令 | 源差异与裁决 |
| --- | --- | --- |
| 创建field1D0 | 41517F/415185/41518B | rider指针先，源S32值后；拆为明确三句 |
| 创建x/y/offset40/offset44 | 415229..415256 | 四阶段各重取rider后读源S32/F32，逐项发布，不能合为一次指针缓存 |
| 406260调用后effect.deltaX | 41527A/415280/415289 | effect指针先、frameX后；明确借当前对象 |
| 创建Turn实参 | 41528C..4152AB | effect→rider→deltaX→Y→offsetX加法→F32参数 |
| 运行Distance实参 | 4152F5..415310 | 同上读取序，原距离阈值5保持 |
| 抛物线 | 41539B..4153B4 | rider→time→baseY先取，再S32算式和发布 |

原float坐标仍F32，整数及存储宽度保持。上述正常旧值一致，恢复显式取样合同，不宣称并发或数值故障。四LegacyCryptInt复制原encoded W32读/写→mask W32读/写→readCount W8读/写交替，普通CopyPayload已符合，保留；内存载荷机制不属于网络加解密。

<a id="battle-pet-rider-sampling-decision"></a>
## 实施裁决与完整正常合同

依既有委托决定：只在225九阶段引入局部非拥有指针/字段值并明确读取次序，不改共享结构、头文件或其它函数。223完整正确体不改；224保持已有BRC01记录和源码。保留原重复清字段、坐标还原、每个调用前后的fresh槽和已有借指针，不新增空指针保护/索引限制或数学异常模型。

1. 223：先取共享当前玩家位置，再固定action.payload，GetBattlePetPosition(playerPosition)弃返回；调用后fresh currentIndex读取position S32一次。position>=0先发布共享5ABBFC，再439470→439340后返回；负值恰-2且共享旧snapshot>=0才4965D0→snapshot=-1；其它负值直接共同439470→439340。无新增影子状态/索引限制，两个原ret及回调次序保持，现有正确本体不改。

2. 225创建：固定payload，初始rider13C8为空才source.imageNo存13AC，再fresh imageNo映射→NewBattleRider(action,image)，返回T*写13C8且直接phase1。各fresh骑乘槽field58=0/characterAction11/nameC4原字节含NUL复制/field1D0；field1D0和四坐标各先当前rider T*再源S32/F32值。四独立rider先读，readiness/maxHp/barValue/barMaximum按encoded W32→mask W32→readCount W8逐项读写，普通CopyPayload不齐读、不复制token/reserved。x/y/offset40/44各独立fresh、flags|14，source graphicIndex=-1/lastImage0→406260，fresh effect T*后frameX S32→effect.deltaX；再effect T*、rider T*、deltaX S32、targetY F32、offset40 F32加法→F32 targetX，Turn(rider,targetX,targetY,1,0)，fresh rider.value54=8→PlayFrame(rider,0)弃返→fresh rider声465(x)后返回，音频暂停，原第三Y未消费。

3. 225运行：初始已捕rider.characterAction11，fresh effect/rider先，deltaX→targetY→offset40加法F32参数，Distance(rider,targetX,targetY)>=5走fresh rider Move；否则先effect指针/deltaX+offset40算式，后rider指针/F32offset40发布，再fresh effect/rider→offset44复制。逐阶段重取rider，将offset40/44分别按原__ftol正常S32边界转换、各共享屏幕offset W32读后加并发布deltaX/Y，再fresh rider各复制deltaX/Y到x/y。另fresh rider，先field58 S32、baseY S32，正常S32 displacement=(time-15)^2-225后baseY+displacement发布Y；另fresh rider取previous并++发布，previous>15才读source.frameY且<=displacement进入退场。原两遍phase0/field580与flags&~14顺序保，MarkDeath(fresh rider)后先借effect，再清rider槽，复制source.offset40，再fresh effect、source.offset40先捕、effect.offset44发布、S32投影x/y分别发布，baseImage13AC恢复image→MarkDeath(fresh effect)→effect槽null→声462(x)→PlayFrame(action,0)→439470→439340。最后fresh rider非空才PlayFrame0，无防空/钳制/回绕或其它异常专项，正确坐标还原/双重清字段和其它旧源保持。

两完整原根289条1074B/五页、25call/14入口/十普通内部jump/四ret，两唯一404DB0入向六邻窗。只225九源阶段恢复指针先源值、两实参deltaX先Y后offsetX、抛物线time和baseY先取后算；正常旧值一致，不称并发或机器故障。223正确完整体不改；四普通CopyPayload交替encoded/mask/readCount、真实S32/F32/W32/W8边界、每阶段fresh槽/已有借指针、贴近先算后rider、两遍清字段/坐标还原、创建/回收/调用/返回保持。224既有BRC01 adapted完整记录及正确体保，不重复纳管计数。只当前完整正常源接线静态，不外推递归父/全部子依赖/其它生产与owner寿命、SDK/生成机器ABI/实际x87/画面运行；音频暂停，RPLAN.24不专项异常/保护/回绕/故障等价保。

<a id="battle-pet-rider-sampling-acceptance"></a>
## 可度量的限定验收

- 两根全部289条/1074B、五页、25原call/14callee、十jump/四ret及两父六邻窗可追；九源码片段分别绑定真实原指令。
- 整YobiRuntime只225九片段变化，223/224及普通CopyPayload、共享字段/公开头和其它正文保持；四复制、四正常投影、真实重复清字段与先借effect再清rider、原坐标/image还原及调用/返回顺序可对照。
- 新增1474/F:00013FE0、1475/F:000150F0正常语义/equivalent，冻结定位全文保。语义1474→1476/有效1469→1471、关联1587→1589/3680（43.18%）、BATTLE151→153枚举open、分类599→601/E754→756。只有两局部I/S、各十个独立实际来源。
- R7 260/3061/余2801、差队列172/待归属43、其它全部机器对象与完整门禁不变；U08/R01/R04/V04只当前描述/引用仍open，原60任务关闭条件保。完整战斗/生产与owner寿命/生成机器/运行及R8继续open。

<a id="battle-pet-rider-sampling-source-state"></a>
## 当前交付

225九源阶段修复与223/225完整正常基线纳管，快照 `cg2d-re-battle-pet-rider-sampling-20261007`。定向静态及measure/-Check记录到唯一进度；未编译/调试/测试/运行/摘要，音频暂停、网络原格式明文及原验收标准保持。
